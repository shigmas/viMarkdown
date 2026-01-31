#pragma once
#include <vector>
#include <QTextCursor>
#include "C:\Qt\6.10.0\msvc2022_64\include\QtWidgets\qtextedit.h"

class MainWindow;

class MarkdownViewer : public QTextEdit
{
	Q_OBJECT 

public:
    MarkdownViewer(const MainWindow*, QWidget* parent = nullptr);

public:
    void	setMarkdown(class QTextDocument*);
    const QStringList&	getHeadings() const { return m_headingList; }
    const std::vector<int>&	getHeadingsLineNum() const { return m_headingSrcLineNum; }
    const std::vector<int>&	getHeadingsBlockNum() const { return m_headingBlockNum; }
    void	setCursorAt(int srcBlockNum, QString=QString(), int=0);
    void	setCursorAtNthPat(int srcBlockNum, QString pat, int nth);
    void	ensureLineVisible(int srcBlockNum);
    void	scrollToBlock(int blockIndex);

signals:
    // クリックされたブロック番号を通知するシグナル
    void	lineClicked(int blockNumber);
    void	anchorClicked(const QString &anchor);
    void	textInserted(QString);

protected:
    void	mouseMoveEvent(QMouseEvent *e) override;
    void	mouseReleaseEvent(QMouseEvent *e) override;    // マウスクリックイベントをオーバーライド
    void	paintEvent(QPaintEvent *e) override;
    void	onCurPosChanged();
    void	onContentsChanged(int position, int charsRemoved, int charsAdded);
    bool	isTableLine(const QString&);
    bool	isTableHyphenLine(const QString&);

    void	do_body(QTextCursor&);
    void	do_body_sub(QTextCursor&, const QString&);
    void	do_table(QTextCursor&);
    bool	do_underlineHeading(QTextCursor&, QString buf);
    void	do_heading(QTextCursor&, QString buf);
    void	do_heading_sub(QTextCursor&, QString buf, int h, int ln);
    void	do_list(QTextCursor&, QString buf);
    void	do_quote(QTextCursor&, QString buf);
    void	do_code(QTextCursor&);
    void	do_code_keisen(QTextCursor&);
    void	do_CSV(QTextCursor&);

private:
    int		m_ln;
    int		m_nSpaces;
    int		m_nEmptyLines = 0;		//	本文最後の空行数
    bool	m_hasBody = false;		//	do_body() で空文以外を出力したか？
    bool	m_inComment = false;
    bool	m_processing = false;	//	再入防止用フラグ
    //QString	m_bodyText;
    QStringList	m_bodyList;
	QStringList	m_lst;
	QStringList	m_headingList;		//	見出しレベル（1～9）＋見出し文字列
	std::vector<int>	m_headingSrcLineNum;	//	各見出し行 行番号（0 org.）in マークダウンソース
	std::vector<int>	m_headingBlockNum;	//	各見出し行 ブロック番号（0 org.）in マークダウンプレビューワ
	QList<QStringView>	m_tableTokens;
	std::vector<char>	m_tableAlign;		//	各カラムの水平方向アライメント

    const MainWindow *m_mainWindow = nullptr;
};

