//----------------------------------------------------------------------
//
//			File:			"diff.cpp"
//			Created:		04-7-2026
//			Author:			vivisuke
//			Description:
//
//----------------------------------------------------------------------

#include <QTextDocument>
#include <QTextBlock>
#include "dtl/dtl.hpp"
#include "MainWindow.h"
#include "DocWidget.h"
#include "MarkdownEditor.h"
#include "MarkdownPreview.h"

// 1. ダミー行（高さを揃えるための空行）かどうかの判定
bool isDummyLine(const QTextBlock &block) {
    return block.userState() == 0;
}

// 2. 行番号の取得（1ビット右シフトするだけ）
int lineNumber(const QTextBlock &block) {
    return (unsigned)block.userState() >> 1;
}

// 3. 差分の有無（最下位ビットが 1 なら差分あり / 0 なら差分なし）
bool hasDiff(const QTextBlock &block) {
    return (block.userState() & 0x01) != 0;
}

// -------------------------------------------------------------
// 【書き込み時の処理例】
// -------------------------------------------------------------

// ダミー行をセットする場合
void setDummyLine(QTextBlock block) {
    block.setUserState(0);
}

// 物理的な行をセットする場合
void setPhysicalLine(QTextBlock &block, int ln, bool changed) {
    int state = (ln << 1) | (changed ? 1 : 0);
    block.setUserState(state);
}
//
void removeAllDummyLines(QTextDocument *doc) {
    if (!doc) return;

    QTextCursor cursor(doc);
    // 描画更新を一時停止し、処理を高速化
    cursor.beginEditBlock();

    // 末尾のブロックから先頭に向かって逆順に巡回します
    QTextBlock block = doc->lastBlock();
    while (block.isValid()) {
        // 削除操作によってブロックが破棄される前に、前のブロックへの参照を確保しておく
        QTextBlock prevBlock = block.previous();

        if (isDummyLine(block)) {
            cursor.setPosition(block.position());

            if (block.next().isValid()) {
                // パターンA: 末尾以外のブロックを削除する場合
                // 「自ブロックの先頭」から「次のブロックの先頭」までを選択（これで改行コードも含まれます）
                cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
            } else {
                // パターンB: 末尾のブロックを削除する場合
                if (prevBlock.isValid()) {
                    // 前の行が存在する場合、手前の改行コードも含めて後方から選択・削除します
                    cursor.movePosition(QTextCursor::EndOfBlock);
                    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
                    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor); // 手前の改行を巻き込む
                    cursor.removeSelectedText();
                } else {
                    // ドキュメントにこの1行しか残っていない場合は、プレーンにテキストのみクリア
                    cursor.select(QTextCursor::BlockUnderCursor);
                    cursor.removeSelectedText();
                }
            }
        }

        // 次の巡回先（手前のブロック）に移行
        block = prevBlock;
    }

    cursor.endEditBlock(); // レイアウトを再計算して画面を更新
}
// -------------------------------------------------------------
std::vector<QString> extractLinesFromDocument(const QTextDocument *doc) {
    std::vector<QString> lines;
    if (!doc) return lines;

    // メモリ確保のオーバーヘッドを減らすため大まかにリザーブ
    lines.reserve(doc->blockCount());

    // QTextBlock を走査することで、巨大なテキストでも分割時のメモリ消費を最小限に抑えます
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        lines.push_back(block.text());
    }
    return lines;
}
void MainWindow::onAction_DiffMode(bool checked) {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_diffMode = checked;
	docWidget->m_editor->setDiffMode(checked);
	if (checked) {
		docWidget->m_editor->expandAll();
		docWidget->m_editor->setHighlightMarkdown(false);
		docWidget->m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);
	} else {
		if( docWidget->m_docType == DocType::Markdown )
			docWidget->m_editor->setHighlightMarkdown(true);
		docWidget->m_editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
		QTextDocument *doc1 = docWidget->m_editor->document();
		QTextDocument *doc2 = docWidget->m_diffview->document();
		removeAllDummyLines(doc1);
		removeAllDummyLines(doc2);
	    docWidget->m_editor->setDummyInserted(false);
	    docWidget->m_diffview->setDummyInserted(false);
	}
	docWidget->m_editor->rehighlight();
	docWidget->updatePanes();
}
void MainWindow::onDiffViewChanged() {
	qDebug() << "MainWindow::onDiffViewChanged()";
    if (m_processing) return;
    do_diff();
}
void MainWindow::do_diff() {
	if( m_processing ) return;
	DocWidget *docWidget = getCurDocWidget();
	if (docWidget == nullptr || !docWidget->m_diffMode)
		return;
	m_processing = true;
	QTextDocument *doc1 = docWidget->m_editor->document();
	QTextDocument *doc2 = docWidget->m_diffview->document();
	if( docWidget->m_editor->dummyInserted() )
		removeAllDummyLines(doc1);
	if( docWidget->m_diffview->dummyInserted() )
		removeAllDummyLines(doc2);
	std::vector<QString> lines1 = extractLinesFromDocument(doc1);
    std::vector<QString> lines2 = extractLinesFromDocument(doc2);

    dtl::Diff<QString, std::vector<QString>> d(lines1, lines2);
    d.compose();

    do_output("\n");
    QTextBlock block1 = doc1->begin();
    QTextBlock block2 = doc2->begin();
    QTextCursor cur1 = docWidget->m_editor->textCursor();
    QTextCursor cur2 = docWidget->m_diffview->textCursor();
    int ln1 = 0, ln2 = 0;
    auto ses = d.getSes().getSequence();
    int diffLn1 = INT_MAX, diffLn2 = INT_MAX;
    int nDelete = 0, nAdd = 0;
    auto flushPending = [&](int endLn1, int endLn2) {
        if (nDelete == 0 && nAdd == 0) return;
        if (nAdd == 0) {        // 右側（doc2）で削除された場合のみ
            for (int ln = diffLn1; ln < endLn1; ++ln) {
                do_output(QString("- %1 0 '%2'\n").arg(ln).arg(lines1[ln-1]));
	        	setPhysicalLine(block1, ++ln1, true);
	        	block1 = block1.next();
	        	cur2.setPosition(block2.position()); 
                cur2.insertText("\n");
                setDummyLine(block2);   // 空になった現在のブロック（行）をダミーに設定
                block2 = block2.next();  // 下に押し出されたテキストが入っているブロックへ進む
            }
        } else if (nDelete == 0) { // 右側（doc2）で新しく追加された場合のみ
            for (int ln = diffLn2; ln < endLn2; ++ln) {
                if (ln - 1 >= lines2.size()) break;
                do_output(QString("+ 0 %1 '%2'\n").arg(ln).arg(lines2[ln-1]));
	        	setPhysicalLine(block2, ++ln2, true);
	        	block2 = block2.next();
	        	cur1.setPosition(block1.position());
                cur1.insertText("\n");
                setDummyLine(block1);   // 空になった現在のブロック（行）をダミーに設定
                block1 = block1.next();  // 下に押し出されたテキストが入っているブロックへ進む
            }
        } else {
            for (int ln = diffLn1; ln < endLn1; ++ln) {
                do_output(QString("! %1 0 '%2'\n").arg(ln).arg(lines1[ln-1]));
	        	setPhysicalLine(block1, ++ln1, true);
	        	block1 = block1.next();
            }
            for (int ln = diffLn2; ln < endLn2; ++ln) {
                do_output(QString("! 0 %1 '%2'\n").arg(ln).arg(lines2[ln-1]));
	        	setPhysicalLine(block2, ++ln2, true);
	        	block2 = block2.next();
            }
            int d = (endLn1 - diffLn1) - (endLn2 - diffLn2);
            if( d > 0 ) {
            	for(int i = 0; i < d; ++i) {
		        	cur2.setPosition(block2.position());
                    cur2.insertText("\n");
                    setDummyLine(block2);
                    block2 = block2.next();
            	}
            } else if( d < 0 ) {
            	for(int i = 0; i < -d; ++i) {
		        	cur1.setPosition(block1.position());
                    cur1.insertText("\n");
                    setDummyLine(block1);
                    block1 = block1.next();
            	}
            }
        }
        nDelete = nAdd = 0;
        diffLn1 = diffLn2 = INT_MAX;
    };
    docWidget->m_editor->setTextCursor(cur1);
    docWidget->m_diffview->setTextCursor(cur2);
    for (const auto& item : ses) {
        const QString& line = item.first;
        dtl::elemInfo info = item.second;
        switch (info.type) {
        case dtl::SES_COMMON:
       		flushPending(info.beforeIdx, info.afterIdx);
        	do_output(QString("= %1 %2 '%3'\n").arg(info.beforeIdx).arg(info.afterIdx).arg(line));
        	setPhysicalLine(block1, ++ln1, false);
        	block1 = block1.next();
        	setPhysicalLine(block2, ++ln2, false);
        	block2 = block2.next();
        	break;
        case dtl::SES_DELETE:	//	右側（doc2）で削除された行
        	diffLn1 = qMin(diffLn1, info.beforeIdx);
        	nDelete += 1;
        	break;
        case dtl::SES_ADD:		// 右側（doc2）で新しく追加された行
        	diffLn2 = qMin(diffLn2, info.afterIdx);
        	nAdd += 1;
        	break;
        }
    }
    flushPending(doc1->blockCount() + 1, doc2->blockCount() + 1);
    docWidget->m_editor->setDummyInserted(true);
    docWidget->m_diffview->setDummyInserted(true);
    //
	if( docWidget->m_mmPixmap != nullptr )
		delete docWidget->m_mmPixmap;
	//##docWidget->m_mmPixmap = new QPixmap(MINMAP_WIDTH, doc1->blockCount());
	//##docWidget->m_minimap->addChild(docWidget->m_mmPixmap);
	//
	m_processing = false;
}
