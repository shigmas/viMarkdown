#pragma once

#include <QtWidgets/QMainWindow>
#include <qsplitter>
#include "ui_MainWindow.h"
//#include "markdowntohtmlconvertor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class DocWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool	isKeisenMode() const { return m_keisenMode; }
    bool	isThickKeisenMode() const { return m_thickKeisen; }

protected:
    void	setup_connections();
    void	restore_win();
    DocWidget	*newTabWidget(const QString& title, const QString& fullPath);
    void	onMDTextChanged();
    QSplitter	*getCurTabSplitter();
    DocWidget	*getCurDocWidget();
    //DocWidget	*findDocWidget(const QString& fullPath);
    int		tabIndexOf(const QString& title, const QString& fullPath);
    void	addTab(const QString& title, const QString fullPath = "", const QString txt = "");
    void	addTopItemToTreeWidget(const QString& title, const QString fullPath);
    void	updateHTMLModeCheck();
    void	updateThinThickCheck();
    void	updatePreview();
    void	updateOutlineTree();
    void	insertInline(const QString&);
    QTreeWidgetItem* findTopLevelItemByFullPath(const QString& title, const QString fullPath);
    void	removeTopLevelItem(DocWidget*);
    bool	do_open(const QString&);
    void	do_load(const QString&);
    void	close_empty_doc();
    void	do_save(bool fDialog = false);
    int		treeItemToTabIndex(QTreeWidgetItem *current);
    void	addToRecentFiles(const QString& fullPath);
    void	insertSearchComboBox();
    void	do_find(bool backward=false);

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
    void	onAction_Find();
    void	onAction_ForwardAgain();
    void	onAction_BackwardAgain();
    void	onAction_FindWord();
    void	onAction_KeisenMode(bool);
    void	onAction_ThinKeisen(bool);
    void	onAction_ThickKeisen(bool);
    void	onAction_HTML(bool);
    void	onAction_Source(bool);
    void	onAction_OutlineBar(bool);
    void	onAction_FocusOutline();
    void	onAction_About();
    void	onAction_Exit();

    void	onAboutToShow_RecentFiles();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    bool	m_opening_file = false;
    bool	m_ignore_changed = false;
    bool	m_htmlMode = true;
    bool	m_keisenMode = false;
    bool	m_thickKeisen = false;		//	Thin or Thick
    int		m_tab_number = 0;
    //QString	m_plainText;
    //QString	m_htmlText;
    //MarkdownToHtmlConvertor	m_htmlComvertor;
    class QLabel		*m_lcLabel;		//	行カラムラベル
    class QComboBox				*m_searchCB = nullptr;
    class QFileSystemWatcher	*m_watcher;

    Ui::MainWindowClass *ui;
};

