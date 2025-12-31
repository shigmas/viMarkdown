#include <QTextDocument>
#include <QTextBlock>
#include <QRegularExpression>
#include "MarkdownViewer.h"

void MarkdownViewer::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton) {
	    QTextCursor cursor = cursorForPosition(e->pos());
	    //int blockNumber = cursor.blockNumber();
	    emit lineClicked(cursor.block().userState());
	}
    QTextEdit::mousePressEvent(e);
}
void MarkdownViewer::do_body(QTextCursor& cursor) {
	if( m_bodyText.isEmpty() ) return;
	cursor.insertMarkdown(m_bodyText);
	QTextBlockFormat blockFormat;
	blockFormat.setAlignment(Qt::AlignJustify); // 左右両端揃え
	cursor.mergeBlockFormat(blockFormat);
	cursor.insertBlock();
	m_bodyText.clear();
}

static QRegularExpression re_list(R"(^ *[-\*+] )");

void MarkdownViewer::setMarkdown(QTextDocument *doc) {
	m_headingList.clear();
	m_headingLineNum.clear();
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
		} else if( re_list.match(buf).hasMatch() ) {
			do_body(cursor);
			do_list(cursor, buf);
		} else if( buf.startsWith("> ") ) {
			do_body(cursor);
			do_quote(cursor, buf);
		} else {
			if( buf.isEmpty() ) {		//	空行
				do_body(cursor);
				m_bodyText += u'\n';
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
	cursor.insertMarkdown(buf);
	if( h == 1 ) {
		QTextBlockFormat blockFormat;
		blockFormat.setAlignment(Qt::AlignCenter);
		cursor.mergeBlockFormat(blockFormat);
	}
	cursor.setCharFormat(QTextCharFormat());
	cursor.insertBlock();			//	新規ブロック
	QTextBlockFormat blockFormat0;
	blockFormat0.setHeadingLevel(0); // 見出し設定を解除
	cursor.setBlockFormat(blockFormat0);
	// 4. 文字書式のリセット（フォントサイズや太字設定を解除）
	cursor.setCharFormat(QTextCharFormat()); // 空のフォーマットをセットすることでデフォルトに戻る

	m_headingList.push_back(QChar(u'0'+h) + buf.mid(i-1));
	m_headingLineNum.push_back(m_ln);
}
void MarkdownViewer::do_quote(QTextCursor& cursor, QString buf) {
	buf = buf.mid(2);
	while( ++m_ln < m_lst.size() ) {
		if( !m_lst[m_ln].startsWith("> ") ) break;
		buf += u"  \n" + m_lst[m_ln].mid(2);
	}
	//cursor.insertMarkdown(buf);
	// 1. フレームの書式を設定
	QTextFrameFormat frameFormat;
	frameFormat.setBackground(QColor("#f0f8ff")); // 薄い青
	frameFormat.setMargin(0);                     // 外側の余白
	frameFormat.setPadding(10);                   // 内側の余白（文字と端の隙間）
	frameFormat.setBorder(0);                     // 枠線はなし
	// 2. フレームを挿入（この中にカーソルが入る）
	cursor.insertFrame(frameFormat);
	// 3. フレームの中で Markdown を挿入
	cursor.insertText(buf);
	cursor.movePosition(QTextCursor::End);
	this->setTextCursor(cursor);
	cursor.insertBlock();
	QTextBlockFormat blockFormat;
	cursor.setBlockFormat(blockFormat);
	--m_ln;
}
void MarkdownViewer::do_list(QTextCursor& cursor, QString buf) {
	static QRegularExpression re_checkbox(R"(^( *)- \[[ xX]\] )");
	int pos = cursor.position();
	int n_item = 1;
	int ln = m_ln;
	bool is_checkbox = re_checkbox.match(buf).hasMatch();
	if( is_checkbox ) {
		while( ++m_ln < m_lst.size() ) {
			if( !re_checkbox.match(m_lst[m_ln]).hasMatch() ) break;
			buf += u'\n' + m_lst[m_ln];
			++n_item;
		}
	} else {
		//static QRegularExpression re(R"(^( *)- )");
		while( ++m_ln < m_lst.size() ) {
			if( !re_list.match(m_lst[m_ln]).hasMatch() ) break;
			if( re_checkbox.match(m_lst[m_ln]).hasMatch() ) break;
			buf += u'\n' + m_lst[m_ln];
		}
	}
	int startPos = cursor.position();
	cursor.insertMarkdown(buf);
	QTextBlock firstBlock = document()->findBlock(startPos);
	if (firstBlock.isValid() && firstBlock.text().isEmpty()) {
	    // ブロックが空なら削除する（バックスペース的な処理）
	    QTextCursor helper(firstBlock);
	    helper.deleteChar(); 
	}
	if( is_checkbox ) {
		QTextBlock block = document()->findBlock(pos);
		for(int i = 0; i < n_item; ++i) {
			block.setUserState(ln++);
			block = block.next();
		}
	}
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
