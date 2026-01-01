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
    bool	isTableLine(const QString&);
    bool	isTableHyphenLine(const QString&);

    void	do_body(QTextCursor&);
    void	do_table(QTextCursor&);
    bool	do_underlineHeading(QTextCursor&, QString buf);
    void	do_heading(QTextCursor&, QString buf);
    void	do_heading_sub(QTextCursor&, QString buf, int h, int ln);
    void	do_list(QTextCursor&, QString buf);
    void	do_quote(QTextCursor&, QString buf);
    void	do_code(QTextCursor&);

private:
    int		m_ln;
    int		m_nSpaces;
    //QString	m_bodyText;
    QStringList	m_bodyList;
	QStringList	m_lst;
	QStringList	m_headingList;		//	見出しレベル（1～9）＋見出し文字列
	std::vector<int>	m_headingLineNum;	//	各見出し行 行番号（0 org.）
	QList<QStringView>	m_tableTokens;
	std::vector<char>	m_tableAlign;		//	各カラムの水平方向アライメント
};

