/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *action_New;
    QAction *action_Close;
    QAction *action_HTML;
    QAction *action_Source;
    QAction *action_Open;
    QAction *action_Save;
    QAction *action_Bold;
    QAction *action_Italic;
    QAction *action_Undo;
    QAction *action_Redo;
    QAction *action_List;
    QAction *action_NumList;
    QAction *action_Indent;
    QAction *action_UnIndent;
    QAction *action_OutlineBar;
    QAction *actiondummy;
    QAction *action_AboutViMarkdown;
    QAction *action_Strikethrough;
    QAction *action_Checkbox;
    QAction *action_Cut;
    QAction *action_Copy;
    QAction *action_Paste;
    QAction *action_FocusOutline;
    QAction *action_Find;
    QAction *action_ForwardAgain;
    QAction *action_BackwardAgain;
    QAction *action_FindWord;
    QAction *action_SaveAs;
    QAction *action_AddThisFavorite;
    QAction *action_Exit;
    QAction *action_AsHTML;
    QAction *action_AsPDF;
    QAction *action_SelectAll;
    QAction *action_Print;
    QAction *action_KeisenMode;
    QAction *action_Heading;
    QAction *action_AlignLeft;
    QAction *action_AlignCenter;
    QAction *action_AlignRight;
    QAction *actionTickness;
    QAction *action_OpenNext;
    QAction *action_ThinKeisen;
    QAction *action_ThickKeisen;
    QAction *action_NewTab;
    QAction *action_TodayString_1;
    QAction *action_OpenPrev;
    QAction *action_NextTab;
    QAction *action_PrevTab;
    QAction *action_TodayString_2;
    QAction *action_TodayString_3;
    QAction *action_Reload;
    QAction *action_CSV_MarkdownTable;
    QAction *action_MarkdownTable_CSV;
    QAction *action_Settings;
    QAction *action_ToggleFocus;
    QAction *action_SwitchToAltFile;
    QAction *action_NaviBack;
    QAction *action_NaviForward;
    QAction *action_ClearSearchHighlights;
    QAction *action_Replace;
    QAction *action_Help;
    QAction *action_TagJump;
    QAction *action_Link;
    QAction *action_TestCharFlags;
    QAction *action_TestAll;
    QAction *action_TestLineCrsp;
    QAction *action_TestEtoPCurSync;
    QAction *action_TestContextAt;
    QAction *action_DumpBlockUserStates;
    QAction *action_DumpCharFlags;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QTabWidget *tabWidget;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_RecentFiles;
    QMenu *menu_FavoriteFiles;
    QMenu *menu_Exoirt;
    QMenu *menu_View;
    QMenu *menu_Edit;
    QMenu *menuinline;
    QMenu *menublock;
    QMenu *menu_format;
    QMenu *menu_Insert;
    QMenu *menu_Convert;
    QMenu *menu_Help;
    QMenu *menu_Test;
    QMenu *menu_Search;
    QMenu *menu_Tool;
    QToolBar *fileToolBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *outlineBar;
    QWidget *dockWidgetContents;
    QHBoxLayout *horizontalLayout_2;
    QTreeWidget *treeWidget;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName("MainWindowClass");
        MainWindowClass->resize(600, 400);
        action_New = new QAction(MainWindowClass);
        action_New->setObjectName("action_New");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/MainWindow/images/new_main_window_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_New->setIcon(icon);
        action_Close = new QAction(MainWindowClass);
        action_Close->setObjectName("action_Close");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/MainWindow/images/close_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Close->setIcon(icon1);
        action_HTML = new QAction(MainWindowClass);
        action_HTML->setObjectName("action_HTML");
        action_HTML->setCheckable(true);
        action_Source = new QAction(MainWindowClass);
        action_Source->setObjectName("action_Source");
        action_Source->setCheckable(true);
        action_Open = new QAction(MainWindowClass);
        action_Open->setObjectName("action_Open");
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/MainWindow/images/file_open_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Open->setIcon(icon2);
        action_Save = new QAction(MainWindowClass);
        action_Save->setObjectName("action_Save");
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/MainWindow/images/save_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Save->setIcon(icon3);
        action_Bold = new QAction(MainWindowClass);
        action_Bold->setObjectName("action_Bold");
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/MainWindow/images/format_bold_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Bold->setIcon(icon4);
        action_Italic = new QAction(MainWindowClass);
        action_Italic->setObjectName("action_Italic");
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/MainWindow/images/format_italic_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Italic->setIcon(icon5);
        action_Undo = new QAction(MainWindowClass);
        action_Undo->setObjectName("action_Undo");
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/MainWindow/images/undo_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Undo->setIcon(icon6);
        action_Redo = new QAction(MainWindowClass);
        action_Redo->setObjectName("action_Redo");
        QIcon icon7;
        icon7.addFile(QString::fromUtf8("../../../../../Users/ntsud/.designer/backup/images/redo_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon7.addFile(QString::fromUtf8(":/MainWindow/images/redo_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        action_Redo->setIcon(icon7);
        action_List = new QAction(MainWindowClass);
        action_List->setObjectName("action_List");
        QIcon icon8;
        icon8.addFile(QString::fromUtf8("../../../../../Users/ntsud/.designer/backup/images/format_list_bulleted_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon8.addFile(QString::fromUtf8(":/MainWindow/images/format_list_bulleted_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        action_List->setIcon(icon8);
        action_NumList = new QAction(MainWindowClass);
        action_NumList->setObjectName("action_NumList");
        QIcon icon9;
        icon9.addFile(QString::fromUtf8("../../../../../Users/ntsud/.designer/backup/images/format_list_numbered_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon9.addFile(QString::fromUtf8(":/MainWindow/images/format_list_numbered_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        action_NumList->setIcon(icon9);
        action_Indent = new QAction(MainWindowClass);
        action_Indent->setObjectName("action_Indent");
        QIcon icon10;
        icon10.addFile(QString::fromUtf8("../../../../../Users/ntsud/.designer/backup/images/format_indent_increase_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon10.addFile(QString::fromUtf8(":/MainWindow/images/format_indent_increase_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        action_Indent->setIcon(icon10);
        action_UnIndent = new QAction(MainWindowClass);
        action_UnIndent->setObjectName("action_UnIndent");
        QIcon icon11;
        icon11.addFile(QString::fromUtf8("../../../../../Users/ntsud/.designer/backup/images/format_indent_decrease_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon11.addFile(QString::fromUtf8(":/MainWindow/images/format_indent_decrease_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        action_UnIndent->setIcon(icon11);
        action_OutlineBar = new QAction(MainWindowClass);
        action_OutlineBar->setObjectName("action_OutlineBar");
        action_OutlineBar->setCheckable(true);
        actiondummy = new QAction(MainWindowClass);
        actiondummy->setObjectName("actiondummy");
        action_AboutViMarkdown = new QAction(MainWindowClass);
        action_AboutViMarkdown->setObjectName("action_AboutViMarkdown");
        QIcon icon12;
        icon12.addFile(QString::fromUtf8("../../../../../Users/ntsud/.designer/backup/images/info_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon12.addFile(QString::fromUtf8(":/MainWindow/images/info_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        action_AboutViMarkdown->setIcon(icon12);
        action_Strikethrough = new QAction(MainWindowClass);
        action_Strikethrough->setObjectName("action_Strikethrough");
        QIcon icon13;
        icon13.addFile(QString::fromUtf8("../../../../../Users/ntsud/.designer/backup/images/strikethrough_s_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon13.addFile(QString::fromUtf8(":/MainWindow/images/strikethrough_s_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        action_Strikethrough->setIcon(icon13);
        action_Checkbox = new QAction(MainWindowClass);
        action_Checkbox->setObjectName("action_Checkbox");
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/MainWindow/images/format_checkbox_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Checkbox->setIcon(icon14);
        action_Cut = new QAction(MainWindowClass);
        action_Cut->setObjectName("action_Cut");
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/MainWindow/images/content_cut_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Cut->setIcon(icon15);
        action_Copy = new QAction(MainWindowClass);
        action_Copy->setObjectName("action_Copy");
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/MainWindow/images/content_copy_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Copy->setIcon(icon16);
        action_Paste = new QAction(MainWindowClass);
        action_Paste->setObjectName("action_Paste");
        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/MainWindow/images/content_paste_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Paste->setIcon(icon17);
        action_FocusOutline = new QAction(MainWindowClass);
        action_FocusOutline->setObjectName("action_FocusOutline");
        action_Find = new QAction(MainWindowClass);
        action_Find->setObjectName("action_Find");
        QIcon icon18;
        icon18.addFile(QString::fromUtf8(":/MainWindow/images/search_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Find->setIcon(icon18);
        action_ForwardAgain = new QAction(MainWindowClass);
        action_ForwardAgain->setObjectName("action_ForwardAgain");
        QIcon icon19;
        icon19.addFile(QString::fromUtf8(":/MainWindow/images/search_forward_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_ForwardAgain->setIcon(icon19);
        action_BackwardAgain = new QAction(MainWindowClass);
        action_BackwardAgain->setObjectName("action_BackwardAgain");
        QIcon icon20;
        icon20.addFile(QString::fromUtf8(":/MainWindow/images/search_backard_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_BackwardAgain->setIcon(icon20);
        action_FindWord = new QAction(MainWindowClass);
        action_FindWord->setObjectName("action_FindWord");
        QIcon icon21;
        icon21.addFile(QString::fromUtf8(":/MainWindow/images/search_word_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_FindWord->setIcon(icon21);
        action_SaveAs = new QAction(MainWindowClass);
        action_SaveAs->setObjectName("action_SaveAs");
        QIcon icon22;
        icon22.addFile(QString::fromUtf8(":/MainWindow/images/save_as_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_SaveAs->setIcon(icon22);
        action_AddThisFavorite = new QAction(MainWindowClass);
        action_AddThisFavorite->setObjectName("action_AddThisFavorite");
        action_Exit = new QAction(MainWindowClass);
        action_Exit->setObjectName("action_Exit");
        QIcon icon23;
        icon23.addFile(QString::fromUtf8(":/MainWindow/images/exit_to_app_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Exit->setIcon(icon23);
        action_AsHTML = new QAction(MainWindowClass);
        action_AsHTML->setObjectName("action_AsHTML");
        action_AsPDF = new QAction(MainWindowClass);
        action_AsPDF->setObjectName("action_AsPDF");
        action_SelectAll = new QAction(MainWindowClass);
        action_SelectAll->setObjectName("action_SelectAll");
        action_Print = new QAction(MainWindowClass);
        action_Print->setObjectName("action_Print");
        QIcon icon24;
        icon24.addFile(QString::fromUtf8(":/MainWindow/images/print_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Print->setIcon(icon24);
        action_KeisenMode = new QAction(MainWindowClass);
        action_KeisenMode->setObjectName("action_KeisenMode");
        action_KeisenMode->setCheckable(true);
        QIcon icon25;
        icon25.addFile(QString::fromUtf8(":/MainWindow/images/table_edit_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon25.addFile(QString::fromUtf8(":/MainWindow/images/table_edit_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        action_KeisenMode->setIcon(icon25);
        action_Heading = new QAction(MainWindowClass);
        action_Heading->setObjectName("action_Heading");
        QIcon icon26;
        icon26.addFile(QString::fromUtf8(":/MainWindow/images/format_heading_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Heading->setIcon(icon26);
        action_AlignLeft = new QAction(MainWindowClass);
        action_AlignLeft->setObjectName("action_AlignLeft");
        QIcon icon27;
        icon27.addFile(QString::fromUtf8(":/MainWindow/images/format_align_left_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_AlignLeft->setIcon(icon27);
        action_AlignCenter = new QAction(MainWindowClass);
        action_AlignCenter->setObjectName("action_AlignCenter");
        QIcon icon28;
        icon28.addFile(QString::fromUtf8(":/MainWindow/images/format_align_center_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_AlignCenter->setIcon(icon28);
        action_AlignRight = new QAction(MainWindowClass);
        action_AlignRight->setObjectName("action_AlignRight");
        QIcon icon29;
        icon29.addFile(QString::fromUtf8(":/MainWindow/images/format_align_right_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_AlignRight->setIcon(icon29);
        actionTickness = new QAction(MainWindowClass);
        actionTickness->setObjectName("actionTickness");
        action_OpenNext = new QAction(MainWindowClass);
        action_OpenNext->setObjectName("action_OpenNext");
        action_ThinKeisen = new QAction(MainWindowClass);
        action_ThinKeisen->setObjectName("action_ThinKeisen");
        action_ThinKeisen->setCheckable(true);
        QIcon icon30;
        icon30.addFile(QString::fromUtf8(":/MainWindow/images/thin_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_ThinKeisen->setIcon(icon30);
        action_ThickKeisen = new QAction(MainWindowClass);
        action_ThickKeisen->setObjectName("action_ThickKeisen");
        action_ThickKeisen->setCheckable(true);
        QIcon icon31;
        icon31.addFile(QString::fromUtf8(":/MainWindow/images/thick_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_ThickKeisen->setIcon(icon31);
        action_NewTab = new QAction(MainWindowClass);
        action_NewTab->setObjectName("action_NewTab");
        QIcon icon32;
        icon32.addFile(QString::fromUtf8(":/MainWindow/images/new_tab_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_NewTab->setIcon(icon32);
        action_TodayString_1 = new QAction(MainWindowClass);
        action_TodayString_1->setObjectName("action_TodayString_1");
        action_OpenPrev = new QAction(MainWindowClass);
        action_OpenPrev->setObjectName("action_OpenPrev");
        action_NextTab = new QAction(MainWindowClass);
        action_NextTab->setObjectName("action_NextTab");
        action_PrevTab = new QAction(MainWindowClass);
        action_PrevTab->setObjectName("action_PrevTab");
        action_TodayString_2 = new QAction(MainWindowClass);
        action_TodayString_2->setObjectName("action_TodayString_2");
        action_TodayString_3 = new QAction(MainWindowClass);
        action_TodayString_3->setObjectName("action_TodayString_3");
        action_Reload = new QAction(MainWindowClass);
        action_Reload->setObjectName("action_Reload");
        action_CSV_MarkdownTable = new QAction(MainWindowClass);
        action_CSV_MarkdownTable->setObjectName("action_CSV_MarkdownTable");
        action_MarkdownTable_CSV = new QAction(MainWindowClass);
        action_MarkdownTable_CSV->setObjectName("action_MarkdownTable_CSV");
        action_Settings = new QAction(MainWindowClass);
        action_Settings->setObjectName("action_Settings");
        QIcon icon33;
        icon33.addFile(QString::fromUtf8(":/MainWindow/images/settings_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Settings->setIcon(icon33);
        action_ToggleFocus = new QAction(MainWindowClass);
        action_ToggleFocus->setObjectName("action_ToggleFocus");
        action_SwitchToAltFile = new QAction(MainWindowClass);
        action_SwitchToAltFile->setObjectName("action_SwitchToAltFile");
        action_NaviBack = new QAction(MainWindowClass);
        action_NaviBack->setObjectName("action_NaviBack");
        QIcon icon34;
        icon34.addFile(QString::fromUtf8(":/MainWindow/images/arrow_back_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_NaviBack->setIcon(icon34);
        action_NaviForward = new QAction(MainWindowClass);
        action_NaviForward->setObjectName("action_NaviForward");
        QIcon icon35;
        icon35.addFile(QString::fromUtf8(":/MainWindow/images/arrow_forward_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_NaviForward->setIcon(icon35);
        action_ClearSearchHighlights = new QAction(MainWindowClass);
        action_ClearSearchHighlights->setObjectName("action_ClearSearchHighlights");
        action_Replace = new QAction(MainWindowClass);
        action_Replace->setObjectName("action_Replace");
        action_Help = new QAction(MainWindowClass);
        action_Help->setObjectName("action_Help");
        QIcon icon36;
        icon36.addFile(QString::fromUtf8(":/MainWindow/images/help_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Help->setIcon(icon36);
        action_TagJump = new QAction(MainWindowClass);
        action_TagJump->setObjectName("action_TagJump");
        action_Link = new QAction(MainWindowClass);
        action_Link->setObjectName("action_Link");
        QIcon icon37;
        icon37.addFile(QString::fromUtf8(":/MainWindow/images/link_48.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        action_Link->setIcon(icon37);
        action_TestCharFlags = new QAction(MainWindowClass);
        action_TestCharFlags->setObjectName("action_TestCharFlags");
        action_TestAll = new QAction(MainWindowClass);
        action_TestAll->setObjectName("action_TestAll");
        action_TestLineCrsp = new QAction(MainWindowClass);
        action_TestLineCrsp->setObjectName("action_TestLineCrsp");
        action_TestEtoPCurSync = new QAction(MainWindowClass);
        action_TestEtoPCurSync->setObjectName("action_TestEtoPCurSync");
        action_TestContextAt = new QAction(MainWindowClass);
        action_TestContextAt->setObjectName("action_TestContextAt");
        action_DumpBlockUserStates = new QAction(MainWindowClass);
        action_DumpBlockUserStates->setObjectName("action_DumpBlockUserStates");
        action_DumpCharFlags = new QAction(MainWindowClass);
        action_DumpCharFlags->setObjectName("action_DumpCharFlags");
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName("centralWidget");
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setMovable(true);

        horizontalLayout->addWidget(tabWidget);

        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 600, 33));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName("menu_File");
        menu_RecentFiles = new QMenu(menu_File);
        menu_RecentFiles->setObjectName("menu_RecentFiles");
        menu_FavoriteFiles = new QMenu(menu_File);
        menu_FavoriteFiles->setObjectName("menu_FavoriteFiles");
        menu_Exoirt = new QMenu(menu_File);
        menu_Exoirt->setObjectName("menu_Exoirt");
        menu_View = new QMenu(menuBar);
        menu_View->setObjectName("menu_View");
        menu_Edit = new QMenu(menuBar);
        menu_Edit->setObjectName("menu_Edit");
        menuinline = new QMenu(menu_Edit);
        menuinline->setObjectName("menuinline");
        menublock = new QMenu(menu_Edit);
        menublock->setObjectName("menublock");
        menu_format = new QMenu(menu_Edit);
        menu_format->setObjectName("menu_format");
        menu_Insert = new QMenu(menu_Edit);
        menu_Insert->setObjectName("menu_Insert");
        menu_Convert = new QMenu(menu_Edit);
        menu_Convert->setObjectName("menu_Convert");
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName("menu_Help");
        menu_Test = new QMenu(menu_Help);
        menu_Test->setObjectName("menu_Test");
        menu_Search = new QMenu(menuBar);
        menu_Search->setObjectName("menu_Search");
        menu_Tool = new QMenu(menuBar);
        menu_Tool->setObjectName("menu_Tool");
        MainWindowClass->setMenuBar(menuBar);
        fileToolBar = new QToolBar(MainWindowClass);
        fileToolBar->setObjectName("fileToolBar");
        MainWindowClass->addToolBar(Qt::ToolBarArea::LeftToolBarArea, fileToolBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName("mainToolBar");
        MainWindowClass->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName("statusBar");
        MainWindowClass->setStatusBar(statusBar);
        outlineBar = new QDockWidget(MainWindowClass);
        outlineBar->setObjectName("outlineBar");
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName("dockWidgetContents");
        horizontalLayout_2 = new QHBoxLayout(dockWidgetContents);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        treeWidget = new QTreeWidget(dockWidgetContents);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName("treeWidget");
        treeWidget->setHeaderHidden(true);

        horizontalLayout_2->addWidget(treeWidget);

        outlineBar->setWidget(dockWidgetContents);
        MainWindowClass->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, outlineBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_Edit->menuAction());
        menuBar->addAction(menu_Search->menuAction());
        menuBar->addAction(menu_View->menuAction());
        menuBar->addAction(menu_Tool->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menu_File->addAction(action_New);
        menu_File->addAction(action_NewTab);
        menu_File->addAction(action_Open);
        menu_File->addAction(action_Reload);
        menu_File->addAction(action_Save);
        menu_File->addAction(action_SaveAs);
        menu_File->addAction(menu_Exoirt->menuAction());
        menu_File->addAction(action_Close);
        menu_File->addAction(menu_RecentFiles->menuAction());
        menu_File->addAction(menu_FavoriteFiles->menuAction());
        menu_File->addAction(action_Print);
        menu_File->addSeparator();
        menu_File->addAction(action_Exit);
        menu_RecentFiles->addAction(actiondummy);
        menu_FavoriteFiles->addAction(action_AddThisFavorite);
        menu_FavoriteFiles->addSeparator();
        menu_Exoirt->addAction(action_AsPDF);
        menu_View->addSeparator();
        menu_View->addAction(action_OutlineBar);
        menu_View->addAction(action_FocusOutline);
        menu_View->addAction(action_NextTab);
        menu_View->addAction(action_PrevTab);
        menu_View->addAction(action_ToggleFocus);
        menu_View->addAction(action_SwitchToAltFile);
        menu_View->addAction(action_NaviBack);
        menu_View->addAction(action_NaviForward);
        menu_View->addAction(action_TagJump);
        menu_Edit->addAction(action_Undo);
        menu_Edit->addAction(action_Redo);
        menu_Edit->addSeparator();
        menu_Edit->addAction(action_Cut);
        menu_Edit->addAction(action_Copy);
        menu_Edit->addAction(action_Paste);
        menu_Edit->addAction(action_SelectAll);
        menu_Edit->addSeparator();
        menu_Edit->addAction(menublock->menuAction());
        menu_Edit->addAction(menuinline->menuAction());
        menu_Edit->addAction(menu_format->menuAction());
        menu_Edit->addAction(action_Indent);
        menu_Edit->addAction(action_UnIndent);
        menu_Edit->addAction(menu_Insert->menuAction());
        menu_Edit->addAction(menu_Convert->menuAction());
        menuinline->addSeparator();
        menuinline->addAction(action_Bold);
        menuinline->addAction(action_Italic);
        menuinline->addAction(action_Strikethrough);
        menuinline->addAction(action_Link);
        menublock->addAction(action_List);
        menublock->addAction(action_NumList);
        menublock->addAction(action_Checkbox);
        menublock->addAction(action_Heading);
        menu_format->addAction(action_AlignLeft);
        menu_format->addAction(action_AlignCenter);
        menu_format->addAction(action_AlignRight);
        menu_Insert->addAction(action_TodayString_1);
        menu_Insert->addAction(action_TodayString_2);
        menu_Insert->addAction(action_TodayString_3);
        menu_Convert->addAction(action_CSV_MarkdownTable);
        menu_Convert->addAction(action_MarkdownTable_CSV);
        menu_Help->addAction(action_Settings);
        menu_Help->addAction(action_Help);
        menu_Help->addAction(menu_Test->menuAction());
        menu_Help->addAction(action_AboutViMarkdown);
        menu_Help->addSeparator();
        menu_Test->addAction(action_TestCharFlags);
        menu_Test->addAction(action_TestContextAt);
        menu_Test->addAction(action_TestLineCrsp);
        menu_Test->addAction(action_TestEtoPCurSync);
        menu_Test->addAction(action_TestAll);
        menu_Test->addAction(action_DumpCharFlags);
        menu_Test->addAction(action_DumpBlockUserStates);
        menu_Search->addAction(action_Find);
        menu_Search->addAction(action_ForwardAgain);
        menu_Search->addAction(action_BackwardAgain);
        menu_Search->addAction(action_FindWord);
        menu_Search->addAction(action_ClearSearchHighlights);
        menu_Search->addAction(action_Replace);
        menu_Tool->addAction(action_KeisenMode);
        menu_Tool->addAction(action_ThinKeisen);
        menu_Tool->addAction(action_ThickKeisen);
        menu_Tool->addAction(action_OpenPrev);
        menu_Tool->addAction(action_OpenNext);
        fileToolBar->addAction(action_New);
        fileToolBar->addAction(action_NewTab);
        fileToolBar->addAction(action_Open);
        fileToolBar->addAction(action_Save);
        fileToolBar->addAction(action_SaveAs);
        fileToolBar->addAction(action_Close);
        fileToolBar->addSeparator();
        fileToolBar->addAction(action_NaviBack);
        fileToolBar->addAction(action_NaviForward);
        fileToolBar->addSeparator();
        fileToolBar->addAction(action_Settings);
        mainToolBar->addSeparator();
        mainToolBar->addAction(action_Undo);
        mainToolBar->addAction(action_Redo);
        mainToolBar->addSeparator();
        mainToolBar->addAction(action_List);
        mainToolBar->addAction(action_NumList);
        mainToolBar->addAction(action_Checkbox);
        mainToolBar->addAction(action_Heading);
        mainToolBar->addAction(action_Bold);
        mainToolBar->addAction(action_Italic);
        mainToolBar->addAction(action_Strikethrough);
        mainToolBar->addAction(action_Link);
        mainToolBar->addSeparator();
        mainToolBar->addAction(action_Indent);
        mainToolBar->addAction(action_UnIndent);
        mainToolBar->addAction(action_KeisenMode);
        mainToolBar->addAction(action_ThinKeisen);
        mainToolBar->addAction(action_ThickKeisen);
        mainToolBar->addAction(action_AlignLeft);
        mainToolBar->addAction(action_AlignCenter);
        mainToolBar->addAction(action_AlignRight);
        mainToolBar->addSeparator();

        retranslateUi(MainWindowClass);

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QCoreApplication::translate("MainWindowClass", "MainWindow", nullptr));
        action_New->setText(QCoreApplication::translate("MainWindowClass", "&NewWin", nullptr));
#if QT_CONFIG(shortcut)
        action_New->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Close->setText(QCoreApplication::translate("MainWindowClass", "&Close", nullptr));
#if QT_CONFIG(shortcut)
        action_Close->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+W", nullptr));
#endif // QT_CONFIG(shortcut)
        action_HTML->setText(QCoreApplication::translate("MainWindowClass", "&HTML", nullptr));
        action_Source->setText(QCoreApplication::translate("MainWindowClass", "&Source", nullptr));
#if QT_CONFIG(tooltip)
        action_Source->setToolTip(QCoreApplication::translate("MainWindowClass", "Source", nullptr));
#endif // QT_CONFIG(tooltip)
        action_Open->setText(QCoreApplication::translate("MainWindowClass", "&Open", nullptr));
#if QT_CONFIG(shortcut)
        action_Open->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Save->setText(QCoreApplication::translate("MainWindowClass", "&Save", nullptr));
#if QT_CONFIG(shortcut)
        action_Save->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Bold->setText(QCoreApplication::translate("MainWindowClass", "&Bold", nullptr));
        action_Italic->setText(QCoreApplication::translate("MainWindowClass", "&Italic", nullptr));
        action_Undo->setText(QCoreApplication::translate("MainWindowClass", "&Undo", nullptr));
#if QT_CONFIG(shortcut)
        action_Undo->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Z", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Redo->setText(QCoreApplication::translate("MainWindowClass", "&Redo", nullptr));
#if QT_CONFIG(shortcut)
        action_Redo->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Y", nullptr));
#endif // QT_CONFIG(shortcut)
        action_List->setText(QCoreApplication::translate("MainWindowClass", "&List", nullptr));
        action_NumList->setText(QCoreApplication::translate("MainWindowClass", "&NumList", nullptr));
        action_Indent->setText(QCoreApplication::translate("MainWindowClass", "Inden&t", nullptr));
#if QT_CONFIG(shortcut)
        action_Indent->setShortcut(QCoreApplication::translate("MainWindowClass", "Tab", nullptr));
#endif // QT_CONFIG(shortcut)
        action_UnIndent->setText(QCoreApplication::translate("MainWindowClass", "u&nIndent", nullptr));
#if QT_CONFIG(shortcut)
        action_UnIndent->setShortcut(QCoreApplication::translate("MainWindowClass", "Shift+Tab", nullptr));
#endif // QT_CONFIG(shortcut)
        action_OutlineBar->setText(QCoreApplication::translate("MainWindowClass", "&OutlineBar", nullptr));
        actiondummy->setText(QCoreApplication::translate("MainWindowClass", "dummy", nullptr));
        action_AboutViMarkdown->setText(QCoreApplication::translate("MainWindowClass", "&About viMarkdown", nullptr));
        action_Strikethrough->setText(QCoreApplication::translate("MainWindowClass", "&Strikethrough", nullptr));
#if QT_CONFIG(tooltip)
        action_Strikethrough->setToolTip(QCoreApplication::translate("MainWindowClass", "Strikethrough", nullptr));
#endif // QT_CONFIG(tooltip)
        action_Checkbox->setText(QCoreApplication::translate("MainWindowClass", "&Checkbox", nullptr));
        action_Cut->setText(QCoreApplication::translate("MainWindowClass", "Cu&t", nullptr));
#if QT_CONFIG(shortcut)
        action_Cut->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+X", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Copy->setText(QCoreApplication::translate("MainWindowClass", "&Copy", nullptr));
#if QT_CONFIG(shortcut)
        action_Copy->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+C", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Paste->setText(QCoreApplication::translate("MainWindowClass", "&Paste", nullptr));
#if QT_CONFIG(shortcut)
        action_Paste->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+V", nullptr));
#endif // QT_CONFIG(shortcut)
        action_FocusOutline->setText(QCoreApplication::translate("MainWindowClass", "Focus Outline (&Q)", nullptr));
#if QT_CONFIG(tooltip)
        action_FocusOutline->setToolTip(QCoreApplication::translate("MainWindowClass", "Focus to OutlineBar", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        action_FocusOutline->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Find->setText(QCoreApplication::translate("MainWindowClass", "&Find", nullptr));
#if QT_CONFIG(shortcut)
        action_Find->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+F", nullptr));
#endif // QT_CONFIG(shortcut)
        action_ForwardAgain->setText(QCoreApplication::translate("MainWindowClass", "ForwardAgain", nullptr));
#if QT_CONFIG(tooltip)
        action_ForwardAgain->setToolTip(QCoreApplication::translate("MainWindowClass", "Forward Search Again", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        action_ForwardAgain->setShortcut(QCoreApplication::translate("MainWindowClass", "F3", nullptr));
#endif // QT_CONFIG(shortcut)
        action_BackwardAgain->setText(QCoreApplication::translate("MainWindowClass", "BackwardAgain", nullptr));
#if QT_CONFIG(tooltip)
        action_BackwardAgain->setToolTip(QCoreApplication::translate("MainWindowClass", "Backward Search Again", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        action_BackwardAgain->setShortcut(QCoreApplication::translate("MainWindowClass", "Shift+F3", nullptr));
#endif // QT_CONFIG(shortcut)
        action_FindWord->setText(QCoreApplication::translate("MainWindowClass", "Find&Word", nullptr));
#if QT_CONFIG(shortcut)
        action_FindWord->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+F3", nullptr));
#endif // QT_CONFIG(shortcut)
        action_SaveAs->setText(QCoreApplication::translate("MainWindowClass", "Save&As...", nullptr));
#if QT_CONFIG(shortcut)
        action_SaveAs->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Shift+S", nullptr));
#endif // QT_CONFIG(shortcut)
        action_AddThisFavorite->setText(QCoreApplication::translate("MainWindowClass", "AddThis(&@)", nullptr));
#if QT_CONFIG(tooltip)
        action_AddThisFavorite->setToolTip(QCoreApplication::translate("MainWindowClass", "Add This to Favorite Files", nullptr));
#endif // QT_CONFIG(tooltip)
        action_Exit->setText(QCoreApplication::translate("MainWindowClass", "e&xit", nullptr));
        action_AsHTML->setText(QCoreApplication::translate("MainWindowClass", "As &HTML...", nullptr));
        action_AsPDF->setText(QCoreApplication::translate("MainWindowClass", "As &PDF", nullptr));
        action_SelectAll->setText(QCoreApplication::translate("MainWindowClass", "Select&All", nullptr));
#if QT_CONFIG(shortcut)
        action_SelectAll->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+A", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Print->setText(QCoreApplication::translate("MainWindowClass", "&Print", nullptr));
#if QT_CONFIG(shortcut)
        action_Print->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
        action_KeisenMode->setText(QCoreApplication::translate("MainWindowClass", "&KeisenMode", nullptr));
#if QT_CONFIG(tooltip)
        action_KeisenMode->setToolTip(QCoreApplication::translate("MainWindowClass", "Keisen Mode", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        action_KeisenMode->setShortcut(QCoreApplication::translate("MainWindowClass", "Shift+F5", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Heading->setText(QCoreApplication::translate("MainWindowClass", "&Heading", nullptr));
        action_AlignLeft->setText(QCoreApplication::translate("MainWindowClass", "Align&Left", nullptr));
#if QT_CONFIG(tooltip)
        action_AlignLeft->setToolTip(QCoreApplication::translate("MainWindowClass", "AlignLeft", nullptr));
#endif // QT_CONFIG(tooltip)
        action_AlignCenter->setText(QCoreApplication::translate("MainWindowClass", "Align&Center", nullptr));
#if QT_CONFIG(tooltip)
        action_AlignCenter->setToolTip(QCoreApplication::translate("MainWindowClass", "AlignCenter", nullptr));
#endif // QT_CONFIG(tooltip)
        action_AlignRight->setText(QCoreApplication::translate("MainWindowClass", "Align&Right", nullptr));
#if QT_CONFIG(tooltip)
        action_AlignRight->setToolTip(QCoreApplication::translate("MainWindowClass", "AlignRight", nullptr));
#endif // QT_CONFIG(tooltip)
        actionTickness->setText(QCoreApplication::translate("MainWindowClass", "Tickness", nullptr));
        action_OpenNext->setText(QCoreApplication::translate("MainWindowClass", "Open&Next", nullptr));
#if QT_CONFIG(shortcut)
        action_OpenNext->setShortcut(QCoreApplication::translate("MainWindowClass", "F7", nullptr));
#endif // QT_CONFIG(shortcut)
        action_ThinKeisen->setText(QCoreApplication::translate("MainWindowClass", "&ThinKeisen", nullptr));
#if QT_CONFIG(tooltip)
        action_ThinKeisen->setToolTip(QCoreApplication::translate("MainWindowClass", "ThinKeisen", nullptr));
#endif // QT_CONFIG(tooltip)
        action_ThickKeisen->setText(QCoreApplication::translate("MainWindowClass", "ThickKeisen(&F)", nullptr));
#if QT_CONFIG(tooltip)
        action_ThickKeisen->setToolTip(QCoreApplication::translate("MainWindowClass", "ThickKeisen", nullptr));
#endif // QT_CONFIG(tooltip)
        action_NewTab->setText(QCoreApplication::translate("MainWindowClass", "New&Tab", nullptr));
#if QT_CONFIG(shortcut)
        action_NewTab->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+T", nullptr));
#endif // QT_CONFIG(shortcut)
        action_TodayString_1->setText(QCoreApplication::translate("MainWindowClass", "&1 Today'sString", nullptr));
#if QT_CONFIG(tooltip)
        action_TodayString_1->setToolTip(QCoreApplication::translate("MainWindowClass", "Today's string", nullptr));
#endif // QT_CONFIG(tooltip)
        action_OpenPrev->setText(QCoreApplication::translate("MainWindowClass", "Open&Prev", nullptr));
#if QT_CONFIG(shortcut)
        action_OpenPrev->setShortcut(QCoreApplication::translate("MainWindowClass", "Shift+F7", nullptr));
#endif // QT_CONFIG(shortcut)
        action_NextTab->setText(QCoreApplication::translate("MainWindowClass", "&NextTab", nullptr));
#if QT_CONFIG(shortcut)
        action_NextTab->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Tab", nullptr));
#endif // QT_CONFIG(shortcut)
        action_PrevTab->setText(QCoreApplication::translate("MainWindowClass", "&PrevTab", nullptr));
#if QT_CONFIG(shortcut)
        action_PrevTab->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Backtab", nullptr));
#endif // QT_CONFIG(shortcut)
        action_TodayString_2->setText(QCoreApplication::translate("MainWindowClass", "&2 Today'sString", nullptr));
        action_TodayString_3->setText(QCoreApplication::translate("MainWindowClass", "&3 Today'sString", nullptr));
        action_Reload->setText(QCoreApplication::translate("MainWindowClass", "&Reload", nullptr));
        action_CSV_MarkdownTable->setText(QCoreApplication::translate("MainWindowClass", "CSV -> &MarkdownTable", nullptr));
        action_MarkdownTable_CSV->setText(QCoreApplication::translate("MainWindowClass", "MarkdownTable -> &CSV", nullptr));
        action_Settings->setText(QCoreApplication::translate("MainWindowClass", "&Settings...", nullptr));
#if QT_CONFIG(shortcut)
        action_Settings->setShortcut(QCoreApplication::translate("MainWindowClass", "F8", nullptr));
#endif // QT_CONFIG(shortcut)
        action_ToggleFocus->setText(QCoreApplication::translate("MainWindowClass", "&ToggleFocus", nullptr));
#if QT_CONFIG(shortcut)
        action_ToggleFocus->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+\\", nullptr));
#endif // QT_CONFIG(shortcut)
        action_SwitchToAltFile->setText(QCoreApplication::translate("MainWindowClass", "Switch to &Alt File", nullptr));
#if QT_CONFIG(shortcut)
        action_SwitchToAltFile->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+^", nullptr));
#endif // QT_CONFIG(shortcut)
        action_NaviBack->setText(QCoreApplication::translate("MainWindowClass", "&Back", nullptr));
#if QT_CONFIG(shortcut)
        action_NaviBack->setShortcut(QCoreApplication::translate("MainWindowClass", "Alt+Left", nullptr));
#endif // QT_CONFIG(shortcut)
        action_NaviForward->setText(QCoreApplication::translate("MainWindowClass", "&Forward", nullptr));
#if QT_CONFIG(shortcut)
        action_NaviForward->setShortcut(QCoreApplication::translate("MainWindowClass", "Alt+Right", nullptr));
#endif // QT_CONFIG(shortcut)
        action_ClearSearchHighlights->setText(QCoreApplication::translate("MainWindowClass", "&ClearSearchHighlights", nullptr));
#if QT_CONFIG(tooltip)
        action_ClearSearchHighlights->setToolTip(QCoreApplication::translate("MainWindowClass", "ClearSearchHighlights", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        action_ClearSearchHighlights->setShortcut(QCoreApplication::translate("MainWindowClass", "Alt+F3", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Replace->setText(QCoreApplication::translate("MainWindowClass", "&Replace...", nullptr));
#if QT_CONFIG(shortcut)
        action_Replace->setShortcut(QCoreApplication::translate("MainWindowClass", "F4", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Help->setText(QCoreApplication::translate("MainWindowClass", "&Help", nullptr));
#if QT_CONFIG(shortcut)
        action_Help->setShortcut(QCoreApplication::translate("MainWindowClass", "F1", nullptr));
#endif // QT_CONFIG(shortcut)
        action_TagJump->setText(QCoreApplication::translate("MainWindowClass", "&TagJump", nullptr));
#if QT_CONFIG(shortcut)
        action_TagJump->setShortcut(QCoreApplication::translate("MainWindowClass", "F9", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Link->setText(QCoreApplication::translate("MainWindowClass", "&Link", nullptr));
        action_TestCharFlags->setText(QCoreApplication::translate("MainWindowClass", "&CharFlags", nullptr));
#if QT_CONFIG(tooltip)
        action_TestCharFlags->setToolTip(QCoreApplication::translate("MainWindowClass", "Test CharFlags", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        action_TestCharFlags->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Alt+1", nullptr));
#endif // QT_CONFIG(shortcut)
        action_TestAll->setText(QCoreApplication::translate("MainWindowClass", "&All", nullptr));
#if QT_CONFIG(tooltip)
        action_TestAll->setToolTip(QCoreApplication::translate("MainWindowClass", "Test All", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        action_TestAll->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Alt+T", nullptr));
#endif // QT_CONFIG(shortcut)
        action_TestLineCrsp->setText(QCoreApplication::translate("MainWindowClass", "&LineCoresponding", nullptr));
#if QT_CONFIG(tooltip)
        action_TestLineCrsp->setToolTip(QCoreApplication::translate("MainWindowClass", "Test LineCoresponding", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        action_TestLineCrsp->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Alt+3", nullptr));
#endif // QT_CONFIG(shortcut)
        action_TestEtoPCurSync->setText(QCoreApplication::translate("MainWindowClass", "&EtoP CurSync", nullptr));
#if QT_CONFIG(shortcut)
        action_TestEtoPCurSync->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Alt+4", nullptr));
#endif // QT_CONFIG(shortcut)
        action_TestContextAt->setText(QCoreApplication::translate("MainWindowClass", "Conte&xtAt", nullptr));
#if QT_CONFIG(shortcut)
        action_TestContextAt->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Alt+2", nullptr));
#endif // QT_CONFIG(shortcut)
        action_DumpBlockUserStates->setText(QCoreApplication::translate("MainWindowClass", "Dump&BlockUserStates", nullptr));
#if QT_CONFIG(tooltip)
        action_DumpBlockUserStates->setToolTip(QCoreApplication::translate("MainWindowClass", "DumpBlockUserStates", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        action_DumpBlockUserStates->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Alt+B", nullptr));
#endif // QT_CONFIG(shortcut)
        action_DumpCharFlags->setText(QCoreApplication::translate("MainWindowClass", "DumpChar&Flags", nullptr));
#if QT_CONFIG(shortcut)
        action_DumpCharFlags->setShortcut(QCoreApplication::translate("MainWindowClass", "Ctrl+Alt+F", nullptr));
#endif // QT_CONFIG(shortcut)
        menu_File->setTitle(QCoreApplication::translate("MainWindowClass", "&File", nullptr));
        menu_RecentFiles->setTitle(QCoreApplication::translate("MainWindowClass", "RecentFiles(&1)", nullptr));
        menu_FavoriteFiles->setTitle(QCoreApplication::translate("MainWindowClass", "&FavoriteFile", nullptr));
        menu_Exoirt->setTitle(QCoreApplication::translate("MainWindowClass", "&Export", nullptr));
        menu_View->setTitle(QCoreApplication::translate("MainWindowClass", "&View", nullptr));
        menu_Edit->setTitle(QCoreApplication::translate("MainWindowClass", "&Edit", nullptr));
        menuinline->setTitle(QCoreApplication::translate("MainWindowClass", "In&line", nullptr));
        menublock->setTitle(QCoreApplication::translate("MainWindowClass", "&Block", nullptr));
        menu_format->setTitle(QCoreApplication::translate("MainWindowClass", "&format", nullptr));
        menu_Insert->setTitle(QCoreApplication::translate("MainWindowClass", "&Insert", nullptr));
        menu_Convert->setTitle(QCoreApplication::translate("MainWindowClass", "Convert(&X)", nullptr));
        menu_Help->setTitle(QCoreApplication::translate("MainWindowClass", "&Other", nullptr));
        menu_Test->setTitle(QCoreApplication::translate("MainWindowClass", "&Test", nullptr));
        menu_Search->setTitle(QCoreApplication::translate("MainWindowClass", "&Search", nullptr));
        menu_Tool->setTitle(QCoreApplication::translate("MainWindowClass", "&Tool", nullptr));
        outlineBar->setWindowTitle(QCoreApplication::translate("MainWindowClass", "OutlineBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
