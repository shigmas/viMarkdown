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

const QChar endOfCell(0xfdd0);		//	セル終端文字

extern CharType getCharType(QChar ch);

MarkdownPreview::MarkdownPreview(const MainWindow *mainWindow, DocWidget *docWidget, QWidget* parent, bool readOnly)
	: m_mainWindow(mainWindow), m_docWidget(docWidget), QTextEdit(parent)
{
	setReadOnly(readOnly);
	setUndoRedoEnabled(false);
	setFrameStyle(QFrame::NoFrame);
	setCursorWidth(2);
	//QString css = "hr { height: 1px; border: none; background-color: #333; margin-top: 10px; margin-bottom: 10px; }";
	//document()->setDefaultStyleSheet(css);
	//setStyleSheet("QTextEdit { caret-color: red; }");
	connect(this, &MarkdownPreview::cursorPositionChanged, this, &MarkdownPreview::onCursorPosChanged);
	connect(document(), &QTextDocument::contentsChange, this, &MarkdownPreview::onContentsChanged);
	QFont font = this->font();
	font.setPointSize(12);
	setFont(font);
}
void MarkdownPreview::inputMethodEvent(QInputMethodEvent *event) {
	m_isComposing = !event->preeditString().isEmpty();
	QTextEdit::inputMethodEvent(event);
}
void MarkdownPreview::onCursorPosChanged() {
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
	PosContext acontext;
	if( cursor.hasSelection() ) {
		acontext = contextAt(cursor.anchor());
		acontext.m_srcHBlockNum = m_docWidget->prvToSrcHeading(acontext.m_prvHBlockNum);
	}
	emit posContextChanged(context, acontext);
	m_mainWindow->setCursorCyncing(false);
	m_processing = false;
}
void MarkdownPreview::onContentsChanged(int position, int charsRemoved, int charsAdded) {
	if( m_processing ) return;
	if (m_isComposing) return;		//	IME変換中
	qDebug() << "*** MarkdownPreview::onContentsChanged(" << position << ", " << charsRemoved << ", " << charsAdded << ")";
	m_processing = true;
	QTextCursor cursor = this->textCursor();
	QTextBlock block = cursor.block();
	const QString &text = block.text();		//	編集後ブロックテキスト
	int cpos = cursor.position();
	int bpos = position - cursor.block().position();	//	ブロック先頭からの編集位置
	QString strAdded = text.mid(bpos, charsAdded);
	if( block.userState() == US_DEFAULT && charsAdded != 0 && strAdded.isEmpty() &&
		document()->characterAt(position) == QChar::ParagraphSeparator )
	{
		strAdded = "  \n";
	}
	QString strRemoved = m_lastCurBlockText.mid(bpos, charsRemoved);
	charsRemoved = qMin(charsRemoved, strRemoved.size());		//	行末を超えている場合対応
	charsAdded = qMin(charsAdded, strAdded.size());		//	行末を超えている場合対応
	int c = 0;
	while(charsAdded-c-1 > 0 && charsRemoved-c-1 > 0 &&
		strAdded[charsAdded-c-1] == strRemoved[charsRemoved-c-1] )
	{
		++c;	//	末尾共通部分
	}
	charsRemoved -= c;
	charsAdded -= c;
	//	先頭共通部分削除
	while( !strAdded.isEmpty() && !strRemoved.isEmpty() && strAdded[0] == strRemoved[0] ) {
		--charsRemoved;
		--charsAdded;
		++position;
		strAdded = strAdded.mid(1);
		strRemoved = strRemoved.mid(1);
	}
	if( charsAdded > 0 ) {
		QTextCursor cursor = this->textCursor();
		int pos0 = cursor.position();		//	文字挿入後プレビューカーソル位置
		cursor.setPosition(position);
		QTextBlock block = cursor.block();
		QString addedStr = block.text().mid(cursor.position() - block.position(), charsAdded);
		addedStr.replace("\\", "\\\\").replace("#", "\\#").replace("-", "\\-").replace("<", "\\<")
				.replace("`", "\\`").replace("[", "\\[").replace("*", "\\*").replace("_", "\\_").replace("~", "\\~");
		if( addedStr.isEmpty() && charsRemoved == 0 && charsAdded == 1  )
		{		//	改行が入力された場合
			if( block.userState() == US_DEFAULT )
				addedStr = "  \n";
			else
				addedStr = "\n";
			++pos0;
		}
		qDebug() << "addedStr = " << addedStr << ", pos0 = " << pos0;
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
		if( ix < sv.size() && sv[ix] == u':' ) { aln = ALIGHN_LEFT; ++ix; }
		while( ix < sv.size() && sv[ix] != u'|' ) {		//	次の'|' までループ
			if( ix+1 < sv.size() && sv[ix] == u'\\' ) ++ix;
			if( sv[ix] != u'-' && sv[ix] != u' ' ) return false;
			++ix;
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
	if (e->key() == Qt::Key_Delete) {
		emit Del_pressed();
		return;
	}
	if( (e->modifiers() & Qt::ControlModifier) != 0 ) {		//	Ctrl +
		bool shift = (e->modifiers() & Qt::ShiftModifier) != 0;
		if (e->key() == Qt::Key_Right ) {
			QTextCursor cursor = textCursor();
			moveToNextWord(cursor, shift);
			setTextCursor(cursor);
			return;
		} else if (e->key() == Qt::Key_Left) {
			QTextCursor cursor = textCursor();
			moveToPrevWord(cursor, shift);
			setTextCursor(cursor);
			return;
		}
		if (e->key() == Qt::Key_Z ) {
			emit undo_triggered();
			return;
		}
		if (e->key() == Qt::Key_Y ) {
			emit redo_triggered();
			return;
		}
	}
	QTextEdit::keyPressEvent(e);	// 通常キーは通常通りの処理
}
void MarkdownPreview::moveToNextWord(QTextCursor& cursor, bool shift) {
	int pos = cursor.position();
	QTextDocument *doc = document();
	if (pos >= doc->characterCount() - 1) return;
	CharType startType = getCharType(doc->characterAt(pos));
	// 同じ種別の間は進む
	while (pos < doc->characterCount() - 1 && getCharType(doc->characterAt(pos)) == startType) {
		pos++;
	}
	if( startType != Type_Space ) {
		while (pos < doc->characterCount() - 1 && getCharType(doc->characterAt(pos)) == Type_Space) {
			pos++;
		}
	}
	cursor.setPosition(pos, shift ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
}
void MarkdownPreview::moveToPrevWord(QTextCursor& cursor, bool shift) {
	int pos = cursor.position();
	QTextDocument *doc = document();
	if (pos <= 0) return;
	CharType startType = getCharType(doc->characterAt(pos - 1));
	while (pos > 0 && getCharType(doc->characterAt(pos - 1)) == startType) {
		pos--;
	}
	if( startType == Type_Space ) {
		CharType startType = getCharType(doc->characterAt(pos - 1));
		while (pos > 0 && getCharType(doc->characterAt(pos - 1)) == startType) {
			pos--;
		}
	}
	cursor.setPosition(pos, shift ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
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
	if( anchor.isEmpty() )		//	文書内ジャンプ
		return anchor;
	QString fullPath = anchor;
	if( fullPath.startsWith("http://") || fullPath.startsWith("https://") )
		return fullPath;
	if( fullPath.startsWith("file://") ) {
		fullPath = fullPath.mid(strlen("file://"));
		static QRegularExpression re("^/[A-Za-z]:");
		if( fullPath.indexOf(re) == 0 )
			fullPath = fullPath.mid(strlen("/"));
	}
	if( !anchor.endsWith(".md", Qt::CaseInsensitive) && !QFile::exists(anchor) )
		fullPath += ".md";
	fullPath = QDir::cleanPath(QDir::current().absoluteFilePath(fullPath));
	return fullPath;
}
void MarkdownPreview::mousePressEvent(QMouseEvent *me) {
	m_processing = true;
	QTextEdit::mousePressEvent(me);
	//onCursorPosChanged();
}
void MarkdownPreview::mouseMoveEvent(QMouseEvent *me) {
	QString anchor = anchorAt(me->pos());
	QString name = splitName(anchor);
	//qDebug() << "anchor = " << anchor << ", name = " << name;
	if (!anchor.isEmpty() || !name.isEmpty() ) {	// リンクの上なら指差し
		viewport()->setCursor(Qt::PointingHandCursor);
		QString fullPath = anchorToFullPath(anchor);
		m_mainWindow->statusBar()->showMessage(fullPath);
	} else {	// それ以外なら通常（I型または矢印）
		viewport()->setCursor(Qt::IBeamCursor);
	}
	QTextEdit::mouseMoveEvent(me);
}
#if 0
int MarkdownPreview::countCheckBox(QTextBlock block) {
	int count = 1;
	while( (block = block.previous()).isValid() ) {
		auto t = block.userState();
		if( t == US_HEADING ) break;
		if( t == US_CSV_BLOCK ) ++count;
	}
	return count;
}
#endif
void MarkdownPreview::mouseReleaseEvent(QMouseEvent *me)
{
	if (me->button() == Qt::LeftButton) {
		QString anchor = anchorAt(me->pos());
		QString name = splitName(anchor);
		if (!anchor.isEmpty() || !name.isEmpty() ) {	// リンク上
			//qDebug() << "anchor = " << anchor;
			QString fullPath = anchor;
			if(!anchor.isEmpty() ) {
				fullPath = anchorToFullPath(anchor);
			}
			emit anchorClicked("", fullPath, name);
			QTextEdit::mouseReleaseEvent(me);
			m_processing = false;
			return;
		} else {
			QTextCursor cursor = cursorForPosition(me->pos());
			QTextBlock block = cursor.block();
			if (block.userState() == US_CHECKBOX) {
				QTextEdit::mouseReleaseEvent(me);
				m_processing = false;
				onCursorPosChanged();
				QTextBlockFormat fmt = block.blockFormat();
				bool checked = fmt.marker() == QTextBlockFormat::MarkerType::Checked;
				qDebug() << "checked = " << checked;
				//emit lineClicked(block.blockNumber());
				//int nth = countCheckBox(block);
				emit checkboxLineClicked(/*nth,*/ checked);
				return;
			}
#if 0
			QTextCharFormat format = cursor.charFormat();
			if( format.isAnchor() ) {
				qDebug() << "checkbox is clicked";
			}
			emit lineClicked(cursor.block().userState());
#endif
		}
	}
	QTextEdit::mouseReleaseEvent(me);
	m_processing = false;
	onCursorPosChanged();
}
void MarkdownPreview::mouseDoubleClickEvent(QMouseEvent *e) {
	QTextCursor cursor = cursorForPosition(e->pos());
	int pos = cursor.position();
	QTextDocument *doc = document();
	CharType type = getCharType(doc->characterAt(pos));
	// 前方（左）への探索
	int start = pos;
	while (start > 0 && getCharType(doc->characterAt(start - 1)) == type) {
		start--;
	}
	// 後方（右）への探索
	int end = pos;
	while (end < doc->characterCount() - 1 && getCharType(doc->characterAt(end)) == type) {
		end++;
	}
	// 選択範囲を設定
	cursor.setPosition(start);
	cursor.setPosition(end, QTextCursor::KeepAnchor);
	setTextCursor(cursor);
}
#if 1
void MarkdownPreview::wheelEvent(QWheelEvent *event) {
	qDebug() << "MarkdownPreview::wheelEvent()";
	qDebug() << "e->angleDelta() = " << event->angleDelta();
	if ((event->modifiers() & Qt::ControlModifier) != 0)  {
		//emit changeFontSize(event->angleDelta().y());
		if (event->angleDelta().y() > 0)
            zoomIn();
        else
            zoomOut();
		event->accept();
	} else
		QTextEdit::wheelEvent(event);
}
#endif
void MarkdownPreview::paintEvent(QPaintEvent *e) {
	QTextEdit::paintEvent(e); // 先にテキストを普通に描画
	QPainter p(viewport());
	//	カーソル描画
	if( !isReadOnly() ) {
		QRect rect = cursorRect();
		if( !hasFocus() ) {
			p.setPen(Qt::gray);
			p.setBrush(Qt::gray);
			p.drawRect(rect);
		}
		QPen pen(hasFocus() ? g.m_activeLnColor: g.m_inactiveLnColor, 1); // 色、太さ1px
		if( !hasFocus() ) pen.setStyle(Qt::DashLine);	//	破線
		p.setPen(pen);
		int y = rect.bottom();
		int left = 0;	//-lnAreaWidth();
		int right = viewport()->width();
		p.drawLine(left, y, right, y);
	}
}

static QRegularExpression image_re(R"((?<!\\)!\[([^\]]+)\]\(([^)]+)\))");
static QRegularExpression link_re(R"((?<![!\\])\[([^\]]+)\]\(([^)]+)\))");
#if 0
void MarkdownPreview::do_body_sub(QTextCursor& cursor, const QString &buf) {
#if 1
	QTextBlock block = cursor.block();
	BlockData *data = getBlockData(block);
	bool modified = false;
	auto match = image_re.match(buf);		//	![title](image.png) か？
	while( match.hasMatch() ) {
		modified = true;
		int start = match.capturedStart(); // マッチした最初の位置
	    int length = match.capturedLength(); // マッチした全体の長さ
	    for(int i = start; i < start + length; ++i)
	    	data->m_charFlags[i] = PCF_IMAGE;
		match = image_re.match(buf, start + length);
	}
	if( modified )
		block.setUserData(data);
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
#endif
//	ボールド、イタリック、打ち消し線 部分に PCF_EMPHASIZED 設定
bool updateCharFlags(BlockData* data, const QString &buf, int ix, int ix9) {
	bool modified = false;
	while( ix < ix9 ) {
		if( buf[ix] == u'\\' )
			ix += 2;
		else if( buf[ix] == u'*' || buf[ix] == u'_' || buf[ix] == u'~' && ix+1 < ix9 && buf[ix+1] == buf[ix]) {
			QString sym;
			if( buf[ix] == u'~' ) {
				sym = "~~";
			} else {
				if( ix+1 < ix9 && buf[ix+1] == buf[ix] ) {
					if(ix+2 < ix9 && buf[ix+2] == buf[ix] )
						sym = buf.mid(ix, 3);
					else
						sym = buf.mid(ix, 2);
				} else
					sym = buf[ix];
			}
			int ix2 = buf.indexOf(sym, ix+sym.size());
			if( ix2 >= 0 ) {	//	バランスしている場合
				for(int i = 0; i < sym.size(); ++i) {
					data->m_charFlags[ix+i] = PCF_EMPHASIZED;
					data->m_charFlags[ix2+i] = PCF_EMPHASIZED;
					modified = true;
				}
				if( updateCharFlags(data, buf, ix+sym.size(), ix2) )
					modified = true;
				ix = ix2 + sym.size();
			} else
				++ix;
		} else
			++ix;
	}
	return modified;
}
void updateCharFlags(QTextBlock srcBlock) {
	QString buf = srcBlock.text();
	BlockData *data = getBlockData(srcBlock, true);
	bool modified = false;
	auto match = image_re.match(buf);		//	![title](image.png) を含むか？
	while( match.hasMatch() ) {
		modified = true;
		int start = match.capturedStart(); // マッチした最初の位置
	    int length = match.capturedLength(); // マッチした全体の長さ
	    for(int i = start; i < start + length; ++i)
	    	data->m_charFlags[i] = PCF_IMAGE;
		match = image_re.match(buf, start + length);
	}
	match = link_re.match(buf);		//	[title](image.png) を含むか？
	while( match.hasMatch() ) {
		modified = true;
		int start = match.capturedStart(); // マッチした最初の位置（'['）
	    int length = match.capturedLength(); // マッチした全体の長さ
	    data->m_charFlags[start] = PCF_LINK;
	    int ix = buf.indexOf(']', start);
	    for(int i = ix; i < start + length && i < data->m_charFlags.size(); ++i)
	    	data->m_charFlags[i] = PCF_LINK;
		match = link_re.match(buf, start + length);
	}
	int ix = 0;
	while( (ix = buf.indexOf(u'\\', ix)) >= 0 ) {
		modified = true;
		data->m_charFlags[ix] = PCF_ESCAPE;
		ix += 2;
	}
	if( updateCharFlags(data, buf, 0, buf.size()) )
		modified = true;
	if( modified ) {
		srcBlock.setUserData(data);
		printCharFlags(srcBlock);
	}
}
void MarkdownPreview::do_body(QTextBlock srcBlock, QTextCursor& cursor, bool last) {
	m_isPrevLineEmpty = false;
	if( m_bodyList.isEmpty() ) return;
#if 1
	qDebug() << "srcBlock.blockNumber() = " << srcBlock.blockNumber();
	//封印：static QRegularExpression re("\\[\\[(.+?)\\]\\]");		//	[[タイトル]]
	static QRegularExpression re("(?<!!)\\[([^\\]]+)\\]\\(([^)]+)\\)");		//	[タイトル](パス#見出し)
	QString buf;
	for(auto txt: m_bodyList) {
		//assert(txt.size() == srcBlock.text().size());
		//封印：txt.replace(re, "<a href=\"\\1\" class=\"wiki-link\">\\1</a>");
		txt.replace(re, "<a href=\"\\2\">\\1</a>");
		buf += txt + "\n";
		//do {
		//	srcBlock = srcBlock.next();
		//} while( srcBlock.userState() == US_IN_COMMENT );
	}
	if( !buf.isEmpty() ) {
		//QTextBlock block = cursor.block();
		QTextBlockFormat blockFormat;
		blockFormat.setAlignment(Qt::AlignJustify); // 左右両端揃え
		cursor.mergeBlockFormat(blockFormat);
		cursor.insertMarkdown(buf);
	}
	//int sz = m_bodyList.size();
	//if( last && sz >= 2 && m_bodyList[sz-1].isEmpty() && m_bodyList[sz-2].endsWith("  ") ) {
	//	cursor.insertBlock();
	//}
	m_isPrevLineEmpty = m_bodyList.back().isEmpty();	//	最後が空行か？
	//if( m_bodyList.back().isEmpty() )	//	最後が空行
	//	cursor.insertBlock();
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
static QRegularExpression re_block(R"(^ *[#`\d])");
static QRegularExpression re_numlist(R"(^(\s*)(\d+)([.)]) )");

int indexOfComment(QStringView buf, int start) {
	for(int i = start; i < buf.size(); ++i) {
		if( buf[i] == u'\\' && i+1 < buf.size() )	//	エスケープされた文字をスキップ
			++i;
		else if( buf.mid(i).startsWith(u"<!--") )
			return i;
	}
	return -1;
}
void updateCharFlags(QTextBlock srcBlock);
void MarkdownPreview::setMarkdown(QTextDocument *doc) {		//	doc: markdown ソースドキュメント
	m_headingList.clear();
	m_docWidget->m_srcHeadingBlocks.clear();
	m_docWidget->m_prvHeadingBlocks.clear();
	m_docWidget->m_srcHeadingBlocks.push_back(0);
	m_docWidget->m_prvHeadingBlocks.push_back(0);
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
	QTextBlock srcBlock0;
	for(m_ln = 0; m_ln < m_lst.size(); ++m_ln) {
		bool bComment = false;		//	コメントがあった
		//QString buf = m_lst[m_ln];
		QTextBlock srcBlock = doc->findBlockByNumber(m_ln);
		QString buf = srcBlock.text();
		if( !m_inComment ) {
			srcBlock.setUserState(US_DEFAULT);
			BlockData *data = getBlockData(srcBlock);
			int start = 0, ix;
			QString buf2;
			bool modified = false;
			while( (ix = indexOfComment(buf, start)) >= 0 ) {	//	"<!--" 有り
				bComment = true;
				modified = true;
				int ix2 = buf.indexOf("-->", ix + 4);
				if( ix2 < 0 ) {		//	"-->" 無し
					for(int i = ix; i < data->m_charFlags.size(); ++i)
						data->m_charFlags[i] = PCF_COMMENTED;
					buf2 += buf.mid(start, ix-start);
					m_inComment = true;
					break;
				} else {			//	"-->" 有り
					buf2 += buf.mid(start, ix-start) + buf.mid(ix2+3);
					for(int i = ix; i < ix2+3; ++i)
						data->m_charFlags[i] = PCF_COMMENTED;
					start = ix2+3;
				}
			}
			if( modified ) buf = buf2;
			srcBlock.setUserData(data);
			if( bComment && buf.isEmpty() ) continue;
		} else {
			srcBlock.setUserState(US_IN_COMMENT);
			BlockData *data = getBlockData(srcBlock);
			int ix = buf.indexOf("-->");
			if( ix < 0 ) {
				data->m_charFlags.fill(PCF_COMMENTED);
				srcBlock.setUserData(data);
				continue;
			}
			for(int i = 0; i < ix + 3; ++i)
				data->m_charFlags[i] = PCF_COMMENTED;
			srcBlock.setUserData(data);
			m_inComment = false;
			buf = buf.mid(ix+3);
			if( buf.isEmpty() ) continue;
		}
		m_nSpaces = 0;
		while( m_nSpaces < buf.size() && buf[m_nSpaces] == u' ' ) ++m_nSpaces;
		if( m_nSpaces > 0 ) buf = buf.mid(m_nSpaces);
		if( buf.startsWith('#') ) {
			do_body(srcBlock0, cursor);
			do_heading(srcBlock, cursor, buf);
		} else if( re_list.match(buf).hasMatch() ) {
			do_body(srcBlock0, cursor);
			do_list(srcBlock, cursor, buf);		//	"- " or "- [ ] "
		} else if( re_numlist.match(buf).hasMatch() ) {
			do_body(srcBlock0, cursor);
			do_numlist(srcBlock, cursor, buf);
		} else if( buf.startsWith("> ") ) {
			do_body(srcBlock0, cursor);
			do_quote(srcBlock, cursor, buf);
		} else if( buf.startsWith("```CSV", Qt::CaseInsensitive) ) {
			do_body(srcBlock0, cursor);
			do_CSV(srcBlock, cursor);
		} else if( buf.startsWith("```keisen", Qt::CaseInsensitive) ) {
			do_body(srcBlock0, cursor);
			do_keisen_block(srcBlock, cursor);
		} else if( buf.startsWith("```") ) {
			do_body(srcBlock0, cursor);
			do_code(srcBlock, cursor);
		} else if( isTableLine(buf, m_tableTokens) && m_ln + 1 < m_lst.size() && isTableHyphenLine(m_lst[m_ln+1], m_tableAlign) ) {
			do_body(srcBlock0, cursor);
			do_table(srcBlock, cursor);
		} else {
			if( isUnderlineHeading(buf) && do_underlineHeading(cursor, buf) )
				continue;		//	アンダーライン見出しだった場合
			if( m_bodyList.isEmpty() ) {
				m_bodyLineNum = m_ln;
				srcBlock0 = srcBlock;
			}
			updateCharFlags(srcBlock);
			printCharFlags(srcBlock);
			m_bodyList += buf;
		}
	}
	QTextBlock srcBlock = doc->findBlockByNumber(m_bodyLineNum);
	do_body(srcBlock, cursor, true);
	cursor.endEditBlock();
	m_processing = false;
}
void MarkdownPreview::do_table(QTextBlock& srcBlock, QTextCursor& cursor) {
	if( m_isPrevLineEmpty ) {
		cursor.insertBlock();
		cursor.insertText("\n");
	}
	QString buf = m_lst[m_ln] + "\n" + m_lst[m_ln+1] /*+ "\n"*/;
	srcBlock.setUserState(US_TABLE);
	srcBlock = srcBlock.next();
	srcBlock.setUserState(US_TABLE);
	srcBlock = srcBlock.next();
	m_ln += 2;
	while( m_ln < m_lst.size() && isTableLine(m_lst[m_ln], m_tableTokens) ) {
		buf += "\n" + m_lst[m_ln++];
		srcBlock.setUserState(US_TABLE);
		srcBlock = srcBlock.next();
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

void MarkdownPreview::do_heading(QTextBlock& srcBlock, QTextCursor& cursor, QString buf) {
	int i = 1;
	while( i < buf.size() && buf[i] == '#' ) ++i;
	int h = std::min(6, i);		//	[1, 6]
	while( i < buf.size() && buf[i] == u' ' ) ++i;
	BlockData *data = getBlockData(srcBlock);
	for(int k = 0; k < i; ++k)
		data->m_charFlags[k] = PCF_HEADING;
	srcBlock.setUserData(data);
	srcBlock.setUserState(US_HEADING);
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
void MarkdownPreview::do_CSV(QTextBlock& srcBlock, QTextCursor& cursor) {		//	cursor: プレビューカーソル
	srcBlock.setUserState(US_CSV_BLOCK);
	BlockData *data = getBlockData(srcBlock);
	data->m_charFlags.fill(PCF_CSV);
	srcBlock.setUserData(data);
	QList<QStringList> ll;
	int max_clmn = 0;
	bool inQuotes = false;
	bool inComment = false;
	bool commented = false;		//	行単位でコメントアウトされた
	QStringList fields;
	while( ++m_ln < m_lst.size() && !m_lst[m_ln].startsWith("```") ) {
		srcBlock = srcBlock.next();
		srcBlock.setUserState(US_CSV_BLOCK);
		data = getBlockData(srcBlock);
		assert(srcBlock.text() == m_lst[m_ln]);
		assert(srcBlock.text().size() == data->m_charFlags.size());
		inQuotes = parseCsvLine(fields, m_lst[m_ln], inQuotes, inComment, commented, data);
		if( !inQuotes && !inComment && !commented ) {
			max_clmn = qMax(max_clmn, fields.size());
			ll.push_back(fields);
			srcBlock.setUserData(data);
		}
	}
	if( (srcBlock = srcBlock.next()).isValid() ) {
		srcBlock.setUserState(US_CSV_BLOCK);
		BlockData *data = getBlockData(srcBlock);
		data->m_charFlags.fill(PCF_CSV);
		srcBlock.setUserData(data);
	}
	if( ll.isEmpty() ) return;
	static QRegularExpression re("^[+-]?(\\d+\\.\\d*|\\d+|\\.\\d+)$");
	cursor.beginEditBlock();
	if( m_isPrevLineEmpty ) {
		cursor.insertBlock();
		cursor.insertText("\n");
	}
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
void MarkdownPreview::do_keisen_block(QTextBlock& srcBlock, QTextCursor& cursor) {
	srcBlock.setUserState(US_KEISEN_BLOCK);
	QFont font;
	QStringView buf = m_lst[m_ln].mid(QString("```keisen").size());
	QColor bgcolor = g.m_keisenBlockColor;
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
		srcBlock = srcBlock.next();
		srcBlock.setUserState(US_KEISEN_BLOCK);
		width = qMax(width, fm.horizontalAdvance(m_lst[m_ln])+10);
	}
	srcBlock = srcBlock.next();
	srcBlock.setUserState(US_KEISEN_BLOCK);
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
	cursor.block().setUserState(US_KEISEN_BLOCK);
	QTextBlockFormat blockFormat;
	blockFormat.setTopMargin(lineHeight/2);
	cursor.mergeBlockFormat(blockFormat);
	cursor.insertImage(img);
	cursor.insertBlock();
	//cursor.setBlockFormat(QTextBlockFormat());		//	トップマージンリセット
}
void MarkdownPreview::do_code(QTextBlock srcBlock, QTextCursor& cursor) {
	BlockData *data = getBlockData(srcBlock);
	data->m_charFlags.fill(PCF_CODE);
	srcBlock.setUserData(data);
	QString buf;
	while( ++m_ln < m_lst.size() && !m_lst[m_ln].startsWith("```") ) {
		srcBlock = srcBlock.next();
		if( !buf.isEmpty() ) buf += "\n";
		buf += m_lst[m_ln];
	}
	if( (srcBlock = srcBlock.next()).isValid() ) {
		BlockData *data = getBlockData(srcBlock);
		data->m_charFlags.fill(PCF_CODE);
		srcBlock.setUserData(data);
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
	font.setPointSizeF(12); // 精密な計算のために setPointSize より setPointSizeF がおすすめ
	// 3. レタースペーシング（文字間隔）を設定
	// 100% よりわずかに小さくすることで、PDF出力時の隙間を埋めます
	font.setLetterSpacing(QFont::PercentageSpacing, 99.5);
	// 4. 設定済みの QFont を QTextCharFormat に適用
	codeCharFormat.setFont(font);
#endif
	// フレームの書式を設定
	QTextFrameFormat frameFormat;
	frameFormat.setBackground(g.m_codeBlockColor);
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
void MarkdownPreview::do_quote(QTextBlock &srcBlock, QTextCursor& cursor, QString buf) {
	BlockData* data = getBlockData(srcBlock);
	//for(int i = 0; i < mch.capturedLength(); ++i)
	data->m_charFlags[0] = data->m_charFlags[1] = PCF_QUOTE;	//	"> " 固定
	srcBlock.setUserData(data);
	buf = buf.mid(2);
	while( ++m_ln < m_lst.size() ) {
		if( !m_lst[m_ln].startsWith("> ") ) break;
		buf += u"  \n" + m_lst[m_ln].mid(2);
	}
	//cursor.insertMarkdown(buf);
	// 1. フレームの書式を設定
	QTextFrameFormat frameFormat;
	frameFormat.setBackground(g.m_quoteColor);
	frameFormat.setMargin(0);					  // 外側の余白
	frameFormat.setPadding(10);					  // 内側の余白（文字と端の隙間）
	frameFormat.setBorder(0);					  // 枠線はなし
	// 2. フレームを挿入（この中にカーソルが入る）
	cursor.insertFrame(frameFormat);
	// 3. フレームの中で Markdown を挿入
	cursor.insertText(buf);
	cursor.movePosition(QTextCursor::End);
	this->setTextCursor(cursor);
	//cursor.insertBlock();
	QTextBlockFormat blockFormat;
	cursor.setBlockFormat(blockFormat);
	--m_ln;
	m_nEmptyLines = 0;
}
void MarkdownPreview::do_numlist(QTextBlock srcBlock, QTextCursor& cursor, QString buf) {
	bool init = true;
	auto match = re_numlist.match(m_lst[m_ln]);
	while( match.hasMatch() ) {
		BlockData* data = getBlockData(srcBlock);
		for(int i = 0; i < match.capturedLength(); ++i)
			data->m_charFlags[i] = PCF_NUM_LIST;
		srcBlock.setUserData(data);
		if( !init )
			buf += u'\n' + m_lst[m_ln];
		if( ++m_ln >= m_lst.size() ) break;
		srcBlock = srcBlock.next();
		init = false;
		match = re_numlist.match(m_lst[m_ln]);
	}
#if 0
	while( ++m_ln < m_lst.size() ) {
		if( !re_numlist.match(m_lst[m_ln]).hasMatch() ) break;			//	"1. " 連番行が終わった場合
		srcBlock = srcBlock.next();
		buf += u'\n' + m_lst[m_ln];
	}
#endif
	cursor.insertMarkdown(buf);
	cursor.insertBlock();
	QTextBlockFormat blockFormat;
	cursor.setBlockFormat(blockFormat);
	--m_ln;
	m_nEmptyLines = 0;
}
void MarkdownPreview::do_list(QTextBlock srcBlock, QTextCursor& cursor, QString buf) {
	if( m_nEmptyLines >= 1 )
		cursor.insertBlock();			//	新規ブロック
	if( m_nSpaces > 0 )
		buf = QString(m_nSpaces, QChar(u' ')) + buf;
	static QRegularExpression re_checkbox(R"(^( *)- \[[ xX]\] )");
	int pos = cursor.position();
	int n_item = 1;
	int ln = m_ln;
	auto match = re_checkbox.match(buf);
	bool is_checkbox = match.hasMatch();		//	チェックボックス（"- [ ] "）の場合
	if( is_checkbox ) {
		for(;;) {
			BlockData* data = getBlockData(srcBlock);
			for(int i = 0; i < match.capturedLength(); ++i)
				data->m_charFlags[i] = PCF_LIST_MARK;
			srcBlock.setUserData(data);
			if( ++m_ln >= m_lst.size() ) break;
			match = re_checkbox.match(m_lst[m_ln]);
			if( !match.hasMatch() ) break;
			srcBlock = srcBlock.next();
			//data = getBlockData(srcBlock);
			buf += u'\n' + m_lst[m_ln];
			++n_item;
		}
#if 0
		while( ++m_ln < m_lst.size() ) {
			if( !re_checkbox.match(m_lst[m_ln]).hasMatch() ) break;
			buf += u'\n' + m_lst[m_ln];
			++n_item;
		}
#endif
	} else {		//	リストの場合
		//static QRegularExpression re(R"(^( *)- )");
		auto mch = re_list.match(srcBlock.text());
		BlockData* data = getBlockData(srcBlock);
		for(int i = 0; i < mch.capturedLength(); ++i)
			data->m_charFlags[i] = PCF_LIST_MARK;
		srcBlock.setUserData(data);
		printCharFlags(srcBlock);
		bool isPrevlist = true;
		bool spc2Prev = false;
		while( ++m_ln < m_lst.size() ) {
			srcBlock = srcBlock.next();
			QString text = m_lst[m_ln];
			if( text.isEmpty() ) break;		//	空行だった場合
			auto mch = re_list.match(text);
			if( mch.hasMatch() ) {	//	リスト行
				buf += u'\n' + text;
				isPrevlist = true;
				//int length = mch.capturedLength();
				BlockData* data = getBlockData(srcBlock);
				for(int i = 0; i < mch.capturedLength(); ++i)
					data->m_charFlags[i] = PCF_LIST_MARK;
				srcBlock.setUserData(data);
				printCharFlags(srcBlock);
			} else {	//	非リスト行の場合
				if( re_block.match(text).hasMatch() )	//	ブロック行の場合
					break;
				bool spc2 = text.endsWith("  ");
				text = text.trimmed();
				if( isPrevlist || spc2Prev )
					buf += u"<br/>" + text;
				else
					buf += u"\n" + text;
				isPrevlist = false;
				spc2Prev = spc2;
			}
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
		QTextBlock srcBlock = document()->findBlock(pos);
		for(int i = 0; i < n_item; ++i) {
			//srcBlock.setUserState(ln++);
			srcBlock.setUserState(US_CHECKBOX);
			srcBlock = srcBlock.next();
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
void MarkdownPreview::scrollToTop(const QTextCursor &cursor) {
	int visualLineNum = getVisualLineNumber(cursor);
	this->scrollToTop(visualLineNum);
}
int MarkdownPreview::getVisualLineNumber(const QTextCursor &cursor) const {
	//int visualLineNum = 0;
	QTextBlock targetBlock = cursor.block();
	return targetBlock.blockNumber();
}
#endif
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
	const QChar ch = context.m_anchorChar;
	int nth = context.m_nth;
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
				++ix;
			} else {
				block = block.next();
				ix = 0;
			}
		}
	}
	if( block.isValid() ) {
		return block.position() + ix + context.m_offset;
	} else
		return -1;
}
void MarkdownPreview::setCursorByContext(const PosContext &context, const PosContext &acontext) {
	if( m_processing ) return;		//	再入禁止
	qDebug() << "MarkdownPreview::setCursorByContext(context)";
	qDebug() << "context.ancharChar = " << context.m_anchorChar << ", nth = " << context.m_nth << ", offset = " << context.m_offset <<
					", srcHBNum = " << context.m_srcHBlockNum << ", prvHBNum = " << context.m_prvHBlockNum;
	if( acontext.m_nth != 0 ) {
		qDebug() << "acontext.ancharChar = " << acontext.m_anchorChar << ", nth = " << acontext.m_nth << ", offset = " << acontext.m_offset <<
					", srcHBNum = " << acontext.m_srcHBlockNum << ", prvHBNum = " << acontext.m_prvHBlockNum;
	}
	m_processing = true;
	QTextCursor cursor = textCursor();
	if( acontext.m_nth == 0 ) {		//	非選択状態
		int pos = findPosition(context);
		if( pos >= 0 ) {
			cursor.setPosition(pos);
			setTextCursor(cursor);
		}
	} else {
		int pos = findPosition(acontext);
		if( pos >= 0 ) {
			cursor.setPosition(pos);
			pos = findPosition(context);
			if (pos >= 0) {
				cursor.setPosition(pos, QTextCursor::KeepAnchor);
				setTextCursor(cursor);
			}
		}
	}
	m_processing = false;
}
PosContext MarkdownPreview::contextAt(int pos) {	//	pos 位置から PosContext を構築
	PosContext pc;
	auto *doc = document();
	QTextBlock block = doc->findBlock(pos);
	if( block.userState() == US_KEISEN_BLOCK ) {
		pc.m_anchorChar = QChar(U_KEISEN_BLOCK);
	} else {
		//pc.m_chPrev = pos != block.position() ? doc->characterAt(pos-1) : QChar();
		auto chat = doc->characterAt(pos);
		while( pos > 0 && (chat == endOfCell || chat == u' ' || chat == QChar::ParagraphSeparator) ) {
			pc.m_offset += 1;
			chat = doc->characterAt(--pos);
		}
#if 0
		if( pos > 0 && (chat == endOfCell || chat == u' ') ) {
			//while( pos > 0 && (chat == endOfCell || chat == u' ') ) {
				pc.m_offset += 1;
				chat = doc->characterAt(--pos);
			//}
		} else {
			while( pos > 0 && chat == QChar::ParagraphSeparator ) {
				pc.m_offset += 1;
				chat = doc->characterAt(--pos);
			}
		}
#endif
		pc.m_anchorChar = chat != QChar::ParagraphSeparator ? chat : QChar();
	}
	//pc.m_anchorChar = chat;
	while( block.userState() != US_HEADING ) {		//	直前の見出し行を探す
		if( !block.previous().isValid() )
			break;
		block = block.previous();
	}
	pc.m_prvHBlockNum = block.blockNumber();
	//	pos の {chPrev, chNext} が見出し行先頭から何番目かを計算
	int count = 1;
	if( pc.m_anchorChar == QChar() ) {		//	行末の場合
		while( block.isValid() ) {
			if( block.position() + block.text().length() >= pos )
				break;
			++count;
			block = block.next();
		}
	} else if( pc.m_anchorChar == QChar(U_KEISEN_BLOCK) ) {		//	罫線ブロックの場合
		while( block.isValid() ) {
			if( block.userState() == U_KEISEN_BLOCK ) {
				while( block.userState() == U_KEISEN_BLOCK ) {
					block = block.next();
					if( !block.isValid() ) break;
				}
				if( block.position() > pos )
					break;
				++count;
			} else
				block = block.next();
		}
	} else {
		for (int i = block.position(); i < pos; ++i) {
			QChar chAt = doc->characterAt(i);
			if (chAt == QChar::ParagraphSeparator)		//	改行記号の場合
				chAt = QChar();
			if( chAt == pc.m_anchorChar )
				++count;
		}
	}
	pc.m_nth = count;
	return pc;
}
