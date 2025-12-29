#pragma once
#include <QTextCursor>
#include "C:\Qt\6.10.0\msvc2022_64\include\QtWidgets\qtextedit.h"
class MarkdownViewer : public QTextEdit
{
	Q_OBJECT 

public:
    MarkdownViewer(QWidget* parent = nullptr) : QTextEdit(parent)
    {
    }
public:
    void	setMarkdown(class QTextDocument*);

signals:
    // クリックされたブロック番号を通知するシグナル
    void lineClicked(int blockNumber);

protected:
    void mousePressEvent(QMouseEvent *e) override;    // マウスクリックイベントをオーバーライド

    void	do_heading(QTextCursor&, QString buf);
};

