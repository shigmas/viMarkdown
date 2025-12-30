#include <QTextDocument>
#include <QTextBlock>
#include <QRegularExpression>
#include "MarkdownViewer.h"

void MarkdownViewer::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton) {
	    // 1. クリックされた位置のカーソルオブジェクトを取得
	    // viewport()->mapFromGlobal(e->globalPos()) ではなく e->pos() でOKです
	    QTextCursor cursor = cursorForPosition(e->pos());

	    // 2. その位置のブロック番号を取得（0ベース）
	    int blockNumber = cursor.blockNumber();

	    // 3. シグナルを発行
	    emit lineClicked(blockNumber);
	}
    QTextEdit::mousePressEvent(e);
}
void MarkdownViewer::do_body(QTextCursor& cursor) {
	if( m_bodyText.isEmpty() ) return;
	QTextBlockFormat blockFormat;
	blockFormat.setAlignment(Qt::AlignJustify); // 左右両端揃え
	cursor.mergeBlockFormat(blockFormat);
	cursor.insertMarkdown(m_bodyText);
	cursor.insertBlock();
	m_bodyText.clear();
}
void MarkdownViewer::setMarkdown(QTextDocument *doc) {
	QString mdtext = doc->toPlainText();
#if 1
	this->clear();
    QTextFrame *root = this->document()->rootFrame();
    QTextFrameFormat rformat = root->frameFormat();
    rformat.setTopMargin(9);
    rformat.setBottomMargin(9);
    rformat.setLeftMargin(9);
    rformat.setRightMargin(9);
    root->setFrameFormat(rformat);
#if 1
    m_bodyText.clear();
	QTextCursor cursor(this->document());
	cursor.movePosition(QTextCursor::Start);
	m_lst = mdtext.split(u'\n');
	for(m_ln = 0; m_ln < m_lst.size(); ++m_ln) {
		QString buf = m_lst[m_ln];
		m_nSpaces = 0;
		while( m_nSpaces < buf.size() && buf[m_nSpaces] == u' ' ) ++m_nSpaces;
		if( m_nSpaces > 0 ) buf = buf.mid(m_nSpaces);
		if( buf.startsWith('#') ) {
			do_body(cursor);
			do_heading(cursor, buf);
		} else if( buf.startsWith("- ") ) {
			do_body(cursor);
			do_list(cursor, buf);
		} else {
			if( buf.isEmpty() ) {		//	空行
				m_bodyText += u'\n';
				do_body(cursor);
				//cursor.insertMarkdown("\n");
			} else {
				if( buf.endsWith("  ") )
					m_bodyText += buf + u'\n';
				else
					m_bodyText += buf + u' ';
			}
		}
	}
	do_body(cursor);
#else
    bool inList = false;
	QTextCursor cursor(this->document());
	cursor.movePosition(QTextCursor::Start);
	QStringList lst = mdtext.split(u'\n');
	for(int ln = 0; ln < lst.size(); ++ln) {
		QString buf = lst[ln];
		m_nSpaces = 0;
		while( m_nSpaces < buf.size() && buf[m_nSpaces] == u' ' ) ++m_nSpaces;
		if( m_nSpaces > 0 ) buf = buf.mid(m_nSpaces);
		if( buf.startsWith("- ") ) {
			do_list(cursor, buf);
			inList = true;
		} else {
			if( inList ) {
				cursor.insertBlock();
				inList = false;
			}
			QTextBlockFormat plainBlockFormat; 
		    cursor.setBlockFormat(plainBlockFormat);
			if( buf.startsWith('#') ) {
				do_heading(cursor, buf);
			} else {
				cursor.insertText(buf);
				QTextBlockFormat format;
				format.setAlignment(Qt::AlignJustify);
				format.setIndent(0);
				cursor.mergeBlockFormat(format);
				QTextCharFormat charFormat;
				charFormat.setFontPointSize(12);
			    charFormat.setFontWeight(QFont::Normal);
			    cursor.select(QTextCursor::BlockUnderCursor);
			    cursor.mergeCharFormat(charFormat);
			    cursor.clearSelection();
			}
			cursor.insertBlock();
		}
	}
	qDebug() << "blockCount() = " << this->document()->blockCount();
#endif
#else
	document()->setMarkdown(mdtext);
	for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        
        QTextBlockFormat format = block.blockFormat();

        if (format.headingLevel() == 0) {
            format.setAlignment(Qt::AlignJustify);
            QTextCursor cursor(block);
            cursor.setBlockFormat(format);
        } else if(format.headingLevel() == 1) {
            format.setAlignment(Qt::AlignCenter);
            QTextCursor cursor(block);
            cursor.setBlockFormat(format);
        }
    }
#endif
}
int h_font_size[] = {12, 25, 21, 18, 16, 14, 12};

void MarkdownViewer::do_heading(QTextCursor& cursor, QString buf) {
	int i = 1;
	while( i < buf.size() && buf[i] == '#' ) ++i;
	int h = std::min(6, i);		//	[1, 6]
	while( i < buf.size() && buf[i] == ' ' ) ++i;
#if 1
	cursor.insertMarkdown(buf);
	if( h == 1 ) {
		QTextBlockFormat blockFormat;
		blockFormat.setAlignment(Qt::AlignCenter);
		cursor.mergeBlockFormat(blockFormat);
	}
	cursor.insertBlock();			//	新規ブロック
	QTextBlockFormat blockFormat0;
	blockFormat0.setHeadingLevel(0); // 見出し設定を解除
	cursor.setBlockFormat(blockFormat0);
	// 4. 文字書式のリセット（フォントサイズや太字設定を解除）
	cursor.setCharFormat(QTextCharFormat()); // 空のフォーマットをセットすることでデフォルトに戻る
#else
	cursor.insertText(buf.mid(i));
	//QTextBlock block = cursor.block();
	QTextBlockFormat format;
	format.setHeadingLevel(h); // Hx（見出しレベルx）に設定
	if( h == 1 )
		format.setAlignment(Qt::AlignCenter);
	format.setIndent(0);
	cursor.mergeBlockFormat(format);

	QTextCharFormat charFormat;
	charFormat.setFontPointSize(h_font_size[h]);
    charFormat.setFontWeight(QFont::Bold);
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.mergeCharFormat(charFormat);
    cursor.clearSelection();
#endif
}
void MarkdownViewer::do_list(QTextCursor& cursor, QString buf) {
	static QRegularExpression re_checkbox(R"(^( *)- \[[ xX\] )");
	if( re_checkbox.match(buf).hasMatch() ) {
		while( ++m_ln < m_lst.size() ) {
			if( !re_checkbox.match(m_lst[m_ln]).hasMatch() ) break;
			buf += u'\n' + m_lst[m_ln];
		}
	} else {
		static QRegularExpression re(R"(^( *)- )");
		while( ++m_ln < m_lst.size() ) {
			if( !re.match(m_lst[m_ln]).hasMatch() ) break;
			if( re_checkbox.match(m_lst[m_ln]).hasMatch() ) break;
			buf += u'\n' + m_lst[m_ln];
		}
	}
	cursor.insertMarkdown(buf);
	cursor.insertBlock();
	QTextBlockFormat blockFormat;
	cursor.setBlockFormat(blockFormat);
	--m_ln;
#if 0
	//if (!cursor.atBlockStart()) cursor.insertBlock();
	buf = buf.mid(2);		//	remove "- "
	QTextBlockFormat blockFormat;
	blockFormat.setIndent(m_nSpaces/2);
	QTextListFormat listFormat;
	if( buf.startsWith("[ ] ") || buf.startsWith("[x] ") || buf.startsWith("[X] ") ) {
		bool uc = buf[1] == u' ';
	    blockFormat.setMarker(uc ? QTextBlockFormat::MarkerType::Unchecked
	    							: QTextBlockFormat::MarkerType::Checked);
	    cursor.setBlockFormat(blockFormat);	    // 現在のブロック（行）に適用
		buf = buf.mid(3);		//	remove "[ ]"
	} else {
	    listFormat.setStyle(QTextListFormat::ListDisc); // これが中黒 (●)、他にも ListCircle (○), ListSquare (■), ListDecimal (1, 2...)
	}
    cursor.insertList(listFormat);
    cursor.insertText(buf);
	//cursor.insertText("・ " + buf.mid(2));
	QTextCharFormat charFormat;
	charFormat.setFontPointSize(12);
    charFormat.setFontWeight(QFont::Normal);
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.mergeCharFormat(charFormat);
    cursor.clearSelection();

	//QTextBlockFormat format;
	//cursor.mergeBlockFormat(blockformat);
#endif
}
