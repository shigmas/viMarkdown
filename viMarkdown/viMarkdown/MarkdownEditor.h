#pragma once
#include <QScrollBar>
#include "C:\Qt\6.10.0\msvc2022_64\include\QtWidgets\qplaintextedit.h"

enum class Align { Left, Center, Right };

class MainWindow;

class MarkdownEditor : public QPlainTextEdit
{
	Q_OBJECT
public:
	MarkdownEditor(const MainWindow* mainWindow, QWidget *parent = nullptr);
public:
	void	scrollToTop(int lineNum) {		//	lineNum: 0 org.
		verticalScrollBar()->setValue(lineNum);
	}
	void	scrollToTop(const QTextCursor &cursor);
	//{
	//	this->scrollToTop(cursor.blockNumber());
	//}
	int getVisualLineNumber(const QTextCursor &cursor) const;
	void	onAlignLeft();
	void	onAlignCenter();
	void	onAlignRight();

signals:
    void tab_pressed();
    void esc_pressed();

protected:
    void	keyPressEvent(QKeyEvent *e) override;
    void	paintEvent(QPaintEvent *e) override;
    void	onContentsChanged(int position, int charsRemoved, int charsAdded);

    void	do_keisen_left(bool erase = false);
    void	do_keisen_right(bool erase = false);
    void	do_keisen_up(bool erase = false);
    void	do_keisen_down(bool erase = false);
	void	applyAlignment(Align align);

private:
    class MarkdownHighlighter *m_highlighter;

    const MainWindow *m_mainWindow = nullptr;
};

