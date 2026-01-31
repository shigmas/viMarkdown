#include <vector>
#include <QTextDocument>
#include <QTextBlock>
#include <QRegularExpression>
#include <QTextTable>
#include <QAbstractTextDocumentLayout>
#include <QScrollbar>
#include <QDir>
#include <qpainter.h>
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
	//QString css = "hr { height: 1px; border: none; background-color: #333; margin-top: 10px; margin-bottom: 10px; }";
	//document()->setDefaultStyleSheet(css);
	connect(this, &MarkdownViewer::cursorPositionChanged, this, &MarkdownViewer::onCurPosChanged);
	connect(document(), &QTextDocument::contentsChange, this, &MarkdownViewer::onContentsChanged);
}
void MarkdownViewer::onCurPosChanged() {
	viewport()->update();	//	強制再描画
}
void MarkdownViewer::onContentsChanged(int position, int charsRemoved, int charsAdded) {
	if( m_processing ) return;
	if( charsAdded > 0 ) {
		m_processing = true;
		QTextCursor cursor = this->textCursor();
		cursor.setPosition(position);
		QTextBlock block = cursor.block();
		QString addedStr = block.text().mid(cursor.position() - block.position(), charsAdded);
		qDebug() << "addedStr = " << addedStr;
		emit textInserted(addedStr);
		m_processing = false;
	}
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

void MarkdownViewer::mouseMoveEvent(QMouseEvent *me) {
	QString anchor = anchorAt(me->pos());
	if (!anchor.isEmpty()) {	// リンクの上なら指差し
        viewport()->setCursor(Qt::PointingHandCursor);
		if( !anchor.endsWith(".md", Qt::CaseInsensitive) )
			anchor += ".md";
		QString fullPath = QDir::current().absoluteFilePath(anchor);
        m_mainWindow->statusBar()->showMessage(fullPath);
    } else {	// それ以外なら通常（I型または矢印）
        viewport()->setCursor(Qt::IBeamCursor);
    }
}
void MarkdownViewer::mouseReleaseEvent(QMouseEvent *me)
{
	if (me->button() == Qt::LeftButton) {
		QString anchor = anchorAt(me->pos());
		if (!anchor.isEmpty()) {	// リンク上
			//qDebug() << "anchor = " << anchor;
			if( !anchor.endsWith(".md", Qt::CaseInsensitive) )
				anchor += ".md";
			QString fullPath = QDir::current().absoluteFilePath(anchor);
			emit anchorClicked(fullPath);
		} else {
		    QTextCursor cursor = cursorForPosition(me->pos());
		    emit lineClicked(cursor.block().userState());
		}
	}
    QTextEdit::mouseReleaseEvent(me);
}
void MarkdownViewer::paintEvent(QPaintEvent *e) {
	QTextEdit::paintEvent(e); // 先にテキストを普通に描画
	QPainter p(viewport());
	//	行カーソル描画
	QRect rect = cursorRect();
    QPen pen(hasFocus() ? Qt::red : Qt::gray, 1); // 赤色、太さ1px
    p.setPen(pen);
    int y = rect.bottom();
    int left = 0;	//-lnAreaWidth();
    int right = viewport()->width();
    p.drawLine(left, y, right, y);
}
void MarkdownViewer::do_body_sub(QTextCursor& cursor, const QString &buf) {
#if 1
	int startPos = cursor.position(); // 挿入前の位置
	cursor.insertMarkdown(buf);
	int endPos = cursor.position();   // 挿入後の位置
	QTextCursor selectCursor = cursor;
	selectCursor.setPosition(startPos);
	selectCursor.setPosition(endPos, QTextCursor::KeepAnchor);
	QTextBlockFormat blockFormat;
	blockFormat.setAlignment(Qt::AlignJustify);
	selectCursor.mergeBlockFormat(blockFormat);
#else
	cursor.insertMarkdown(buf);
	//cursor.insertBlock();
	QTextBlockFormat blockFormat;
	blockFormat.setAlignment(Qt::AlignJustify); // 左右両端揃え
	cursor.mergeBlockFormat(blockFormat);
	//cursor.insertBlock();
	//cursor.insertMarkdown(buf);
#endif
}
void MarkdownViewer::do_body(QTextCursor& cursor) {
	if( m_bodyList.isEmpty() ) return;
#if 1
	static QRegularExpression re("\\[\\[(.+?)\\]\\]");		//	[[タイトル]]
	QString buf;
	for(auto txt: m_bodyList) {
		txt.replace(re, "<a href=\"\\1\" class=\"wiki-link\">\\1</a>");
		buf += txt + "\n";
	}
	if( !buf.isEmpty() ) {
		QTextBlockFormat blockFormat;
		blockFormat.setAlignment(Qt::AlignJustify); // 左右両端揃え
		cursor.mergeBlockFormat(blockFormat);
		cursor.insertMarkdown(buf);
	}
	m_bodyList.clear();
#else
	m_hasBody = false;
	QString buf;
	for(auto txt: m_bodyList) {
		if( txt.isEmpty() ) {		//	空行の場合
			++m_nEmptyLines;
			do_body_sub(cursor, buf);
			buf.clear();
		} else {
			buf += txt + "\n";
			m_hasBody = true;
			m_nEmptyLines = 0;
		}
	}
	if( !buf.isEmpty() ) {
		do_body_sub(cursor, buf);
		m_hasBody = true;
		m_nEmptyLines = 0;
	}
	m_bodyList.clear();
#endif
}

static QRegularExpression re_list(R"(^ *[-\*+] )");

int indexOfComment(QStringView buf, int start) {
	for(int i = start; i < buf.size(); ++i) {
		if( buf[i] == u'\\' && i+1 < buf.size() )
			++i;
		else if( buf.mid(i).startsWith(u"<!--") )
			return i;
	}
	return -1;
}

void MarkdownViewer::setMarkdown(QTextDocument *doc) {
	m_headingList.clear();
	m_headingSrcLineNum.clear();
	m_headingBlockNum.clear();
	QString mdtext = doc->toPlainText();
	QList<QStringView> tableTokens;
	vector<char> tableAlign;

	m_processing = true;
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
	cursor.beginEditBlock();
	cursor.movePosition(QTextCursor::Start);
	m_lst = mdtext.split(u'\n');
	m_nEmptyLines = 0;
	m_inComment = false;
	for(m_ln = 0; m_ln < m_lst.size(); ++m_ln) {
		bool bComment = false;		//	コメントがあった
		QString buf = m_lst[m_ln];
		if( !m_inComment ) {
			int start = 0, ix;
			while( (ix = indexOfComment(buf, start)) >= 0 ) {
				bComment = true;
				int ix2 = buf.indexOf("-->", ix + 4);
				if( ix2 < 0 ) {
					buf = buf.left(ix);
					m_inComment = true;
					break;
				}
				buf = buf.left(ix) + buf.mid(ix2+3);
			}
			if( bComment && buf.isEmpty() ) continue;
		} else {
			int ix = buf.indexOf("-->");
			if( ix < 0 ) continue;
			m_inComment = false;
			buf = buf.mid(ix+3);
			if( buf.isEmpty() ) continue;
		}
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
		} else if( buf.startsWith("```CSV", Qt::CaseInsensitive) ) {
			do_body(cursor);
			do_CSV(cursor);
		} else if( buf.startsWith("```keisen", Qt::CaseInsensitive) ) {
			do_body(cursor);
			do_code_keisen(cursor);
		} else if( buf.startsWith("```") ) {
			do_body(cursor);
			do_code(cursor);
		} else if( isTableLine(buf) && m_ln + 1 < m_lst.size() && isTableHyphenLine(m_lst[m_ln+1]) ) {
			do_body(cursor);
			do_table(cursor);
		} else {
			if( isUnderlineHeading(buf) && do_underlineHeading(cursor, buf) )
				continue;		//	アンダーライン見出しだった場合
			m_bodyList += buf;
		}
	}
	do_body(cursor);
	cursor.endEditBlock();
	m_processing = false;
}
void MarkdownViewer::do_table(QTextCursor& cursor) {
	QString buf = m_lst[m_ln] + "\n" + m_lst[m_ln+1] /*+ "\n"*/;
	m_ln += 2;
	while( m_ln < m_lst.size() && isTableLine(m_lst[m_ln]) ) {
		buf += "\n" + m_lst[m_ln++];
	}
	cursor.insertMarkdown(buf);
	cursor.movePosition(QTextCursor::Left);
	QTextTable *table = cursor.currentTable();
	if (table) {
		QColor headerBgColor(230, 240, 255); // #E6F0FF くらいの淡い青
	    // 1行目（ヘッダ行）のすべての列をループ
	    for (int col = 0; col < table->columns(); ++col) {
	        QTextTableCell cell = table->cellAt(0, col);
	        // --- 背景色の設定 ---
	        QTextCharFormat cellFormat = cell.format();
	        cellFormat.setBackground(headerBgColor);
	        cell.setFormat(cellFormat);
	        // --- センタリングの設定  ---
	        QTextCursor cellCursor = cell.firstCursorPosition();
	        QTextBlockFormat blockFormat = cellCursor.blockFormat();
	        blockFormat.setAlignment(Qt::AlignCenter);
	        cellCursor.setBlockFormat(blockFormat);
	    }
		QColor cellBgColor(255, 255, 224);
	    for (int row = 2; row < table->rows(); row+=2) {
		    for (int col = 0; col < table->columns(); ++col) {
		        QTextTableCell cell = table->cellAt(row, col);
		        QTextCharFormat cellFormat = cell.format();
		        cellFormat.setBackground(cellBgColor);
		        cell.setFormat(cellFormat);
		    }
	    }
	}
	cursor.movePosition(QTextCursor::Right);
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
	while( i < buf.size() && buf[i] == u' ' ) ++i;
	do_heading_sub(cursor, buf.mid(i), h, m_ln);
}
void MarkdownViewer::do_heading_sub(QTextCursor& cursor, QString buf, int h, int ln) {
	if( !cursor.atBlockStart() )
		cursor.insertBlock();			//	新規ブロック
	cursor.block().setUserState(US_HEADING);
	cursor.insertMarkdown(QString(h, u'#') + u' ' + buf /*+ "\n"*/);		//	改行を付加すると、２行になってしまう
	QTextBlockFormat blockFormat;
	if( h == 1 ) {		//	H1 の場合はセンタリング（viMarkdown 独自？仕様）
		blockFormat.setAlignment(Qt::AlignCenter);
	} else {
		blockFormat.setTopMargin(12);    // 上側の余白（ピクセル）
		blockFormat.setBottomMargin(12); // 下側の余白（ピクセル）
	}
	cursor.mergeBlockFormat(blockFormat);
	m_headingBlockNum.push_back(cursor.blockNumber());

	cursor.insertBlock(QTextBlockFormat(), QTextCharFormat());		//	新規ブロック

	m_headingList.push_back(QChar(u'0'+h) + buf.remove("^ +"));
	m_headingSrcLineNum.push_back(ln);
	m_nEmptyLines = 0;
}
bool parseCsvLine(QStringList &fields, const QString &line, bool inQuotes) {
    //QStringList fields;
    QString currentField;
	if( !inQuotes ) {
		fields.clear();
	} else {
		currentField = fields.back() + u'\n';
		fields.pop_back();
	}
    //bool inQuotes = false;
    for (int i = 0; i < line.length(); ++i) {
        QChar c = line.at(i);
        if (c == '"') {
            // ダブルクォートの中のダブルクォート（エスケープ）をチェック
            if (inQuotes && i + 1 < line.length() && line.at(i + 1) == '"') {
                currentField += '"';
                i++; // 1文字飛ばす
            } else {
                // クォート状態の反転
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            // クォートの外にあるカンマはセパレータ
            fields.append(currentField.trimmed());
            currentField.clear();
        } else {
            // 通常の文字
            currentField += c;
        }
    }
    // 最後のフィールドを追加
    fields.append(currentField.trimmed());
    return inQuotes;
}
void MarkdownViewer::do_CSV(QTextCursor& cursor) {
	QList<QStringList> ll;
	int max_clmn = 0;
    bool inQuotes = false;
    QStringList fields;
	while( ++m_ln < m_lst.size() && !m_lst[m_ln].startsWith("```") ) {
		inQuotes = parseCsvLine(fields, m_lst[m_ln], inQuotes);
		if( !inQuotes ) {
			max_clmn = qMax(max_clmn, fields.size());
			ll.push_back(fields);
		}
	}
	if( ll.isEmpty() ) return;
	static QRegularExpression re("^[+-]?(\\d+\\.\\d*|\\d+|\\.\\d+)$");
	cursor.beginEditBlock();
	QTextTable *table = cursor.insertTable(ll.size(), max_clmn);
	for(int row = 0; row < ll.size(); ++row) {
		for(int col = 0; col < ll[row].size(); ++col) {
			QTextTableCell cell = table->cellAt(row, col);
			if (cell.isValid()) {
			    QTextCursor cellCursor = cell.firstCursorPosition();
			    QTextCharFormat charFormat;
			    QTextBlockFormat blockFormat;
				if (row == 0) {
					QTextTableCellFormat cellFormat;
				    cellFormat.setBackground(QColor("lightblue"));
				    cell.setFormat(cellFormat);
					charFormat.setFontWeight(QFont::Bold);
				    blockFormat.setAlignment(Qt::AlignCenter); // ヘッダは中央
				} else {
					if( (row % 2) == 0 ) {
						QTextTableCellFormat cellFormat;
					    cellFormat.setBackground(QColor("lightyellow"));
					    cell.setFormat(cellFormat);
					}
					charFormat.setFontWeight(QFont::Normal);
					if (re.match(ll[row][col]).hasMatch()) {
					    blockFormat.setAlignment(Qt::AlignRight);  // 数値は右
					} else {
					    blockFormat.setAlignment(Qt::AlignLeft);   // その他は左
					}
				}
				cellCursor.setCharFormat(charFormat);
				cellCursor.setBlockFormat(blockFormat);
			    cellCursor.insertText(ll[row][col]);
			}
		}
	}
	cursor.setPosition(table->lastPosition());
	cursor.movePosition(QTextCursor::NextCharacter);
	cursor.insertBlock();
	cursor.endEditBlock();
	//++m_ln;
}
void MarkdownViewer::do_code_keisen(QTextCursor& cursor) {
    QFont font;
    QStringView buf = m_lst[m_ln].mid(QString("```keisen").size());
    QColor bgcolor("#c0f0c0");
    QColor color("black");
    for(;;) {
	    while( !buf.isEmpty() && buf[0] == u' ' ) buf = buf.mid(1);		//	skip u' ';
	    if( buf.startsWith(u"background-color:", Qt::CaseInsensitive) ) {
	    	buf = buf.mid(QString(u"background-color:").size());
		    while( !buf.isEmpty() && buf[0] == u' ' ) buf = buf.mid(1);		//	skip u' ';
		    int ix = buf.indexOf(u';');
		    if( ix < 0 ) break;
		    bgcolor = QColor(buf.left(ix));
		    buf = buf.mid(ix+1);
	    } else if( buf.startsWith(u"color:", Qt::CaseInsensitive) ) {
	    	buf = buf.mid(QString(u"color:").size());
		    while( !buf.isEmpty() && buf[0] == u' ' ) buf = buf.mid(1);		//	skip u' ';
		    int ix = buf.indexOf(u';');
		    if( ix < 0 ) break;
		    color = QColor(buf.left(ix));
		    buf = buf.mid(ix+1);
	    } else
	    	break;
    }
	font.setFamilies({"MS Gothic", "MS UI Gothic", "Osaka-mono", "monospace"});
	font.setFixedPitch(true);
	font.setPointSizeF(12);
	QFontMetrics fm(font);
	int width = 100;
	int m_ln0 = m_ln + 1;
	while( ++m_ln < m_lst.size() && !m_lst[m_ln].startsWith("```") ) {
		width = qMax(width, fm.horizontalAdvance(m_lst[m_ln])+10);
	}
	int lineHeight = fm.lineSpacing(); // 行の間隔（高さ＋行間）
	
	int height = lineHeight * (m_ln - m_ln0);
	QImage img(width, height, QImage::Format_RGB32);
    img.fill(bgcolor);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(font);
    painter.setPen(color);
    for(int i = 0; i < m_ln - m_ln0; ++i)
    	painter.drawText(5, lineHeight*i+fm.height(), m_lst[m_ln0+i]);
    painter.end();
    //
    cursor.insertBlock();
    QTextBlockFormat blockFormat;
	blockFormat.setTopMargin(lineHeight/2);
    cursor.mergeBlockFormat(blockFormat);
    cursor.insertImage(img);
    cursor.insertBlock();
    //cursor.setBlockFormat(QTextBlockFormat());		//	トップマージンリセット
}
void MarkdownViewer::do_code(QTextCursor& cursor) {
	QString buf;
	while( ++m_ln < m_lst.size() && !m_lst[m_ln].startsWith("```") ) {
		if( !buf.isEmpty() ) buf += "\n";
		buf += m_lst[m_ln];
	}
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
	// フレームの書式を設定
	QTextFrameFormat frameFormat;
	frameFormat.setBackground(QColor("#fffff0")); // 薄い黄色
	frameFormat.setMargin(0);                     // 外側の余白
	frameFormat.setPadding(10);                   // 内側の余白（文字と端の隙間）
	frameFormat.setBorder(0);                     // 枠線はなし
	// 2. フレームを挿入（この中にカーソルが入る）
	cursor.insertFrame(frameFormat);

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
			//block.setUserState(ln++);
			block.setUserState(US_CHECKBOX);
			block = block.next();
		}
	}
	cursor.insertBlock();
	QTextBlockFormat blockFormat;
	cursor.setBlockFormat(blockFormat);
	--m_ln;
	m_nEmptyLines = 0;
}
void MarkdownViewer::setCursorAt(int srcBlockNum, QString srcText, int ix) {		//	ix: ブロック内カーソル位置
	int i = 0;
	while( i+1 < m_headingSrcLineNum.size() && m_headingSrcLineNum[i+1] <= srcBlockNum ) ++i;
	if( i >= m_headingBlockNum.size() ) return;
	QTextBlock block = document()->findBlockByNumber(m_headingBlockNum[i]);
	QTextCursor cursor = textCursor();
	cursor.setPosition(block.position());
	if( !srcText.isEmpty() ) {
		QString t3 = srcText.mid(ix, 3);
		auto cur = document()->find(t3, cursor);
		if( !cur.isNull()) {
			//cur.clearSelection();
			cur.setPosition(cur.selectionStart(), QTextCursor::MoveAnchor);
			cursor = cur;
		}
	}
	setTextCursor(cursor);
}
void MarkdownViewer::setCursorAtNthPat(int srcBlockNum, QString pat, int nth) {		//	nth: 何番目か（>0）
	int i = 0;
	while( i+1 < m_headingSrcLineNum.size() && m_headingSrcLineNum[i+1] <= srcBlockNum ) ++i;
	if( i >= m_headingBlockNum.size() ) return;
	QTextBlock block = document()->findBlockByNumber(m_headingBlockNum[i]);
	QTextCursor cursor = textCursor();
	cursor.setPosition(block.position());
	for(int n = 0; n < nth; ++n) {
		cursor = document()->find(pat, cursor);
		if( cursor.isNull()) return;
	}
	cursor.setPosition(cursor.selectionStart(), QTextCursor::MoveAnchor);
	setTextCursor(cursor);
}
void MarkdownViewer::ensureLineVisible(int srcBlockNum) {
	//qDebug() << "srcBlockNum = " << srcBlockNum;
	int i = 0;
	while( i+1 < m_headingSrcLineNum.size() && m_headingSrcLineNum[i+1] <= srcBlockNum ) ++i;
	if( i < m_headingBlockNum.size() )
		scrollToBlock(m_headingBlockNum[i]);
}
void MarkdownViewer::scrollToBlock(int blockIndex) {
    QTextBlock block = document()->findBlockByNumber(blockIndex);
    if (!block.isValid()) return;
#if 0
	QTextCursor cursor = textCursor();
	cursor.setPosition(block.position());
	setTextCursor(cursor);
	ensureCursorVisible();
#else
    int y = (int)document()->documentLayout()->blockBoundingRect(block).top();
    verticalScrollBar()->setValue(y);
#endif
}
