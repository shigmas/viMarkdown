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
void setDummyLine(QTextBlock &block) {
    block.setUserState(0);
}

// 物理的な行をセットする場合
void setPhysicalLine(QTextBlock &block, int ln, bool changed) {
    int state = (ln << 1) | (changed ? 1 : 0);
    block.setUserState(state);
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
void MainWindow::onDiffViewChanged() {
	qDebug() << "MainWindow::onDiffViewChanged()";
	do_diff();
}
void MainWindow::do_diff() {
	DocWidget *docWidget = getCurDocWidget();
	if (docWidget == nullptr || !docWidget->m_diffMode)
		return;
	QTextDocument *doc1 = docWidget->m_editor->document();
	QTextDocument *doc2 = docWidget->m_diffview->document();
	std::vector<QString> lines1 = extractLinesFromDocument(doc1);
    std::vector<QString> lines2 = extractLinesFromDocument(doc2);

    dtl::Diff<QString, std::vector<QString>> d(lines1, lines2);
    d.compose();

    auto ses = d.getSes().getSequence();
    int diffLn1 = INT_MAX, diffLn2 = INT_MAX;
    int nDelete = 0, nAdd = 0;
    auto flushPending = [&](int endLn1, int endLn2) {
        if (nDelete == 0 && nAdd == 0) return;
        if (nAdd == 0) {        // 右側（doc2）で削除された場合のみ
            for (int ln = diffLn1; ln < endLn1; ++ln)
                do_output(QString("- %1 0 '%2'\n").arg(ln).arg(lines1[ln-1]));
        } else if (nDelete == 0) { // 右側（doc2）で新しく追加された場合のみ
            for (int ln = diffLn2; ln < endLn2; ++ln)
                do_output(QString("+ 0 %1 '%2'\n").arg(ln).arg(lines2[ln-1]));
        } else {
            for (int ln = diffLn1; ln < endLn1; ++ln)
                do_output(QString("! %1 0 '%2'\n").arg(ln).arg(lines1[ln-1]));
            for (int ln = diffLn2; ln < endLn2; ++ln)
                do_output(QString("! 0 %1 '%2'\n").arg(ln).arg(lines2[ln-1]));
        }
        nDelete = nAdd = 0;
        diffLn1 = diffLn2 = INT_MAX;
    };
    for (const auto& item : ses) {
        const QString& line = item.first;
        dtl::elemInfo info = item.second;
        switch (info.type) {
        case dtl::SES_COMMON:
       		flushPending(info.beforeIdx, info.afterIdx);
        	do_output(QString("= %1 %2 '%3'\n").arg(info.beforeIdx).arg(info.afterIdx).arg(line));
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
 		flushPending(doc1->blockCount() + 1, doc2->blockCount() + 1);
    }
}
