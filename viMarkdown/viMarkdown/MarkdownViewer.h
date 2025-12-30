#pragma once
#include <vector>
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
    const QStringList&	getHeadings() const { return m_headingList; }
    const std::vector<int>&	getHeadingsLineNum() const { return m_headingLineNum; }

signals:
    // クリックされたブロック番号を通知するシグナル
    void lineClicked(int blockNumber);

protected:
    void mousePressEvent(QMouseEvent *e) override;    // マウスクリックイベントをオーバーライド

    void	do_body(QTextCursor&);
    void	do_heading(QTextCursor&, QString buf);
    void	do_list(QTextCursor&, QString buf);
    void	do_quote(QTextCursor&, QString buf);

private:
    int		m_ln;
    int		m_nSpaces;
    QString	m_bodyText;
	QStringList	m_lst;
	QStringList	m_headingList;		//	見出しレベル（1～9）＋見出し文字列
	std::vector<int>	m_headingLineNum;	//	各見出し行 行番号（0 org.）
};

