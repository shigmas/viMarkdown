#include <vector>
#include <QTextDocument>
#include <QTextBlock>
#include <QRegularExpression>
#include <QTextTable>
#include <QAbstractTextDocumentLayout>
#include <QScrollbar>
#include <QDir>
#include <qpainter.h>
#include <assert.h>
#include "MarkdownPreview.h"
#include "MainWindow.h"
#include "DocWidget.h"

using namespace std;

extern Global g;

bool isTableLine(const QString& lnStr, QList<QStringView> &tableTokens);
bool isTableLine(const QString& lnStr, QStringList &tableTokens);
bool isTableHyphenLine(const QString& lnStr, std::vector<char> &tableAlign);

enum {
	ALIGHN_LEFT = 1, ALIGHN_RIGHT = 2, ALIGHN_CENTER = ALIGHN_LEFT| ALIGHN_RIGHT,
};

MarkdownPreview::MarkdownPreview(const MainWindow *mainWindow, DocWidget *docWidget, QWidget* parent)
	: m_mainWindow(mainWindow), m_docWidget(docWidget), QTextEdit(parent)
{
	setUndoRedoEnabled(false);
	setFrameStyle(QFrame::NoFrame);
	setCursorWidth(2);
	//QString css = "hr { height: 1px; border: none; background-color: #333; margin-top: 10px; margin-bottom: 10px; }";
	//document()->setDefaultStyleSheet(css);
	connect(this, &MarkdownPreview::cursorPositionChanged, this, &MarkdownPreview::onCurPosChanged);
	connect(document(), &QTextDocument::contentsChange, this, &MarkdownPreview::onContentsChanged);
}
void MarkdownPreview::onCurPosChanged() {
	QTextCursor cursor = this->textCursor();
	m_lastCurBlockText = cursor.block().text();
	viewport()->update();	//	強制再描画
	//	カーソル同期処理
	if( m_processing || m_mainWindow->isCursorCyncing() ) return;		//	再入禁止
	m_processing = true;
	m_mainWindow->setCursorCyncing();
	//QTextCursor cursor = this->textCursor();
	auto context = contextAt(cursor.position());
	context.m_srcHBlockNum = m_docWidget->prvToSrcHeading(context.m_prvHBlockNum);
	emit posContextChanged(context);
	m_mainWindow->setCursorCyncing(false);
	m_processing = false;
}
void MarkdownPreview::onContentsChanged(int position, int charsRemoved, int charsAdded) {
	if( m_processing ) return;
	m_processing = true;
	if( charsAdded > 0 ) {
		QTextCursor cursor = this->textCursor();
		int pos0 = cursor.position();
		cursor.setPosition(position);
		QTextBlock block = cursor.block();
		QString addedStr = block.text().mid(cursor.position() - block.position(), charsAdded);
		if( addedStr.isEmpty() && charsRemoved == 0 && charsAdded == 1 )		//	改行が入力された場合
			addedStr = "  \n";
		qDebug() << "addedStr = " << addedStr;
		emit textInserted(addedStr);
		cursor.setPosition(pos0);
		setTextCursor(cursor);
	}
	if( charsRemoved > 0 ) {
		emit textRemoved(charsRemoved);
	}
	m_processing = false;
}

bool isUnderlineHeading(const QString& txt);
#if 1
bool isTableLine(const QString& lnStr, QList<QStringView> &tableTokens) {
	QStringView sv(lnStr);
	tableTokens.clear();
	int ix = 0;
	while( ix < sv.size() && sv[ix] == u' ' ) ++ix;		//	空白スキップ
	if (ix < sv.size() && sv[ix] == u'|') ++ix;			//	先頭の '|' スキップ
	while( ix < sv.size() ) {
		int ix0 = ix;
		while( ix < sv.size() && sv[ix] != u'|' ) {		//	'|' までループ
			if( ix+1 < sv.size() && sv[ix] == u'\\' ) ix += 2;
			else ++ix;
		}
		tableTokens.push_back(sv.mid(ix0, ix-ix0));
		++ix;			//	'|' スキップ
	}
	return tableTokens.size() > 1;
}
bool isTableLine(const QString& lnStr, QStringList &tableTokens) {
	QString sv(lnStr);
	tableTokens.clear();
	int ix = 0;
	while( ix < sv.size() && sv[ix] == u' ' ) ++ix;		//	空白スキップ
	if (ix < sv.size() && sv[ix] == u'|') ++ix;			//	先頭の '|' スキップ
	while( ix < sv.size() ) {
		int ix0 = ix;
		while( ix < sv.size() && sv[ix] != u'|' ) {		//	'|' までループ
			if( ix+1 < sv.size() && sv[ix] == u'\\' ) ix += 2;
			else ++ix;
		}
		tableTokens.push_back(sv.mid(ix0, ix-ix0));
		++ix;			//	'|' スキップ
	}
	return tableTokens.size() > 1;
}
bool isTableHyphenLine(const QString& lnStr, std::vector<char> &tableAlign) {
	tableAlign.clear();
	QStringView sv(lnStr);
	int ix = 0;
	while( ix < sv.size() && sv[ix] == u' ' ) ++ix;		//	空白スキップ
	if (ix < sv.size() && sv[ix] == u'|') ++ix;			//	先頭 '|' スキップ
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
		tableAlign.push_back(aln);
		++ix;
	}
	return tableAlign.size() > 1;
}
#else
bool MarkdownPreview::isTableLine(const QString& lnStr) {
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
bool MarkdownPreview::isTableHyphenLine(const QString& lnStr) {
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
#endif

void MarkdownPreview::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Backspace) {
		emit BS_pressed();
		return;
	}
	if (e->key() == Qt::Key_Z && (e->modifiers() & Qt::ControlModifier) != 0) {
		emit undo_triggered();
		return;
	}
	if (e->key() == Qt::Key_Y && (e->modifiers() & Qt::ControlModifier) != 0) {
		emit redo_triggered();
		return;
	}
	QTextEdit::keyPressEvent(e);	// 通常キーは通常通りの処理
}
QString splitName(QString& anchor) {
	QString name;
	int ix = anchor.indexOf(u'#');
	if( ix >= 0 ) {
		name = anchor.mid(ix+1);
		anchor = anchor.left(ix);
	}
	return name;
}
QString anchorToFullPath(const QString &anchor) {
	QString fullPath = anchor;
	if( fullPath.startsWith("http://") || fullPath.startsWith("https://") )
		return fullPath;
	if( !anchor.endsWith(".md", Qt::CaseInsensitive) )
		fullPath += ".md";
	fullPath = QDir::cleanPath(QDir::current().absoluteFilePath(fullPath));
	return fullPath;
}
void MarkdownPreview::mouseMoveEvent(QMouseEvent *me) {
	QString anchor = anchorAt(me->pos());
	QString name = splitName(anchor);
	//qDebug() << "anchor = " << anchor << ", name = " << name;
	if (!anchor.isEmpty() || !name.isEmpty() ) {	// リンクの上なら指差し
		viewport()->setCursor(Qt::PointingHandCursor);
		//if( !anchor.endsWith(".md", Qt::CaseInsensitive) )
		//	anchor += ".md";
		//QString fullPath = QDir::cleanPath(QDir::current().absoluteFilePath(anchor));
		QString fullPath = anchorToFullPath(anchor);
		m_mainWindow->statusBar()->showMessage(fullPath);
	} else {	// それ以外なら通常（I型または矢印）
		viewport()->setCursor(Qt::IBeamCursor);
	}
}
void MarkdownPreview::mouseReleaseEvent(QMouseEvent *me)
{
	if (me->button() == Qt::LeftButton) {
		QString anchor = anchorAt(me->pos());
		QString name = splitName(anchor);
		if (!anchor.isEmpty() || !name.isEmpty() ) {	// リンク上
			//qDebug() << "anchor = " << anchor;
			QString fullPath = anchor;
			if(!anchor.isEmpty() ) {
				//if( anchor.startsWith("./") || anchor.startsWith(".\\") )
				//	anchor = anchor.mid(2);
				//if( !anchor.endsWith(".md", Qt::CaseInsensitive) )
				//	anchor += ".md";
				//fullPath = QDir::cleanPath(QDir::current().absoluteFilePath(anchor));
				fullPath = anchorToFullPath(anchor);
			}
			emit anchorClicked("", fullPath, name);
		} else {
			QTextCursor cursor = cursorForPosition(me->pos());
			QTextCharFormat format = cursor.charFormat();
			if( format.isAnchor() ) {
				qDebug() << "checkbox is clicked";
			}
			emit lineClicked(cursor.block().userState());
		}
	}
	QTextEdit::mouseReleaseEvent(me);
}
void MarkdownPreview::paintEvent(QPaintEvent *e) {
	QTextEdit::paintEvent(e); // 先にテキストを普通に描画
	QPainter p(viewport());
	//	行カーソル描画
	QRect rect = cursorRect();
	//QPen pen(hasFocus() ? Qt::red : Qt::gray, 1); // 赤色、太さ1px
	//QPen pen(Qt::red, 1); // 赤色、太さ1px
	QPen pen(hasFocus() ? g.m_activeLnColor: g.m_inactiveLnColor, 1); // 色、太さ1px
	if( !hasFocus() ) pen.setStyle(Qt::DashLine);	//	破線
	p.setPen(pen);
	int y = rect.bottom();
	int left = 0;	//-lnAreaWidth();
	int right = viewport()->width();
	p.drawLine(left, y, right, y);
}
void MarkdownPreview::do_body_sub(QTextCursor& cursor, const QString &buf) {
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
void MarkdownPreview::do_body(QTextCursor& cursor) {
	if( m_bodyList.isEmpty() ) return;
#if 1
	//封印static QRegularExpression re("\\[\\[(.+?)\\]\\]");		//	[[タイトル]]
	static QRegularExpression re("\\[([^\\]]+)\\]\\(([^)]+)\\)");		//	[タイトル](パス#見出し)
	QString buf;
	for(auto txt: m_bodyList) {
		//封印txt.replace(re, "<a href=\"\\1\" class=\"wiki-link\">\\1</a>");
		txt.replace(re, "<a href=\"\\2\">\\1</a>");
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
static QRegularExpression re_numlist(R"(^(\s*)(\d+)([.)]) )");

int indexOfComment(QStringView buf, int start) {
	for(int i = start; i < buf.size(); ++i) {
		if( buf[i] == u'\\' && i+1 < buf.size() )
			++i;
		else if( buf.mid(i).startsWith(u"<!--") )
			return i;
	}
	return -1;
}

void MarkdownPreview::setMarkdown(QTextDocument *doc) {
	m_headingList.clear();
	m_docWidget->m_srcHeadingBlocks.clear();
	m_docWidget->m_prvHeadingBlocks.clear();
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
		} else if( re_numlist.match(buf).hasMatch() ) {
			do_body(cursor);
			do_numlist(cursor, buf);
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
		} else if( isTableLine(buf, m_tableTokens) && m_ln + 1 < m_lst.size() && isTableHyphenLine(m_lst[m_ln+1], m_tableAlign) ) {
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
void MarkdownPreview::do_table(QTextCursor& cursor) {
	QString buf = m_lst[m_ln] + "\n" + m_lst[m_ln+1] /*+ "\n"*/;
	m_ln += 2;
	while( m_ln < m_lst.size() && isTableLine(m_lst[m_ln], m_tableTokens) ) {
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
bool MarkdownPreview::do_underlineHeading(QTextCursor& cursor, QString buf) {
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

void MarkdownPreview::do_heading(QTextCursor& cursor, QString buf) {
	int i = 1;
	while( i < buf.size() && buf[i] == '#' ) ++i;
	int h = std::min(6, i);		//	[1, 6]
	while( i < buf.size() && buf[i] == u' ' ) ++i;
	do_heading_sub(cursor, buf.mid(i), h, m_ln);
}
void MarkdownPreview::do_heading_sub(QTextCursor& cursor, QString buf, int h, int ln) {
	if( !cursor.atBlockStart() )
		cursor.insertBlock();			//	新規ブロック
	cursor.block().setUserState(US_HEADING);
	cursor.insertMarkdown(QString(h, u'#') + u' ' + buf /*+ "\n"*/);		//	改行を付加すると、２行になってしまう
	QTextBlockFormat blockFormat;
	if( h == 1 ) {		//	H1 の場合はセンタリング（viMarkdown 独自？仕様）
		blockFormat.setAlignment(Qt::AlignCenter);
	} else {
		blockFormat.setTopMargin(12);	 // 上側の余白（ピクセル）
		blockFormat.setBottomMargin(12); // 下側の余白（ピクセル）
	}
	cursor.mergeBlockFormat(blockFormat);
	m_docWidget->m_prvHeadingBlocks.push_back(cursor.blockNumber());

	cursor.insertBlock(QTextBlockFormat(), QTextCharFormat());		//	新規ブロック

	m_headingList.push_back(QChar(u'0'+h) + buf.remove("^ +"));
	m_docWidget->m_srcHeadingBlocks.push_back(ln);
	m_nEmptyLines = 0;
}
bool parseCsvLine(QStringList &fields, const QString &line, bool inQuotes, bool &inComment, bool &commented) {
	//QStringList fields;
	commented = false;
	int i = 0;
	if( inComment ) {
		int ix = line.indexOf("-->");
		if( ix < 0 ) return inQuotes;
		inComment = false;
		if( (i = ix + 3) >= line.length() ) {	//	"-->" までスキップ
			commented = fields.isEmpty();
			return inQuotes;
		}

	}
	QString currentField;
	if( !inQuotes ) {
		fields.clear();
	} else {
		currentField = fields.back() + u'\n';
		fields.pop_back();
	}
	//bool inQuotes = false;
	for (; i < line.length(); ++i) {
		if( line.mid(i).startsWith("<!--") ) {
			int ix = line.indexOf("-->", i + 4);
			if( ix > 0 ) {	//	コメント終了を発見
				if( (i = ix + 3) >= line.length() ) {	//	"-->" までスキップ
					commented = fields.isEmpty();
					return inQuotes;
				}
			} else {
				inComment = true;
				return inQuotes;
			}
		}
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
void MarkdownPreview::do_CSV(QTextCursor& cursor) {
	QList<QStringList> ll;
	int max_clmn = 0;
	bool inQuotes = false;
	bool inComment = false;
	bool commented = false;		//	行単位でコメントアウトされた
	QStringList fields;
	while( ++m_ln < m_lst.size() && !m_lst[m_ln].startsWith("```") ) {
		inQuotes = parseCsvLine(fields, m_lst[m_ln], inQuotes, inComment, commented);
		if( !inQuotes && !inComment && !commented ) {
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
					cellFormat.setBackground(g.m_CSVHeaderColor);
					cell.setFormat(cellFormat);
					charFormat.setFontWeight(QFont::Bold);
					blockFormat.setAlignment(Qt::AlignCenter); // ヘッダは中央
				} else {
					QTextTableCellFormat cellFormat;
					cellFormat.setBackground((row % 2) != 0 ? g.m_CSVZebraColor1 : g.m_CSVZebraColor2);
					cell.setFormat(cellFormat);
					charFormat.setFontWeight(QFont::Normal);
					if (re.match(ll[row][col]).hasMatch()) {
						blockFormat.setAlignment(Qt::AlignRight);  // 数値は右
					} else {
						blockFormat.setAlignment(Qt::AlignLeft);   // その他は左
					}
				}
				cellCursor.setCharFormat(charFormat);
				cellCursor.setBlockFormat(blockFormat);
				//cellCursor.insertText(ll[row][col]);
				cellCursor.insertMarkdown(ll[row][col]);
			}
		}
	}
	cursor.setPosition(table->lastPosition());
	cursor.movePosition(QTextCursor::NextCharacter);
	cursor.insertBlock();
	cursor.endEditBlock();
	//++m_ln;
}
void MarkdownPreview::do_code_keisen(QTextCursor& cursor) {
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
void MarkdownPreview::do_code(QTextCursor& cursor) {
	QString buf;
	while( ++m_ln < m_lst.size() && !m_lst[m_ln].startsWith("```") ) {
		if( !buf.isEmpty() ) buf += "\n";
		buf += m_lst[m_ln];
	}
	QTextCharFormat codeCharFormat;
#if 0
	codeCharFormat.setFontFamilies({"MS Gothic", "MS UI Gothic", "Osaka-mono", "monospace"});
	codeCharFormat.setFontFixedPitch(true); // 等幅フォントであることを明示
	codeCharFormat.setFontPointSize(10);	// 必要に応じてサイズを調整
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
	frameFormat.setMargin(0);					  // 外側の余白
	frameFormat.setPadding(10);					  // 内側の余白（文字と端の隙間）
	frameFormat.setBorder(0);					  // 枠線はなし
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
void MarkdownPreview::do_quote(QTextCursor& cursor, QString buf) {
	buf = buf.mid(2);
	while( ++m_ln < m_lst.size() ) {
		if( !m_lst[m_ln].startsWith("> ") ) break;
		buf += u"  \n" + m_lst[m_ln].mid(2);
	}
	//cursor.insertMarkdown(buf);
	// 1. フレームの書式を設定
	QTextFrameFormat frameFormat;
	frameFormat.setBackground(QColor("#f0f8ff")); // 薄い青
	frameFormat.setMargin(0);					  // 外側の余白
	frameFormat.setPadding(10);					  // 内側の余白（文字と端の隙間）
	frameFormat.setBorder(0);					  // 枠線はなし
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
void MarkdownPreview::do_numlist(QTextCursor& cursor, QString buf) {
	while( ++m_ln < m_lst.size() ) {
		if( !re_numlist.match(m_lst[m_ln]).hasMatch() ) break;			//	"1. " 連番行が終わった場合
		buf += u'\n' + m_lst[m_ln];
	}
	cursor.insertMarkdown(buf);
	cursor.insertBlock();
	QTextBlockFormat blockFormat;
	cursor.setBlockFormat(blockFormat);
	--m_ln;
	m_nEmptyLines = 0;
}
void MarkdownPreview::do_list(QTextCursor& cursor, QString buf) {
	if( m_nEmptyLines >= 1 )
		cursor.insertBlock();			//	新規ブロック
	if( m_nSpaces > 0 )
		buf = QString(m_nSpaces, QChar(u' ')) + buf;
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
			if( !re_list.match(m_lst[m_ln]).hasMatch() ) break;			//	"- " リスト行が終わった場合
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
void MarkdownPreview::setCursorAt(int srcBlockNum, QString srcText, int ix) {		//	ix: ブロック内カーソル位置
	int i = 0;
	while( i+1 < m_docWidget->m_srcHeadingBlocks.size() && m_docWidget->m_srcHeadingBlocks[i+1] <= srcBlockNum ) ++i;
	if( i >= m_docWidget->m_prvHeadingBlocks.size() ) return;
	QTextBlock block = document()->findBlockByNumber(m_docWidget->m_prvHeadingBlocks[i]);
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
void MarkdownPreview::setCursorAtNthPat(int srcBlockNum, QString pat, int nth, bool tail) {		//	nth: 何番目か（>0）
	qDebug() << QString("MarkdownPreview::setCursorAtNthPat(%1, '%2', %3,").arg(srcBlockNum).arg(pat).arg(nth) << tail << ")";
	int i = 0;
	while( i+1 < m_docWidget->m_srcHeadingBlocks.size() && m_docWidget->m_srcHeadingBlocks[i+1] <= srcBlockNum ) ++i;
	if( i >= m_docWidget->m_prvHeadingBlocks.size() ) return;
	QTextBlock block = document()->findBlockByNumber(m_docWidget->m_prvHeadingBlocks[i]);
	QTextCursor cursor = textCursor();
	if( !tail ) {
		cursor.setPosition(block.position());
		for(int n = 0; n < nth; ++n) {
			cursor = document()->find(pat, cursor);
			if( cursor.isNull()) return;
		}
		cursor.setPosition(cursor.selectionStart(), QTextCursor::MoveAnchor);
	} else {
		for(;;) {
			if( block.text().endsWith(pat) ) {
				if( --nth == 0 )
					break;
			}
			block = block.next();
			if( !block.isValid() ) {	
				block = document()->lastBlock();
				break;
			}
		}
		cursor.setPosition(block.position());
		cursor.movePosition(QTextCursor::EndOfBlock);
	}
	setTextCursor(cursor);
}
void MarkdownPreview::ensureLineVisible(int srcBlockNum) {
	//qDebug() << "srcBlockNum = " << srcBlockNum;
	int i = 0;
	while( i+1 < m_docWidget->m_srcHeadingBlocks.size() && m_docWidget->m_srcHeadingBlocks[i+1] <= srcBlockNum ) ++i;
	if( i < m_docWidget->m_prvHeadingBlocks.size() )
		scrollToBlock(m_docWidget->m_prvHeadingBlocks[i]);
}
void MarkdownPreview::scrollToBlock(int blockIndex) {
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
#if 0
int MarkdownPreview::prvToSrcHeading(int blockNum) {
	assert( m_docWidget->m_prvHeadingBlocks.size() == m_docWidget->m_srcHeadingBlocks.size() );
#if 1
	auto it = std::lower_bound(m_docWidget->m_prvHeadingBlocks.begin(), m_docWidget->m_prvHeadingBlocks.end(), blockNum);
	if (it != m_docWidget->m_prvHeadingBlocks.end()) {
		size_t ix = std::distance(m_docWidget->m_prvHeadingBlocks.begin(), it);
		return m_docWidget->m_srcHeadingBlocks[ix];
	} else
		return 0;
#else
	int ix = 0;
	while( ix+1 < m_docWidget->m_prvHeadingBlocks.size() && m_docWidget->m_prvHeadingBlocks[ix] < blockNum ) ++ix;
	if( ix < m_docWidget->m_srcHeadingBlocks.size() )
		return m_docWidget->m_srcHeadingBlocks[ix];
	else
		return 0;
#endif
}
#endif
int MarkdownPreview::findPosition(const PosContext &context) {
	QTextBlock block = document()->findBlockByNumber(context.m_prvHBlockNum);
	const QChar ch = context.m_charAt;
	int nth = context.m_indexOfPrevNext;
	int ix = 0;
	while( block.isValid() ) {
		QString buf = block.text();
		if( ch == QChar() ) {		//	行末の場合
			ix = buf.size();
			if( --nth == 0 ) break;
			block = block.next();
		} else {		//	非行末の場合
			ix = buf.indexOf(ch, ix);
			if( ix >= 0 ) {
				if( --nth == 0 ) break;
			} else {
				block = block.next();
				ix = 0;
			}
		}
	}
	if( block.isValid() ) {
		return block.position() + ix;
	} else
		return -1;
}
void MarkdownPreview::setCursorByContext(const PosContext &context) {
	if( m_processing ) return;		//	再入禁止
	m_processing = true;
	int pos = findPosition(context);
	if( pos >= 0 ) {
		QTextCursor cursor = textCursor();
		cursor.setPosition(pos);
		setTextCursor(cursor);
	}
	m_processing = false;
}
PosContext MarkdownPreview::contextAt(int pos) {	//	pos 位置から PosContext を構築
	PosContext pc;
	auto *doc = document();
	QTextBlock block = doc->findBlock(pos);
	//pc.m_chPrev = pos != block.position() ? doc->characterAt(pos-1) : QChar();
	auto chat = doc->characterAt(pos);
	pc.m_charAt = chat != QChar::ParagraphSeparator ? chat : QChar();
	while( block.userState() != US_HEADING ) {		//	直前の見出し行を探す
		if( !block.previous().isValid() )
			break;
		block = block.previous();
	}
	pc.m_prvHBlockNum = block.blockNumber();
	//	pos の {chPrev, chNext} が見出し行先頭から何番目かを計算
	int count = 1;
	if( pc.m_charAt == QChar() ) {		//	行末の場合
		while( block.isValid() ) {
			if( block.position() + block.text().length() >= pos )
				break;
			++count;
			block = block.next();
		}
	} else {
		for (int i = block.position(); i < pos; ++i) {
			QChar chAt = doc->characterAt(i);
			if (chAt == QChar::ParagraphSeparator)		//	改行記号の場合
				chAt = QChar();
			if( chAt == pc.m_charAt )
				++count;
		}
	}
#if 0
	int curPos = block.position();
	for (int i = curPos; i < pos; ++i) {
		// 1. 直前の文字 (prev) の取得
		QChar chBefore;
		if (i == block.position()) {
			// ブロックの先頭なら QChar() 扱い
			chBefore = QChar();
		} else {
			chBefore = doc->characterAt(i - 1);
			// 改行記号なら行頭扱いにする
			if (chBefore == QChar::ParagraphSeparator) {
				chBefore = QChar();
			}
		}
		// 2. 現在の位置の文字 (next) の取得
		QChar chAt = doc->characterAt(i);
		// 行末（改行記号）なら QChar() 扱いにする
		if (chAt == QChar::ParagraphSeparator) {
			chAt = QChar();
		}
		// 3. マッチ判定
		if (chBefore == pc.m_chPrev && chAt == pc.m_chNext) {
			count++;
		}
		// もし block を跨いだ場合は、ブロック先頭判定のために更新
		if (i >= block.position() + block.length() - 1) {
			block = block.next();
		}
	}
#endif
	pc.m_indexOfPrevNext = count;
	return pc;
}
