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
#define	KEY_ACTIVA_LINE_COLOR		u"activeLineColor"
#define	KEY_INACTIVA_LINE_COLOR		u"inactiveLineColor"
#define	KEY_HEADINGS_COLOR			u"headingsColor"
#define	KEY_BOLD_COLOR				u"boldColor"
#define	KEY_CSV_HEADER_COLOR		u"CSVHeadingsColor"
#define	KEY_CSV_ZEBRA_COLOR1		u"CSVZebraColor1"
#define	KEY_CSV_ZEBRA_COLOR2		u"CSVZebraColor2"

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
	QColor	m_activeLnColor;		//	アクティブ時アンダーライン行カーソル色
	QColor	m_inactiveLnColor;		//	非アクティブ時アンダーライン行カーソル色
	QColor	m_headingsColor;
	QColor	m_boldColor;
	QColor	m_CSVHeaderColor;
	QColor	m_CSVZebraColor1;		//	本体奇数行
	QColor	m_CSVZebraColor2;		//	本体偶数行
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

protected:
    void	load_settings();
    void	save_settings();
    void	setup_connections();
    void	setup_tabMenu();
    void	setup_encodingCombo();
    void	restore_win();
    DocWidget	*newTabWidget(const QString& title, const QString& fullPath);
    void	onMDTextChanged();
    QSplitter	*getCurTabSplitter();
    DocWidget	*getCurDocWidget();
    //DocWidget	*findDocWidget(const QString& fullPath);
    int		tabIndexOf(const QString& title, const QString& fullPath);
    void	addTab(const QString& title, const QString fullPath = "", const QString txt = "",
    				bool=true, QStringConverter::Encoding encoding = QStringConverter::Utf8);
    void	addTopItemToTreeWidget(const QString& title, const QString fullPath);
    void	updateHTMLModeCheck();
    void	updateThinThickCheck();
    void	updatePreview();
    void	updateOutlineTree();
    void	insertInline(const QString&);
    QTreeWidgetItem* findTopLevelItemByFullPath(const QString& title, const QString fullPath);
    void	removeTopLevelItem(DocWidget*);
    bool	do_open(const QString& title, const QString& fullPath, const QString name = QString());
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

    void	onOutlineBarVisibilityChanged(bool visible);
    void	onTreeCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void	onTreeItemActivated(QTreeWidgetItem *current, int);
    void	onMdEditCurPosChanged();
    void	onMdEditTabPressed();
    void	onMdEditEscPressed();
    void	onFileChanged(const QString&);
    void	onModificationChanged(bool);
    void	onSearchCBActivated();
    void	onMarkdownViewerLineClicked(int);
    void	onEditorCurPosChanged();
    void	onViewerCurPosChanged();
    //void	onAnchorClicked(const QString&);
    void	onCurrentTabChanged(int);
    void	appendToDocLoc(const QString&, const QString&, int);
    void	onEncodingChanged(int);
    void	onTextInsertedAtViewer(QString);
    void	onTextRemovedAtViewer(int);
    void	onBS_pressed();
    void	syncPreviewCursorWithEditor();
    void	onChangeEditorFontSize(int);
    void	updateEditorFontSize(int);
    void	onSettingsChanged();

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
    void	onAction_Heading();
    void	onAction_Bold();
    void	onAction_Italic();
    void	onAction_Strikethrough();
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
    void	onAction_About();
    void	onAction_AddThisFavorite();
    void	onAction_Settings();
    void	onAction_Exit();

    void	onAboutToShow_RecentFiles();
    void	onAboutToShow_FavoriteFiles();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    bool	m_processing = false;
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

