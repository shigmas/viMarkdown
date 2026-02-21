#pragma once
#include <vector>
#include <QTextCursor>
#include <QDragEnterEvent>
#include "C:\Qt\6.10.0\msvc2022_64\include\QtWidgets\qtextedit.h"

class MainWindow;
class DocWidget;

/*
	マークダウンプレビューを表示するためのクラス

	QTextBlock.userState : int型：行種別情報（US_XXX）

*/

enum {
	US_DEFAULT = -1,	//	本文など
	//US_BODY = 0,		//	本文など
	US_HEADING,			//	タイトル・見出し
	US_CHECKBOX,		//	チェックボックス
};

class MarkdownPreview : public QTextEdit
{
	Q_OBJECT 

public:
    MarkdownPreview(const MainWindow*, DocWidget*, QWidget* parent = nullptr);

public:
    bool	isProcessing() const { return m_processing; }
    void	setMarkdown(class QTextDocument*);
    const QStringList&	getHeadings() const { return m_headingList; }
    //const std::vector<int>&	getSrcHeadingsBlocks() const { return m_srcHeadingBlocks; }
    //const std::vector<int>&	getPrvHeadingsBlocks() const { return m_prvHeadingBlocks; }		//	見出し行だけの行番号（0 org）リスト
    void	setCursorAt(int srcBlockNum, QString=QString(), int=0);
    void	setCursorAtNthPat(int srcBlockNum, QString pat, int nth, bool=false);
    void	ensureLineVisible(int srcBlockNum);
    void	scrollToBlock(int blockIndex);
    struct PosContext	contextAt(int pos);
    int		prvToSrcHeading(int blockNum);		//	プレビューの見出し行番号（0 org.）をエディタのそれに変換
	void	setCursorByContext(const struct PosContext &context);
    int		findPosition(const struct PosContext&);

signals:
    // クリックされたブロック番号を通知するシグナル
    void	lineClicked(int blockNumber);
    void	anchorClicked(const QString &title, const QString &anchor, const QString name);
    void	textInserted(QString);
    void	textRemoved(int);
    void	BS_pressed();		//	BackSpace
    void	Del_pressed();		//	Delete
    void	undo_triggered();
    void	redo_triggered();
    void	posContextChanged(const PosContext &context);

protected:
    void	keyPressEvent(QKeyEvent *e) override;
    void	mouseMoveEvent(QMouseEvent *e) override;
    void	mouseReleaseEvent(QMouseEvent *e) override;    // マウスクリックイベントをオーバーライド
    void	paintEvent(QPaintEvent *e) override;
    void	dragEnterEvent(QDragEnterEvent *e) override {
    	e->ignore();
        //e->acceptProposedAction();
    }
    void	dropEvent(QDropEvent *e) override {
    	e->ignore();
    }
    void	inputMethodEvent(QInputMethodEvent *event) override;

    void	onCurPosChanged();
    void	onContentsChanged(int position, int charsRemoved, int charsAdded);
    //bool	isTableLine(const QString&);
    //bool	isTableHyphenLine(const QString&);

    void	do_body(QTextCursor&);
    void	do_body_sub(QTextCursor&, const QString&);
    void	do_table(QTextCursor&);
    bool	do_underlineHeading(QTextCursor&, QString buf);
    void	do_heading(QTextCursor&, QString buf);
    void	do_heading_sub(QTextCursor&, QString buf, int h, int ln);
    void	do_list(QTextCursor&, QString buf);
    void	do_numlist(QTextCursor&, QString buf);
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
	bool	m_isComposing = false;			// IME入力中フラグ
	QString	m_lastCurBlockText;				//	事前のカーソルブロックテキスト
    //QString	m_bodyText;
    QStringList	m_bodyList;
	QStringList	m_lst;
	QStringList	m_headingList;		//	見出しレベル（1～9）＋見出し文字列
	//std::vector<int>	m_srcHeadingBlocks;		//	各見出し行 ブロック番号（0 org.）in マークダウンソース
	//std::vector<int>	m_prvHeadingBlocks;		//	各見出し行 ブロック番号（0 org.）in マークダウンプレビューワ
	QList<QStringView>	m_tableTokens;
	std::vector<char>	m_tableAlign;		//	各カラムの水平方向アライメント

	class DocWidget	*m_docWidget;
    const MainWindow *m_mainWindow = nullptr;
};

