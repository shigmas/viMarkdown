#pragma once

#include <QtWidgets/QMainWindow>
#include <qsplitter>
#include "ui_MainWindow.h"
//#include "markdowntohtmlconvertor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

#define	KEY_IGNORE_CASE				u"ignoreCase"
#define	KEY_EDITOR_FONT_SIZE		u"editorFontSize"
#define	KEY_PREVIEW_FONT_SIZE		u"previewFontSize"
#define	KEY_ACTIVA_LINE_COLOR		u"activeLineColor"
#define	KEY_INACTIVA_LINE_COLOR		u"inactiveLineColor"
#define	KEY_HEADINGS_COLOR			u"headingsColor"
#define	KEY_BOLD_ITALIC_COLOR		u"boldItalicColor"
#define	KEY_BOLD_COLOR				u"boldColor"
#define	KEY_ITALIC_COLOR			u"italicColor"
#define	KEY_STRIKETHROUGH_COLOR		u"strikethroughColor"
#define	KEY_MATCH_COLOR				u"matchColor"
#define	KEY_CSV_HEADER_COLOR		u"CSVHeadingsColor"
#define	KEY_CSV_ZEBRA_COLOR1		u"CSVZebraColor1"
#define	KEY_CSV_ZEBRA_COLOR2		u"CSVZebraColor2"
#define	KEY_QUOTE_COLOR				u"quoteColor"
#define	KEY_CODE_BLOCK_COLOR		u"codeBlockColor"
#define	KEY_KEISEN_BLOCK_COLOR		u"keisenBlockColor"

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
//	行フラグ
enum {
	US_DEFAULT = -1,	//	本文など
	//US_BODY = 0,		//	本文など
	US_HEADING,			//	タイトル・見出し
	US_CHECKBOX,		//	チェックボックス
	US_IN_COMMENT,		//	行頭がコメントブロック（<!-- -->）の中
	US_CSV_BLOCK,		//	CSVブロック内 ```CSV ～ ```
	US_KEISEN_BLOCK,	//	罫線ブロック内 ```keisen ～ ```
	US_TABLE,			//	マークダウンテーブル内
};

struct PosContext {
    QChar	m_anchorChar;			//	アンカー文字、行末の場合は QChar() かもしれない
    int		m_nth = 0;				//	直前見出しからの m_anchorChar 出現回数 (1 org. 自分自身も含むため)、0 for 無効
    int		m_offset = 0;			//	m_anchorChar からカーソル位置までの距離（0以上）
    int		m_srcHBlockNum = 0;		//	直前見出し行ブロック番号（0 org.）、見つからない場合は 0
    int		m_prvHBlockNum = 0;		//	直前見出し行ブロック番号（0 org.）、見つからない場合は 0
};
#define		U_KEISEN_BLOCK	0xe000		//	罫線ブロック文字 ユニコード（PAU)

class DocWidget;

#define		N_PATH_HIST		32

//using QStringPair = std::pair<QString, QString>;
struct DocLocation {		//	Alt + 左右 ナビゲーション用
	QString	m_title;
	QString	m_fullPath;
	int		m_cursorLine;		//	0 オリジンブロック番号
public:
	DocLocation(const QString title = QString(), const QString fullPath = QString(), int bn = 0)
		: m_title(title), m_fullPath(fullPath), m_cursorLine(bn)
	{
	}
};

struct Global {
	bool	m_ignoreCase;			//	検索時：大文字小文字同一視
	int		m_editorFontSize;
	int		m_previewFontSize;
	QColor	m_activeLnColor;		//	アクティブ時アンダーライン行カーソル色
	QColor	m_inactiveLnColor;		//	非アクティブ時アンダーライン行カーソル色
	QColor	m_headingsColor;
	QColor	m_boldItalicColor;
	QColor	m_boldColor;
	QColor	m_italicColor;
	QColor	m_strikethroughColor;
	QColor	m_matchColor;			//	検索マッチ背景色
	QColor	m_CSVHeaderColor;
	QColor	m_CSVZebraColor1;		//	本体奇数行
	QColor	m_CSVZebraColor2;		//	本体偶数行
	QColor	m_quoteColor;			//	引用ブロック背景色
	QColor	m_codeBlockColor;		//	コードブロック背景色
	QColor	m_keisenBlockColor;		//	罫線ブロック背景色
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool	isKeisenMode() const { return m_keisenMode; }
    bool	isThickKeisenMode() const { return m_thickKeisen; }
    //int		editorFontSize() const { return m_editorFontSize; }		//	暫定的
    const QString	srcText() const { return m_srcText; }
    void	setCursorCyncing(bool b = true) const { m_isCursorSyncing = b; }
    bool	isCursorCyncing() const { return m_isCursorSyncing; }
    bool	do_open(const QString& title, const QString& fullPath, const QString name = QString(), bool readOnly = false);

protected:
    void	load_settings();
    void	save_settings();
    void	setup_connections();
    void	setup_tabMenu();
    void	setup_encodingCombo();
    void	restore_win();
    DocWidget	*newTabWidget(const QString& title, const QString& fullPath, bool readOnly = false);
    void	onMDTextChanged();
    QSplitter	*getCurTabSplitter();
    DocWidget	*getCurDocWidget();
    //DocWidget	*findDocWidget(const QString& fullPath);
    int		tabIndexOf(const QString& title, const QString& fullPath);
    void	addTab(const QString& title, const QString fullPath = "", const QString txt = "",
    				bool=true, QStringConverter::Encoding encoding = QStringConverter::Utf8, bool readOnly = false);
    void	addTopItemToTreeWidget(const QString& title, const QString fullPath);
    void	updateHTMLModeCheck();
    void	updateThinThickCheck();
    void	updatePreview();
    void	updateOutlineTree();
    void	insertInline(const QString&);
    QTreeWidgetItem* findTopLevelItemByFullPath(const QString& title, const QString fullPath);
    void	removeTopLevelItem(DocWidget*);
    void	do_load(const QString&);
    void	close_empty_doc();
    void	do_save(bool fDialog = false);
    int		treeItemToTabIndex(QTreeWidgetItem *current);
    void	addToRecentFiles(const QString& fullPath);
    void	insertSearchComboBox();
    void	do_find(bool backward=false);
    void	do_search(const QString txt, bool backward);
    void	do_replace_next(const QString, const QString);
    void	do_replace_all(const QString, const QString);
    void	do_undo_replaceDlg();
    void	do_redo_replaceDlg();

    void	onOutlineBarVisibilityChanged(bool visible);
    void	onTreeCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void	onTreeItemActivated(QTreeWidgetItem *current, int);
    void	onMdEditCurPosChanged();
    void	onMdEditTabPressed();
    void	onMdEditEscPressed();
    void	onFileChanged(const QString&);
    void	onModificationChanged(bool);
    void	onSearchCBActivated();
    void	onMarkdownPreviewLineClicked(int);
    void	onEditorCurPosChanged();
    void	onPreviewCurPosChanged();
    //void	onAnchorClicked(const QString&);
    void	onCurrentTabChanged(int);
    void	appendToDocLoc(const QString&, const QString&, int);
    void	removeHistoryEntries(const QString&, const QString&);
    void	onEncodingChanged(int);
    void	onTextInsertedAtPreview(QString);
    void	onTextRemovedAtPreview(int);
    void	onBS_pressed();
    void	onDel_pressed();
    //void	syncPreviewCursorWithEditor();
    void	onChangeEditorFontSize(int);
    void	onChangePreviewFontSize(int);
    void	updateEditorFontSize(int);
    void	onSettingsChanged();
    void	onUndoTriggered();
    void	onRedoTriggered();
    void	onPrvPosContextChanged(const struct PosContext&, const PosContext &acontext);
    void	onSrcPosContextChanged(const struct PosContext&, const PosContext &acontext);
    void	onSrcCursorPosChanged();
    void	syncEditorPreviewScroll();

    void	onAction_New();
    void	onAction_NewTab();
    void	onAction_Open();
    void	onAction_Save();
    void	onAction_SaveAs();
    void	onAction_ExportAsPDF();
    void	onAction_Close();
    void	onAction_Print();
    void	onAction_Undo();
    void	onAction_Redo();
    void	onAction_SelectAll();
    void	onAction_Paste();
    void	onAction_Heading();
    void	onAction_Bold();
    void	onAction_Italic();
    void	onAction_Strikethrough();
    void	onAction_Link();
    void	onAction_List();
    void	onAction_NumList();
    void	onAction_Checkbox();
    void	onAction_Indent();
    void	onAction_UnIndent();
    void	onAction_AlignLeft();
    void	onAction_AlignCenter();
    void	onAction_AlignRight();
    void	onAction_TodayString_1();
    void	onAction_TodayString_2();
    void	onAction_TodayString_3();
    void	onAction_CSV_MarkdownTable();
    void	onAction_MarkdownTable_CSV();
    void	onAction_NaviForward();
    void	onAction_NaviBack();
    void	onAction_Find();
    void	onAction_ForwardAgain();
    void	onAction_BackwardAgain();
    void	onAction_FindWord();
    void	onAction_ClearSearchHighlights();
    void	onAction_Replace();
    void	onAction_KeisenMode(bool);
    void	onAction_ThinKeisen(bool);
    void	onAction_ThickKeisen(bool);
    void	onAction_OpenPrev();
    void	onAction_OpenNext();
    void	onAction_HTML(bool);
    void	onAction_Source(bool);
    void	onAction_OutlineBar(bool);
    void	onAction_FocusOutline();
    void	onAction_NextTab();
    void	onAction_PrevTab();
    void	onAction_ToggleFocus();
    void	onAction_SwitchToAltFile();
    void	onAction_TagJump();
    void	onAction_About();
    void	onAction_AddThisFavorite();
    void	onAction_Settings();
    void	onAction_Help();
    void	onAction_Exit();

    void	onAboutToShow_RecentFiles();
    void	onAboutToShow_FavoriteFiles();
    void	onAboutToShow_Insert();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    bool	m_processing = false;
    mutable bool	m_isCursorSyncing = false;		//	カーソル同期処理中
    bool	m_opening_file = false;
    bool	m_ignore_changed = false;
    bool	m_htmlMode = true;
    bool	m_keisenMode = false;
    bool	m_thickKeisen = false;		//	Thin or Thick
    int		m_tab_number = 0;
    //int		m_editorFontSize = 12;		//	暫定的
    //QString	m_plainText;
    //QString	m_htmlText;
    //MarkdownToHtmlConvertor	m_htmlComvertor;
    QString		m_curFullPath;			//	現文書フルパス
    QString		m_curTitle;				//	現文書タイトル
    QString		m_altFullPath;			//	直前文書フルパス
    QString		m_altTitle;				//	直前文書タイトル
    QString		m_srcText;				//	検索文字列
    QStringList	m_searchHist;			//	検索履歴、先頭が最新検索文字列
    QStringList	m_replaceHist;			//	置換履歴、先頭が最新置換文字列
    class QLabel		*m_lcLabel;		//	行カラムラベル
    class QLabel		*m_encLabel;	//	文字エンコーディングラベル
    class QComboBox				*m_encodingCombo = nullptr;
    class QComboBox				*m_searchCB = nullptr;
    class QFileSystemWatcher	*m_watcher;
    //QList<QStringPair>			m_pathTitleList;
    int							m_docLocIX = -1;
    QList<DocLocation>			m_docLocHist;

    Ui::MainWindowClass *ui;
};

