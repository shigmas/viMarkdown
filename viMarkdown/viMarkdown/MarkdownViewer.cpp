#include <vector>
#include <QTextDocument>
#include <QTextBlock>
#include <QRegularExpression>
#include "MarkdownViewer.h"
#include "MainWindow.h"

using namespace std;

enum {
	ALIGHN_LEFT = 1, ALIGHN_RIGHT = 2, ALIGHN_CENTER = ALIGHN_LEFT| ALIGHN_RIGHT,
};

MarkdownViewer::MarkdownViewer(const MainWindow *mainWindow, QWidget* parent)
	: m_mainWindow(mainWindow), QTextEdit(parent)
{
	setFrameStyle(QFrame::NoFrame);
}

bool isUnderlineHeading(const QString& txt);
bool MarkdownViewer::isTableLine(const QString& lnStr) {
	QStringView sv(lnStr);
	m_tableTokens.clear();
	int ix = 0;
	while( ix < sv.size() && sv[ix] == u' ' ) ++ix;		//	空白スキップ
	if (ix < sv.size() && sv[ix] == u'|') ++ix;			//	先頭の '|' スキップ
	while( ix < sv.size() ) {
		int ix0 = ix;
		while( ix < sv.size() && sv[ix] != u'|' ) {		//	'|' までループ
			if( ix+1 < sv.size() && sv[ix] == u'\\' ) ix += 2;
			else ++ix;
		}
		m_tableTokens.push_back(sv.mid(ix0, ix-ix0));
		++ix;			//	'|' スキップ
	}
	return m_tableTokens.size() > 1;
}
bool MarkdownViewer::isTableHyphenLine(const QString& lnStr) {
	m_tableAlign.clear();
	QStringView sv(lnStr);
	int ix = 0;
	while( ix < sv.size() && sv[ix] == u' ' ) ++ix;		//	空白スキップ
	if (ix < sv.size() && sv[ix] == u'|') ++ix;			//	先頭の '|' スキップ
	while( ix < sv.size() ) {
		char aln = 0;
		while( ix < sv.size() && sv[ix] == u' ' ) ++ix;		//	空白スキップ
		if( ix < sv.size() && sv[ix] == u':' ) aln = ALIGHN_LEFT;
		while( ix < sv.size() && sv[ix] != u'|' ) {		//	次の'|' までループ
			if( ix+1 < sv.size() && sv[ix] == u'\\' ) ix += 2;
			else ++ix;
		}
		int i = ix - 1;
		while( i >= 0 && sv[i] == u' ' ) --i;		//	空白スキップ
		if( i >= 0 && sv[i] == u':' )
			aln |= ALIGHN_RIGHT;
		m_tableAlign.push_back(aln);
		++ix;
	}
	return m_tableAlign.size() > 1;
}

void MarkdownViewer::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton) {
	    QTextCursor cursor = cursorForPosition(e->pos());
	    //int blockNumber = cursor.blockNumber();
	    emit lineClicked(cursor.block().userState());
	}
    QTextEdit::mouseReleaseEvent(e);
}
void MarkdownViewer::do_body(QTextCursor& cursor) {
	if( m_bodyList.isEmpty() ) return;
	m_hasBody = false;
	QString buf;
	for(auto txt: m_bodyList) {
		if( txt.isEmpty() ) {		//	空行の場合
			++m_nEmptyLines;
			cursor.insertMarkdown(buf);
			QTextBlockFormat blockFormat;
			blockFormat.setAlignment(Qt::AlignJustify); // 左右両端揃え
			cursor.mergeBlockFormat(blockFormat);
			cursor.insertBlock();
			buf.clear();
		} else {
			buf += txt + "\n";
			m_hasBody = true;
			m_nEmptyLines = 0;
		}
	}
	if( !buf.isEmpty() ) {
		cursor.insertMarkdown(buf);
		QTextBlockFormat blockFormat;
		blockFormat.setAlignment(Qt::AlignJustify); // 左右両端揃え
		cursor.mergeBlockFormat(blockFormat);
		cursor.insertBlock();
		m_hasBody = true;
		m_nEmptyLines = 0;
	}
	m_bodyList.clear();
}

static QRegularExpression re_list(R"(^ *[-\*+] )");

void MarkdownViewer::setMarkdown(QTextDocument *doc) {
	m_headingList.clear();
	m_headingLineNum.clear();
	m_headingBlockNum.clear();
	QString mdtext = doc->toPlainText();
	QList<QStringView> tableTokens;
	vector<char> tableAlign;
#if 1
	this->clear();
    QTextFrame *root = this->document()->rootFrame();
    QTextFrameFormat rformat = root->frameFormat();
    rformat.setTopMargin(9);
    rformat.setBottomMargin(9);
    rformat.setLeftMargin(9);
    rformat.setRightMargin(9);
    root->setFrameFormat(rformat);
    m_bodyList.clear();
	QTextCursor cursor(this->document());
	cursor.movePosition(QTextCursor::Start);
	m_lst = mdtext.split(u'\n');
	m_nEmptyLines = 0;
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
		} else if( buf.startsWith("```") ) {
			do_body(cursor);
			do_code(cursor);
		} else if( isTableLine(buf) && m_ln + 1 < m_lst.size() && isTableHyphenLine(m_lst[m_ln+1]) ) {
			do_table(cursor);
		} else {
			if( buf.isEmpty() ) {		//	空行
				m_bodyList += buf;
				//if( m_bodyList.isEmpty() ) {
				//	cursor.insertBlock();
				//} else
				//	m_bodyList += "  ";	//	強制改行
				//do_body(cursor);
				//cursor.insertMarkdown("\n");
			} else {
				if( isUnderlineHeading(buf) ) {
					if( do_underlineHeading(cursor, buf) ) continue;	//	アンダーライン見出しだった場合
				}
				m_bodyList += buf;
				//if( buf.endsWith("  ") )
				//	m_bodyList += buf + u'\n';
				//else
				//	m_bodyList += buf + u' ';
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
void MarkdownViewer::do_table(QTextCursor& cursor) {
	QString buf = m_lst[m_ln] + "\n" + m_lst[m_ln+1] + "\n";
	m_ln += 2;
	while( m_ln < m_lst.size() && isTableLine(m_lst[m_ln]) ) {
		buf += m_lst[m_ln++] + "\n";
	}
	cursor.insertMarkdown(buf);
	cursor.insertBlock();
	--m_ln;
}
bool MarkdownViewer::do_underlineHeading(QTextCursor& cursor, QString buf) {
	if( m_bodyList.isEmpty() || m_bodyList.back() == "" ) return false;
	int h = 3;
	if( buf[0] == u'=' ) {
		if( cursor.blockNumber() == 0 )
			h = 1;
		else
			h = 2;
	}
	buf = m_bodyList.back();
	m_bodyList.pop_back();
	do_heading_sub(cursor, buf, h, m_ln-1);
	return true;
}
int h_font_size[] = {12, 26*4, 22*4, 18, 16, 14, 12};		//	body, h1, h2, h3 ... h6

void MarkdownViewer::do_heading(QTextCursor& cursor, QString buf) {
	int i = 1;
	while( i < buf.size() && buf[i] == '#' ) ++i;
	int h = std::min(6, i);		//	[1, 6]
	while( i < buf.size() && buf[i] == ' ' ) ++i;
	do_heading_sub(cursor, buf.mid(i - 1), h, m_ln);
}
void MarkdownViewer::do_heading_sub(QTextCursor& cursor, QString buf, int h, int ln) {
	//## このロジックがあると、見出し＋空行＋見出し で空行が２つ生成されてしまう
	//## だが、無いと 本文＋空行＋見出し の時、空行が生成されない
	//##if( m_nEmptyLines >= 1 )
	//##	cursor.insertBlock();			//	新規ブロック
	//cursor.insertBlock();			//	新規ブロック
	cursor.insertMarkdown(QString(h, u'#') + u' ' + buf + "\n");
	if( h == 1 ) {
		QTextBlockFormat blockFormat;
		blockFormat.setAlignment(Qt::AlignCenter);
		cursor.mergeBlockFormat(blockFormat);
	}
#if 0		//	うまくいかない
	QTextCharFormat charFormat;
	charFormat.setFontPointSize(h_font_size[h]);
	cursor.select(QTextCursor::BlockUnderCursor);
	cursor.mergeCharFormat(charFormat);
	cursor.clearSelection();
	this->setTextCursor(cursor);
#endif

	cursor.setCharFormat(QTextCharFormat());
	m_headingBlockNum.push_back(cursor.blockNumber());
	cursor.insertBlock();			//	新規ブロック
	QTextBlockFormat blockFormat0;
	blockFormat0.setHeadingLevel(0); // 見出し設定を解除
	cursor.setBlockFormat(blockFormat0);
	// 4. 文字書式のリセット（フォントサイズや太字設定を解除）
	cursor.setCharFormat(QTextCharFormat()); // 空のフォーマットをセットすることでデフォルトに戻る

	m_headingList.push_back(QChar(u'0'+h) + buf.remove("^ +"));
	m_headingLineNum.push_back(ln);
	m_nEmptyLines = 0;
}
void MarkdownViewer::do_code(QTextCursor& cursor) {
	QString buf;
	while( ++m_ln < m_lst.size() && !m_lst[m_ln].startsWith("```") ) {
		if( !buf.isEmpty() ) buf += "\n";
		buf += m_lst[m_ln];
	}
	// 1. フレームの書式を設定
	QTextFrameFormat frameFormat;
	frameFormat.setBackground(QColor("#fffff0")); // 薄い黄色
	frameFormat.setMargin(0);                     // 外側の余白
	frameFormat.setPadding(10);                   // 内側の余白（文字と端の隙間）
	frameFormat.setBorder(0);                     // 枠線はなし
	// 2. フレームを挿入（この中にカーソルが入る）
	cursor.insertFrame(frameFormat);

	QTextCharFormat codeCharFormat;
#if 0
    codeCharFormat.setFontFamilies({"MS Gothic", "MS UI Gothic", "Osaka-mono", "monospace"});
    codeCharFormat.setFontFixedPitch(true); // 等幅フォントであることを明示
    codeCharFormat.setFontPointSize(10);    // 必要に応じてサイズを調整
#else
    QFont font;
	// 2. フォントの基本属性を設定
	font.setFamilies({"MS Gothic", "MS UI Gothic", "Osaka-mono", "monospace"});
	font.setFixedPitch(true);
	font.setPointSizeF(10); // 精密な計算のために setPointSize より setPointSizeF がおすすめ
	// 3. レタースペーシング（文字間隔）を設定
	// 100% よりわずかに小さくすることで、PDF出力時の隙間を埋めます
	font.setLetterSpacing(QFont::PercentageSpacing, 99.5);
	// 4. 設定済みの QFont を QTextCharFormat に適用
	codeCharFormat.setFont(font);
#endif
	// 3. フレームの中で Markdown を挿入
	cursor.insertText(buf, codeCharFormat);
	cursor.movePosition(QTextCursor::End);
	this->setTextCursor(cursor);
	cursor.insertBlock();
	QTextBlockFormat blockFormat;
	cursor.setBlockFormat(blockFormat);
	//--m_ln;
	m_nEmptyLines = 0;
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
	m_nEmptyLines = 0;
}
void MarkdownViewer::do_list(QTextCursor& cursor, QString buf) {
	if( m_nEmptyLines >= 1 )
		cursor.insertBlock();			//	新規ブロック
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
	m_nEmptyLines = 0;
}
