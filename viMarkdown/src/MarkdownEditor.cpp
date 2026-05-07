#include <QPlainTextEdit>
#include <QTextEdit>
#include <QTextCursor>
#include <QTextBlock>
#include <QSyntaxHighlighter>
#include <QPainter>
#include <QRegularExpression>
#include <QInputMethod>
#include <QMouseEvent>
#include <QDate>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>
#include "MarkdownEditor.h"
#include "MainWindow.h"
#include "DocWidget.h"

extern Global g;
//extern bool parseCsvLine(QStringList &fields, const QString &line, bool inQuotes, bool &inComment, bool &commented);
//extern bool isTableLine(const QString& lnStr, QList<QStringView> &tableTokens);
//extern bool isTableLine(const QString& lnStr, QStringList &tableTokens);
//extern bool isTableHyphenLine(const QString& lnStr, std::vector<char> &tableAlign);
extern QString splitName(QString& anchor);
extern QString anchorToFullPath(const QString &anchor);
bool isTableHyphenLine(const QString& lnStr) {
	std::vector<char> tableAlign;
	return isTableHyphenLine(lnStr, tableAlign);
}

#if 0
enum CharType {
	Type_Other,
	Type_Space,
	Type_NewLine,
	Type_Kanji,
	Type_Hiragana,
	Type_Katakana,
	Type_HalfAlphaNum,
	Type_FullSymbol
};
#endif
CharType getCharType(QChar ch) {
	ushort uc = ch.unicode();
	if( uc == 0x2028 || uc == 0x2029 ) return Type_NewLine;
	if (ch.isSpace()) return Type_Space;
	if (uc >= 0x4E00 && uc <= 0x9FFF) return Type_Kanji;
	if (uc >= 0x3040 && uc <= 0x309F) return Type_Hiragana;
	if (uc >= 0x30A0 && uc <= 0x30FF) return Type_Katakana;
	if (ch.isLetterOrNumber()) return Type_HalfAlphaNum; // 半角英数
	return Type_Other;
}

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
	void mousePressEvent(QMouseEvent *event) override {
		MarkdownEditor* mdEditor = (MarkdownEditor*)parent();
		mdEditor->lnAreaMousePressEvent(event);
	}
	void mouseMoveEvent(QMouseEvent *event) override {
		MarkdownEditor* mdEditor = (MarkdownEditor*)parent();
		mdEditor->lnAreaMouseMoveEvent(event);
	}
	void mouseReleaseEvent(QMouseEvent *event) override {
		MarkdownEditor* mdEditor = (MarkdownEditor*)parent();
		mdEditor->lnAreaMouseReleaseEvent(event);
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
#if 1
	/* 2500-2507─ ━ │ ┃ ┄ ┅ ┆ ┇ */
	Left|Right, ThickLeft|ThickRight, Up|Down, ThickUp|ThickDown,
	Left|Right, ThickLeft|ThickRight, Up|Down, ThickUp|ThickDown, // 破線は基本線と同じフラグ

	/* 2508-250F ┈ ┉ ┊ ┋ ┌ ┍ ┎ ┏ */
	Left|Right, ThickLeft|ThickRight, Up|Down, ThickUp|ThickDown,
	Down|Right, Down|ThickRight, ThickDown|Right, ThickDown|ThickRight,

	/* 2510-2517 ┐ ┑ ┒ ┓ └ ┕ ┖ ┗ */
	Down|Left, Down|ThickLeft, ThickDown|Left, ThickDown|ThickLeft,
	Up|Right, Up|ThickRight, ThickUp|Right, ThickUp|ThickRight,

	/* 2518-251F ┘ ┙ ┚ ┛ ├ ┝ ┞ ┟ */
	Up|Left, Up|ThickLeft, ThickUp|Left, ThickUp|ThickLeft,
	Up|Down|Right, Up|Down|ThickRight, ThickUp|Down|Right, Up|ThickDown|Right,

	/* 2520-2527 ┠ ┡ ┢ ┣ ┤ ┥ ┦ ┧ */
	ThickUp|ThickDown|Right,	  // 2520: ┠ (Vertical Heavy, Right Light)
	ThickUp|Down|ThickRight,	  // 2521: ┡ (Up Heavy, Down Light, Right Heavy)
	Up|ThickDown|ThickRight,	  // 2522: ┢ (Up Light, Down Heavy, Right Heavy)
	ThickUp|ThickDown|ThickRight, // 2523: ┣ (Vertical Heavy, Right Heavy)
	Up|Down|Left,				  // 2524: ┤ (Vertical Light, Left Light)
	Up|Down|ThickLeft,			  // 2525: ┥ (Vertical Light, Left Heavy)
	ThickUp|Down|Left,			  // 2526: ┦ (Up Heavy, Down Light, Left Light)
	Up|ThickDown|Left,			  // 2527: ┧ (Up Light, Down Heavy, Left Light)

	/* 2528-252F ┨ ┩ ┪ ┫ ┬ ┭ ┮ ┯ */
	ThickUp|ThickDown|Left,		  // 2528: ┨ (Vertical Heavy, Left Light)
	ThickUp|Down|ThickLeft,		  // 2529: ┩ (Up Heavy, Down Light, Left Heavy)
	Up|ThickDown|ThickLeft,		  // 252A: ┪ (Up Light, Down Heavy, Left Heavy)
	ThickUp|ThickDown|ThickLeft,  // 252B: ┫ (Vertical Heavy, Left Heavy)
	Left|Right|Down,			  // 252C: ┬ (Horizontal Light, Down Light)
	Left|ThickRight|Down,		  // 252D: ┭ (Left Light, Right Heavy, Down Light)
	ThickLeft|Right|Down,		  // 252E: ┮ (Left Heavy, Right Light, Down Light)
	ThickLeft|ThickRight|Down,	  // 252F: ┯ (Horizontal Heavy, Down Light)

	/* 2530-2537 ┰ ┱ ┲ ┳ ┴ ┵ ┶ ┷ */
	Left|Right|ThickDown,		  // 2530: ┰ (Horizontal Light, Down Heavy)
	Left|ThickRight|ThickDown,	  // 2531: ┱ (Left Light, Right Heavy, Down Heavy)
	ThickLeft|Right|ThickDown,	  // 2532: ┲ (Left Heavy, Right Light, Down Heavy)
	ThickLeft|ThickRight|ThickDown, // 2533: ┳ (Horizontal Heavy, Down Heavy)
	Left|Right|Up,				  // 2534: ┴ (Horizontal Light, Up Light)
	Left|ThickRight|Up,			  // 2535: ┵ (Left Light, Right Heavy, Up Light)
	ThickLeft|Right|Up,			  // 2536: ┶ (Left Heavy, Right Light, Up Light)
	ThickLeft|ThickRight|Up,	  // 2537: ┷ (Horizontal Heavy, Up Light)

	/* 2538-253F ┸ ┹ ┺ ┻ ┼ ┽ ┾ ┿ */
	Left|Right|ThickUp,			  // 2538: ┸ (Horizontal Light, Up Heavy)
	Left|ThickRight|ThickUp,	  // 2539: ┹ (Left Light, Right Heavy, Up Heavy)
	ThickLeft|Right|ThickUp,	  // 253A: ┺ (Left Heavy, Right Light, Up Heavy)
	ThickLeft|ThickRight|ThickUp, // 253B: ┻ (Horizontal Heavy, Up Heavy)
	Up|Down|Left|Right,			  // 253C: ┼ (Vertical Light, Horizontal Light)
	Up|Down|ThickLeft|Right,	  // 253D: ┽ (Vertical Light, Left Heavy, Right Light)
	Up|Down|Left|ThickRight,	  // 253E: ┾ (Vertical Light, Left Light, Right Heavy)
	Up|Down|ThickLeft|ThickRight, // 253F: ┿ (Vertical Light, Horizontal Heavy)

	/* 2540-2547 ╀ ╁ ╂ ╃ ╄ ╅ ╆ ╇ */
	ThickUp|Down|Left|Right,	  // 2540: ╀ (Up Heavy, Down Light, Horizontal Light)
	Up|ThickDown|Left|Right,	  // 2541: ╁ (Up Light, Down Heavy, Horizontal Light)
	ThickUp|ThickDown|Left|Right, // 2542: ╂ (Vertical Heavy, Horizontal Light)
	ThickUp|Down|ThickLeft|Right, // 2543: ╃ (Up Heavy, Down Light, Left Heavy, Right Light)
	Up|ThickDown|ThickLeft|Right, // 2544: ╄ (Up Light, Down Heavy, Left Heavy, Right Light)
	ThickUp|Down|Left|ThickRight, // 2545: ╅ (Up Heavy, Down Light, Left Light, Right Heavy)
	Up|ThickDown|Left|ThickRight, // 2546: ╆ (Up Light, Down Heavy, Left Light, Right Heavy)
	ThickUp|Down|ThickLeft|ThickRight, // 2547: ╇ (Up Heavy, Down Light, Horizontal Heavy)
	
	/* 2548-254F ╈ ╉ ╊ ╋ ╌ ╍ ╎ ╏ */
	Up|ThickDown|ThickLeft|ThickRight, // 2548: ╈ (Up Light, Down Heavy, Horizontal Heavy)
	ThickUp|ThickDown|ThickLeft|Right, // 2549: ╉ (Vertical Heavy, Left Heavy, Right Light)
	ThickUp|ThickDown|Left|ThickRight, // 254A: ╊ (Vertical Heavy, Left Light, Right Heavy)
	ThickUp|ThickDown|ThickLeft|ThickRight, // 254B: ╋ (Vertical Heavy, Horizontal Heavy)
	Left|Right, ThickLeft|ThickRight, Up|Down, ThickUp|ThickDown, // 254C-254F: 破線
#else
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
	ThickUp|ThickDown|Right, ThickUp|Down|ThickRight, Up|ThickDown|ThickRight, ThickUp|ThickDown|ThickRight,
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
#endif
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
	// 0x00 - 0x0F: 全て細線 (Up=1, Down=2, Left=4, Right=8)
	u' ',  u'╵',  u'╷',  u'│',	u'╴',  u'┘',  u'┐',  u'┤',
	u'╶',  u'└',  u'┌',  u'├',	u'─',  u'┴',  u'┬',  u'┼',

	// 0x10 - 0x1F: 太線 上 (ThickUp=0x10)
	u'╹',  u'╹',  u'╿',  u'╿',	u'┚',  u'┚',  u'┒',  u'┦',
	u'┖',  u'┖',  u'┎',  u'┞',	u'┸',  u'┸',  u'┰',  u'╀',

	// 0x20 - 0x2F: 太線 下 (ThickDown=0x20)
	u'╻',  u'╽',  u'╻',  u'╽',	u'┙',  u'┚',  u'┑',  u'┧',
	u'┕',  u'┖',  u'┍',  u'┟',	u'┷',  u'╁',  u'┷',  u'╁',

	// 0x30 - 0x3F: 太線 上下 (ThickUp + ThickDown)
	u'┃',  u'┃',  u'┃',  u'┃',	u'┨',  u'┨',  u'┨',  u'┨',
	u'┠',  u'┠',  u'┠',  u'┠',	u'╂',  u'╂',  u'╂',  u'╂',

	// 0x40 - 0x4F: 太線 左 (ThickLeft=0x40)
	u'╸',  u'┙',  u'┑',  u'┥',	u'╸',  u'┙',  u'┑',  u'┥',
	u'╼',  u'┕',  u'┍',  u'┽',	u'━',  u'┿',  u'┮',  u'┿',

	// 0x50 - 0x5F: 太線 上左 (ThickUp + ThickLeft)
	u'┛',  u'┛',  u'┩',  u'┩',	u'┛',  u'┛',  u'┩',  u'┩',
	u'┕',  u'┛',  u'┍',  u'┽',	u'━',  u'┿',  u'┮',  u'┿',

	// 0x60 - 0x6F: 太線 下左 (ThickDown + ThickLeft)
	u'┓',  u'┪',  u'┓',  u'┪',	u'┓',  u'┪',  u'┓',  u'┪',
	u'┍',  u'┪',  u'┓',  u'┽',	u'━',  u'┿',  u'┮',  u'┿',

	// 0x70 - 0x7F: 太線 上下左 (ThickUp + ThickDown + ThickLeft)
	u'┫',  u'┫',  u'┫',  u'┫',	u'┫',  u'┫',  u'┫',  u'┫',
	u'┍',  u'┫',  u'┓',  u'┽',	u'━',  u'┿',  u'┮',  u'┿',

	// 0x80 - 0x8F: 太線 右 (ThickRight=0x80)
	u'╺',  u'┖',  u'┎',  u'┝',	u'╾',  u'┕',  u'┍',  u'┾',
	u'╺',  u'┖',  u'┎',  u'┝',	u'━',  u'┵',  u'┭',  u'┿',

	// 0x90 - 0x9F: 太線 上右 (ThickUp + ThickRight)
	u'┗',  u'┗',  u'┡',  u'┡',	u'╾',  u'┕',  u'┍',  u'╅',
	u'┗',  u'┗',  u'┡',  u'┡',	u'━',  u'┿',  u'┯',  u'╇',

	// 0xA0 - 0xAF: 太線 下右 (ThickDown + ThickRight)
	u'┏',  u'┢',  u'┏',  u'┢',	u'╾',  u'┕',  u'┍',  u'╆',
	u'┏',  u'┢',  u'┏',  u'┢',	u'━',  u'┿',  u'┯',  u'╇',

	// 0xB0 - 0xBF: 太線 上下右 (ThickUp + ThickDown + ThickRight)
	u'┣',  u'┣',  u'┣',  u'┣',	u'╾',  u'┕',  u'┍',  u'╊',
	u'┣',  u'┣',  u'┣',  u'┣',	u'━',  u'┿',  u'┯',  u'╇',

	// 0xC0 - 0xCF: 太線 左右 (ThickLeft + ThickRight)
	u'━',  u'┸',  u'┰',  u'┿',	u'━',  u'┸',  u'┰',  u'┿',
	u'━',  u'┸',  u'┰',  u'┿',	u'━',  u'┸',  u'┰',  u'┿',

	// 0xD0 - 0xDF: 太線 上左右 (ThickUp + ThickLeft + ThickRight)
	u'┻',  u'┻',  u'╈',  u'╈',	u'┻',  u'┻',  u'╈',  u'╈',
	u'┻',  u'┻',  u'╈',  u'╈',	u'┻',  u'┻',  u'╈',  u'╈',

	// 0xE0 - 0xEF: 太線 下左右 (ThickDown + ThickLeft + ThickRight)
	u'┳',  u'╇',  u'┳',  u'╇',	u'┳',  u'╇',  u'┳',  u'╇',
	u'┳',  u'╇',  u'┳',  u'╇',	u'┳',  u'╇',  u'┳',  u'╇',

	// 0xF0 - 0xFF: 太線 全方向 (ThickUp + ThickDown + ThickLeft + ThickRight)
	u'╋',  u'╋',  u'╋',  u'╋',	u'╋',  u'╋',  u'╋',  u'╋',
	u'╋',  u'╋',  u'╋',  u'╋',	u'╋',  u'╋',  u'╋',  u'╋'
};
bool isKeisenChar(QChar ch) {
	return ch.unicode() >= KEISEN_CODE_BEGIN && ch.unicode() < KEISEN_CODE_END;
}
ushort getConnectionBits(QChar ch) {
	ushort uc = ch.unicode();
	if( uc < KEISEN_CODE_BEGIN || uc >= KEISEN_CODE_END ) return 0;
	return keisenTable[uc - KEISEN_CODE_BEGIN];
}

class MarkdownHighlighter : public QSyntaxHighlighter {
public:
	MarkdownHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
	{
		m_boldItalicFormat.setForeground(g.m_boldItalicColor);
		//m_boldFormat.setFontWeight(QFont::Bold);
		m_boldFormat.setForeground(g.m_boldColor);
		m_italicFormat.setForeground(g.m_italicColor);
		m_strikethroughFormat.setForeground(g.m_strikethroughColor);
		m_boldItalicRegex = QRegularExpression(R"(\*\*\*([^\*]+)\*\*\*)");
		m_boldRegex = QRegularExpression(R"((?<![\*\\])\*\*([^\\\*]+)\*\*)");
		m_italicRegex = QRegularExpression(R"((?<![\*\\])\*([^\\\*]+)\*)");
		m_strikethroughRegex = QRegularExpression(R"((?<![\*\\])(\~\~([^\\\*]+)\~\~))");
	}
	//void setBoldColor(const QColor &color) {
	//	  m_boldFormat.setForeground(color);
	//	  rehighlight(); // これを呼ぶことでドキュメント全体の highlightBlock が再実行される
	//}
	void updateInlineColors() {
		m_boldItalicFormat.setForeground(g.m_boldItalicColor);
		m_boldFormat.setForeground(g.m_boldColor);
		m_italicFormat.setForeground(g.m_italicColor);
		m_strikethroughFormat.setForeground(g.m_strikethroughColor);
		rehighlight(); // これを呼ぶことでドキュメント全体の highlightBlock が再実行される
	}
protected:
	void highlightBlock(const QString &text) override {
		if (text.startsWith("#")) {
			QTextCharFormat fmt_darkred;
			fmt_darkred.setForeground(g.m_headingsColor);
			setFormat(0, text.length(), fmt_darkred);
		} else {
			// デフォルトの色（黒）
			setFormat(0, text.length(), QColor("black"));
			auto it = m_boldItalicRegex.globalMatch(text);
			while (it.hasNext()) {
				QRegularExpressionMatch match = it.next();
				setFormat(match.capturedStart(), match.capturedLength(), m_boldItalicFormat);
			}
			it = m_boldRegex.globalMatch(text);
			while (it.hasNext()) {
				QRegularExpressionMatch match = it.next();
				setFormat(match.capturedStart(), match.capturedLength(), m_boldFormat);
			}
			it = m_italicRegex.globalMatch(text);
			while (it.hasNext()) {
				QRegularExpressionMatch match = it.next();
				setFormat(match.capturedStart(), match.capturedLength(), m_italicFormat);
			}
			it = m_strikethroughRegex.globalMatch(text);
			while (it.hasNext()) {
				QRegularExpressionMatch match = it.next();
				setFormat(match.capturedStart(), match.capturedLength(), m_strikethroughFormat);
			}
		}
	}
private:
	QTextCharFormat m_boldItalicFormat;
	QTextCharFormat m_boldFormat;
	QTextCharFormat m_italicFormat;
	QTextCharFormat m_strikethroughFormat;
	QRegularExpression m_boldItalicRegex;
	QRegularExpression m_boldRegex;
	QRegularExpression m_italicRegex;
	QRegularExpression m_strikethroughRegex;
};
//----------------------------------------------------------------------
MarkdownEditor::MarkdownEditor(const MainWindow* mainWindow, DocWidget* docWidget, QWidget *parent, bool readOnly)
	: m_mainWindow(mainWindow), m_docWidget(docWidget), MarkdownBaseEdit(parent)
{
	setReadOnly(readOnly);
	m_highlighter = new MarkdownHighlighter(this->document());
#if 0
	QTextCursor cursor = this->textCursor();
	QTextBlockFormat format;
	format.setLineHeight(150, QTextBlockFormat::ProportionalHeight); // 1.5倍
	cursor.setBlockFormat(format);
#endif
	QFont font("MS Gothic");
	//QFont font("Consolas");
	font.setPointSize(g.m_editorFontSize);		// フォントサイズ設定
	font.setFixedPitch(true);	// 明示的に固定幅として扱う設定
	this->setFont(font);
	setCursorWidth(2);
	setLineSpacing(150);
	//QPalette pal = palette();
	//pal.setColor(QPalette::TextCursor, Qt::red);
	//setPalette(pal);
	if( m_mainWindow->isKeisenMode() )
		onKeisenMode(true);
	setViewportMargins(lnAreaWidth(), 0, 0, 0);
	m_lnAreaWidget = new LnAreaWidget(this);
	connect(this, &MarkdownEditor::updateRequest, this, &MarkdownEditor::updateLnArea);
	connect(this, &MarkdownEditor::cursorPositionChanged, this, &MarkdownEditor::onCursorPosChanged);
	connect(document(), &QTextDocument::contentsChange, this, &MarkdownEditor::onContentsChanged);
}
void MarkdownEditor::rehighlight() { m_highlighter->rehighlight(); }
//void MarkdownEditor::setBoldColor(QColor col) {
//	m_highlighter->setBoldColor(col);
//}
void MarkdownEditor::updateInlineColors() {
	m_highlighter->updateInlineColors();
}
void MarkdownEditor::updateViewportMargines() {
	setViewportMargins(lnAreaWidth(), 0, 0, 0);
}
void MarkdownEditor::onKeisenMode(bool b) {
	int charWidth = 2;
	if( b ) {
		charWidth = fontMetrics().horizontalAdvance(u'　'); // 全角スペースの幅
	}
	setCursorWidth(charWidth);
	viewport()->update();
}
void MarkdownEditor::setLineSpacing(int percentage) {
	QTextBlockFormat format;
	format.setLineHeight(percentage, QTextBlockFormat::ProportionalHeight);
	QTextCursor cursor(document());
	cursor.select(QTextCursor::Document);
	cursor.mergeBlockFormat(format);
	QTextCursor editorCursor = textCursor();
	editorCursor.setBlockFormat(format);
}
void MarkdownEditor::inputMethodEvent(QInputMethodEvent *event) {
	m_isComposing = !event->preeditString().isEmpty();
	MarkdownBaseEdit::inputMethodEvent(event);
}
void MarkdownEditor::insertFromMimeData(const QMimeData *source) {
	qDebug() << "MarkdownEditor::insertFromMimeData()";
	if (source->hasImage()) {
		if( m_docWidget->m_fullPath.isEmpty() ) {
			qDebug() << "has not fullpath, please save first.";
			QApplication::beep();
			m_mainWindow->statusBar()->showMessage(tr("To paste images, please save your document first."), 5000);
			return;
		}
		QImage image = qvariant_cast<QImage>(source->imageData());
		QString fileName = "image_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".png";
		QFileInfo fi(m_docWidget->m_fullPath);
        QString savePath = fi.absoluteDir().absolutePath() + "/" + fileName;
        if (image.save(savePath, "PNG")) {
            this->textCursor().insertText(QString("![画像](%1)").arg(fileName));
        }
		return;
	}
	QPlainTextEdit::insertFromMimeData(source);
}
void MarkdownEditor::moveToNextWord(QTextCursor& cursor, bool shift) {
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
void MarkdownEditor::moveToPrevWord(QTextCursor& cursor, bool shift) {
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
void MarkdownEditor::moveToStartOfWord(QTextCursor& cursor, bool shift) {
	int pos = cursor.position();
	QTextDocument *doc = document();
	if (pos <= 0) return;
	CharType startType = getCharType(doc->characterAt(pos));
	while (pos > 0 && getCharType(doc->characterAt(pos - 1)) == startType) {
		pos--;
	}
	cursor.setPosition(pos, shift ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
}
void MarkdownEditor::moveToEndOfWord(QTextCursor& cursor, bool shift) {
	int pos = cursor.position();
	QTextDocument *doc = document();
	if (pos >= doc->characterCount() - 1) return;
	CharType startType = getCharType(doc->characterAt(pos));
	// 同じ種別の間は進む
	while (pos < doc->characterCount() - 1 && getCharType(doc->characterAt(pos)) == startType) {
		pos++;
	}
	cursor.setPosition(pos, shift ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
}
void MarkdownEditor::insertEnter() {
	//static QRegularExpression re(R"(^\d[\.\)] )");
	static QRegularExpression re(R"(^\d\. )");
	static QRegularExpression re2(R"(^\d\) )");
	QTextCursor cursor = this->textCursor();
	QTextBlock currentBlock = cursor.block();
	QString text = currentBlock.text();
	int n = 0;
	while( n < text.length() && text[n].isSpace() ) ++n;
	QString atxt = text.left(n);		//	オートインデントテキスト
	const QString mtxt = text.mid(n);
	if( mtxt == "- " || mtxt == "- [ ] " || mtxt == "- [x] " || mtxt == "- [X] " || mtxt == "1. " || mtxt == "1) " || mtxt == "> ") {
		cursor.movePosition(QTextCursor::StartOfBlock);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		cursor.deleteChar();
		atxt.clear();
		return;
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
	cursor.insertText("\n" + atxt);
	setTextCursor(cursor);
	// カーソル位置を画面内に維持
	this->ensureCursorVisible();
}
void MarkdownEditor::deleteWord() {
	QTextCursor cursor = textCursor();
	moveToNextWord(cursor, true);
	cursor.deleteChar();
	setTextCursor(cursor);
}
void MarkdownEditor::backSpaceWord() {
	QTextCursor cursor = textCursor();
	moveToPrevWord(cursor, true);
	cursor.deleteChar();
	setTextCursor(cursor);
}
void MarkdownEditor::keyPressEvent(QKeyEvent *e) {
	QTextCursor cursor = this->textCursor();
	if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {		//	改行入力
		QTextBlock currentBlock = cursor.block();
		if( (e->modifiers() & Qt::ShiftModifier) == 0 &&		//	Shift + Enter でない
			cursor.position() != currentBlock.position())		//	行頭にいない場合
		{
			insertEnter();
		} else {
			cursor.insertText("\n");
		}
		return;
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
	} else if (e->key() == Qt::Key_Delete && (e->modifiers() & Qt::ControlModifier) != 0) {
		deleteWord();
		return;
	} else if (e->key() == Qt::Key_Backspace && (e->modifiers() & Qt::ControlModifier) != 0) {
		backSpaceWord();
		return;
	} else if( !m_mainWindow->isKeisenMode() ) {		//	非罫線モードの場合
		if( (e->modifiers() & Qt::ControlModifier) != 0 ) {
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
		}
	} else {		//	罫線モードの場合
		bool erase = (e->modifiers() & Qt::ShiftModifier) != 0;
		if( (e->modifiers() & Qt::ControlModifier) != 0 /*|| erase*/ ) {
			bool thickKeisen = m_mainWindow->isThickKeisenMode();
			m_processing = true;		//	罫線保護処理を行わないおまじない
			if (e->key() == Qt::Key_Right ) {
				do_keisen_right(erase, thickKeisen);
				m_processing = false;
				return;
			} else if (e->key() == Qt::Key_Left) {
				do_keisen_left(erase, thickKeisen);
				m_processing = false;
				return;
			} else if (e->key() == Qt::Key_Up) {
				do_keisen_up(erase, thickKeisen);
				m_processing = false;
				return;
			} else if (e->key() == Qt::Key_Down) {
				do_keisen_down(erase, thickKeisen);
				m_processing = false;
				return;
			}
			m_processing = false;
		}
	}
	MarkdownBaseEdit::keyPressEvent(e);	// 通常キーは通常通りの処理
}
int indexOfNotEsc(const QString &text, QChar ch, int ix) {
	for(;;) {
		ix = text.indexOf(ch, ix);
		if( ix < 0 ) return -1;		//	not found
		if( ix == 0 || text[ix-1] != u'\\' ) return ix;
		++ix;
	}
}
void MarkdownEditor::tagJump_sub(QTextCursor cursor) {
	QTextBlock block = cursor.block();
	QString text = block.text();
	int ix0 = cursor.positionInBlock();
	int ix = text.lastIndexOf(u'[', ix0);
	while( ix > 0 && text[ix-1] == u'\\' )
		ix = text.lastIndexOf(u'[', ix-1);
	if( ix >= 0 ) {
		int openIX = ix;
		int closeIX = indexOfNotEsc(text, u']', openIX);
		if( closeIX > 0 && closeIX < text.size() && text[closeIX+1] == u'(' ) {
			int start = closeIX + 1;
			int end = indexOfNotEsc(text, u')', start);
			if( end >= ix0 ) {
				QString anchor = text.mid(start + 1, end - start - 1);
				QString name = splitName(anchor);
				QString fullPath = anchorToFullPath(anchor);
				m_linkClickedPos = cursor.position();
				emit link_clicked("", fullPath, name);
			}
		}
	}
}
void MarkdownEditor::tagJump() {
	tagJump_sub(textCursor());
}
void MarkdownEditor::make_link() {
	QTextCursor cursor = textCursor();
	if( !cursor.hasSelection() ) {
		//int start, end;
		//getWordStartEnd(cursor, start, end);
		//cursor.setPosition(start);
		//cursor.setPosition(end, QTextCursor::KeepAnchor);
		selectWordAt(cursor);
		if( !cursor.hasSelection() ) return;
	}
	QString text = cursor.selectedText();
	text = u'[' + text + u"](url)";
	cursor.insertText(text);
	cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 4);		//	url 先頭まで移動
	cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 3);	//	url 選択
	setTextCursor(cursor);
}
void MarkdownEditor::mouseReleaseEvent(QMouseEvent *event) {
	if( (event->modifiers() & Qt::ControlModifier) != 0 ) {
		auto pos = event->position();
		QTextCursor cursor = cursorForPosition(pos.toPoint());
		tagJump_sub(cursor);
	}
#if 0	//	[[ ]] は当面封印
	if( (event->modifiers() & Qt::ControlModifier) == 0 ) return;
	auto pos = event->position();
	QTextCursor cursor = cursorForPosition(pos.toPoint());
	QTextBlock block = cursor.block();
	QString text = block.text();
	int ix = cursor.positionInBlock();
	int openIX = text.lastIndexOf("[[", ix);
	if( openIX < 0 ) return;
	int closeIX = text.indexOf("]]", openIX);
	if( closeIX < 0 || ix > closeIX + 1 ) return;
	int titleStart = openIX + 2;
	QString title = text.mid(titleStart, closeIX - titleStart);
	if( !title.endsWith(".md") ) title += ".md";
	//qDebug() << "title = " << title;
	emit title_clicked(title);
#endif
	MarkdownBaseEdit::mouseReleaseEvent(event);
}
void MarkdownEditor::selectWordAt(QTextCursor& cursor) {
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
}
#if 0
void MarkdownEditor::getWordStartEnd(QTextCursor cursor, int& start, int& end) {
	int pos = cursor.position();
	QTextDocument *doc = document();
	CharType type = getCharType(doc->characterAt(pos));
	// 前方（左）への探索
	start = pos;
	while (start > 0 && getCharType(doc->characterAt(start - 1)) == type) {
		start--;
	}
	// 後方（右）への探索
	end = pos;
	while (end < doc->characterCount() - 1 && getCharType(doc->characterAt(end)) == type) {
		end++;
	}
}
#endif
void MarkdownEditor::mouseDoubleClickEvent(QMouseEvent *e) {
	QTextCursor cursor = cursorForPosition(e->pos());
	//int start, end;
	//getWordStartEnd(cursor, start, end);
	selectWordAt(cursor);
	setTextCursor(cursor);
}
void MarkdownEditor::wheelEvent(QWheelEvent *event) {
	qDebug() << "MarkdownEditor::wheelEvent()";
	qDebug() << "e->angleDelta() = " << event->angleDelta();
	if ((event->modifiers() & Qt::ControlModifier) != 0)  {
		emit changeFontSize(event->angleDelta().y());
		event->accept();
	} else
		MarkdownBaseEdit::wheelEvent(event);
}
void MarkdownEditor::jumpToHeading(const QString& name) {
	QTextBlock block = document()->begin();
	while( block.isValid() ) {
		QString text = block.text();
		if( text.startsWith("#") ) {
			int i = 1;
			while( i < text.size() && text[i] == u'#' ) ++i;
			while( i < text.size() && text[i] == u' ' ) ++i;
			if( text.mid(i) == name ) {
				QTextCursor cursor = textCursor();
				cursor.setPosition(block.position());		//	行頭位置
				setTextCursor(cursor);
				return;
			}
		}
		block = block.next();
	}
}
bool isMatch(const QString& buf, int ix, QChar ch) {
	if( ix < 0 ) return false;
	//++ix;
	while( ix < buf.size() && (buf[ix] == u'*' || buf[ix] == u'_' || buf[ix] == u'~') ) ++ix;
	return ix < buf.size() && buf[ix] == ch;
}
int indexOf(bool &inComment, const QString& buf, int ix, QChar ch, bool isNextBlankLine, const BlockData *data) {
	while( ix < buf.size() ) {
		if( inComment ) {
			if( (ix = buf.indexOf("-->", ix)) < 0 ) {
				return -1;
			}
			inComment = false;
		} else if( QStringView(buf).mid(ix).startsWith(u"<!--") ) {
			inComment = true;
			ix += 4;
		} else {
			//if( !(buf[ix] == u'*' || buf[ix] == u'_' || buf[ix] == u'~') )
			if( data->m_charFlags[ix] < PCF_NOT_VISIBLE && buf[ix] == ch )
			{
				return ix;
			}
			++ix;
		}
	}
	if( !inComment && !isNextBlankLine && ch == u' ' ) return ix;
	return -1;
}
bool isPrevEmptyBlock(QTextBlock block) {
	QString txt;
	do {
		if( !block.previous().isValid() ) return false;
		block = block.previous();
		txt = block.text();
		if( !txt.isEmpty() ) return false;
	} while( txt.startsWith("<!--") && txt.endsWith("-->") );		//	全体コメント行はスキップ
	return true;
}
int MarkdownEditor::findPosition(const PosContext &context) {
	qDebug() << "MarkdownEditor::findPosition():";
	qDebug() << ".ancharChar = " << context.m_anchorChar << ", nth = " << context.m_nth << ", offset = " << context.m_offset;
	static QRegularExpression re("^(#+ *| *- )[\\*_~]*");
  	QTextBlock block = document()->findBlockByNumber(context.m_srcHBlockNum);
	const QChar ch = context.m_anchorChar;
	int nth = context.m_nth;
	int ix = 0;
	int offset = 0;
	bool inComment = false;
	bool isPrevLineEmpty = false;
	while( block.isValid() ) {
		if( isCommentOuted(getBlockData(block)) ) {
			block = block.next();
			ix = 0;
			continue;
		}
		if( ch == QChar(U_KEISEN_BLOCK) ) {
			if( block.userState() == US_KEISEN_BEGIN ) {
				if( --nth == 0 ) break;
				while( block.userState() == US_KEISEN_BEGIN || block.userState() == US_KEISEN_BLOCK )
					block = block.next();
			} else
				block = block.next();
			continue;
		}
		if( ch == EOB ) {	//	罫線ブロック末尾
			if( block.userState() == US_KEISEN_BEGIN ) {
				if( --nth == 0 ) {
					while( block.next().isValid() && !block.next().text().startsWith("```") )
						block = block.next();
					ix = block.text().size();
					break;
				}
				while( block.userState() == US_KEISEN_BEGIN || block.userState() == US_KEISEN_BLOCK )
					block = block.next();
			} else
				block = block.next();
			continue;
		}
		if( block.userState() == US_KEISEN_BLOCK ) {
			//	最初の罫線ブロック以外の場合
			block = block.next();
			continue;
		}
		if( block.userState() == US_TABLE && isTableHyphenLine(block.text()) ) {
			//	GFM 表のハイフン行はスキップ
			block = block.next();
			continue;
		}
#if 0
		if( block.userState() == US_CODE_BLOCK ) {
			block = block.next();
			continue;
		}
#endif
		const auto charFlags = getCharFlags(block);
		QString buf = block.text();
		bool isLineEmpty = buf.isEmpty();
		if( ch == QChar(CODE_IMAGE) ) {
			for(ix = 0; ix < charFlags.size(); ++ix) {
				if( charFlags[ix] == PCF_IMAGE_BEGIN ) {
					if( --nth == 0 ) break;
				}
			}
			if( nth == 0 ) break;
			block = block.next();
			ix = 0;
			isPrevLineEmpty = isLineEmpty;
			continue;
		}
		//buf.remove(re);				//	/# /, /- / などを削除
		offset = block.text().size() - buf.size();
		if( ch == STX ) {		//	行頭の場合
			ix = 0;
			if( block.userState() == US_KEISEN_BEGIN ) {
				if( --nth == 0 ) {
					block = block.next();	//	罫線開始行の次行
					break;
				}
			}
			if( !block.text().startsWith("```") &&
				!(isLineEmpty && isPrevLineEmpty) )
				//!(block.text().isEmpty() && isPrevEmptyBlock(block)) )
				//!(block.text().isEmpty() && block.previous().isValid() && block.previous().text().isEmpty()) )	//	連続空行ではない
			{
				if( --nth == 0 ) {
					while (ix < charFlags.size() && charFlags[ix] > PCF_IMAGE_BEGIN && block.text()[ix] != u',') {
						++ix;		//	非表示文字をスキップ
						if( block.text()[ix] == u'"' ) break;	//	"" の場合は最初の " だけスキップ
					}
					break;
				}
			}
			block = block.next();
		} else if( ch == ETX ) {		//	行末の場合
			if( block.userState() == US_KEISEN_BEGIN ) {
				if( --nth == 0 ) {
					while( block.next().isValid() && block.next().userState() == US_CODE_BLOCK )
						block = block.next();
					ix = block.text().size();
					break;
				}
			} else if( block.userState() != US_KEISEN_BLOCK &&
				!block.text().startsWith("```") &&
				!(isLineEmpty && isPrevLineEmpty) )
				//!(block.text().isEmpty() && isPrevEmptyBlock(block)) )
				//!(block.text().isEmpty() && block.previous().isValid() && block.previous().text().isEmpty()) )	//	連続空行ではない
			{
				while( block.userState() == US_KEISEN_BLOCK /*&& block.next().isValid() && !block.next().text().startsWith("```")*/) {
					block = block.next();
				}
				ix = buf.size();
				if( --nth == 0 ) {
					if( block.userState() == US_TABLE && block.text().endsWith("|") ) --ix;
					break;
				}
			}
			block = block.next();
			ix = 0;
		} else {		//	非行末の場合
			bool isNextBlankLine = !block.next().isValid() || block.next().text().isEmpty();
			const BlockData* data = getBlockData(block);
			while( (ix = indexOf(inComment, buf, ix, ch, isNextBlankLine, data)) >= 0 ) {
				if( charFlags[ix] == PCF_VISIBLE && --nth == 0 ) break;
				++ix;
			}
			if (nth == 0) break;
			block = block.next();
			ix = 0;
		}
		isPrevLineEmpty = isLineEmpty;
	}
	if( block.isValid() ) {
		int pos = block.position() + ix + offset + context.m_offset;
		if( block.userState() == US_CSV_BLOCK ) {
			pos = qMin(pos, block.position() + block.text().size());
			int ix = pos - block.position();
			const BlockData* data = getBlockData(block);
			//if( ch == STX && data->m_charFlags[ix] != PCF_CELL_SEPARATOR && data->m_charFlags[ix] != PCF_VISIBLE )
			//	++pos;
			//else
			if( ch == ETX && ix > 0 && data->m_charFlags[ix-1] != PCF_CELL_SEPARATOR && data->m_charFlags[ix-1] != PCF_VISIBLE )
				--pos;
		}
		return pos;
	} else
		return -1;
}
void MarkdownEditor::setCursorByContext(const PosContext &context, const PosContext &acontext) {
	if( m_processing ) return;		//	再入禁止
	qDebug() << "MarkdownEditor::setCursorByContext(context)";
	qDebug() << ".ancharChar = " << context.m_anchorChar << ", nth = " << context.m_nth << ", offset = " << context.m_offset;
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
void MarkdownEditor::setCursorAtNthPat(int srcHeadingBlockNum, QString pat, int nth, bool tail) {		//	nth: 見出し行から何番目か（>0）
	qDebug() << QString("MarkdownEditor::setCursorAtNthPat(%1, '%2', %3,").arg(srcHeadingBlockNum).arg(pat).arg(nth) << tail << ")";
	QTextBlock block = document()->findBlockByNumber(srcHeadingBlockNum);
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
int MarkdownEditor::nColumn(const QString &text) const {
	QFontMetrics fm(font());
	int halfWidth = fm.horizontalAdvance(u'9'); 
	int fullWidth = fm.horizontalAdvance(text);
	return fullWidth / halfWidth;
}
int getVisualColumn(const QString&text, QFont font) {
	QFontMetrics fm(font);
	int halfWidth = fm.horizontalAdvance("A"); 
	int fullWidth = fm.horizontalAdvance(text);
	return fullWidth / halfWidth;
}
#if 0
int getVisualColumn(const QString&text, MarkdownBaseEdit *editor) {
	QFontMetrics fm(editor->font());
	int halfWidth = fm.horizontalAdvance("A"); 
	int fullWidth = fm.horizontalAdvance(text);
	return fullWidth / halfWidth;
}
#endif
int getVisualColumn(QTextCursor cursor, MarkdownBaseEdit *editor) {
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
QString getUpSrcString(bool erase, bool thickKeisen, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'←' || txt[ix] == u'└' || txt[ix] == u'┗' ) return thickKeisen ? "┗" : "└";
			if( txt[ix] == u'→' || txt[ix] == u'┘' || txt[ix] == u'┛' ) return thickKeisen ? "┛" : "┘";
			if( txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┐' /*|| txt[ix] == u'┤'*/ ) return thickKeisen ? "┨" : "┤";
			if( txt[ix] == u'┌' /*|| txt[ix] == u'├'*/ ) return "├";
			if( txt[ix] == u'├' || txt[ix] == u'┠' ) return thickKeisen ? "┠" : "├";	//	右が細線
			if( txt[ix] == u'┝' || txt[ix] == u'┣' ) return thickKeisen ? "┣" : "├";	//	右が太線
			if( txt[ix] == u'┤' || txt[ix] == u'┨' ) return thickKeisen ? "┨" : "┨";	//	左が細線
			if( txt[ix] == u'┥' || txt[ix] == u'┫' ) return thickKeisen ? "┫" : "┥";	//	左が太線
			if( txt[ix] == u'┬' || txt[ix] == u'┰' || txt[ix] == u'┼' || txt[ix] == u'╂' )	//	左右が細線の場合
				return thickKeisen ? "┿"  : "┼";
			if( txt[ix] == u'┰' || txt[ix] == u'┳' || txt[ix] == u'╂' || txt[ix] == u'╋' )	//	左右が太細線の場合
				return thickKeisen ? "╋" : "┼";
			if( txt[ix] == u'─' ) {
				if( ix < 2 )		//	undone: 左に文字がある場合
					return "└";
				else
					return "┴";
			}
			if( txt[ix] == u'━' ) {
				if( ix < 2 )		//	undone: 左に文字がある場合
					return "└";
				else
					return thickKeisen ? "┻" : "┷";
			}
		}
		return thickKeisen ? "┃" : "│";
	} else {
		if( ix < txt.size() ) {
			// 変化無し（すでに上に線がない）
			if( txt[ix] == u'─' || txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' || txt[ix] == u'┯' ) return txt[ix];
			if( txt[ix] == u'━' || txt[ix] == u'┏' || txt[ix] == u'┓' || txt[ix] == u'┯' || txt[ix] == u'┳' ) return txt[ix];
			// 縦要素が消えて横棒だけ残る
			if( txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' || txt[ix] == u'┸' ) return "─";
			if( txt[ix] == u'┗' || txt[ix] == u'┛' || txt[ix] == u'┷' || txt[ix] == u'┻' ) return "━";
			// Ｔ字・角に格下げ
			if( txt[ix] == u'┼' ) return "┬";
			if( txt[ix] == u'╂' ) return "┰";		//	縦のみ太罫線
			if( txt[ix] == u'┿' ) return "┯";		//	横のみ太罫線
			if( txt[ix] == u'╋' ) return "┳";
			if( txt[ix] == u'├' ) return "┌";
			if( txt[ix] == u'┣' ) return "┏";
			if( txt[ix] == u'┤' ) return "┐";
			if( txt[ix] == u'┫' ) return "┓";
		}
		return "  ";
	}
}
//┌┰┐┌┰┐┏┯━┓┏┳━┓┌┬─┐┌┬┐
//│┃││┃│┃│	┃┃┃  ┃││  ││││
//┝╋┥├╂┤┠┼─┨┣╋━┫┝┿━┥├┼┤
//│┃││┃│┃│	┃┃┃  ┃││  ││││
//└┸┘└┸┘┗┷━┛┗┻━┛└┴─┘└┴┘
QString getUpDstString(bool erase, bool thickKeisen, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'│' || txt[ix] == u'┃' || txt[ix] == u'┐' || txt[ix] == u'┌' )
				return txt[ix];
			if( txt[ix] == u'┘' ) return "┤";
			if( txt[ix] == u'└' ) return "├";
			//if( txt[ix] == u'┴' ) return "┼";		//	左右が細線
			if( txt[ix] == u'├' || txt[ix] == u'┠' ) return thickKeisen ? "┠" : "├";	//	右が細線
			if( txt[ix] == u'┝' || txt[ix] == u'┣' ) return thickKeisen ? "┣" : "├";	//	右が太線
			if( txt[ix] == u'┤' || txt[ix] == u'┨' ) return thickKeisen ? "┨" : "┨";	//	左が細線
			if( txt[ix] == u'┥' || txt[ix] == u'┫' ) return thickKeisen ? "┫" : "┥";	//	左が太線
			if( txt[ix] == u'┴' || txt[ix] == u'┸' || txt[ix] == u'┼' || txt[ix] == u'╂' )	//	左右が細線の場合
				return thickKeisen ? "┿"  : "┼";
			if( txt[ix] == u'┷' || txt[ix] == u'┻' || txt[ix] == u'╂' || txt[ix] == u'╋' )	//	左右が太細線の場合
				return thickKeisen ? "╋" : "┼";
			if( txt[ix] == u'─' ) {
				//if( ix > 0 && (txt[ix-1] == u'─' || txt[ix-1] == u'└' || txt[ix-1] == u'┌' || txt[ix-1] == u'←') )
				if( ix > 0 && ((getConnectionBits(txt[ix-1])&Right) != 0 || txt[ix-1] == u'←') )
					return thickKeisen ? "┰" : "┬";
				else
					return thickKeisen ? "┏" : "┌";
			}
			if( txt[ix] == u'━' ) {
				//if( ix > 0 && (txt[ix-1] == u'━' || txt[ix-1] == u'←') )
				if( ix > 0 && ((getConnectionBits(txt[ix-1])&ThickRight) != 0 || txt[ix-1] == u'←') )
					return thickKeisen ? "┳" : "┯";
				else
					return thickKeisen ? "┏" : "┌";
			}
		}
		return "↑";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'─' || txt[ix] == u'┘' || txt[ix] == u'└' || txt[ix] == u'┴' || txt[ix] == u'┷' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'━' || txt[ix] == u'┛' || txt[ix] == u'┗' || txt[ix] == u'┷' || txt[ix] == u'┻' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' || txt[ix] == u'┰' ) return "─";
			if( txt[ix] == u'┏' || txt[ix] == u'┓' || txt[ix] == u'┳' || txt[ix] == u'┯' ) return "━";

			if( txt[ix] == u'┼' ) return "┴";
			if( txt[ix] == u'╂' ) return "┸";		//	縦のみ太罫線
			if( txt[ix] == u'┿' ) return "┷";		//	横のみ太罫線
			if( txt[ix] == u'╋' ) return "┻";
			if( txt[ix] == u'├' ) return "└";
			if( txt[ix] == u'┣' ) return "┗";
			if( txt[ix] == u'┤' ) return "┘";
			if( txt[ix] == u'┫' ) return "┛";
		}
		return "  ";
	}
}
void MarkdownEditor::do_keisen_up(bool erase, bool thickKeisen) {
	QTextCursor cursor = this->textCursor();
	if( cursor.blockNumber() == 0 ) return;		//	１行目では実行不可
	cursor.beginEditBlock();
	int vc0 = getVisualColumn(cursor, this); // 開始位置の表示列(VC)を保存
	int ix = cursor.positionInBlock();

// 1. 移動元（現在地）の置換
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 + 2 )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getUpSrcString(erase, thickKeisen, cursor.block().text(), ix));
	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);

	// 2. 上の行へ移動し、表示列(vc0)に正確に合わせる
	cursor.movePosition(QTextCursor::Up);
	cursor.movePosition(QTextCursor::StartOfBlock); // 行頭から辿るのが全角混じりでも最も確実
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 )
		cursor.movePosition(QTextCursor::Right);
	
	// 足りなければスペース補完（これで確実に vc0 に到達する）
	int vc = getVisualColumn(cursor, this);
	if( vc < vc0 ) cursor.insertText(QString(vc0 - vc, u' '));

	// 3. 移動先の置換
	QString pdLeft, pdRight;
	while (getVisualColumn(cursor, this) > vc0) {
		pdLeft += u' ';
		cursor.movePosition(QTextCursor::Left);
	}
	ix = cursor.positionInBlock();
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 + 2) {
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor); // 1文字だけ選択
		if( vc > vc0 ) pdRight = u' ';
	}
	cursor.insertText(pdLeft + getUpDstString(erase, thickKeisen, cursor.block().text(), ix) + pdRight);	
	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);
	cursor.endEditBlock();
	setTextCursor(cursor);
}
QString getDownSrcString(bool erase, bool thickKeisen, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'→' || txt[ix] == u'┐' || txt[ix] == u'┓' ) return thickKeisen ? "┓" : "┐";
			if( txt[ix] == u'←' || txt[ix] == u'┌' || txt[ix] == u'┏' ) return thickKeisen ? "┏" : "┌";
			if( txt[ix] == u'┬' ) return thickKeisen ? "┰" : "┬";
			if( txt[ix] == u'┘' /*|| txt[ix] == u'┤'*/ ) return "┤";
			if( txt[ix] == u'└' /*|| txt[ix] == u'├'*/ ) return "├";
			if( txt[ix] == u'├' || txt[ix] == u'┠' ) return thickKeisen ? "┠" : "├";	//	右が細線
			if( txt[ix] == u'┝' || txt[ix] == u'┣' ) return thickKeisen ? "┣" : "├";	//	右が太線
			if( txt[ix] == u'┤' || txt[ix] == u'┨' ) return thickKeisen ? "┨" : "┨";	//	左が細線
			if( txt[ix] == u'┥' || txt[ix] == u'┫' ) return thickKeisen ? "┫" : "┥";	//	左が太線
			if( txt[ix] == u'┴' || txt[ix] == u'┸' || txt[ix] == u'┼' || txt[ix] == u'╂' )	//	左右が細線の場合
				return thickKeisen ? "╂" : "┼";
			if( txt[ix] == u'┷' || txt[ix] == u'┻' || txt[ix] == u'┿' || txt[ix] == u'╋' )	//	左右が太細線の場合
				return thickKeisen ? "╋" : "┿";
			if( txt[ix] == u'─' ) {
				if( ix < 2 )		//	undone: 左に文字がある場合
					return thickKeisen ? "┏" : "┌";
				else
					return thickKeisen ? "┰" : "┬";
			}
			if( txt[ix] == u'━' ) {
				if( ix < 2 )		//	undone: 左に文字がある場合
					return thickKeisen ? "┏" : "┌";
				else
					return thickKeisen ? "┳" : "┯";
			}
		}
		return thickKeisen ? "┃" : "│";
	} else {
		if( ix < txt.size() ) {
			// 変化無し（すでに下に線がない）
			if( txt[ix] == u'─' || txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' || txt[ix] == u'←' || txt[ix] == u'→' ) return txt[ix];
			if( txt[ix] == u'━' || txt[ix] == u'┗' || txt[ix] == u'┛' || txt[ix] == u'┻' ) return txt[ix];
			// 縦要素が消えて横棒だけ残る
			if( txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' || txt[ix] == u'┰' ) return "─";
			if( txt[ix] == u'┏' || txt[ix] == u'┓' || txt[ix] == u'┯' || txt[ix] == u'┳' ) return "━";
			// Ｔ字・角に格下げ
			if( txt[ix] == u'┼' ) return "┴";
			if( txt[ix] == u'╂' ) return "┸";		//	縦のみ太罫線
			if( txt[ix] == u'┿' ) return "┷";		//	横のみ太罫線
			if( txt[ix] == u'╋' ) return "┻";
			if( txt[ix] == u'├' ) return "└";
			if( txt[ix] == u'┣' ) return "┗";
			if( txt[ix] == u'┤' ) return "┘";
			if( txt[ix] == u'┫' ) return "┛";
			//if( txt[ix] == u'│' || txt[ix] == u'┃' ) return "  ";
		}
		return "  ";
	}
}
//┌┰┐┌┰┐┏┯━┓┏┳━┓┌┬─┐┌┬─┐
//│┃││┃│┃│	┃┃┃  ┃││  │││  │
//┝╋┥├╂┤┠┼─┨┣╋━┫┝┿━┥├┼─┤
//│┃││┃│┃│	┃┃┃  ┃││  │││  │
//└┸┘└┸┘┗┷━┛┗┻━┛└┴─┘└┴─┘
QString getDownDstString(bool erase, bool thickKeisen, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'│' || txt[ix] == u'┃' || txt[ix] == u'┘' || txt[ix] == u'└' )
				return txt[ix];
			if( txt[ix] == u'├' || txt[ix] == u'┠' ) return thickKeisen ? "┠" : "├";	//	右が細線
			if( txt[ix] == u'┝' || txt[ix] == u'┣' ) return thickKeisen ? "┣" : "├";	//	右が太線
			if( txt[ix] == u'┤' || txt[ix] == u'┨' ) return thickKeisen ? "┨" : "┨";	//	左が細線
			if( txt[ix] == u'┥' || txt[ix] == u'┫' ) return thickKeisen ? "┫" : "┥";	//	左が太線
			//if( txt[ix] == u'─' ) return thickKeisen ? "┸" : "┴";
			if( txt[ix] == u'─' ) {
				if( ix == 0 || (getConnectionBits(txt[ix-1]) & (Right|ThickRight)) == 0 )		//	左に文字があるが右に結合しない場合
					return thickKeisen ? "┗" : "└";
				else
					return thickKeisen ? "┸" : "┴";
			}
			//if( txt[ix] == u'━' ) return thickKeisen ? "┻" : "┷";
			if( txt[ix] == u'━' ) {
				if( ix == 0 || (getConnectionBits(txt[ix-1]) & (Right|ThickRight)) == 0 )		//	左に文字があるが、右に結合しない場合
					return thickKeisen ? "┗" : "└";
				else
					return thickKeisen ? "┻" : "┷";
			}
			if( txt[ix] == u'┐' ) return "┤";
			if( txt[ix] == u'┌' ) return "├";
			if( txt[ix] == u'┴' || txt[ix] == u'┬' || txt[ix] == u'┼' || txt[ix] == u'╂' ) return thickKeisen ? "╂" : "┼";	//	左右が細線
			if( txt[ix] == u'┷' || txt[ix] == u'┯' || txt[ix] == u'┿' || txt[ix] == u'╋' ) return thickKeisen ? "╋" : "┿";	//	左右が太線
		}
		return "↓";
	} else {
		if( ix < txt.size() ) {
			// 変化無し（すでに上に線がない）
			if( txt[ix] == u'─' || txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' || txt[ix] == u'←' || txt[ix] == u'→' ) return txt[ix];
			if( txt[ix] == u'━' || txt[ix] == u'┏' || txt[ix] == u'┓' || txt[ix] == u'┳' ) return txt[ix];
			// 縦要素が消えて横棒だけ残る
			if( txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' || txt[ix] == u'┸' ) return "─";
			if( txt[ix] == u'┗' || txt[ix] == u'┛' || txt[ix] == u'┷' || txt[ix] == u'┻' ) return "━";
			// Ｔ字・角に格下げ
			if( txt[ix] == u'┼' ) return "┬";
			if( txt[ix] == u'╂' ) return "┰";		//	縦のみ太罫線
			if( txt[ix] == u'┿' ) return "┯";		//	横のみ太罫線
			if( txt[ix] == u'╋' ) return "┳";		//	縦横太罫線
			if( txt[ix] == u'├' ) return "┌";
			if( txt[ix] == u'┣' ) return "┏";
			if( txt[ix] == u'┤' ) return "┐";
			if( txt[ix] == u'┣' ) return "┓";
		}
		return "  ";
	}
}
void MarkdownEditor::do_keisen_down(bool erase, bool thickKeisen) {
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
	QString src = getDownSrcString(erase, thickKeisen, cursor.block().text(), ix);
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
	int vc = getVisualColumn(cursor, this);
	if( vc < vc0 ) cursor.insertText(QString(vc0 - vc, u' ')); // 足りない分を補完

	// 4. 移動先（下）の置換
	QString pdLeft, pdRight;
	while (getVisualColumn(cursor, this) > vc0) {
		pdLeft += u' ';
		cursor.movePosition(QTextCursor::Left);
	}
	ix = cursor.positionInBlock(); // 位置が確定してからインデックス取得
	while( !cursor.atBlockEnd() && (vc = getVisualColumn(cursor, this)) < vc0 + 2 ) {
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		if (vc > vc0) pdRight = u' ';
	}
	cursor.insertText(pdLeft + getDownDstString(erase, thickKeisen, cursor.block().text(), ix) + pdRight);
	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);
	cursor.endEditBlock();
	setTextCursor(cursor);
}
QString getLeftSrcString(bool erase, bool thickKeisen, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'↓' || txt[ix] == u'┘' || txt[ix] == u'┛' ) return thickKeisen ? "┛" : "┘";
			if( txt[ix] == u'↑' || txt[ix] == u'┐' || txt[ix] == u'┓' ) return thickKeisen ? "┓" : "┐";
			if( txt[ix] == u'│' || txt[ix] == u'┤' || txt[ix] == u'┥' ) return thickKeisen ? "┥" : "┤";
			if( txt[ix] == u'┃' || txt[ix] == u'┨' || txt[ix] == u'┫' ) return thickKeisen ? "┫" : "┨";
			if( txt[ix] == u'└' || txt[ix] == u'┴' || txt[ix] == u'┷' ) return thickKeisen ? "┷" : "┴";
			if( txt[ix] == u'┌' || txt[ix] == u'┬' || txt[ix] == u'┯' ) return thickKeisen ? "┯" : "┬";
			if( txt[ix] == u'┘' || txt[ix] == u'┴' ) return thickKeisen ? "┷" : "┴";	//	上方向が細線
			if( txt[ix] == u'┛' || txt[ix] == u'┸' ) return thickKeisen ? "┻" : "┸";	//	上方向が太線
			if( txt[ix] == u'┐' || txt[ix] == u'┬' ) return thickKeisen ? "┯" : "┬";	//	下方向が細線
			if( txt[ix] == u'┓' || txt[ix] == u'┰' ) return thickKeisen ? "┳" : "┰";	//	下方向が太線
			if( txt[ix] == u'├' || txt[ix] == u'┝' || txt[ix] == u'┼' || txt[ix] == u'┿' )	//	上下が細線の場合
				return thickKeisen ? "┿" : "┼";
			if( txt[ix] == u'┠' || txt[ix] == u'┣' || txt[ix] == u'╂' || txt[ix] == u'╋' )	//	上下が太線の場合
				return thickKeisen ? "╋" : "╂";
		}
		return thickKeisen ? "━" : "─";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'│' || txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'┃' || txt[ix] == u'┗' || txt[ix] == u'┏' || txt[ix] == u'┣' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'┘' || txt[ix] == u'┐' || txt[ix] == u'┤' )
				return "│";
			if( txt[ix] == u'┼' ) return "├";
			if( txt[ix] == u'╋' ) return "┣";
		}
		return "  ";
	}
}
//┌┰┐┌┰┐┏┯━┓┏┳━┓┌┬─┐
//│┃││┃│┃│	┃┃┃  ┃││  │
//┝╋┥├╂┤┠┼─┨┣╋━┫┝┿━┥
//│┃││┃│┃│	┃┃┃  ┃││  │
//└┸┘└┸┘┗┷━┛┗┻━┛└┴─┘
QString getLeftDstString(bool erase, bool thickKeisen, const QString txt, int ix, const QString prev, const QString next) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'─' || txt[ix] == u'━' || txt[ix] == u'┌' || txt[ix] == u'┼')
				return txt[ix];
#if 0
			if( txt[ix] == u'│' || txt[ix] == u'┃' ) {
				auto bits = getConnectionBits(txt[ix]);
				if( ix >= prev.size() || (getConnectionBits(prev[ix])&(Down|ThickDown)) == 0 )
					bits &= ~(Up|ThickUp);
				if( ix >= next.size() || (getConnectionBits(next[ix])&(Up|ThickUp)) == 0 )
					bits &= ~(Down|ThickDown);
				bits |= thickKeisen ? ThickRight : Right;
				if( bits != 0 && bits < 0x100 )
					return QString(revKeisenTable[bits]);
				else
					return "←";
			}
#endif
			if( txt[ix] == u'│' || txt[ix] == u'├' || txt[ix] == u'┝' ) return thickKeisen ? "┝" : "├";	//	縦：細罫線の場合
			if( txt[ix] == u'┃' || txt[ix] == u'┠' || txt[ix] == u'┣' ) return thickKeisen ? "┣" : "┠";	//	縦：太罫線の場合
			if( txt[ix] == u'↓' || txt[ix] == u'└' ) return "└";
			if( txt[ix] == u'↑' || txt[ix] == u'┌' ) return "┌";
			if( txt[ix] == u'┘' || txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┐' || txt[ix] == u'┬' ) return "┬";
			if( txt[ix] == u'┤' ) return "┼";
		}
		return "←";
	} else {
		if( ix < txt.size() ) {
			if (txt[ix] == u'│' || txt[ix] == u'┃' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'┐' || txt[ix] == u'┘' || txt[ix] == u'┤' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'┓' || txt[ix] == u'┛' || txt[ix] == u'┫' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' ) return "│";
			if( txt[ix] == u'┼' ) return "┤";
			if( txt[ix] == u'╋' ) return "┫";
		}
		return "  ";
	}
}
void MarkdownEditor::do_keisen_left(bool erase, bool thickKeisen) {
	QTextCursor cursor = this->textCursor();
	if( cursor.atBlockStart() ) return;				//	行頭にいる場合は無視
	int vc0 = getVisualColumn(cursor, this);
	QString src = erase ? "  " : thickKeisen ? "━" : "─";
	if( !cursor.atBlockEnd() ) {
		int ix = cursor.positionInBlock();
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while (!cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 + 2);
		src = getLeftSrcString(erase, thickKeisen, cursor.block().text(), ix);
		cursor.clearSelection();	//	選択解除、カーソル位置は movePosition() 後の位置、つまり選択末尾
	}
	while( !cursor.atBlockStart() && getVisualColumn(cursor, this) > vc0 - 2 )
		cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
	QString prev, next;
	QTextBlock pb = cursor.block().previous();
	if( pb.isValid() ) prev = pb.text();
	QTextBlock nb = cursor.block().next();
	if( nb.isValid() ) next = nb.text();
	QString dst = getLeftDstString(erase, thickKeisen, cursor.block().text(), cursor.positionInBlock(), prev, next);
	cursor.insertText(dst+src);
	while( getVisualColumn(cursor, this) > vc0 - 2 )
		cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
	setTextCursor(cursor);
}
QString getRightSrcString(bool erase, bool thickKeisen, const QString txt, int ix, const QString prev, const QString next) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'↑' || txt[ix] == u'┌' || txt[ix] == u'┏' ) return thickKeisen ? "┏" : "┌";
			if( txt[ix] == u'↓' || txt[ix] == u'└' || txt[ix] == u'┗' ) return thickKeisen ? "┗" : "└";
			ushort pbits = ix < prev.size() ? getConnectionBits(prev[ix]) : 0;
			ushort nbits = ix < next.size() ? getConnectionBits(next[ix]) : 0;
			if( txt[ix] == u'│' || txt[ix] == u'├' || txt[ix] == u'┝' ) {
				if( (pbits&(Down|ThickDown)) != 0 ) {		//	上部から下に接続している
					if( (nbits&(Up|ThickUp)) != 0 )			//	下部から上に接続している
						return thickKeisen ? "┝" : "├";
					else
						return thickKeisen ? "┗" : "└";
				} else {
					if( (nbits&(Up|ThickUp)) != 0 )			//	下部から上に接続している
						return thickKeisen ? "┏" : "┌";
					else
						return thickKeisen ? "━" : "─";
				}
			}
			if( txt[ix] == u'┃' || txt[ix] == u'┣' || txt[ix] == u'┠' ) {
				if( (pbits&(Down|ThickDown)) != 0 ) {		//	上部から下に接続している
					if( (nbits&(Up|ThickUp)) != 0 )			//	下部から上に接続している
						return thickKeisen ? "┣" : "┠";
					else
						return thickKeisen ? "┗" : "└";
				} else {
					if( (nbits&(Up|ThickUp)) != 0 )			//	下部から上に接続している
						return thickKeisen ? "┏" : "┌";
					else
						return thickKeisen ? "━" : "─";
				}
			}
			if( txt[ix] == u'┘' || txt[ix] == u'┴' ) return thickKeisen ? "┷" : "┴";	//	上方向が細線
			if( txt[ix] == u'┛' || txt[ix] == u'┸' ) return thickKeisen ? "┻" : "┸";	//	上方向が太線
			if( txt[ix] == u'┐' || txt[ix] == u'┬' ) return thickKeisen ? "┯" : "┬";	//	下方向が細線
			if( txt[ix] == u'┓' || txt[ix] == u'┰' ) return thickKeisen ? "┳" : "┰";	//	下方向が太線
			if( txt[ix] == u'┤' || txt[ix] == u'┥' || txt[ix] == u'┼' || txt[ix] == u'┿' )	//	上下が細線の場合
				return thickKeisen ? "┿" : "┼";
			if( txt[ix] == u'┨' || txt[ix] == u'┫' || txt[ix] == u'╂' || txt[ix] == u'╋' )	//	上下が太線の場合
				return thickKeisen ? "╋" : "╂";
		}
		return thickKeisen ? "━" : "─";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'│' || txt[ix] == u'┘' || txt[ix] == u'┐' || txt[ix] == u'┤' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'┃' || txt[ix] == u'┛' || txt[ix] == u'┓' || txt[ix] == u'┫' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' || txt[ix] == u'┝' ) return "│";		//	上下が細線の場合
			if( txt[ix] == u'┗' || txt[ix] == u'┏' || txt[ix] == u'┠' || txt[ix] == u'┣' ) return "┃";		//	上下が太線の場合
			if( txt[ix] == u'┼' ) return "┤";
			if( txt[ix] == u'╋' ) return "┫";
		}
		return "  ";
	}
}
//┌┰┐┌┰┐┏┯━┓┏┳━┓┌┬─┐
//│┃││┃│┃│	┃┃┃  ┃││  │
//┝╋┥├╂┤┠┼─┨┣╋━┫┝┿━┥
//│┃││┃│┃│	┃┃┃  ┃││  │
//└┸┘└┸┘┗┷━┛┗┻━┛└┴─┘
QString getRightDstString(bool erase, bool thickKeisen, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'─' || txt[ix] == u'━' || txt[ix] == u'┐' || txt[ix] == u'┼')
				return txt[ix];
			if( txt[ix] == u'│' || txt[ix] == u'┤' )		//	上下が細線の場合
				return thickKeisen ? "┥" : "┤";
			if( txt[ix] == u'┃' || txt[ix] == u'┨' )		//	上下が太線の場合
				return thickKeisen ? "┫" : "┨";
			if( txt[ix] == u'↑' || txt[ix] == u'┐' ) return "┐";
			if( txt[ix] == u'↓' || txt[ix] == u'┘' ) return "┘";
			if( txt[ix] == u'└' || txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┌' || txt[ix] == u'┬' ) return "┬";
			if( txt[ix] == u'├' ) return "┼";
		}
		return "→";
	} else {
		if( ix < txt.size() ) {
			if (txt[ix] == u'│' || txt[ix] == u'┃' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'┐' || txt[ix] == u'┘' || txt[ix] == u'┤' ) return "│";
			if( txt[ix] == u'┼' ) return "├";
			if( txt[ix] == u'╋' ) return "┣";
		}
		return "  ";
	}
}
void MarkdownEditor::do_keisen_right(bool erase, bool thickKeisen) {
	QTextCursor cursor = this->textCursor();
	int vc0 = getVisualColumn(cursor, this);
	QString str = erase ? "  " : thickKeisen ? "━": "─";
	int ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() ) {
		int vc = getVisualColumn(cursor, this);
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while( getVisualColumn(cursor, this) < vc + 2);
		QString prev, next;
		QTextBlock pb = cursor.block().previous();
		if( pb.isValid() ) prev = pb.text();
		QTextBlock nb = cursor.block().next();
		if( nb.isValid() ) next = nb.text();
		str = getRightSrcString(erase, thickKeisen, cursor.block().text(), ix, prev, next);
	}
	QString str2 = erase ? "  " : "→";
	if (!cursor.atBlockEnd()) {
		int ix = cursor.positionInBlock();;
		int vc = getVisualColumn(cursor, this);
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc + 2);
		str2 = getRightDstString(erase, thickKeisen, cursor.block().text(), ix);
	}
	cursor.insertText(str + str2);
	while( getVisualColumn(cursor, this) > vc0 + 2 )
		cursor.movePosition(QTextCursor::Left);
	setTextCursor(cursor);
}
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
void MarkdownEditor::openPrev() {
	QTextCursor cursor = this->textCursor();
	cursor.beginEditBlock();
	QString ctext = cursor.block().text();	//	カーソル行テキスト
	QString text;
	int vc0 = 0;
	for(int i = 0; i < ctext.size(); ++i) {
		ushort bits = getConnectionBits(ctext[i]);
		if( (bits&(Up|ThickUp)) != 0 ) {
			int vc = getVisualColumn(ctext.left(i), this->font());
			if( vc != vc0 )
				text += QString(vc - vc0, u' ');
			if( (bits&Up) != 0 )
				text += u'│';
			else
				text += u'┃';
			vc0 = vc + 2;
		}
	}
	cursor.movePosition(QTextCursor::StartOfBlock);
	cursor.insertBlock();		//	新規行作成
	cursor.movePosition(QTextCursor::Up);	//	新行先頭に移動
	cursor.insertText(text);
	cursor.endEditBlock();
}
void MarkdownEditor::openNext() {		//	罫線補完次行オープン
	QTextCursor cursor = this->textCursor();
	cursor.beginEditBlock();
	QString ctext = cursor.block().text();	//	カーソル行テキスト
	QString text;
	int vc0 = 0;
	for(int i = 0; i < ctext.size(); ++i) {
		ushort bits = getConnectionBits(ctext[i]);
		if( (bits&(Down|ThickDown)) != 0 ) {
			int vc = getVisualColumn(ctext.left(i), this->font());
			if( vc != vc0 )
				text += QString(vc - vc0, u' ');
			if( (bits&Down) != 0 )
				text += u'│';
			else
				text += u'┃';
			vc0 = vc + 2;
		}
	}
	cursor.movePosition(QTextCursor::EndOfBlock);
	cursor.insertBlock();		//	新規行作成
	cursor.insertText(text);
	cursor.endEditBlock();
}
void MarkdownEditor::insertTodayString(const QString &fmt) {
	QTextCursor cursor = this->textCursor();
	QString today = QDate::currentDate().toString(fmt);
	cursor.insertText(today);
	setTextCursor(cursor);
}
bool findCSVBlock(QTextCursor &cursor) {
	QTextBlock block = cursor.block();
	bool curline = true;
	while( block.isValid() ) {
		if( block.text().startsWith("```CSV", Qt::CaseInsensitive)) {
			cursor.setPosition(block.position());
			return true;
		}
		else if (block.text().startsWith("```") && !curline)
			break;
		curline = false;
		block = block.previous();
	}
	return false;
}
bool findTableHeader(QTextCursor &cursor) {
	//QList<QStringView> tableTokens;
	QStringList tableTokens;
	QTextBlock block = cursor.block();
	bool found = false;
	while( block.isValid() ) {
		if( !isTableLine(block.text(), block.text(), tableTokens) )
			break;
		found = true;
		block = block.previous();
	}
	if( found ) {
		cursor.setPosition(block.next().position());
		return true;
	} else
		return false;
}
void MarkdownEditor::convert_CSV_MarkdownTable() {
	QTextCursor cursor = this->textCursor();
	if( !findCSVBlock(cursor) ) return;
	int startPosition = cursor.position();
	QTextBlock block = cursor.block();
	block = block.next();		//	skip "```CSV" line
	bool inQuotes = false;
	bool inComment = false;
	bool commented = false;
	int endPosition = -1;
	QList<QStringList> ll;
	QByteArray ba;
	QStringList fields;
	while( block.isValid() ) {
		if( block.text().startsWith("```")) {
			endPosition = block.position() + block.length();
			break;
		}
		inQuotes = parseCsvLine(fields, ba, block.text(), inQuotes, inComment, commented);
		if( !inQuotes && !inComment ) {
			ll.push_back(fields);
		}
		block = block.next();
	}
	//if( endPosition < 0 )
	QString mdtext;
	bool header = true;
	for(int row = 0; row < ll.size(); ++row) {
		for(int col = 0; col < ll[row].size(); ++col) {
			mdtext += "|" + ll[row][col];
		}
		mdtext += "|\n";
		if( header ) {
			for(int col = 0; col < ll[row].size(); ++col) {
				mdtext += "|---";
			}
			mdtext += "|\n";
			header = false;
		}
	}
	cursor.setPosition(startPosition);
	cursor.setPosition(endPosition, QTextCursor::KeepAnchor);
	cursor.insertText(mdtext);
	setTextCursor(cursor);
}
void MarkdownEditor::convert_MarkdownTable_CSV() {
	QTextCursor cursor = this->textCursor();
	if( !findTableHeader(cursor) ) return;
	QTextBlock block = cursor.block();
	int startPosition = block.position();
	int endPosition = -1;
	bool initial = true;
	QString mdtext = "```CSV\n";;
	//QList<QStringView> tableTokens;
	QStringList tableTokens;
	while( block.isValid() && isTableLine(block.text(), block.text(), tableTokens) ) {
		for(int i = 0; i < tableTokens.size(); ++i) {
			QString txt(tableTokens[i]);
			txt.replace("\"", "\"\"");
			//mdtext += QString(u'"') + tableTokens[i] + QString(u'"');
			mdtext += QString(u'"') + txt + QString(u'"');
			if( i+1 < tableTokens.size() )
				mdtext += ", ";
			else
				mdtext += "\n";
		}
		if( initial ) {
			initial = false;
			if( !(block = block.next()).isValid() ) break;	//	ハイフン行をスキップ
		}
		block = block.next();
	}
	endPosition = block.position();
	cursor.setPosition(startPosition);
	cursor.setPosition(endPosition, QTextCursor::KeepAnchor);
	cursor.insertText(mdtext + "```\n");
	setTextCursor(cursor);
}
void MarkdownEditor::onContentsChanged(int position, int charsRemoved, int charsAdded) {
	if( m_ignoreContentsChanged ) return;
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
		const QString strRemoved = m_lastCurBlockText.mid(bpos, charsRemoved);
		charsRemoved = qMin(charsRemoved, strRemoved.size());		//	行末を超えている場合対応
		charsAdded = qMin(charsAdded, strAdded.size());		//	行末を超えている場合対応
		int c = 0;
		while(charsAdded-c-1 >= 0 && charsRemoved-c-1 >= 0 &&
			strAdded[charsAdded-c-1] == strRemoved[charsRemoved-c-1] )
		{
			++c;	//	末尾共通部分
		}
		charsRemoved -= c;
		charsAdded -= c;
		int ncRemoved = nColumn(strRemoved.left(charsRemoved));
		int ncAdded = nColumn(strAdded.left(charsAdded));
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
	//rehighlight();
	highlightSearchText(m_mainWindow->srcText());
	//syncEditorCursorFromPreview();
	m_processing = false;
}
	//	カーソル同期処理
void MarkdownEditor::syncPreviewCursorFromEditor() {
	if( m_mainWindow->isEdittingInPreview() ) return;
	if( m_processing || m_mainWindow->isCursorCyncing() ) return;		//	再入禁止
	m_processing = true;
	m_mainWindow->setCursorCyncing();	//	同期処理中フラグON
	//QTextCursor cursor = this->textCursor();
	QTextCursor cursor = this->textCursor();
	auto context = contextAt(cursor.position());
	context.m_prvHBlockNum = m_docWidget->srcToPrvHeading(context.m_srcHBlockNum);
	PosContext acontext;
	if( cursor.hasSelection() ) {
		acontext = contextAt(cursor.anchor());
		acontext.m_prvHBlockNum = m_docWidget->srcToPrvHeading(acontext.m_srcHBlockNum);
	}
	emit posContextChanged(context, acontext);
	m_mainWindow->setCursorCyncing(false);
	m_processing = false;
}
void MarkdownEditor::onCursorPosChanged() {
	QTextCursor cursor = this->textCursor();
	QTextBlock block = cursor.block();
	m_lastCurBlockText = block.text();
	viewport()->update();
	//	SVGブロック補完
	if( block.userState() == US_SVG_BLOCK && m_lastCurBlockText.isEmpty() && block.previous().userState() == US_SVG_BEGIN) {
		qDebug() << "to show completion widget.";
		QString txt = R"(
<svg width="320" height="200">

</svg>
)";
		m_svgCompleter = new QTextEdit(this);
		m_svgCompleter->setWindowFlags(Qt::Popup); 
		m_svgCompleter->setReadOnly(true);			//	リードオンリー
        m_svgCompleter->resize(250, 95);
        QTextCharFormat format;
		format.setBackground(QColor("#e0e0e0")); // 背景色
		QTextCursor cur = m_svgCompleter->textCursor();
        cur.insertText(txt, format);
        QRect cr = cursorRect();
        QPoint pos = viewport()->mapToGlobal(cr.bottomLeft());
        pos.setY(pos.y() + 2);
        m_svgCompleter->move(pos);
		m_svgCompleter->show();
	} else {
		if( m_svgCompleter != nullptr ) {
			delete m_svgCompleter;
			m_svgCompleter = nullptr;
		}
	}
	//	Undone: プレビューの対応段落（見出し行＋本文）を画面内に
	syncPreviewCursorFromEditor();
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
void drawCRLF(QPainter &p, QRect r) {
	// 枠に対して少し余白を持たせる
	int x_start = r.left() + 4;
	int x_end = r.right() - 4;
	int y_start = r.top() + 2;
	int y_q = r.center().y()+r.height()/4+2;
	int headSize = r.height() / 8; // 矢印の頭の大きさ
	// 軸（縦横線）
	p.drawLine(x_end, y_start, x_end, y_q);
	p.drawLine(x_start, y_q, x_end, y_q);
	// 矢印の頭（＜の部分）
	p.drawLine(x_start, y_q, x_start + headSize, y_q - headSize);
	p.drawLine(x_start, y_q, x_start + headSize, y_q + headSize);
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
	MarkdownBaseEdit::paintEvent(e); // 先にテキストを普通に描画

	QPainter p(viewport());
	//p.setPen(QColor(0, 120, 215, 80)); // 薄い青色（透過度 80）
	p.setPen(QColor(100, 160, 220, 0xc0));

	QFontMetrics fm(this->font());
	int zWidth = fm.horizontalAdvance("□"); 
	//	全角空白 □、改行マーク描画
	for (QTextBlock b = firstVisibleBlock(); b.isValid(); b = b.next()) {
		QRectF r = blockBoundingRect(b).translated(contentOffset());
		if (r.top() > viewport()->height()) break; // 画面外なら終了
		// --- 改行マーク（←）の描画 ---
		QTextCursor cursor(b);
		cursor.movePosition(QTextCursor::EndOfBlock);
		QRect cr = cursorRect(cursor);
		cr.setWidth(zWidth);
		if( b != document()->lastBlock() ) {
			//drawLeftArrow(p, cr);		 
			drawCRLF(p, cr);		 
		} else
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
	if( !isReadOnly() ) {
		//	カーソル描画
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
void MarkdownEditor::highlightSearchText(const QString &searchText) {
	QList<QTextEdit::ExtraSelection> extraSelections;
	if (searchText.isEmpty()) {
		setExtraSelections(extraSelections);
		return;
	}

	// 検索時の書式設定
	QTextCharFormat format;
	format.setBackground(Qt::yellow);		// 背景を設定色に
	//format.setBackground(g.m_matchColor);		// 背景を設定色に
	format.setForeground(Qt::black);		// 文字を黒に（必要に応じて）

	// ドキュメント全体から検索
	QTextDocument *doc = document();
	QTextCursor cursor(doc);

	while (!cursor.isNull() && !cursor.atEnd()) {
		// 次のヒットを検索
		// 引数に FindFlags (大文字小文字区別など) を指定可能
		QTextDocument::FindFlags flags;
		if( !g.m_ignoreCase )
			flags |= QTextDocument::FindCaseSensitively;
		cursor = doc->find(searchText, cursor, flags);

		if (!cursor.isNull()) {
			QTextEdit::ExtraSelection selection;
			selection.format = format;
			selection.cursor = cursor;
			extraSelections.append(selection);
		}
	}
	setExtraSelections(extraSelections);	// エディタに適用
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
			painter.drawText(0, top, m_lnAreaWidget->width() - charWidth*2, fontMetrics().height(),
							 Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
	if( !isReadOnly() ) {
		//	行カーソル描画
		QRect rect = cursorRect();
		//QPen pen(hasFocus() ? Qt::red : Qt::gray, 1); // 赤色、太さ1px
		//QPen pen(Qt::red, 1); // 赤色、太さ1px
		QPen pen(hasFocus() ? g.m_activeLnColor: g.m_inactiveLnColor, 1); // 色、太さ1px
		if( !hasFocus() ) pen.setStyle(Qt::DashLine);	//	破線
		painter.setPen(pen);
		int y = rect.bottom();
		int left = 0;
		int right = lnAreaWidth();
		painter.drawLine(left, y, right, y);
	}
}
void MarkdownEditor::lnAreaMousePressEvent(QMouseEvent *event) {
	auto pos = event->position();
	QTextCursor cursor = cursorForPosition(QPoint(0, (int)pos.y()));
	cursor.movePosition(QTextCursor::StartOfBlock);			 // 行頭へ移動
	m_anchorStartPosition = cursor.position();
	cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor); // 次行先頭まで選択
	m_selStart = cursor.selectionStart();
	m_selEnd = cursor.selectionEnd();
	m_curBlockNum = m_anchorBlockNum = cursor.blockNumber();
	setTextCursor(cursor);
	m_isCursorAboveAnchor = false;
	m_lnAreaPressed = true;
}
void MarkdownEditor::lnAreaMouseMoveEvent(QMouseEvent *event) {
	if( !m_lnAreaPressed ) return;
	auto pos = event->position();
	QTextCursor cursor = cursorForPosition(QPoint(0, (int)pos.y()));
	int cbn = cursor.blockNumber();
	if( cbn == m_curBlockNum ) return;
	m_curBlockNum = cbn;
	if( cbn >= m_anchorBlockNum ) {
		cursor.movePosition(QTextCursor::NextBlock);	// 次行先頭移動
		//if( m_isCursorAboveAnchor )
			cursor.setPosition(m_anchorStartPosition, QTextCursor::KeepAnchor);
		//else
		//	cursor.setPosition(m_selStart, QTextCursor::KeepAnchor);
		m_selEnd = cursor.selectionEnd();
		m_isCursorAboveAnchor = false;
	} else {
		cursor.movePosition(QTextCursor::StartOfBlock);	// 行頭移動
		cursor.setPosition(m_selEnd, QTextCursor::KeepAnchor);
		m_selStart = cursor.selectionStart();
		m_isCursorAboveAnchor = true;
	}
	setTextCursor(cursor);
}
void MarkdownEditor::lnAreaMouseReleaseEvent(QMouseEvent *event) {
	m_lnAreaPressed = false;
}
void MarkdownEditor::resizeEvent(QResizeEvent *event) {
	MarkdownBaseEdit::resizeEvent(event);

	QRect cr = contentsRect();
	m_lnAreaWidget->setGeometry(QRect(cr.left(), cr.top(), lnAreaWidth(), cr.height()));
}
void MarkdownEditor::dragEnterEvent(QDragEnterEvent *e) {
   	e->ignore();
}
void MarkdownEditor::dropEvent(QDropEvent *e) {
   	e->ignore();
}
bool MarkdownEditor::isInComment(int pos) const {		//	指定位置がコメント内か？
	auto* doc = document();
	QTextBlock block = doc->findBlock(pos);
	bool inComment = block.userState() == US_IN_COMMENT;	//	ブロック先頭コメント状態
	const QString buf = block.text();
	int ip = pos - block.position();
	for(int ix = 0; ix < ip; ) {
		if( inComment ) {
			int ix2 = buf.indexOf(u"-->", ix);
			if (ix2 < 0 || ix2 >= ip) break;
			inComment = false;
			ix = ix2 + 3;
		} else {
			int ix2 = buf.indexOf(u"<!--", ix);
			if( ix2 < 0 || ix2 > ip ) break;
			inComment = true;
			ix = ix2 + 4;
		}
	}
	return inComment;
}
bool MarkdownEditor::isInLinkURL(int pos, int& openIX, int& closeIX) const {
	auto* doc = document();
    QTextBlock block = doc->findBlock(pos);
    const QString text = block.text();
    int ip = pos - block.position(); // ブロック内の相対カーソル位置
    openIX = text.lastIndexOf(u'(', ip - 1);		//	'(' を逆方向探索
    if (openIX < 0) return false; // '(' が無ければURL内ではない
    if (openIX == 0 || text.at(openIX - 1) != u']') {
        return false;
    }
    int openBracket = text.lastIndexOf(u'[', openIX);		//	'[' を逆方向探索
    if (openBracket < 0) return false; // '[' が無ければURL内ではない
    if( ip < openBracket ) return false;	//	pos が [title](url) より前
    closeIX = text.indexOf(u')', openIX);
    if (closeIX < 0 || closeIX >= 0 && ip > closeIX) {
        return false;
    }
    return true;
}
bool isVisibleEmpty(const BlockData* data) {
	//if( data->m_charFlags.isEmpty() ) return true;
	for(int i = 0; i < data->m_charFlags.size(); ++i) {
		if( data->m_charFlags[i] == PCF_VISIBLE )
			return false;
	}
	return true;
}
static QRegularExpression re_prefix("^(#+ *| *- (\\[[ xX]\\] )?| *\\d+[\\.)] |(> )+)");

PosContext MarkdownEditor::contextAt(int pos) {	//	pos 位置から PosContext を構築
	PosContext pc;
	auto* doc = document();
	QTextBlock block = doc->findBlock(pos);
	assert(block.isValid());
	//assert(block.blockNumber() >= 0);
	int offset = 0;	// , openIX, closeIX;
	if( isInComment(pos) ) {		//	pos がコメント内の場合は、コメント部分をスキップ
		int ix = pos - block.position();
		while( block.isValid() ) {
			if( (offset = block.text().indexOf("-->", ix)) >= 0 ) {
				offset += (int)strlen("-->");
				pos = block.position() + offset;
				break;
			}
			if( !block.next().isValid() ) break;
			block = block.next();
			ix = 0;
		}
	}
	if( block.userState() == US_CSV_BLOCK ) {
		if( block.text().compare(QString("```CSV"), Qt::CaseInsensitive) == 0 && block.next().isValid() ) {
			block = block.next();
			pos = block.position();
		} else if( block.text() == "```" && block.previous().isValid() ) {
			block = block.previous();
			pos = block.position() + block.text().size();
		}
	} else if( block.userState() == US_CODE_BLOCK && block.text().startsWith("```") ) {
		if( block.next().isValid() ) {
			block = block.next();
			pos = block.position();
		}
	} else if( block.userState() == US_CODE_BLOCK_END ) {
		block = block.previous();
		pos = block.position() + block.text().size();
	}
	assert( block.isValid() );
	if( block.userState() == US_KEISEN_BEGIN ) {	//	罫線ブロック開始
		if( block.next().isValid() ) {
			block = block.next();
		}
	} else if( block.userState() != US_KEISEN_BLOCK ) {
		if( block.userState() == US_TABLE && isTableHyphenLine(block.text()) && block.next().isValid() ) {
			block = block.next();
			pos = block.position();
		}
		if( block.userState() == US_TABLE && doc->characterAt(pos) == u'|' ) {
			if( pos == block.position() )
				++pos;
			else {
				--pos;
				pc.m_offset += 1;
			}
		}
	}
	assert( block.isValid() );
	//assert( block.blockNumber() >= 0 );
	int ix = pos - block.position();
	while( ix < 0 ) {
		block = block.previous();
		ix = pos - block.position();
	}
	const QString buf = block.text();
	const BlockData* data = getBlockData(block);
	QChar ch;	//	アンカー文字
	if( isVisibleEmpty(data) ) {
		if( block.userState() == US_LIST && ix == strlen("- ") )
			ch = STX;
		else
			ch = ix == 0 ? STX : ETX;
	} else if( block.userState() == US_KEISEN_BEGIN ) {
		ch = STX;
	} else if( block.userState() == US_KEISEN_BLOCK ) {
		ch = ETX;
	} else {
		bool found = false;
		if( ix != data->m_charFlags.size() && (ix == 0 || ix > 0 && data->m_charFlags[ix-1] != PCF_VISIBLE) ) {		//	直前が非表示文字の場合
			//	undone: CSV内で ',' を見つけた場合は、行頭方向にアンカー文字を探す
			found = true;
			while( ix < data->m_charFlags.size() && data->m_charFlags[ix] != PCF_VISIBLE ) {
				if( block.userState() == US_CSV_BLOCK && buf[ix] == u',' ) {
					found = false;
					break;
				}
				++pos;
				++ix;
			}
		}
		if( !found && ix < data->m_charFlags.size() ) {	//	改行位置でない場合
			while( pos > 0 && ix >= 0 && ix < data->m_charFlags.size() && data->m_charFlags[ix] != PCF_VISIBLE ) {
				//if( ix == data->m_charFlags.size() )
				if( ix == 0 || data->m_charFlags[ix-1] == PCF_VISIBLE || data->m_charFlags[ix-1] == PCF_CELL_SEPARATOR ) {
					pc.m_offset += 1;
				}
				--pos;
				if( --ix < 0 ) {
					bool isEmptyLine = block.text().isEmpty();
					block = block.previous();
					if( isEmptyLine && block.text().isEmpty() )		//	空行が続く場合
						pc.m_offset -= 1;
					ix = pos - block.position();
					data = getBlockData(block);
				}
			}
		}
		//	Undone: block が見出し・リスト・連番・チェックボックス行で、pos が接頭辞内にある場合対応
		bool prefix = false;
		if( offset ==0 && (prefix = block.text().indexOf(re_prefix) == 0) ) {
			auto buf = block.text();
			buf.remove(re_prefix);
			int ofst = block.text().size() - buf.size();
			if( pos < block.position() + ofst ) {	//	接頭辞内にある
				pos = block.position() + ofst;
			}
		}
		const QString text = block.text();
		if( text.endsWith("  ") && pos - block.position() >= text.size() - 2 && pos > 0 ) {	//	行末 "  " にカーソルがある場合
			int ix = --pos - block.position();
			while( ix > 0 && text[ix] == u' ' ) {
				--ix;
				--pos;
			}
			pc.m_offset += 1;
		}
		ch = doc->characterAt(pos);
		if( pos > 0 && doc->characterAt(pos-1) != u'\\' ) {
			//while( ch == u'*' || ch == u'_' || ch == u'~' )
			while( ix < data->m_charFlags.size() && data->m_charFlags[ix] != PCF_VISIBLE ) {
				++ix;
				ch = doc->characterAt(++pos);
			}
		}
		if (ch == QChar::ParagraphSeparator) ch = ETX;
	}
	//	Undone: "  +\n" の場合も改行扱い
	pc.m_anchorChar = ch;
	while( block.userState() == US_IN_COMMENT || !block.text().startsWith(u'#') ) {		//	直前の見出し行を探す
		if( !block.previous().isValid() )
			break;
		block = block.previous();
	}
	auto t = block.text();
	pc.m_srcHBlockNum = block.blockNumber();
	pc.m_nth = countCharUntil(block, pos, ch);
	return pc;
}
//	pos の ch が見出し行先頭から何番目かを計算
int MarkdownEditor::countCharUntil(QTextBlock block, int pos, QChar ch) const
{
	//static QRegularExpression re("^(#+ *| *- (\\[[ xX]\\] )?| *\\d+[\\.)] |(> )+)");
	int count = ch == STX ? 0 : 1;
	while( block.isValid() ) {
		auto t = block.text();
		const BlockData *data = getBlockData(block);
		//printCharFlags(block);
		if( block.userState() == US_KEISEN_BEGIN ) {
			if( ch == STX || ch == ETX )
				++count;
			do {
				block = block.next();
			} while( block.isValid() && block.userState() == US_KEISEN_BLOCK );
			if (block.position() > pos) {
				if (ch == ETX) --count;
				break;
			}
			continue;
		}
		if( ch == STX ) {		//	行頭の場合
			if (!block.next().isValid()) {
				++count;
				break;		//	最終行の場合
			}
			//if( block.next().position() == pos ) break;
			const QString t = block.text();
			if( !block.text().startsWith("```") &&		//	``` 行は無視
				!(block.userState() == US_LIST && block.text().size() == 2) &&		//	"- " だけのリスト行は無視
				!(block.text().isEmpty() && block.previous().isValid() && block.previous().text().isEmpty()) )		//	連続空行でない場合
			{
				++count;
			}
			if( block.position() >= pos ) break;
		} else if( ch == ETX ) {		//	行末の場合
			if( !block.next().isValid() ) break;		//	最終行の場合
			//if( block.userState() == US_KEISEN_BLOCK ) {
			//	if( block.position() + block.text().size() >= pos ) break;
			//} else
			if( !block.text().startsWith("```") ) {		//	``` 行は無視
#if 1	//	GFM
				if( pos >= block.position() && pos < block.next().position() ) break;
				if( !block.text().isEmpty() || !(block.previous().isValid() && block.previous().text().isEmpty()) )		//	連続空行でない場合
					++count;
#else	//	コモンマークダウン
				bool prefix = block.text().indexOf(re_prefix) == 0;		//	# 等の接頭辞？あり
				if( prefix || block.text().endsWith("  ") || block.next().text().isEmpty() ) {
					if( block.next().position() >= pos )
						break;
					++count;
				}
#endif
			}
		} else {
			bool inComment = block.userState() == US_IN_COMMENT;
			bool inCSVBlock = block.userState() == US_CSV_BLOCK;
			bool finished = false;		//	pos まで探索した
			const QString buf = block.text();
			int ix = 0;
			if( inCSVBlock ) {
				bool inQuotes = false;
				bool commented = false;
				QStringList fields;
				QByteArray ba;
				QString buf2 = buf;
				if( block.next().isValid() && block.next().position() > pos) {
					buf2 = buf2.left(pos - block.position());
					finished = true;
				}
				parseCsvLine(fields, ba, buf2, inQuotes, inComment, commented);
				if( !fields.isEmpty() ) {
					for(auto txt: fields) {
						for(int i = 0; i < txt.size(); ++i)
							if( txt[i] == ch ) ++count;
					}
				}
			} else {
				//	Undone: 接頭辞部分を削除
				auto match = re_prefix.match(buf);
				if( match.hasMatch() )
					ix = match.capturedLength();
				while( ix < buf.size() ) {
					if( inComment ) {	//	コメントブロック中
						int ix2 = buf.indexOf(u"-->", ix);
						if( ix2 >= 0 ) {
							inComment = false;
							ix = ix2 + 3;	//	skip "-->"
							continue;
						} else
							break;
					} else {	//	非コメントブロック中
						if( QStringView(buf).mid(ix).startsWith(u"<!--") ) {	//	コメントブロック開始
							inComment = true;
							ix += 4;	//	skip "<!--"
							continue;
						} else {
							int openIX, closeIX;
							if( isInLinkURL(block.position() + ix, openIX, closeIX) && ix >= openIX - 1 ) {		//	"](" 以降
								//while( ++ix < buf.size() && buf[ix] != ')' ) {}
								ix = closeIX;
							}
							if( block.position() + ix >= pos ) {
								finished = true;
								break;
							}
							if( ix < buf.size() && buf[ix] == ch && data->m_charFlags[ix] == PCF_VISIBLE )
								++count;
							++ix;
						}
					}
				}
			}
			if( finished ) break;
		}
		block = block.next();
	}
	return count;
}
