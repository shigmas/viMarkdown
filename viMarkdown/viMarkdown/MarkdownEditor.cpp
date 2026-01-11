#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextBlock>
#include <QSyntaxHighlighter>
#include <QPainter>
#include <QRegularExpression>
#include <QInputMethod>
#include "MarkdownEditor.h"
#include "MainWindow.h"

class LnAreaWidget : public QWidget {
public:
	LnAreaWidget(QWidget *parent = nullptr) : QWidget(parent) {}

protected:
    void paintEvent(QPaintEvent *event) override {
    	QPainter painter(this);
    	QRect rc = event->rect();
    	//rc.setWidth(rc.width());
        painter.fillRect(rc, QColor("lightgray"));
        MarkdownEditor *mdEditor = (MarkdownEditor*)parent();
        mdEditor->lnAreaPaintEvent(event);
    }
};

const int KEISEN_CODE_BEGIN = 0x2500;
const int KEISEN_CODE_END = 0x2580;			//	0x257f まで有効

enum KeisenDir {		//	罫線各文字の連結方向＆罫線種フラグ
	None  = 0x00,
	Up	  = 0x01,
	Down  = 0x02,
	Left  = 0x04,
	Right = 0x08,
	Thick = 4,			//	太罫線
	ThickUp    = Up << Thick,
	ThickDown  = Down << Thick,
	ThickLeft  = Left << Thick,
	ThickRight = Right << Thick,
	DblLine = 8,		//	二重線
	DblUp	 = Up << DblLine,
	DblDown  = Down << DblLine,
	DblLeft  = Left << DblLine,
	DblRight = Right << DblLine,
};
static const unsigned short keisenTable[KEISEN_CODE_END - KEISEN_CODE_BEGIN] = {
	/* 00-03 ─ ━ │ ┃ */
	Left|Right, ThickLeft|ThickRight, Up|Down, ThickUp|ThickDown,
	/* 04-0B (破線・点線系) -> None */
	None, None, None, None, None, None, None, None,
	/* 0C-0F ┌ ┍ ┎ ┏ */
	Down|Right, Down|ThickRight, ThickDown|Right, ThickDown|ThickRight,
	/* 10-13 ┐ ┑ ┒ ┓ */
	Down|Left, Down|ThickLeft, ThickDown|Left, ThickDown|ThickLeft,
	/* 14-17 └ ┕ ┖ ┗ */
	Up|Right, Up|ThickRight, ThickUp|Right, ThickUp|ThickRight,
	/* 18-1B ┘ ┙ ┚ ┛ */
	Up|Left, Up|ThickLeft, ThickUp|Left, ThickUp|ThickLeft,
	/* 1C-1F ├ ┝ ┞ ┟ */
	Up|Down|Right, Up|Down|ThickRight, ThickUp|Down|Right, Up|ThickDown|Right,
	/* 20-23 ┠ ┡ ┢ ┣ */
	ThickUp|Down|Right, ThickUp|ThickDown|Right, ThickUp|Down|ThickRight, ThickUp|ThickDown|Right,
	/* 24-27 ┤ ┥ ┦ ┧ */
	Up|Down|Left, Up|Down|ThickLeft, ThickUp|Down|Left, Up|ThickDown|Left,
	/* 28-2B ┨ ┩ ┪ ┫ */
	ThickUp|Down|Left, ThickUp|ThickDown|Left, ThickUp|Down|ThickLeft, ThickUp|ThickDown|Left,
	/* 2C-2F ┬ ┭ ┮ ┯ */
	Down|Left|Right, Down|Left|ThickRight, Down|ThickLeft|Right, Down|ThickLeft|ThickRight,
	/* 30-33 ┰ ┱ ┲ ┳ */
	ThickDown|Left|Right, ThickDown|Left|ThickRight, ThickDown|ThickLeft|Right, ThickDown|ThickLeft|ThickRight,
	/* 34-37 ┴ ┵ ┶ ┷ */
	Up|Left|Right, Up|Left|ThickRight, Up|ThickLeft|Right, Up|ThickLeft|ThickRight,
	/* 38-3B ┸ ┹ ┺ ┻ */
	ThickUp|Left|Right, ThickUp|Left|ThickRight, ThickUp|ThickLeft|Right, ThickUp|ThickLeft|ThickRight,
	/* 3C-3F ┼ ┽ ┾ ┿ */
	Up|Down|Left|Right, Up|Down|Left|ThickRight, Up|Down|ThickLeft|Right, Up|Down|ThickLeft|ThickRight,
	/* 40-43 ╀ ╁ ╂ ╃ */
	ThickUp|Down|Left|Right, Up|ThickDown|Left|Right, Up|Down|ThickLeft|ThickRight, ThickUp|Down|Left|ThickRight,
	/* 44-47 ╄ ╅ ╆ ╇ */
	ThickUp|Down|ThickLeft|Right, Up|ThickDown|Left|ThickRight, Up|ThickDown|ThickLeft|Right, ThickUp|ThickDown|Left|Right,
	/* 48-4B ╈ ╉ ╊ ╋ */
	ThickUp|ThickDown|Left|ThickRight, ThickUp|Down|ThickLeft|ThickRight, Up|ThickDown|ThickLeft|ThickRight, ThickUp|ThickDown|ThickLeft|ThickRight,
	/* 4C-4F (破線太) -> None */
	None, None, None, None,
	/* 50-53 ═ ║ ╒ ╓ (ここから二重線) */
	DblLeft|DblRight, DblUp|DblDown, DblDown|DblRight, DblDown|DblRight,
	/* 54-57 ╔ ╕ ╖ ╗ */
	DblDown|DblRight, DblDown|DblLeft, DblDown|DblLeft, DblDown|DblLeft,
	/* 58-5B ╘ ╙ ╚ ╛ */
	DblUp|DblRight, DblUp|DblRight, DblUp|DblRight, DblUp|DblLeft,
	/* 5C-5F ╜ ╝ ╞ ╟ */
	DblUp|DblLeft, DblUp|DblLeft, DblUp|DblDown|DblRight, DblUp|DblDown|DblRight,
	/* 60-63 ╠ ╡ ╢ ╣ */
	DblUp|DblDown|DblRight, DblUp|DblDown|DblLeft, DblUp|DblDown|DblLeft, DblUp|DblDown|DblLeft,
	/* 64-67 ╤ ╥ ╦ ╧ */
	DblDown|DblLeft|DblRight, DblDown|DblLeft|DblRight, DblDown|DblLeft|DblRight, DblUp|DblLeft|DblRight,
	/* 68-6B ╨ ╩ ╪ ╫ */
	DblUp|DblLeft|DblRight, DblUp|DblLeft|DblRight, DblUp|DblDown|DblLeft|DblRight, DblUp|DblDown|DblLeft|DblRight,
	/* 6C ╬ */
	DblUp|DblDown|DblLeft|DblRight,
	/* 6D-7F (丸角・斜め・終端・混合細太直線) -> None */
	None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None
};
const ushort revKeisenTable[256] = {
    0x0000, 0x2575, 0x2577, 0x2502, 0x2574, 0x2518, 0x250c, 0x251c, // 0x00-0x07
    0x2576, 0x2514, 0x2510, 0x2524, 0x2500, 0x2534, 0x252c, 0x253c, // 0x08-0x0f
    0x2579, 0x2579, 0x257d, 0x257d, 0x2519, 0x2519, 0x251c, 0x251c, // 0x10-0x17 (UpH)
    0x2515, 0x2515, 0x2524, 0x2524, 0x2534, 0x2534, 0x253c, 0x253c, // 0x18-0x1f
    0x257b, 0x257f, 0x257b, 0x257f, 0x2511, 0x2518, 0x2511, 0x251c, // 0x20-0x27 (DownH)
    0x2510, 0x2524, 0x2510, 0x2524, 0x252c, 0x2534, 0x252c, 0x253c, // 0x28-0x2f
    0x2503, 0x2503, 0x2503, 0x2503, 0x2521, 0x2519, 0x250c, 0x2521, // 0x30-0x37 (UpH+DownH)
    0x2529, 0x2515, 0x2510, 0x2529, 0x2537, 0x2534, 0x252c, 0x253c, // 0x38-0x3f
    0x2578, 0x251b, 0x2513, 0x2527, 0x2578, 0x251b, 0x2513, 0x2527, // 0x40-0x47 (LeftH)
    0x2518, 0x2518, 0x2510, 0x2524, 0x2500, 0x2534, 0x252c, 0x253c, // 0x48-0x4f
    0x251a, 0x251a, 0x2513, 0x2527, 0x251a, 0x251a, 0x2513, 0x2527, // 0x50-0x57
    0x2518, 0x2518, 0x2510, 0x2524, 0x2534, 0x2534, 0x252c, 0x253c, // 0x58-0x5f
    0x2512, 0x251b, 0x2512, 0x251c, 0x2512, 0x251b, 0x2512, 0x251c, // 0x60-0x67
    0x2510, 0x2524, 0x2510, 0x2524, 0x252c, 0x2534, 0x252c, 0x253c, // 0x68-0x6f
    0x2501, 0x2535, 0x252d, 0x253f, 0x2501, 0x2535, 0x252d, 0x253f, // 0x70-0x77
    0x2534, 0x2534, 0x252c, 0x253c, 0x2534, 0x2534, 0x252c, 0x253c, // 0x78-0x7f
    0x257a, 0x2517, 0x250f, 0x2523, 0x2501, 0x2535, 0x252d, 0x253f, // 0x80-0x87 (RightH)
    0x257a, 0x2517, 0x250f, 0x2523, 0x2501, 0x2535, 0x252d, 0x253f, // 0x88-0x8f
    0x2516, 0x2516, 0x250f, 0x2523, 0x2501, 0x2535, 0x252d, 0x253f, // 0x90-0x97
    0x2516, 0x2516, 0x250f, 0x2523, 0x2501, 0x2535, 0x252d, 0x253f, // 0x98-0x9f
    0x202a, 0x2517, 0x202a, 0x2523, 0x2501, 0x2535, 0x252d, 0x253f, // 0xa0-0xa7
    0x202a, 0x2517, 0x202a, 0x2523, 0x2501, 0x2535, 0x252d, 0x253f, // 0xa8-0xaf
    0x250b, 0x250b, 0x250b, 0x250b, 0x2501, 0x2535, 0x252d, 0x253f, // 0xb0-0xb7
    0x250b, 0x250b, 0x250b, 0x250b, 0x2501, 0x2535, 0x252d, 0x253f, // 0xb8-0xbf
    0x2501, 0x2536, 0x252e, 0x2040, 0x2501, 0x2536, 0x252e, 0x2040, // 0xc0-0xc7 (LeftH+RightH)
    0x2501, 0x2536, 0x252e, 0x2040, 0x2501, 0x2536, 0x252e, 0x2040, // 0xc8-0xcf
    0x2539, 0x2539, 0x252e, 0x2040, 0x2501, 0x2536, 0x252e, 0x2040, // 0xd0-0xd7
    0x2539, 0x2539, 0x252e, 0x2040, 0x2501, 0x2536, 0x252e, 0x2040, // 0xd8-0xdf
    0x253a, 0x253a, 0x253a, 0x253a, 0x2501, 0x2536, 0x252e, 0x2040, // 0xe0-0xe7
    0x253a, 0x253a, 0x253a, 0x253a, 0x2501, 0x2536, 0x252e, 0x2040, // 0xe8-0xef
    0x254b, 0x254b, 0x254b, 0x254b, 0x254b, 0x254b, 0x254b, 0x254b, // 0xf0-0xf7
    0x254b, 0x254b, 0x254b, 0x254b, 0x254b, 0x254b, 0x254b, 0x254b  // 0xf8-0xff
};
bool isKeisenChar(QChar ch) {
	return ch.unicode() >= KEISEN_CODE_BEGIN && ch.unicode() < KEISEN_CODE_END;
}

class MarkdownHighlighter : public QSyntaxHighlighter {
public:
	MarkdownHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
	{
		boldFormat.setFontWeight(QFont::Bold);
		boldRegex = QRegularExpression(R"(\*\*([^\*]+)\*\*)");
	}

protected:
	void highlightBlock(const QString &text) override {
		if (text.startsWith("#")) {
			QTextCharFormat fmt_darkred;
			fmt_darkred.setForeground(QColor("darkred"));
			setFormat(0, text.length(), fmt_darkred);
		} else {
			// デフォルトの色（黒）
			setFormat(0, text.length(), QColor("black"));
			auto it = boldRegex.globalMatch(text);
			while (it.hasNext()) {
				QRegularExpressionMatch match = it.next();
				// マッチした範囲（開始位置と長さ）にフォーマットを適用
				setFormat(match.capturedStart(), match.capturedLength(), boldFormat);
			}
		}
	}
private:
	QTextCharFormat boldFormat;
	QRegularExpression boldRegex;
};
//----------------------------------------------------------------------
MarkdownEditor::MarkdownEditor(const MainWindow* mainWindow, QWidget *parent)
	: m_mainWindow(mainWindow), QPlainTextEdit(parent)
{
	m_highlighter = new MarkdownHighlighter(this->document());
#if 1
	QTextCursor cursor = this->textCursor();
	QTextBlockFormat format;
	format.setLineHeight(150, QTextBlockFormat::ProportionalHeight); // 1.5倍
	cursor.setBlockFormat(format);
#endif
	QFont font("MS Gothic");
	//QFont font("Consolas");
	font.setPointSize(12);		// 12ptに設定
	font.setFixedPitch(true);	// 明示的に固定幅として扱う設定
	this->setFont(font);
	//QFontMetrics fm(font());
	//int lnAreaWidth = fm.horizontalAdvance('9') * 6;
	setViewportMargins(lnAreaWidth(), 0, 0, 0);
	m_lnAreaWidget = new LnAreaWidget(this);
	connect(this, &MarkdownEditor::updateRequest, this, &MarkdownEditor::updateLnArea);
	connect(this, &MarkdownEditor::cursorPositionChanged, this, &MarkdownEditor::onCurPosChanged);
	connect(document(), &QTextDocument::contentsChange, this, &MarkdownEditor::onContentsChanged);
}
void MarkdownEditor::inputMethodEvent(QInputMethodEvent *event) {
	m_isComposing = !event->preeditString().isEmpty();
    QPlainTextEdit::inputMethodEvent(event);
}
void MarkdownEditor::keyPressEvent(QKeyEvent *e) {
	//static QRegularExpression re(R"(^\d[\.\)] )");
	static QRegularExpression re(R"(^\d\. )");
	static QRegularExpression re2(R"(^\d\) )");
	QTextCursor cursor = this->textCursor();
	if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
		QTextBlock currentBlock = cursor.block();
		if( (e->modifiers() & Qt::ShiftModifier) == 0 &&		//	Shift + Enter でない
			cursor.position() != currentBlock.position())		//	行頭にいない場合
		{
			QString text = currentBlock.text();
			int n = 0;
			while( n < text.length() && text[n].isSpace() ) ++n;
			QString atxt = text.left(n);
			const QString mtxt = text.mid(n);
			if( mtxt == "- " || mtxt == "- [ ] " || mtxt == "- [x] " || mtxt == "- [X] " ) {
				cursor.movePosition(QTextCursor::StartOfBlock);
				cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
				cursor.deleteChar();
			} else if( mtxt.startsWith("- [ ] ") )
				atxt += "- [ ] ";
			else if( mtxt.startsWith("- [x] ") )
				atxt += "- [x] ";
			else if( mtxt.startsWith("- [X] ") )
				atxt += "- [X] ";
			else if( mtxt.startsWith("- ") )
				atxt += "- ";
			else if( re.match(mtxt).hasMatch())
				atxt += "1. ";
			else if( re2.match(mtxt).hasMatch())
				atxt += "1) ";
			else if( mtxt.startsWith("> ") )
				atxt += "> ";
			//cursor.insertText("\n" + atxt);
			QPlainTextEdit::keyPressEvent(e);		//	改行挿入
			if( !atxt.isEmpty() )
				cursor.insertText(atxt);
			// カーソル位置を画面内に維持
			this->ensureCursorVisible();
			return;
		} else {
			cursor.insertText("\n");
			return;
		}
	} else if (e->key() == Qt::Key_Tab ) {
		emit tab_pressed();
		return;
	} else if (e->key() == Qt::Key_Escape ) {
		emit esc_pressed();
		QTextCursor cursor = this->textCursor();
		if (cursor.hasSelection()) {
			cursor.clearSelection();
			setTextCursor(cursor);
		}
		return;
	} else if( m_mainWindow->isKeisenMode() ) {
		bool erase = (e->modifiers() & Qt::ShiftModifier) != 0;
		if( (e->modifiers() & Qt::ControlModifier) != 0 /*|| erase*/ ) {
			if (e->key() == Qt::Key_Right ) {
				do_keisen_right(erase);
				return;
			} else if (e->key() == Qt::Key_Left) {
				do_keisen_left(erase);
				return;
			} else if (e->key() == Qt::Key_Up) {
				do_keisen_up(erase);
				return;
			} else if (e->key() == Qt::Key_Down) {
				do_keisen_down(erase);
				return;
			}
		}
	}
	QPlainTextEdit::keyPressEvent(e);	// Enter 以外のキーは通常通りの処理
}
int MarkdownEditor::nColumn(const QString &text) const {
	QFontMetrics fm(font());
	int halfWidth = fm.horizontalAdvance(u'9'); 
	int fullWidth = fm.horizontalAdvance(text);
	return fullWidth / halfWidth;
}
int getVisualColumn(QTextCursor cursor, QPlainTextEdit *editor) {
	// 1. 行頭から現在のカーソル位置までのテキストを取得
	QString text = cursor.block().text().left(cursor.positionInBlock());
	// 2. フォントの計測準備
	QFontMetrics fm(editor->font());
	// 3. 半角文字（例: 'A'）1文字分の幅を取得
	int halfWidth = fm.horizontalAdvance("A"); 
	// 4. 行頭からカーソルまでの全テキストの幅を取得
	int fullWidth = fm.horizontalAdvance(text);
	// 5. 割り算で「半角何文字分か」を出す
	return fullWidth / halfWidth;
}
QString getUpSrcString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'←' || txt[ix] == u'└' ) return "└";
			if( txt[ix] == u'→' || txt[ix] == u'┘' ) return "┘";
			if( txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┐' || txt[ix] == u'┤' ) return "┤";
			if( txt[ix] == u'┌' || txt[ix] == u'├' ) return "├";
			if( txt[ix] == u'┬' || txt[ix] == u'┼' ) return "┼";
			if( txt[ix] == u'─' )
				if( ix < 2 )		//	undone: 左に文字がある場合
					return "└";
				else
					return "┴";
		}
		return "│";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'─' || txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' )
				return txt[ix];		//	変化無し
			if( txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' )
				return "─";
			if( txt[ix] == u'┼' ) return "┬";
			if( txt[ix] == u'├' ) return "┌";
			if( txt[ix] == u'┤' ) return "┐";
		}
		return "  ";
	}
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getUpDstString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'│' || txt[ix] == u'┐' || txt[ix] == u'┌' || 
				txt[ix] == u'┬' || txt[ix] == u'├' || txt[ix] == u'┤'|| txt[ix] == u'┼' )
			{
				return txt[ix];
			}
			if( txt[ix] == u'┘' ) return "┤";
			if( txt[ix] == u'└' ) return "├";
			if( txt[ix] == u'┴' ) return "┼";
			if( txt[ix] == u'─' ) {
				if( ix > 0 && (txt[ix-1] == u'─' || txt[ix-1] == u'←') )
					return "┬";
				else
					return "┌";
			}
		}
		return "↑";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'─' || txt[ix] == u'┘' || txt[ix] == u'└' || txt[ix] == u'┴' )
				return txt[ix];		//	変化無し
			if( txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'├' ) return "─";
			if( txt[ix] == u'┼' ) return "┴";
			if( txt[ix] == u'├' ) return "└";
			if( txt[ix] == u'┤' ) return "┘";
		}
		return "  ";
	}
}
void MarkdownEditor::do_keisen_up(bool erase) {
	QTextCursor cursor = this->textCursor();
	if( cursor.blockNumber() == 0 ) return;		//	１行目では実行不可
	cursor.beginEditBlock();
	int vc0 = getVisualColumn(cursor, this); // 開始位置の表示列(VC)を保存
	int ix = cursor.positionInBlock();

// 1. 移動元（現在地）の置換
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 + 2 )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getUpSrcString(erase, cursor.block().text(), ix));
	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);

	// 2. 上の行へ移動し、表示列(vc0)に正確に合わせる
	cursor.movePosition(QTextCursor::Up);
	cursor.movePosition(QTextCursor::StartOfBlock); // 行頭から辿るのが全角混じりでも最も確実
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 )
		cursor.movePosition(QTextCursor::Right);
	
	// 足りなければスペース補完（これで確実に vc0 に到達する）
	int currentVc = getVisualColumn(cursor, this);
	if( currentVc < vc0 ) cursor.insertText(QString(vc0 - currentVc, u' '));

	// 3. 移動先の置換
	ix = cursor.positionInBlock();
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 + 2)
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor); // 1文字だけ選択
	cursor.insertText(getUpDstString(erase, cursor.block().text(), ix));	
	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);
	cursor.endEditBlock();
	setTextCursor(cursor);
}
QString getDownSrcString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'→' || txt[ix] == u'┐' ) return "┐";
			if( txt[ix] == u'←' || txt[ix] == u'┌' ) return "┌";
			if( txt[ix] == u'┬' ) return "┬";
			if( txt[ix] == u'┘' || txt[ix] == u'┤' ) return "┤";
			if( txt[ix] == u'└' || txt[ix] == u'├' ) return "├";
			if( txt[ix] == u'┴' || txt[ix] == u'┼' ) return "┼";
			if( txt[ix] == u'─' )
				if( ix < 2 )		//	undone: 左に文字がある場合
					return "┌";
				else
					return "┬";
		}
		return "│";
	} else {
		if( ix < txt.size() ) {
			// 変化無し（すでに下に線がない）
			if( txt[ix] == u'─' || txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' || txt[ix] == u'←' || txt[ix] == u'→' ) return txt[ix];
			// 縦要素が消えて横棒だけ残る
			if( txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' ) return "─";
			// Ｔ字・角に格下げ
			if( txt[ix] == u'┼' ) return "┴";
			if( txt[ix] == u'├' ) return "└";
			if( txt[ix] == u'┤' ) return "┘";
			if( txt[ix] == u'│' ) return "	";
		}
		return "  ";
	}
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getDownDstString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'│' || txt[ix] == u'┘' || txt[ix] == u'└' || 
				/*txt[ix] == u'┬' ||*/ txt[ix] == u'├' || txt[ix] == u'┤'|| txt[ix] == u'┼' )
			{
				return txt[ix];
			}
			if( txt[ix] == u'─' ) return "┴";
			if( txt[ix] == u'┐' ) return "┤";
			if( txt[ix] == u'┌' ) return "├";
			if( txt[ix] == u'┴' ) return "┼";		//	undone: この下も参照？
			if( txt[ix] == u'┬' ) return "┼";
		}
		return "↓";
	} else {
		if( ix < txt.size() ) {
			// 変化無し（すでに上に線がない）
			if( txt[ix] == u'─' || txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' || txt[ix] == u'←' || txt[ix] == u'→' ) return txt[ix];
			// 縦要素が消えて横棒だけ残る
			if( txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' ) return "─";
			// Ｔ字・角に格下げ
			if( txt[ix] == u'┼' ) return "┬";
			if( txt[ix] == u'├' ) return "┌";
			if( txt[ix] == u'┤' ) return "┐";
		}
		return "  ";
	}
}
void MarkdownEditor::do_keisen_down(bool erase) {
	QTextCursor cursor = this->textCursor();
	cursor.beginEditBlock();
	int vc0 = getVisualColumn(cursor, this);
	int ix = cursor.positionInBlock();

	if (cursor.block() == cursor.document()->lastBlock()) {		//	カーソルが最終行にいる場合
		cursor.movePosition(QTextCursor::End);
		cursor.insertBlock();							//	新規行作成
		cursor.movePosition(QTextCursor::Left);			//	新規行作成で下に移動したカーソルを元の行に戻す
		while( getVisualColumn(cursor, this) > vc0 )
			cursor.movePosition(QTextCursor::Left);
	}
	// 1. 移動元（現在地）の置換
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 + 2 ) 
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	QString src = getDownSrcString(erase, cursor.block().text(), ix);
	cursor.insertText(src);
	cursor.movePosition(QTextCursor::Left);


	// 2. 下の行の準備（なければ作成）
	if (cursor.block() == cursor.document()->lastBlock()) {
		cursor.movePosition(QTextCursor::EndOfBlock);
		cursor.insertBlock(); // これでカーソル自体が新行へ移動する
	} else {
		cursor.movePosition(QTextCursor::Down);
	}

	// 3. 表示列(vc0)に正確に合わせる
	cursor.movePosition(QTextCursor::StartOfBlock);
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 )
		cursor.movePosition(QTextCursor::Right);
	int curVc = getVisualColumn(cursor, this);
	if( curVc < vc0 ) cursor.insertText(QString(vc0 - curVc, u' ')); // 足りない分を補完

	// 4. 移動先（下）の置換
	ix = cursor.positionInBlock(); // 位置が確定してからインデックス取得
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 + 2 )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getDownDstString(erase, cursor.block().text(), ix));

	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);
	cursor.endEditBlock();
	setTextCursor(cursor);
}
QString getLeftSrcString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'↓' || txt[ix] == u'┘' ) return "┘";
			if( txt[ix] == u'↑' || txt[ix] == u'┐' ) return "┐";
			if( txt[ix] == u'│' || txt[ix] == u'┤' ) return "┤";
			if( txt[ix] == u'└' || txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┌' || txt[ix] == u'┬' ) return "┬";
			if( txt[ix] == u'├' || txt[ix] == u'┼' ) return "┼";
		}
		return "─";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'│' || txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' )
				return txt[ix];		//	変化無し
			if( txt[ix] == u'┘' || txt[ix] == u'┐' || txt[ix] == u'┤' )
				return "│";
			if( txt[ix] == u'┼' ) return "├";
		}
		return "  ";
	}
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getLeftDstString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'─' || txt[ix] == u'┌' || txt[ix] == u'┼')
				return txt[ix];
			if( txt[ix] == u'│' || txt[ix] == u'├' ) return "├";
			if( txt[ix] == u'↑' || txt[ix] == u'└' ) return "└";
			if( txt[ix] == u'↓' || txt[ix] == u'┌' ) return "┌";
			if( txt[ix] == u'┘' || txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┐' || txt[ix] == u'┬' ) return "┬";
			if( txt[ix] == u'┤' ) return "┼";
		}
		return "←";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'┐' || txt[ix] == u'┘' || txt[ix] == u'┤' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' ) return "│";
			if( txt[ix] == u'┼' ) return "┤";
		}
		return "  ";
	}
}
void MarkdownEditor::do_keisen_left(bool erase) {
	QTextCursor cursor = this->textCursor();
	if( cursor.atBlockStart() ) return;				//	行頭にいる場合は無視
	int vc = getVisualColumn(cursor, this);
	QString src = "─";
	if( !cursor.atBlockEnd() ) {
		int ix = cursor.positionInBlock();
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		src = getLeftSrcString(erase, cursor.block().text(), ix);
		cursor.clearSelection();	//	選択解除、カーソル位置は movePosition() 後の位置、つまり選択末尾
	}
	while( !cursor.atBlockStart() && getVisualColumn(cursor, this) > vc - 2 )
		cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
	QString dst = getLeftDstString(erase, cursor.block().text(), cursor.positionInBlock());
	cursor.insertText(dst+src);
	cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 2);
	setTextCursor(cursor);
}
QString getRightSrcString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'↑' || txt[ix] == u'┌' ) return "┌";
			if( txt[ix] == u'↓' || txt[ix] == u'└' ) return "└";
			if( txt[ix] == u'│' || txt[ix] == u'├' ) return "├";
			if( txt[ix] == u'┘' || txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┐' || txt[ix] == u'┬' ) return "┬";
			if( txt[ix] == u'┤' || txt[ix] == u'┼' ) return "┼";
		}
		return "─";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'│' || txt[ix] == u'┘' || txt[ix] == u'┐' || txt[ix] == u'┤' )
				return txt[ix];		//	変化無し
			if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' )
				return "│";
			if( txt[ix] == u'┼' ) return "┤";
		}
		return "  ";
	}
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getRightDstString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'─' || txt[ix] == u'┐' || txt[ix] == u'┼')
				return txt[ix];
			if( txt[ix] == u'│' || txt[ix] == u'┤' ) return "┤";
			if( txt[ix] == u'↑' || txt[ix] == u'┐' ) return "┐";
			if( txt[ix] == u'↓' || txt[ix] == u'┘' ) return "┘";
			if( txt[ix] == u'└' || txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┌' || txt[ix] == u'┬' ) return "┬";
			if( txt[ix] == u'├' ) return "┼";
		}
		return "→";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'┐' || txt[ix] == u'┘' || txt[ix] == u'┤' ) return "│";
			if( txt[ix] == u'┼' ) return "├";
		}
		return "  ";
	}
}
void MarkdownEditor::do_keisen_right(bool erase) {
	QTextCursor cursor = this->textCursor();
	QString str = "─";
	int ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() ) {
		int vc = getVisualColumn(cursor, this);
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while( getVisualColumn(cursor, this) < vc + 2);
		str = getRightSrcString(erase, cursor.block().text(), ix);
	}
	QString str2 = "→";
	if (!cursor.atBlockEnd()) {
		int ix = cursor.positionInBlock();;
		int vc = getVisualColumn(cursor, this);
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc + 2);
		str2 = getRightDstString(erase, cursor.block().text(), ix);
	}
	cursor.insertText(str + str2);
	cursor.movePosition(QTextCursor::Left);
	setTextCursor(cursor);
}
#if 0
QString getRightSrcString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'│' || txt[ix] == u'┘' || txt[ix] == u'┐' || txt[ix] == u'┤' )
			return txt[ix];		//	変化無し
		if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' )
			return "│";
		if( txt[ix] == u'┼' ) return "┤";
	}
	return "  ";
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getRightDstString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' ) return txt[ix];		//	変化無し
		if( txt[ix] == u'┐' || txt[ix] == u'┘' || txt[ix] == u'┤' ) return "│";
		if( txt[ix] == u'┼' ) return "├";
	}
	return "  ";
}
void MarkdownEditor::do_keisen_right_erase() {
	QTextCursor cursor = this->textCursor();
	int vc0 = getVisualColumn(cursor, this);
	QString str = "  ";
	int ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() ) {
		int vc = getVisualColumn(cursor, this);
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while( getVisualColumn(cursor, this) < vc + 2);
		str = getRightSrcString_erase(cursor.block().text(), ix);
	}
	QString str2 = "  ";
	if (!cursor.atBlockEnd()) {
		int ix = cursor.positionInBlock();;
		int vc = getVisualColumn(cursor, this);
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while( getVisualColumn(cursor, this) < vc + 2);
		str2 = getRightDstString_erase(cursor.block().text(), ix);
	}
	cursor.insertText(str + str2);
	while( getVisualColumn(cursor, this) > vc0 + 2)
		cursor.movePosition(QTextCursor::Left);
	setTextCursor(cursor);
}
QString getLeftSrcString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'│' || txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' )
			return txt[ix];		//	変化無し
		if( txt[ix] == u'┘' || txt[ix] == u'┐' || txt[ix] == u'┤' )
			return "│";
		if( txt[ix] == u'┼' ) return "├";
	}
	return "  ";
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getLeftDstString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'┐' || txt[ix] == u'┘' || txt[ix] == u'┤' ) return txt[ix];		//	変化無し
		if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' ) return "│";
		if( txt[ix] == u'┼' ) return "┤";
	}
	return "  ";
}
void MarkdownEditor::do_keisen_left_erase() {
	QTextCursor cursor = this->textCursor();
	if( cursor.atBlockStart() ) return;				//	行頭にいる場合は無視
	int vc = getVisualColumn(cursor, this);
	QString src = "  ";
	if( !cursor.atBlockEnd() ) {
		int ix = cursor.positionInBlock();
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while (getVisualColumn(cursor, this) < vc + 2);
		src = getLeftSrcString_erase(cursor.block().text(), ix);
		cursor.clearSelection();	//	選択解除、カーソル位置は movePosition() 後の位置、つまり選択末尾
	}
	while( !cursor.atBlockStart() && getVisualColumn(cursor, this) > vc - 2 )
		cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
	QString dst = getLeftDstString_erase(cursor.block().text(), cursor.positionInBlock());
	cursor.insertText(dst+src);
	while (!cursor.atBlockStart() && getVisualColumn(cursor, this) > vc - 2)
		cursor.movePosition(QTextCursor::Left);
	setTextCursor(cursor);
}
QString getUpSrcString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'─' || txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' )
			return txt[ix];		//	変化無し
		if( txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' )
			return "─";
		if( txt[ix] == u'┼' ) return "┬";
		if( txt[ix] == u'├' ) return "┌";
		if( txt[ix] == u'┤' ) return "┐";
	}
	return "  ";
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getUpDstString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'─' || txt[ix] == u'┘' || txt[ix] == u'└' || txt[ix] == u'┴' )
			return txt[ix];		//	変化無し
		if( txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'├' ) return "─";
		if( txt[ix] == u'┼' ) return "┴";
		if( txt[ix] == u'├' ) return "└";
		if( txt[ix] == u'┤' ) return "┘";
	}
	return "  ";
}
void MarkdownEditor::do_keisen_up_erase() {
	QTextCursor cursor = this->textCursor();
	cursor.beginEditBlock();
	int vc0 = getVisualColumn(cursor, this);
	int ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getUpSrcString_erase(cursor.block().text(), ix));
	cursor.movePosition(QTextCursor::Left);
	int vc = getVisualColumn(cursor, this);
	cursor.movePosition(QTextCursor::Up);
	ix = cursor.positionInBlock();
	int vc2 = getVisualColumn(cursor, this);
	if( vc2 < vc ) cursor.insertText(QString(vc-vc2, u' '));
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc + 2 )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getUpDstString_erase(cursor.block().text(), ix));
	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);
	cursor.endEditBlock();
	setTextCursor(cursor);
}
QString getDownSrcString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		// 変化無し（すでに下に線がない）
		if( txt[ix] == u'─' || txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' || txt[ix] == u'←' || txt[ix] == u'→' ) return txt[ix];
		// 縦要素が消えて横棒だけ残る
		if( txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' ) return "─";
		// Ｔ字・角に格下げ
		if( txt[ix] == u'┼' ) return "┴";
		if( txt[ix] == u'├' ) return "└";
		if( txt[ix] == u'┤' ) return "┘";
		if( txt[ix] == u'│' ) return "	";
	}
	return "  ";
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getDownDstString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		// 変化無し（すでに上に線がない）
		if( txt[ix] == u'─' || txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' || txt[ix] == u'←' || txt[ix] == u'→' ) return txt[ix];
		// 縦要素が消えて横棒だけ残る
		if( txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' ) return "─";
		// Ｔ字・角に格下げ
		if( txt[ix] == u'┼' ) return "┬";
		if( txt[ix] == u'├' ) return "┌";
		if( txt[ix] == u'┤' ) return "┐";
	}
	return "  ";
}
void MarkdownEditor::do_keisen_down_erase() {
	QTextCursor cursor = this->textCursor();
	cursor.beginEditBlock();
	int vc0 = getVisualColumn(cursor, this);
	if (cursor.block() == cursor.document()->lastBlock()) {		//	カーソルが最終行にいる場合
		QTextCursor tempCursor(document());
		tempCursor.movePosition(QTextCursor::End);
		tempCursor.insertBlock();		//	新規行作成
	}
	int ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getDownSrcString_erase(cursor.block().text(), ix));
	cursor.movePosition(QTextCursor::Left);
	int vc = getVisualColumn(cursor, this);
	cursor.movePosition(QTextCursor::Down);
	int vc2 = getVisualColumn(cursor, this);
	if( vc2 < vc ) cursor.insertText(QString(vc-vc2, u' '));		//	カーソル位置まで空白挿入
	ix = cursor.positionInBlock();
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc + 2 )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	QString dst = getDownDstString_erase(cursor.block().text(), ix);
	cursor.insertText(dst);
	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);
	cursor.endEditBlock();
	setTextCursor(cursor);
}
#endif
//void MarkdownEditor::scrollToTop(int ln) {		//	ln: 0 org.
//}
void MarkdownEditor::scrollToTop(const QTextCursor &cursor) {
	int visualLineNum = getVisualLineNumber(cursor);
	this->scrollToTop(visualLineNum);
}
int findKeisen(QTextCursor& cursor) {
	const QString &text = cursor.block().text();
	int ix = cursor.positionInBlock();
	while( ix < text.size() ) {
		if( isKeisenChar(text[ix]) )
			return ix;
		++ix;
	}
	return -1;
}
int rfindKeisen(QTextCursor& cursor) {
	const QString &text = cursor.block().text();
	int ix = cursor.positionInBlock();
	while( --ix >= 0 ) {
		if( isKeisenChar(text[ix]) )
			return ix;
	}
	return -1;
}
void MarkdownEditor::onAlignLeft()	 { applyAlignment(Align::Left); }
void MarkdownEditor::onAlignCenter() { applyAlignment(Align::Center); }
void MarkdownEditor::onAlignRight()  { applyAlignment(Align::Right); }

void MarkdownEditor::applyAlignment(Align align) {
	QTextCursor cursor = textCursor();
	if (cursor.hasSelection()) return;

	int ix1 = rfindKeisen(cursor); // 左側位置
	int ix2 = findKeisen(cursor);  // 右側位置
	if (ix1 < 0 || ix2 < 0 || ix1 >= ix2) return;

	QString lineText = cursor.block().text();
	// 罫線の間の文字列を抽出 (ix1+1 から ix2 までの範囲)
	int cellWidth = ix2 - ix1 - 1;
	QString cellText = lineText.mid(ix1 + 1, cellWidth);
	
	// 前後の空白を除去して中身のテキストだけを取り出す
	QString content = cellText.trimmed();
	if (content.isEmpty()) return; // 空なら何もしない

	int contentLen = content.length();
	int totalSpaces = cellWidth - contentLen;
	if (totalSpaces < 0) return; // セル幅より文字が長い場合はガード

	// 各アラインメントに応じた左側の空白数を計算
	int leftSpaces = 0;
	switch (align) {
		case Align::Left:
			leftSpaces = 0;
			break;
		case Align::Center:
			leftSpaces = totalSpaces / 2;
			break;
		case Align::Right:
			leftSpaces = totalSpaces;
			break;
	}
	int rightSpaces = totalSpaces - leftSpaces;

	// 新しいセル内文字列の組み立て
	QString newText = QString(leftSpaces, u' ') + content + QString(rightSpaces, u' ');

	// 置換処理
	int blockPos = cursor.block().position();
	cursor.setPosition(blockPos + ix1 + 1);
	cursor.setPosition(blockPos + ix2, QTextCursor::KeepAnchor);
	
	// 変更がある場合のみ実行（ちらつき防止）
	if (cursor.selectedText() != newText) {
		cursor.insertText(newText);
		cursor.setPosition(blockPos + ix2 - rightSpaces);
		setTextCursor(cursor);
	}
}
void MarkdownEditor::onContentsChanged(int position, int charsRemoved, int charsAdded) {
	if( m_processing || (charsRemoved == 0 && charsAdded == 0) ) return;
	if (m_isComposing) return;		//	IME変換中
	m_processing = true;
	qDebug() << "MarkdownEditor::onContentsChanged()";
	qDebug() << "pos = " << position << ", removed = " << charsRemoved << ", added = " << charsAdded;
	QTextCursor cursor = this->textCursor();
	int k = findKeisen(cursor);
	if( k >= 0 ) {		//	現カーソルの右側に罫線文字がある場合
		const QString &text = cursor.block().text();		//	編集後ブロックテキスト
		int cpos = cursor.position();
		int bpos = position - cursor.block().position();	//	ブロック先頭からの編集位置
		//int bpos = cursor.positionInBlock();				//	ブロック先頭からの現カーソル位置
		const QString strAdded = text.mid(bpos, charsAdded);
		charsRemoved = qMin(charsRemoved, m_lastCurBlockText.size() - bpos);		//	行末を超えている場合対応
		charsAdded = qMin(charsAdded, text.size() - bpos);		//	行末を超えている場合対応
		int c = 0;
		while( charsAdded-c-1 > 0 && charsRemoved-c-1 > 0 &&
			text[bpos+charsAdded-c-1] == m_lastCurBlockText[bpos+charsRemoved-c-1] )
		{
			++c;	//	末尾共通部分
		}
		charsRemoved -= c;
		charsAdded -= c;
		int ncAdded = nColumn(strAdded);
		int ncRemoved = nColumn(m_lastCurBlockText.mid(bpos, charsRemoved));
		cursor.setPosition(cursor.block().position() + k);		//	罫線位置
		int d = ncAdded - ncRemoved;
		if( d > 0 ) {			//	文字列幅が増えた場合
			if( k > 0 && text[k-1] == u' ' ) {		//	空欄があるかどうかチェック
				//qDebug() << "k = " << k << ", cursor.positionInBlock() = " << cursor.positionInBlock();
				if( k > cpos - cursor.block().position() ) {		//	罫線直前まで文字挿入した場合は罫線保護を行わない
					cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, d);
					cursor.deleteChar();
					cursor.setPosition(cpos);
					setTextCursor(cursor);
				}
			}
		} else if( d < 0 ) {	//	文字列幅が減った場合
			if( k > bpos ) {	//	罫線直前を削除された場合は罫線保護を行わない
				cursor.insertText(QString(-d, u' '));
				cursor.setPosition(cpos);
				setTextCursor(cursor);
			}
		}
	}
	m_processing = false;
}
void MarkdownEditor::onCurPosChanged() {
	QTextCursor cursor = this->textCursor();
	m_lastCurBlockText = cursor.block().text();
}
int MarkdownEditor::getVisualLineNumber(const QTextCursor &cursor) const {
	int visualLineNum = 0;
	QTextBlock targetBlock = cursor.block();

	// 1. カーソルがあるブロックより前の全ブロックの表示行数を合計する
	for (QTextBlock block = document()->begin(); block != targetBlock; block = block.next()) {
		if (block.isValid()) {
			// block.layout() から、そのブロックが何行に折り返されているかを取得
			visualLineNum += block.layout()->lineCount();
		}
	}

	// 2. カーソルがある現在のブロック内で、カーソルが「何行目の折り返し」にいるかを取得
	// cursor.positionInBlock() はブロック先頭からの文字数
	int relativePos = cursor.position() - targetBlock.position();
	
	// layout()->lineForTextPosition(n) で、文字位置 n が含まれる QTextLine を取得できる
	int lineInBlock = targetBlock.layout()->lineForTextPosition(relativePos).lineNumber();

	return visualLineNum + lineInBlock;
}
void drawLeftArrow(QPainter &p, QRect r) {
	// 枠に対して少し余白を持たせる
	int x_end = r.right() - 2;
	int x_start = r.left() + 2;
	int y_mid = r.center().y()+2;
	int headSize = r.height() / 4; // 矢印の頭の大きさ
	// 軸（横線）
	p.drawLine(x_start, y_mid, x_end, y_mid);
	// 矢印の頭（＜の部分）
	p.drawLine(x_start, y_mid, x_start + headSize, y_mid - headSize);
	p.drawLine(x_start, y_mid, x_start + headSize, y_mid + headSize);
}
void drawEOF(QPainter &p, QRect r) {
	int x = r.left() + 2;
	int y_mid = r.center().y() + 2;
	int h = r.height() / 4; // 基本サイズ
	int y_top = y_mid - h;
	int y_bot = y_mid + h;
	int char_w = h * 1.2;	// 1文字の幅
	int gap = 2;			// 文字間隔

	// --- E の描画 (4本) ---
	p.drawLine(x, y_top, x, y_bot);
	p.drawLine(x, y_top, x + char_w, y_top);
	p.drawLine(x, y_mid, x + char_w - 1, y_mid);
	p.drawLine(x, y_bot, x + char_w, y_bot);

	x += char_w + gap;

	// --- o の描画 (4本: 小さな四角) ---
	// o は少し低めに配置すると「EoF」らしく見えます
	int yo_top = y_mid; 
	p.drawLine(x, yo_top, x + char_w, yo_top);
	p.drawLine(x, y_bot, x + char_w, y_bot);
	p.drawLine(x, yo_top, x, y_bot);
	p.drawLine(x + char_w, yo_top, x + char_w, y_bot);

	x += char_w + gap;

	// --- F の描画 (3本) ---
	p.drawLine(x, y_top, x, y_bot);
	p.drawLine(x, y_top, x + char_w, y_top);
	p.drawLine(x, y_mid, x + char_w - 1, y_mid);
}
void MarkdownEditor::paintEvent(QPaintEvent *e) {
	QPlainTextEdit::paintEvent(e); // 先にテキストを普通に描画
	QPainter p(viewport());
	//p.setPen(QColor(0, 120, 215, 80)); // 薄い青色（透過度 80）
	p.setPen(QColor(100, 160, 220, 0xc0));

	QFontMetrics fm(this->font());
	int zWidth = fm.horizontalAdvance("□"); 

	for (QTextBlock b = firstVisibleBlock(); b.isValid(); b = b.next()) {
		QRectF r = blockBoundingRect(b).translated(contentOffset());
		if (r.top() > viewport()->height()) break; // 画面外なら終了
		// --- 改行マーク（←）の描画 ---
		QTextCursor cursor(b);
		cursor.movePosition(QTextCursor::EndOfBlock);
		QRect cr = cursorRect(cursor);
		cr.setWidth(zWidth);
		if( b != document()->lastBlock() )
			drawLeftArrow(p, cr);		 
		else
			drawEOF(p, cr);
		QString s = b.text();
		for (int i = 0; i < s.size(); ++i) {
			if (s[i] == u'　') { // 全角空白を見つけたら
				// その文字の描画座標を取得
				QTextCursor cursor(b);
				cursor.setPosition(b.position() + i);
				QRect cr = cursorRect(cursor);
				cr.setWidth(zWidth);
				// 1ピクセル内側に「□」を描画
				p.drawRect(cr.adjusted(1, 1, -2, -2));
			}
		}
	}
}
void MarkdownEditor::updateLnArea(const QRect &rect, int dy) {
	if (dy)
        m_lnAreaWidget->scroll(0, dy);
    else
        m_lnAreaWidget->update(0, rect.y(), m_lnAreaWidget->width(), rect.height());
}
void MarkdownEditor::lnAreaPaintEvent(QPaintEvent *event) {
	QPainter painter(m_lnAreaWidget);
	// 現在表示されている最初のブロックを取得
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    
    // ブロックの表示上の位置（Y座標）を取得
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    int charWidth = fontMetrics().horizontalAdvance('9');

    // 画面内に見える範囲のブロックをループして描画
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black); // 文字色
            
            // 右詰めで描画するために幅を調整（右側に2ピクセルの余白）
            painter.drawText(0, top, m_lnAreaWidget->width() - charWidth, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
void MarkdownEditor::resizeEvent(QResizeEvent *event) {
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    m_lnAreaWidget->setGeometry(QRect(cr.left(), cr.top(), lnAreaWidth(), cr.height()));
}
