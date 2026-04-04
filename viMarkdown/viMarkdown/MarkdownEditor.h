#pragma once
#include <QScrollBar>
#include "C:\Qt\6.10.0\msvc2022_64\include\QtWidgets\qplaintextedit.h"

enum class Align { Left, Center, Right };

const int LN_WIDTH = 7;

class MainWindow;
class DocWidget;

#define		MarkdownBaseEdit	QPlainTextEdit
//#define		MarkdownBaseEdit	QTextEdit		//	QPlainTextEdit にしかないシグナルがあるため無理

class MarkdownEditor : public MarkdownBaseEdit
{
	Q_OBJECT
public:
	MarkdownEditor(const MainWindow* mainWindow, DocWidget*, QWidget *parent = nullptr, bool readOnly = false);
public:
	void	scrollToTop(int lineNum) {		//	lineNum: 0 org.
		verticalScrollBar()->setValue(lineNum);
	}
	void	lnAreaPaintEvent(QPaintEvent *event);
	void	lnAreaMousePressEvent(QMouseEvent *event);
	void	lnAreaMouseMoveEvent(QMouseEvent *event);
	void	lnAreaMouseReleaseEvent(QMouseEvent *event);
	int lnAreaWidth() {
        return fontMetrics().horizontalAdvance('9') * LN_WIDTH;
    }
	void	updateViewportMargines();
	void	scrollToTop(const QTextCursor &cursor);
	//{
	//	this->scrollToTop(cursor.blockNumber());
	//}
	int getVisualLineNumber(const QTextCursor &cursor) const;
	void	onAlignLeft();
	void	onAlignCenter();
	void	onAlignRight();
	void	onKeisenMode(bool);
	void	openPrev();
	void	openNext();
	void	insertTodayString(const QString &fmt);
	void	convert_CSV_MarkdownTable();
	void	convert_MarkdownTable_CSV();
    void	setCursorAtNthPat(int srcHeadingBlockNum, QString pat, int nth, bool=false);
    void	setProcessing(bool b) { m_processing = b; }
    void	setIgnoreCC(bool b) { m_ignoreContentsChanged = b; }
    void	rehighlight();
    //void	setBoldColor(QColor);
    void	updateInlineColors();
    void	highlightSearchText(const QString &searchText);
    void	jumpToHeading(const QString &name);		//	見出し（name）行にカーソル設定
	void	moveToNextWord(QTextCursor& cursor, bool select);
	void	moveToPrevWord(QTextCursor& cursor, bool select);
	void	moveToStartOfWord(QTextCursor& cursor, bool select);
	void	moveToEndOfWord(QTextCursor& cursor, bool select);
	void	setCursorByContext(const struct PosContext &context, const PosContext &acontext);
    int		countCharUntil(QTextBlock block, int pos, QChar ch) const;
    struct PosContext	contextAt(int pos);		//	pos 位置情報を構築
    //int		srcToPrvHeading(int blockNum);		//	エディタの見出し行番号（0 org.）をプレビューのそれに変換
    int		findPosition(const struct PosContext&);
    void	syncEditorCursorFromPreview();
    void	tagJump();
	void	make_link();
	int		linkClickedPos() const { return m_linkClickedPos; }
	bool	isComposing() const { return m_isComposing; }
	void	insertEnter();		//	カーソル位置に改行挿入（＋オートテキスト・インデント）
	void	deleteWord();

signals:
    void	tab_pressed();
    void	esc_pressed();
    //void	title_clicked(const QString title, const QStrin);
    void	link_clicked(const QString& title, const QString&, const QString&, bool readOnly = false);
    void	changeFontSize(int delta);
    void	posContextChanged(const PosContext &context, const PosContext &acontext);

protected:
    void	keyPressEvent(QKeyEvent *e) override;
    void	mouseReleaseEvent(QMouseEvent *event) override;
    void	mouseDoubleClickEvent(QMouseEvent *e) override;
    void	wheelEvent(QWheelEvent *event) override;
    void	dragEnterEvent(QDragEnterEvent *event) override;
    void	dropEvent(QDropEvent *event) override;
    void	paintEvent(QPaintEvent *e) override;
    void	resizeEvent(QResizeEvent *event) override;
    void	inputMethodEvent(QInputMethodEvent *event) override;
    void	insertFromMimeData(const QMimeData *source) override;
#if 0
    void	dragEnterEvent(QDragEnterEvent *e) override {
        e->acceptProposedAction();
    }
    void	dropEvent(QDropEvent *e) override {
    	e->ignore();
    }
#endif
    void	updateLnArea(const QRect &rect, int dy);
    void	onContentsChanged(int position, int charsRemoved, int charsAdded);
    void	onCursorPosChanged();
    int		nColumn(const QString&) const;		//	表示カラム数を計算
    void	setLineSpacing(int percentage);
    void	tagJump_sub(QTextCursor);
    //void	getWordStartEnd(QTextCursor, int& start, int&end);
    void	selectWordAt(QTextCursor&);

    void	do_keisen_left(bool erase = false, bool thickKeisen = false);
    void	do_keisen_right(bool erase = false, bool thickKeisen = false);
    void	do_keisen_up(bool erase = false, bool thickKeisen = false);
    void	do_keisen_down(bool erase = false, bool thickKeisen = false);
	void	applyAlignment(Align align);
	bool	isInComment(int pos) const;
	bool	isInLinkURL(int pos, int& openIX, int& closeIX) const;

private:
	bool	m_ignoreContentsChanged = false;
	bool	m_processing = false;			//	罫線保護処理中
	bool	m_isComposing = false;			// IME入力中フラグ
	bool	m_lnAreaPressed = false;
	bool	m_isCursorAboveAnchor = false;		//	アンカーから上（ドキュメント先頭方向）に向かって選択されている
	int		m_anchorStartPosition = 0;
	int		m_anchorBlockNum = 0;
	int		m_curBlockNum = 0;
	int		m_selStart = 0;
	int		m_selEnd = 0;
	int		m_linkClickedPos = -1;			//	リンククリック位置
	QString	m_lastCurBlockText;				//	事前のカーソルブロックテキスト
    class MarkdownHighlighter *m_highlighter;
	class LnAreaWidget	*m_lnAreaWidget = nullptr;
    DocWidget	*m_docWidget;
    const MainWindow *m_mainWindow = nullptr;
};

