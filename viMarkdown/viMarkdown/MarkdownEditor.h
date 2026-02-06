#pragma once
#include <QScrollBar>
#include "C:\Qt\6.10.0\msvc2022_64\include\QtWidgets\qplaintextedit.h"

enum class Align { Left, Center, Right };

const int LN_WIDTH = 7;

class MainWindow;

#define		MarkdownBaseEdit	QPlainTextEdit
//#define		MarkdownBaseEdit	QTextEdit		//	QPlainTextEdit にしかないシグナルがあるため無理

class MarkdownEditor : public MarkdownBaseEdit
{
	Q_OBJECT
public:
	MarkdownEditor(const MainWindow* mainWindow, QWidget *parent = nullptr);
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
    void	rehighlight();
    void	setBoldColor(QColor);

signals:
    void	tab_pressed();
    void	esc_pressed();
    void	title_clicked(const QString title);
    void	changeFontSize(int delta);

protected:
    void	keyPressEvent(QKeyEvent *e) override;
    void	mouseReleaseEvent(QMouseEvent *event) override;
    void	wheelEvent(QWheelEvent *event) override;
    void	paintEvent(QPaintEvent *e) override;
    void	resizeEvent(QResizeEvent *event) override;
    void	inputMethodEvent(QInputMethodEvent *event) override;
    void	dragEnterEvent(QDragEnterEvent *e) override {
        e->acceptProposedAction();
    }
    void	dropEvent(QDropEvent *e) override {
    	e->ignore();
    }

    void	updateLnArea(const QRect &rect, int dy);
    void	onContentsChanged(int position, int charsRemoved, int charsAdded);
    void	onCurPosChanged();
    int		nColumn(const QString&) const;		//	表示カラム数を計算
    void	setLineSpacing(int percentage);

    void	do_keisen_left(bool erase = false, bool thickKeisen = false);
    void	do_keisen_right(bool erase = false, bool thickKeisen = false);
    void	do_keisen_up(bool erase = false, bool thickKeisen = false);
    void	do_keisen_down(bool erase = false, bool thickKeisen = false);
	void	applyAlignment(Align align);

private:
	bool	m_processing = false;			//	罫線保護処理中
	bool	m_isComposing = false;			// IME入力中フラグ
	bool	m_lnAreaPressed = false;
	bool	m_isCursorAboveAnchor = false;		//	アンカーから上（ドキュメント先頭方向）に向かって選択されている
	int		m_anchorStartPosition = 0;
	int		m_anchorBlockNum = 0;
	int		m_curBlockNum = 0;
	int		m_selStart = 0;
	int		m_selEnd = 0;
	QString	m_lastCurBlockText;				//	事前のカーソルブロックテキスト
    class MarkdownHighlighter *m_highlighter;
	class LnAreaWidget	*m_lnAreaWidget = nullptr;
    const MainWindow *m_mainWindow = nullptr;
};

