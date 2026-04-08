#include <windows.h>
#include <vector>
#include <qsplitter.h>
#include <qplaintextedit>
#include <qtextedit>
#include <QFileDialog>
#include <qsplitter.h>
#include <QMessageBox>
#include <QScrollBar>
#include <QSettings>
#include <QTextBlock>
#include <QComboBox>
#include <QClipboard>
#include <qlabel.h>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileSystemWatcher>
#include <QPrinter>
#include <QPrintDialog>
#include <QDesktopServices>
#include <QLineEdit>
#include <QToolButton>
#include <QTextTable>
#include "ver.h"
#include "MainWindow.h"
#include "DocWidget.h"
#include "MarkdownEditor.h"
#include "MarkdownPreview.h"
#include "SettingsDialog.h"
#include "ReplaceDialog.h"

using namespace std;

const int N_RECENT_FILES = 10 + 26;
const int N_FAVORITE_FILES = 10 + 26;
const int MAX_DOC_LOC_HIST_SIZE = 100;

const QStringView KEY_RECENT_FILES(u"recentFilePaths");
const QStringView KEY_FAVORITE_FILES(u"favoriteFilePaths");
//const QStringView KEY_EDITOR_FONT_SIZE(u"editorFontSize");

Global g;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindowClass())
{
	load_settings();
	//static bool to_restore_win = true;
	ui->setupUi(this);
	ui->mainToolBar->setStyleSheet(
	    "QToolButton:checked {"
	    "   background-color: rgba(0, 120, 215, 60);" // 淡い青色（RGBAの最後が透明度 0-255）
	    "   border: 1px solid #0078d7;"              // 少し濃い目の枠線で引き締める
	    "   border-radius: 2px;"                     // 角を少し丸くするとモダンになります
	    "}"
	    "QToolButton:hover {"
	    "   background-color: rgba(0, 120, 215, 30);" // マウスホバー時も淡く反応させる
	    "}" );
	insertSearchComboBox();
	//updateHTMLModeCheck();		//	HTML or Source チェック状態に
	updateThinThickCheck();		//	細・太罫線モード
	ui->action_OutlineBar->setChecked(true);	//	暫定的
	setWindowTitle(QString("viMarkdown ") + VER_STR); 
	m_watcher = new QFileSystemWatcher(this);			//	外部アプリによる文書変更監視オブジェクト
	(m_lcLabel = new QLabel("0:0", this))->setMinimumWidth(50);
	ui->statusBar->addPermanentWidget(m_lcLabel);		//	ステータスバーに QLabel 設置
	//(m_encLabel = new QLabel("", this))->setMinimumWidth(100);
	//ui->statusBar->addPermanentWidget(m_encLabel);		//	ステータスバーに QLabel 設置
	setup_encodingCombo();
	setAcceptDrops(true);		//	ファイルドロップ可
	setup_tabMenu();
	setup_connections();
	QSettings settings;
	//m_editorFontSize = settings.value(KEY_EDITOR_FONT_SIZE).toInt();
	restore_win();
	onAction_NewTab();
}
MainWindow::~MainWindow()
{
	delete ui;
}
void MainWindow::load_settings() {
	QSettings settings;
	g.m_ignoreCase = settings.value(KEY_IGNORE_CASE, true).toBool();
	g.m_editorFontSize = settings.value(KEY_EDITOR_FONT_SIZE, 12).toInt();		//	デフォルト：12pt
	g.m_previewFontSize = settings.value(KEY_EDITOR_FONT_SIZE, 12).toInt();		//	デフォルト：12pt
	g.m_activeLnColor = settings.value(KEY_ACTIVA_LINE_COLOR, QColor("#ff0000")).value<QColor>();
	g.m_inactiveLnColor = settings.value(KEY_INACTIVA_LINE_COLOR, QColor("#800000")).value<QColor>();
	g.m_headingsColor = settings.value(KEY_HEADINGS_COLOR, QColor("#800000")).value<QColor>();		//	デフォルト：ダークレッド
	g.m_boldItalicColor = settings.value(KEY_BOLD_ITALIC_COLOR, QColor("#FF00FF")).value<QColor>();		//	紫
	g.m_boldColor = settings.value(KEY_BOLD_COLOR, QColor("#00FF00")).value<QColor>();
	g.m_italicColor = settings.value(KEY_ITALIC_COLOR, QColor("#0000FF")).value<QColor>();
	g.m_strikethroughColor = settings.value(KEY_STRIKETHROUGH_COLOR, QColor("#00ffff")).value<QColor>();
	g.m_matchColor = settings.value(KEY_MATCH_COLOR, QColor("#ffff00")).value<QColor>();
	g.m_CSVHeaderColor = settings.value(KEY_CSV_HEADER_COLOR, QColor("lightblue")).value<QColor>();
	g.m_CSVZebraColor1 = settings.value(KEY_CSV_ZEBRA_COLOR1, QColor("white")).value<QColor>();
	g.m_CSVZebraColor2 = settings.value(KEY_CSV_ZEBRA_COLOR2, QColor("lightyellow")).value<QColor>();
	g.m_quoteColor = settings.value(KEY_QUOTE_COLOR, QColor("#f0f8ff")).value<QColor>();	// 薄い青
	g.m_codeBlockColor = settings.value(KEY_CODE_BLOCK_COLOR, QColor("lightyellow")).value<QColor>();
	g.m_keisenBlockColor = settings.value(KEY_KEISEN_BLOCK_COLOR, QColor("#c0f0c0")).value<QColor>();		//	淡い緑
}
void MainWindow::save_settings() {
    QSettings settings;
    settings.setValue(KEY_IGNORE_CASE, g.m_ignoreCase);
    settings.setValue(KEY_EDITOR_FONT_SIZE, g.m_editorFontSize);
    settings.setValue(KEY_PREVIEW_FONT_SIZE, g.m_previewFontSize);
    settings.setValue(KEY_HEADINGS_COLOR, g.m_headingsColor);
    settings.setValue(KEY_BOLD_ITALIC_COLOR, g.m_boldItalicColor);
    settings.setValue(KEY_BOLD_COLOR, g.m_boldColor);
    settings.setValue(KEY_ITALIC_COLOR, g.m_italicColor);
    settings.setValue(KEY_STRIKETHROUGH_COLOR, g.m_strikethroughColor);
    settings.setValue(KEY_MATCH_COLOR, g.m_matchColor);
    settings.setValue(KEY_CSV_HEADER_COLOR, g.m_CSVHeaderColor);
    settings.setValue(KEY_CSV_ZEBRA_COLOR1, g.m_CSVZebraColor1);
    settings.setValue(KEY_CSV_ZEBRA_COLOR2, g.m_CSVZebraColor2);
    settings.setValue(KEY_QUOTE_COLOR, g.m_quoteColor);
    settings.setValue(KEY_CODE_BLOCK_COLOR, g.m_codeBlockColor);
    settings.setValue(KEY_KEISEN_BLOCK_COLOR, g.m_keisenBlockColor);
}
void MainWindow::insertSearchComboBox() {
	m_searchCB = new QComboBox;
	m_searchCB->setMaxCount(20);			//	履歴最大数
	m_searchCB->setEditable(true);					 // 入力可能にする
	m_searchCB->setMinimumWidth(160);				// 幅を少し広げる
	m_searchCB->setPlaceholderText(tr("search text")); // プレースホルダー表示
	m_searchCB->setFocusPolicy(Qt::ClickFocus); 	//	直接クリックのみでフォーカス取得
	m_searchCB->lineEdit()->setClearButtonEnabled(true);	//	クリアボタン（Ｘ）設置
	auto actions = m_searchCB->lineEdit()->findChildren<QAction*>();
	if (!actions.isEmpty()) {
	    QAction* clearAction = actions.at(0);
	    connect(clearAction, &QAction::triggered, this, [this]() {
			this->onAction_ClearSearchHighlights();
	    });
	}
#if 0
	connect(m_searchCB->lineEdit(), &QLineEdit::textChanged, this, [=](const QString &text){
	    //if (text.isEmpty())
	    if (m_searchCB->currentText().isEmpty())
	    {
			this->onAction_ClearSearchHighlights();
	    }
	});
#endif
	//m_searchCB->setInsertPolicy(QComboBox::InsertAtTop); // 検索履歴を一番上に追加する設定
	ui->mainToolBar->insertWidget(ui->action_List, m_searchCB);
	connect(m_searchCB, &QComboBox::activated, this, &MainWindow::onSearchCBActivated);
	QSettings settings;
	m_searchHist = settings.value("search/history").toStringList();
	m_replaceHist = settings.value("search/replace").toStringList();
    m_searchCB->clear();
    m_searchCB->addItems(m_searchHist);
    //	ついでに最近のファイル・ディレクトリをカレントディレクトリに設定
	QStringList recentFilePaths = settings.value(KEY_RECENT_FILES).toStringList();
	if( !recentFilePaths.isEmpty() ) {
		QFileInfo fi(recentFilePaths.front());
		QDir::setCurrent(fi.path());
	}
	//	実験的実装
	ui->mainToolBar->insertWidget(ui->action_Undo, ui->menuBar);
}
void MainWindow::setup_tabMenu() {
	QTabBar* tabBar = ui->tabWidget->tabBar();
	tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(tabBar, &QTabBar::customContextMenuRequested, this, [this, tabBar](const QPoint &pos) {
	    // 4. クリックされた位置にあるタブのインデックスを取得
	    int index = tabBar->tabAt(pos);
	    if (index == -1) return; // タブ以外の場所（隙間など）なら何もしない
	    ui->tabWidget->setCurrentIndex(index);

	    QMenu menu(this);
	    QAction *closeAction = menu.addAction(tr("Close"));
	    QAction *copyPathAction = menu.addAction(tr("Copy Fullpath"));
	    QAction *selectedAction = menu.exec(tabBar->mapToGlobal(pos));
	    if (selectedAction == closeAction) {
	        ui->tabWidget->removeTab(index); 
	    } else if (selectedAction == copyPathAction) {
	        DocWidget *docWidget = (DocWidget*)ui->tabWidget->widget(index);
	        if( docWidget != nullptr ) {
	        	QClipboard *clipboard = QGuiApplication::clipboard();
	        	if( !docWidget->m_fullPath.isEmpty() )
	        		clipboard->setText(docWidget->m_fullPath);
	        	else
	        		clipboard->setText(docWidget->m_title);
	        }
	    }
	});
	QToolButton *plusButton = new QToolButton(this);
	//plusButton->setText("+");
	plusButton->setIcon(QIcon(":/MainWindow/images/add_48.png"));
	plusButton->setCursor(Qt::PointingHandCursor);
	plusButton->setAutoRaise(true); // 背景を透明にし、ホバー時のみ枠を出す（ブラウザ風）
	ui->tabWidget->setCornerWidget(plusButton, Qt::TopLeftCorner);
	connect(plusButton, &QToolButton::clicked, this, [=](){
		    this->onAction_NewTab();
	});
}
void MainWindow::setup_encodingCombo() {
	m_encodingCombo = new QComboBox(this);
    m_encodingCombo->setFrame(false);
    m_encodingCombo->setFocusPolicy(Qt::NoFocus); // エディタからフォーカスを奪わない
    m_encodingCombo->setStyleSheet("QComboBox { background: transparent; padding: 0px 5px; }");
    m_encodingCombo->addItem("UTF-8",				QStringConverter::Utf8*2);
    m_encodingCombo->addItem("UTF-8 with BOM",		QStringConverter::Utf8*2 + 1); // 後でフラグで判定
    m_encodingCombo->addItem("UTF-16 LE",			QStringConverter::Utf16LE*2);
    m_encodingCombo->addItem("UTF-16 LE with BOM",	QStringConverter::Utf16LE*2+1);
    m_encodingCombo->addItem("UTF-16 BE",			QStringConverter::Utf16BE*2);
    m_encodingCombo->addItem("UTF-16 BE with BOM",	QStringConverter::Utf16BE*2+1);
    //m_encodingCombo->addItem("Shift-JIS",      QStringConverter::System); // または明示的に
    // ステータスバーの右端に追加
    statusBar()->addPermanentWidget(m_encodingCombo);
    connect(m_encodingCombo, &QComboBox::activated, this, &MainWindow::onEncodingChanged);
}
void MainWindow::onEncodingChanged(int ix) {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	int data = m_encodingCombo->itemData(ix).toInt();
    docWidget->m_encoding = (QStringConverter::Encoding)(data / 2);
    docWidget->m_withBOM = (data&1) != 0;
#if 1
	QString mess = "UTF-8";
	switch( docWidget->m_encoding ) {
	case QStringConverter::Utf16:
	case QStringConverter::Utf16BE:
	case QStringConverter::Utf16LE:
		mess = "UTF-16";
		break;
	}
	if( docWidget->m_withBOM )
		mess += " with BOM";
	qDebug() << "encoding = " << mess;
#endif
}
void MainWindow::onAction_TodayString_1() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->insertTodayString("yyyy-MM-dd");
}
void MainWindow::onAction_TodayString_2() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->insertTodayString("MM-dd");
}
void MainWindow::onAction_TodayString_3() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->insertTodayString("dd-MMM-yyyy");
}
void MainWindow::onAction_CSV_MarkdownTable() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->convert_CSV_MarkdownTable();
}
void MainWindow::onAction_MarkdownTable_CSV() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->convert_MarkdownTable_CSV();
}
void MainWindow::onAction_NaviBack() {
	if( m_docLocIX < 1 ) return;
	const auto &item = m_docLocHist[--m_docLocIX];
	m_processing = true;
	do_open(item.m_title, item.m_fullPath, item.m_title);
	if( item.m_position >= 0 ) {
		DocWidget *docWidget = getCurDocWidget();
		if( docWidget != nullptr ) {
			docWidget->setEditorCurPos(item.m_position);
		}
	}
	m_processing = false;
	printDocLocHist();
}
void MainWindow::onAction_NaviForward() {
	if( m_docLocIX + 1 >= m_docLocHist.size() ) return;
	const auto &item = m_docLocHist[++m_docLocIX];
	m_processing = true;
	do_open(item.m_title, item.m_fullPath, item.m_title);
	if( item.m_position >= 0 ) {
		DocWidget *docWidget = getCurDocWidget();
		if( docWidget != nullptr ) {
			docWidget->setEditorCurPos(item.m_position);
		}
	}
	m_processing = false;
	printDocLocHist();
}
void MainWindow::onAction_Replace() {
	ReplaceDialog dlg(m_searchHist, m_replaceHist, this);
	connect(&dlg, &ReplaceDialog::do_search, this, &MainWindow::do_search);
	connect(&dlg, &ReplaceDialog::do_replace_next, this, &MainWindow::do_replace_next);
	connect(&dlg, &ReplaceDialog::do_replace_all, this, &MainWindow::do_replace_all);
	connect(&dlg, &ReplaceDialog::do_undo, this, &MainWindow::do_undo_replaceDlg);
	connect(&dlg, &ReplaceDialog::do_redo, this, &MainWindow::do_redo_replaceDlg);
	if (dlg.exec() == QDialog::Accepted) {
	}

}
void MainWindow::onAction_Find() {
	m_searchCB->setFocus();
}
void MainWindow::do_replace_next(const QString srcText, const QString dstText) {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	MarkdownEditor *mdEditor = docWidget->m_editor;
	QTextCursor cursor = mdEditor->textCursor();
	if( cursor.hasSelection() ) {
		cursor.insertText(dstText);
		mdEditor->setTextCursor(cursor);
		m_replaceHist.push_front(dstText);
		m_replaceHist.removeDuplicates();	//	重複削除
	}
	do_search(srcText, false);
}
void MainWindow::do_replace_all(const QString srcText, const QString dstText) {
	if( srcText == dstText ) return;
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	MarkdownEditor *mdEditor = docWidget->m_editor;
	QTextDocument *doc = mdEditor->document();
	QTextCursor cursor(doc), cursor2;
	cursor.beginEditBlock();
	bool replaced = false;
	for (;;) {
	    if( (cursor2 = doc->find(srcText, cursor)) .isNull() )
	    	break;
	    cursor = cursor2;
	    cursor.insertText(dstText);
	    replaced = true;
	}
	cursor.endEditBlock();
	if( replaced ) {
		m_replaceHist.push_front(dstText);
		m_replaceHist.removeDuplicates();	//	重複削除
		mdEditor->setTextCursor(cursor);
	}
}
void MainWindow::do_search(const QString srcText, bool backward) {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	MarkdownEditor *mdEditor = docWidget->m_editor;
	if( !srcText.isEmpty() ) {
		QTextDocument::FindFlags flags;
		if( backward )
			flags |= QTextDocument::FindBackward;
		if( !g.m_ignoreCase )
			flags |= QTextDocument::FindCaseSensitively;
		bool found = mdEditor->find(srcText, flags);
		if (!found) {
	        if( !backward )
				mdEditor->moveCursor(QTextCursor::Start);
	        else
				mdEditor->moveCursor(QTextCursor::End);
	        found = mdEditor->find(srcText, flags);
	    }
		if (!found) {
			qDebug() << "not found";
		}
	}
	m_srcText = srcText;
	mdEditor->highlightSearchText(srcText);
	mdEditor->setFocus();
	m_searchHist.push_front(srcText);
	m_searchHist.removeDuplicates();	//	重複削除
	m_searchCB->clear();
    m_searchCB->addItems(m_searchHist);
    m_searchCB->setCurrentText(srcText);
}
void MainWindow::do_find(bool backward) {
	const QString srcText = m_searchCB->currentText();
	if( srcText.isEmpty() ) return;
	do_search(srcText, backward);
}
void MainWindow::onSearchCBActivated() {
	do_find();
}
void MainWindow::do_undo_replaceDlg() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->undo();
}
void MainWindow::do_redo_replaceDlg() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->redo();
}
void MainWindow::onAction_ForwardAgain() {
	do_find();
}
void MainWindow::onAction_BackwardAgain() {
	do_find(true);
}
void MainWindow::onAction_FindWord() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	MarkdownEditor *mdEditor = docWidget->m_editor;
	QTextCursor cursor = mdEditor->textCursor();
	if( !cursor.hasSelection() ) {
		//cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
		//cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
		mdEditor->moveToStartOfWord(cursor, false);
		mdEditor->moveToEndOfWord(cursor, /*selection:*/true);		
		mdEditor->setTextCursor(cursor);
	}
	m_searchCB->setCurrentText(cursor.selectedText());
	do_find();
}
void MainWindow::onAction_ClearSearchHighlights() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	MarkdownEditor *mdEditor = docWidget->m_editor;
	m_srcText.clear();
	mdEditor->highlightSearchText(m_srcText);
}
void MainWindow::setup_connections() {
	connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onCurrentTabChanged);
	connect(ui->menu_RecentFiles, &QMenu::aboutToShow, this, &MainWindow::onAboutToShow_RecentFiles);
	connect(ui->menu_FavoriteFiles, &QMenu::aboutToShow, this, &MainWindow::onAboutToShow_FavoriteFiles);
	connect(ui->menu_Insert, &QMenu::aboutToShow, this, &MainWindow::onAboutToShow_Insert);
	connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::onFileChanged);
	connect(ui->action_Exit, &QAction::triggered, this, &MainWindow::onAction_Exit);
	connect(ui->action_Settings, &QAction::triggered, this, &MainWindow::onAction_Settings);
	connect(ui->action_Help, &QAction::triggered, this, &MainWindow::onAction_Help);
	//connect(ui->action_Test, &QAction::triggered, this, &MainWindow::onAction_Test);
	connect(ui->action_TestCharFlags, &QAction::triggered, this, &MainWindow::onAction_TestCharFlags);
	connect(ui->action_TestContextAt, &QAction::triggered, this, &MainWindow::onAction_TestContextAt);
	connect(ui->action_TestLineCrsp, &QAction::triggered, this, &MainWindow::onAction_TestLineCrsp);
	connect(ui->action_TestEtoPCurSync, &QAction::triggered, this, &MainWindow::onAction_TestEtoPCurSync);
	connect(ui->action_TestAll, &QAction::triggered, this, &MainWindow::onAction_TestAll);
	connect(ui->action_DumpCharFlags, &QAction::triggered, this, &MainWindow::onAction_DumpCharFlags);
	connect(ui->action_DumpBlockUserStates, &QAction::triggered, this, &MainWindow::onAction_DumpBlockUserStates);
	connect(ui->action_AddThisFavorite, &QAction::triggered, this, &MainWindow::onAction_AddThisFavorite);
	connect(ui->action_New, &QAction::triggered, this, &MainWindow::onAction_New);
	connect(ui->action_NewTab, &QAction::triggered, this, &MainWindow::onAction_NewTab);
	connect(ui->action_Open, &QAction::triggered, this, &MainWindow::onAction_Open);
	connect(ui->action_Save, &QAction::triggered, this, &MainWindow::onAction_Save);
	connect(ui->action_SaveAs, &QAction::triggered, this, &MainWindow::onAction_SaveAs);
	connect(ui->action_AsPDF, &QAction::triggered, this, &MainWindow::onAction_ExportAsPDF);
	connect(ui->action_Close, &QAction::triggered, this, &MainWindow::onAction_Close);
	connect(ui->action_Print, &QAction::triggered, this, &MainWindow::onAction_Print);
	connect(ui->action_List, &QAction::triggered, this, &MainWindow::onAction_List);
	connect(ui->action_NumList, &QAction::triggered, this, &MainWindow::onAction_NumList);
	connect(ui->action_Checkbox, &QAction::triggered, this, &MainWindow::onAction_Checkbox);
	connect(ui->action_Indent, &QAction::triggered, this, &MainWindow::onAction_Indent);
	connect(ui->action_UnIndent, &QAction::triggered, this, &MainWindow::onAction_UnIndent);
	connect(ui->action_Undo, &QAction::triggered, this, &MainWindow::onAction_Undo);
	connect(ui->action_Redo, &QAction::triggered, this, &MainWindow::onAction_Redo);
	connect(ui->action_SelectAll, &QAction::triggered, this, &MainWindow::onAction_SelectAll);
	connect(ui->action_Paste, &QAction::triggered, this, &MainWindow::onAction_Paste);
	connect(ui->action_Heading, &QAction::triggered, this, &MainWindow::onAction_Heading);
	connect(ui->action_Bold, &QAction::triggered, this, &MainWindow::onAction_Bold);
	connect(ui->action_Italic, &QAction::triggered, this, &MainWindow::onAction_Italic);
	connect(ui->action_Strikethrough, &QAction::triggered, this, &MainWindow::onAction_Strikethrough);
	connect(ui->action_Link, &QAction::triggered, this, &MainWindow::onAction_Link);
	connect(ui->action_AlignLeft, &QAction::triggered, this, &MainWindow::onAction_AlignLeft);
	connect(ui->action_AlignCenter, &QAction::triggered, this, &MainWindow::onAction_AlignCenter);
	connect(ui->action_AlignRight, &QAction::triggered, this, &MainWindow::onAction_AlignRight);
	connect(ui->action_TodayString_1, &QAction::triggered, this, &MainWindow::onAction_TodayString_1);
	connect(ui->action_TodayString_2, &QAction::triggered, this, &MainWindow::onAction_TodayString_2);
	connect(ui->action_TodayString_3, &QAction::triggered, this, &MainWindow::onAction_TodayString_3);
	connect(ui->action_CSV_MarkdownTable, &QAction::triggered, this, &MainWindow::onAction_CSV_MarkdownTable);
	connect(ui->action_MarkdownTable_CSV, &QAction::triggered, this, &MainWindow::onAction_MarkdownTable_CSV);
	connect(ui->action_NaviForward, &QAction::triggered, this, &MainWindow::onAction_NaviForward);
	connect(ui->action_NaviBack, &QAction::triggered, this, &MainWindow::onAction_NaviBack);
	connect(ui->action_Find, &QAction::triggered, this, &MainWindow::onAction_Find);
	connect(ui->action_ForwardAgain, &QAction::triggered, this, &MainWindow::onAction_ForwardAgain);
	connect(ui->action_BackwardAgain, &QAction::triggered, this, &MainWindow::onAction_BackwardAgain);
	connect(ui->action_FindWord, &QAction::triggered, this, &MainWindow::onAction_FindWord);
	connect(ui->action_ClearSearchHighlights, &QAction::triggered, this, &MainWindow::onAction_ClearSearchHighlights);
	connect(ui->action_Replace, &QAction::triggered, this, &MainWindow::onAction_Replace);
	connect(ui->action_KeisenMode, &QAction::toggled, this, &MainWindow::onAction_KeisenMode);
	connect(ui->action_ThinKeisen, &QAction::toggled, this, &MainWindow::onAction_ThinKeisen);
	connect(ui->action_ThickKeisen, &QAction::toggled, this, &MainWindow::onAction_ThickKeisen);
	connect(ui->action_OpenPrev, &QAction::triggered, this, &MainWindow::onAction_OpenPrev);
	connect(ui->action_OpenNext, &QAction::triggered, this, &MainWindow::onAction_OpenNext);
	connect(ui->action_HTML, &QAction::toggled, this, &MainWindow::onAction_HTML);
	connect(ui->action_Source, &QAction::toggled, this, &MainWindow::onAction_Source);
	connect(ui->action_OutlineBar, &QAction::toggled, this, &MainWindow::onAction_OutlineBar);
	connect(ui->action_FocusOutline, &QAction::triggered, this, &MainWindow::onAction_FocusOutline);
	connect(ui->action_NextTab, &QAction::triggered, this, &MainWindow::onAction_NextTab);
	connect(ui->action_PrevTab, &QAction::triggered, this, &MainWindow::onAction_PrevTab);
	connect(ui->action_ToggleFocus, &QAction::triggered, this, &MainWindow::onAction_ToggleFocus);
	connect(ui->action_SwitchToAltFile, &QAction::triggered, this, &MainWindow::onAction_SwitchToAltFile);
	connect(ui->action_TagJump, &QAction::triggered, this, &MainWindow::onAction_TagJump);
	connect(ui->outlineBar, &QDockWidget::visibilityChanged, this, &MainWindow::onOutlineBarVisibilityChanged);
	connect(ui->treeWidget, &QTreeWidget::currentItemChanged, this, &MainWindow::onTreeCurrentItemChanged);
	//connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::onTreeItemDoubleClicked);
	connect(ui->treeWidget, &QTreeWidget::itemActivated, this, &MainWindow::onTreeItemActivated);				//	ダブルクリック or Enter 押下
	connect(ui->action_AboutViMarkdown, &QAction::triggered, this, &MainWindow::onAction_About);
}
void MainWindow::restore_win() {
	QSettings settings;
	settings.beginGroup("MainWindow");
	
	// 保存されていた値があれば復元、なければ何もしない
	const QByteArray geometry = settings.value("geometry").toByteArray();
	if (!geometry.isEmpty()) {
		restoreGeometry(geometry);
	}
	const QByteArray windowState = settings.value("windowState").toByteArray();
	if (!windowState.isEmpty()) {
		restoreState(windowState);
	}
	settings.endGroup();
}
void MainWindow::closeEvent(QCloseEvent *event) {
	for(int ix = 0; ix < ui->tabWidget->count(); ++ix) {
		DocWidget *docWidget = (DocWidget*)ui->tabWidget->widget(ix);
		if( docWidget->isModified() ) {
			ui->tabWidget->setCurrentIndex(ix);
			QString mess = QString("The document '%1' has been modified.\nDo you want to save your changes ?").arg(docWidget->m_title);
			QMessageBox::StandardButton reply = QMessageBox::question(this,
								  "Confirm save",
								  mess,
								  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel); // ボタンの種類

			if (reply == QMessageBox::Yes) {
				onAction_Save();
			} else if (reply == QMessageBox::Cancel) {
				event->ignore();
				return;
			}
		}
	}
	QSettings settings;
	//QStringList history;
    //for (int i = 0; i < m_searchCB->count(); ++i) {
    //    history << m_searchCB->itemText(i);
    //}
    settings.setValue("search/history", m_searchHist);
	settings.beginGroup("MainWindow");
	settings.setValue("geometry", saveGeometry()); // 位置・サイズ
	settings.setValue("windowState", saveState()); // ツールバー・ドックの状態
	settings.endGroup();
	//
	event->accept();
}
void MainWindow::onCurrentTabChanged(int ix) {
	if( m_processing ) return;		//	再入禁止
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	qDebug() << "MainWindow::onCurrentTabChanged()";
	m_altFullPath = m_curFullPath;
	m_altTitle = m_curTitle;
	m_curFullPath = docWidget->m_fullPath;
	m_curTitle = docWidget->m_title;
	//int curBlockNum = docWidget->m_editor->textCursor().blockNumber();
	int curPos = docWidget->m_editor->textCursor().position();
	appendToDocLoc(m_curTitle, m_curFullPath, curPos);		//	現在位置をリストに追加
	//QString mess = QString("encoding = %1").arg((int)docWidget->m_encoding);
	//QString mess = docWidget->m_encoding.nema();
	if( !docWidget->m_fullPath.isEmpty() ) {
		QFileInfo fi(docWidget->m_fullPath);
		QString dirPath = fi.absolutePath();
	    QDir::setCurrent(dirPath);
	}
#if 1
	int data = docWidget->m_encoding*2 + (docWidget->m_withBOM ? 1 : 0);
	ix = m_encodingCombo->findData(data);
	if( ix >= 0 ) m_encodingCombo->setCurrentIndex(ix);
#else
	QString mess = "UTF-8";
	switch( docWidget->m_encoding ) {
	case QStringConverter::Utf16:
	case QStringConverter::Utf16BE:
	case QStringConverter::Utf16LE:
		mess = "UTF-16";
		break;
	}
	if( docWidget->m_withBOM )
		mess += " with BOM";
	//statusBar()->showMessage(mess);
	m_encLabel->setText(mess);
#endif
}
void MainWindow::removeHistoryEntries(const QString& title, const QString& fullPath, int pos) {
	for(int i = m_docLocHist.size(); --i >= 0; ) {		//	重複削除
		if( (!fullPath.isEmpty() && m_docLocHist[i].m_fullPath == fullPath || m_docLocHist[i].m_fullPath.isEmpty()) &&
				m_docLocHist[i].m_title == title && m_docLocHist[i].m_position == pos )
		{
			m_docLocHist.removeAt(i);
		}
	}
}
void MainWindow::appendToDocLoc(const QString& title, const QString& fullPath, int pos) {
	while( m_docLocHist.size()-1 > m_docLocIX)
		m_docLocHist.pop_back();
	removeHistoryEntries(title, fullPath, pos);
	m_docLocHist.push_back(DocLocation(title, fullPath, pos));
	while( m_docLocHist.size() > MAX_DOC_LOC_HIST_SIZE )
		m_docLocHist.pop_front();
	m_docLocIX = m_docLocHist.size() - 1;		//	最後要素index
	printDocLocHist();
}
void MainWindow::printDocLocHist() const {
#ifdef	_DEBUG
	for(int i = 0; i < m_docLocHist.size(); ++i)
		qDebug() << i << ": " << m_docLocHist[i].m_title << ", " << m_docLocHist[i].m_fullPath << ", " << m_docLocHist[i].m_position;
	qDebug() << "m_docLocIX = " << m_docLocIX;
#endif
}
void MainWindow::onFileChanged(const QString& fullPath) {
	statusBar()->showMessage("file changed: " + fullPath, 3000);
	int tix = tabIndexOf("", fullPath);
	if( tix < 0 ) return;
	DocWidget *docWidget = (DocWidget*)ui->tabWidget->widget(tix);
	if( docWidget->isModified() ) {
		QMessageBox msgBox(this);
		msgBox.setWindowTitle("外部変更の検知");
		msgBox.setText("未保存文書のファイルが外部で変更されました。");
		msgBox.setInformativeText("最新の状態を読み込みますか？");

		// ボタンの設定 (Yes, No, Ignore を配置)
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);

		int ret = msgBox.exec();
		if( ret == QMessageBox::No ) return;
	}
	do_load(fullPath);
}
#if 0
void MainWindow::updateHTMLModeCheck() {
	ui->action_HTML->setChecked(m_htmlMode);
	ui->action_Source->setChecked(!m_htmlMode);
}
#endif
DocWidget *MainWindow::newTabWidget(const QString& title, const QString& fullPath, bool readOnly) {
	//auto containerWidget = new QWidget;
	auto *docWidget = new DocWidget(title, fullPath);
	//docWidget->setStyleSheet("font-size: 12pt; line-height: 200%;");
	QSplitter *splitter = new QSplitter(Qt::Horizontal, docWidget);
	MarkdownEditor *mdEditor = docWidget->m_editor = new MarkdownEditor(this, docWidget, splitter);
	mdEditor->setReadOnly(readOnly);
	//mdEditor->setLineWrapMode(QPlainTextEdit::NoWrap);		//	折り返しモード OFF
	//QFont font("MS Gothic");
	//QFont font("Consolas");
	//font.setPointSize(12);		// 12ptに設定
	//font.setFixedPitch(true);	// 明示的に固定幅として扱う設定
	//mdEditor->setFont(font);
	//mdEditor->setStyleSheet("font-size: 12pt;");
	QFont font = mdEditor->font();
	font.setPointSize(g.m_editorFontSize);
	mdEditor->setFont(font);
	mdEditor->setUndoRedoEnabled(true);		//	Undo/Redo 有効
	//QFontMetrics fm(mdEditor->font());
	//int lnAreaWidth = fm.horizontalAdvance('9') * 8;
	//mdEditor->setViewportMargins(lnAreaWidth, 0, 0, 0);
	connect(mdEditor, &MarkdownEditor::cursorPositionChanged, this, &MainWindow::onMdEditCurPosChanged);
	connect(mdEditor, &MarkdownEditor::tab_pressed, this, &MainWindow::onMdEditTabPressed);
	connect(mdEditor, &MarkdownEditor::esc_pressed, this, &MainWindow::onMdEditEscPressed);
	connect(mdEditor, &MarkdownEditor::link_clicked, this, &MainWindow::do_open);
	//connect(mdEditor, &MarkdownEditor::title_clicked, this, &MainWindow::do_open);
	//connect(mdEditor, &MarkdownEditor::cursorPositionChanged, this, &MainWindow::onEditorCurPosChanged);
	//connect(mdEditor, &MarkdownEditor::cursorPositionChanged, this, &MainWindow::syncPreviewCursorWithEditor);
	connect(mdEditor, &MarkdownEditor::changeFontSize, this, &MainWindow::onChangeEditorFontSize);
	connect(mdEditor, &MarkdownEditor::posContextChanged, this, &MainWindow::onSrcPosContextChanged);
	connect(mdEditor->document(), &QTextDocument::modificationChanged, this, &MainWindow::onModificationChanged);
	connect(mdEditor, &MarkdownEditor::cursorPositionChanged, this, &MainWindow::onSrcCursorPosChanged);
	//QTextEdit *mdEditor = new QTextEdit(splitter);
	mdEditor->setPlaceholderText("\nここにMarkdownを入力\n"
									"\nマークダウン書式：\n# タイトル\n## 大見出し\n### 中見出し\n"
									"本文...\n"		//	for GFM
									//"本文...（空行：段落区切り）\n行末に半角スペース2つ：強制改行\n"	//	for コモンマークダウン
									"**ボールド** *イタリック* ~~打ち消し線~~\n"
									"1. 連番\n- リスト\n- [ ] チェックボックス\n"
									"リンク: [テキスト](url) or [テキスト](filename.md#見出し)\n"
									"画像: ![テキスト](image.png) or 画像ペースト\n"
									"表組み:\n"
									"|見出し|見出し|\n|-----:|------|\n|   123|abc   |\n\n"
									"```CSV\n見出し, 見出し\n123, abc\n```\n"
									"罫線モード: Shift + F5\n　罫線描画: Ctrl + 上下左右キー\n"
									"　罫線消去: Ctrl + Shift + 上下左右キー\n"
								);
	MarkdownPreview *markdownPreview = docWidget->m_preview = new MarkdownPreview(this, docWidget, splitter, readOnly);
	//markdownPreview->setReadOnly(readOnly);
	markdownPreview->setMouseTracking(true); // マウスの動きを常に追跡
	markdownPreview->setPlaceholderText("プレビュー画面　ここで簡単な編集もできるよ\n");
	//markdownPreview->setStyleSheet("font-size: 12pt;");
	//##markdownPreview->setStyleSheet("font-size: 12pt; line-height: 2.0;");
	connect(markdownPreview, &MarkdownPreview::checkboxLineClicked, this, &MainWindow::onMarkdownPreviewLineClicked);
	connect(markdownPreview, &MarkdownPreview::anchorClicked, this, &MainWindow::do_open);
	connect(markdownPreview, &MarkdownPreview::textInserted, this, &MainWindow::onTextInsertedAtPreview);
	connect(markdownPreview, &MarkdownPreview::textRemoved, this, &MainWindow::onTextRemovedAtPreview);
	connect(markdownPreview, &MarkdownPreview::Enter_pressed, this, &MainWindow::onEnter_pressed);
	connect(markdownPreview, &MarkdownPreview::Tab_pressed, this, &MainWindow::onTab_pressed);
	connect(markdownPreview, &MarkdownPreview::BS_pressed, this, &MainWindow::onBS_pressed);
	connect(markdownPreview, &MarkdownPreview::Del_pressed, this, &MainWindow::onDel_pressed);
	connect(markdownPreview, &MarkdownPreview::undo_triggered, this, &MainWindow::onUndoTriggered);
	connect(markdownPreview, &MarkdownPreview::redo_triggered, this, &MainWindow::onRedoTriggered);
	connect(markdownPreview, &MarkdownPreview::changeFontSize, this, &MainWindow::onChangePreviewFontSize);
	//connect(markdownPreview, &MarkdownPreview::cursorPositionChanged, this, &MainWindow::onPreviewCurPosChanged);
	connect(markdownPreview, &MarkdownPreview::posContextChanged, this, &MainWindow::onPrvPosContextChanged);
	splitter->addWidget(mdEditor);
	splitter->addWidget(markdownPreview);
	splitter->setSizes(QList<int>() << 500 << 500);
	QVBoxLayout *layout = new QVBoxLayout(docWidget);
	layout->addWidget(splitter);
	layout->setContentsMargins(0, 0, 0, 0); // 余白をなくして端まで広げる

	connect(mdEditor, &MarkdownEditor::textChanged, this, &MainWindow::onMDTextChanged);
	//connect(mdEditor, &MarkdownPreview::textChanged, this, &MainWindow::onMDTextChanged);

	docWidget->setModified(false);
	return docWidget;
}
#if 0
void MainWindow::syncPreviewCursorWithEditor() {		//	MarkdownEditor でカーソルが移動した
	if( m_processing || isCursorCyncing() ) return;		//	再入禁止
	m_processing = true;
	setCursorCyncing();
	qDebug() << "MainWindow::syncPreviewCursorWithEditor()";
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	m_processing = true;
	docWidget->m_editor->setProcessing(true);		//	再入禁止
	QTextCursor cursor = docWidget->m_editor->textCursor();
	QTextBlock block = cursor.block();
	int i = 0;
	QString text = block.text();
	static QRegularExpression re("^( *- |- \\[[ xX]\\] )");
	while( text.startsWith("```") ) {
		block = block.next();
		if( !block.isValid() ) {
			setCursorCyncing(false);
			m_processing = false;
			docWidget->m_editor->setProcessing(false);
			return;
		}
		text = block.text();
	}
	if( text.startsWith(u'#') ) {
		while( i < text.size() && text[i] == u'#' ) ++i;
		while( i < text.size() && text[i] == u' ' ) ++i;
	} else {
		QRegularExpressionMatch match = re.match(text);		//	"- ", "- [ ] " の場合
		if( match.hasMatch() ) {
			//i = match.capturedLength();
			while( i < text.size() && text[i] == u' ' ) ++i;
			i += 2;		//	skip "- ";
			if( i < text.size() && text[i] == u'[' )
				i += 4;
		}
	}
	if( i != 0 ) {
		//cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, i);
		text = text.mid(i);
	}
	int ix = cursor.columnNumber() - i;
	QString pat = text.mid(cursor.columnNumber() - i, 3);
	int curBlockNum = cursor.blockNumber();
	bool tail = false;
	if( pat.isEmpty() ) {
		tail = true;
		pat = text.right(3);
	}
	int nth = 1;
	while( block.isValid() && !block.text().startsWith("#") )	//	見出し行までブロック移動
		block = block.previous();
	if( !block.isValid() ) block = docWidget->m_editor->document()->begin();		//	最初のブロック
	int blockNum = block.blockNumber();
	if( !tail ) {
		QTextCursor cur = cursor;
		cur.setPosition(block.position());
		for(;;) {
			cur = docWidget->m_editor->document()->find(pat, cur);
			if( cur.isNull() || cur.position() >= cursor.position() ) break;
			++nth;
		}
	} else {
		while( block.blockNumber() < curBlockNum ) {
			if( block.text().endsWith(pat) )
				++nth;
			block = block.next();
			if( !block.isValid() ) break;
		}
	}
	docWidget->m_preview->setCursorAtNthPat(blockNum, pat, nth, tail);
	docWidget->m_editor->setProcessing(false);
	m_processing = false;
	setCursorCyncing(false);
}
#endif
//	エディタ → プレビュー カーソル位置同期
void MainWindow::onSrcPosContextChanged(const PosContext& context, const PosContext &acontext) {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_preview->setCursorByContext(context, acontext);
}
//	プレビュー → エディタ カーソル位置同期
void MainWindow::onPrvPosContextChanged(const PosContext& context, const PosContext &acontext) {
	//qDebug() << "onPrvPosContextChanged()";
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->setCursorByContext(context, acontext);
}
//	ビューワ → エディタ カーソル位置同期
void MainWindow::onPreviewCurPosChanged() {		//	MarkdownPreview でカーソルが移動した
	if( m_processing ) return;		//	再入禁止
	qDebug() << "MainWindow::onPreviewCurPosChanged()";
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	if( docWidget->m_preview->isProcessing() ) return;
	m_processing = true;
	QTextCursor cursor = docWidget->m_preview->textCursor();		//	ビューワカーソル
	QTextBlock b0 = cursor.block();
	while( b0.userState() != US_HEADING ) {		//	見出し行まで移動
		b0 = b0.previous();
		if( !b0.isValid() ) {
			b0 = cursor.document()->firstBlock();
			break;
		}
	}
	//auto headingList = docWidget->m_preview->
	QTextBlock block = cursor.block();
	QString pat = block.text().mid(cursor.position() - block.position(), 3);
	int curBlockNum = cursor.blockNumber();
	bool tail = false;
	if( pat.isEmpty() ) {
		tail = true;
		pat = block.text().right(3);
	}
	int nth = 1;
	while( block.isValid() && block.userState() != US_HEADING ) block = block.previous();
	if( !block.isValid() ) block = docWidget->m_editor->document()->begin();		//	最初のブロック
	if( !tail ) {
		QTextCursor cur = cursor;
		cur.setPosition(block.position());
		for(;;) {
			cur = docWidget->m_preview->document()->find(pat, cur);
			if( cur.isNull() || cur.position() >= cursor.position() ) break;
			++nth;
		}
	} else {
		while( block.blockNumber() < curBlockNum ) {
			if( block.text().endsWith(pat) )
				++nth;
			block = block.next();
			if( !block.isValid() ) break;
		}
	}
	docWidget->m_editor->setCursorAtNthPat(cursor.blockNumber(), pat, nth, tail);
	m_processing = false;
}
void MainWindow::onChangeEditorFontSize(int delta) {
	//int sz = g.m_editorFontSize
	if( delta > 0 ) {
		g.m_editorFontSize = qMin(64, g.m_editorFontSize + 1);
	} else {
		g.m_editorFontSize = qMax(3, g.m_editorFontSize - 1);
	}
	statusBar()->showMessage(QString("editor font size = %1").arg(g.m_editorFontSize), 5000);
	QSettings settings;
	settings.setValue(KEY_EDITOR_FONT_SIZE, g.m_editorFontSize);
	updateEditorFontSize(g.m_editorFontSize);
}
void MainWindow::onChangePreviewFontSize(int delta) {
}
void MainWindow::updateEditorFontSize(int sz) {
	g.m_editorFontSize = sz;
	for(int i = 0; i < ui->tabWidget->count(); ++i) {
		DocWidget *docWidget = (DocWidget*)ui->tabWidget->widget(i);
		//docWidget->m_editor->setStyleSheet(QString("font-size: %1pt;  line-height: 120%;").arg(g.m_editorFontSize));
		QFont font = docWidget->m_editor->font();
		font.setPointSize(g.m_editorFontSize);
		font.setFixedPitch(true);	// 明示的に固定幅として扱う設定
		docWidget->m_editor->setFont(font);
		docWidget->m_editor->updateViewportMargines();
		//docWidget->m_editor->setBoldColor(g.m_boldColor);
		docWidget->m_editor->updateInlineColors();
		docWidget->m_editor->rehighlight();				//	再ハイライト
		docWidget->m_editor->highlightSearchText(m_srcText);				//	再ハイライト
		docWidget->m_editor->viewport()->update();		//	再表示
	}
}
void MainWindow::onMarkdownPreviewLineClicked(/*int nth,*/ bool checked) {
	//qDebug() << "MainWindow::onMarkdownPreviewLineClicked(" << nth << ")";
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	QTextBlock block = docWidget->m_editor->textCursor().block();
	if (!block.isValid()) return;
	QTextCursor cursor(block);
	QString text = block.text();
	int ix = text.indexOf("- [");
	if( ix < 0 ) return;
	cursor.setPosition(block.position() + ix + 3);
	cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	//cursor.insertText(text[ix+3] == u' ' ? "x" : " ");
	cursor.insertText(checked ? "x" : " ");
	// undone: 完了時は (YYYY-MM-DD) を付加？
#if 0
	statusBar()->showMessage(QString("html block num = %1").arg(bln));
	auto table = docWidget->m_htmlComvertor.getBlockNumTohtmlLineNum();
	for(int i = 0; i < table.size(); ++i) {
		if( table[i] >= bln ) {
			return;
		}
	}
#endif
}
void MainWindow::onTextInsertedAtPreview(QString txt) {
	qDebug() << "MainWindow::onTextInsertedAtPreview(" << txt << ")";
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	QTextCursor cursor = docWidget->m_editor->textCursor();
	//m_processing = true;
	docWidget->m_editor->setIgnoreCC(true);
	cursor.insertText(txt);
	docWidget->m_editor->setTextCursor(cursor);
	docWidget->m_editor->setIgnoreCC(false);
	//m_processing = false;
}
void MainWindow::onTextRemovedAtPreview(int charsRemoved) {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	QTextCursor cursor = docWidget->m_editor->textCursor();
	cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, charsRemoved);
	cursor.removeSelectedText();
	//syncPreviewCursorWithEditor();
}
void MainWindow::onDel_pressed(bool ctrl) {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	QTextCursor cursor = docWidget->m_editor->textCursor();
	if( cursor.hasSelection() ) {
		cursor.deleteChar();
	} else {
		if( ctrl )
			docWidget->m_editor->deleteWord();
		else {
			auto pos = cursor.position();
			if( pos > 0 && docWidget->m_editor->document()->characterAt(pos-1) == '\\' ) {
				cursor.deletePreviousChar();
			}
			cursor.deleteChar();
		}
	}
	docWidget->m_editor->setTextCursor(cursor);
}
void MainWindow::onEnter_pressed() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->insertEnter();
	//QTextCursor cursor = docWidget->m_editor->textCursor();
	//cursor.insertText("\n");
	//docWidget->m_editor->setTextCursor(cursor);
}
void MainWindow::onTab_pressed() {
	onAction_Indent();
}
void MainWindow::onBS_pressed(bool ctrl) {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	QTextCursor cursor = docWidget->m_editor->textCursor();
	if( cursor.hasSelection() ) {
		cursor.deleteChar();
	} else {
		if( ctrl )
			docWidget->m_editor->backSpaceWord();
		else {
			auto pos = cursor.position();
			if( pos > 1 && docWidget->m_editor->document()->characterAt(pos-2) == '\\' ) {
				cursor.deletePreviousChar();
			}
			cursor.deletePreviousChar();
		}
	}
	docWidget->m_editor->setTextCursor(cursor);
	//syncPreviewCursorWithEditor();
}
void MainWindow::onUndoTriggered() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->undo();
}
void MainWindow::onRedoTriggered() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->redo();
}
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	// ドラッグされているデータがファイル（URL）を含んでいるかチェック
	if (event->mimeData()->hasUrls()) {
		// 受け入れる（カーソルが「コピー」や「リンク」の形に変わる）
		event->acceptProposedAction();
	}
}
void MainWindow::dropEvent(QDropEvent *event)
{
	const QList<QUrl> urls = event->mimeData()->urls();    // ドロップされたデータのURLリストを取得
	//if (urls.isEmpty()) return;
	for(const QUrl &url : urls ) {
		QString fullPath = url.toLocalFile();
		qDebug() << "dropped fullPath = " << fullPath;
		do_open("", fullPath);
	}
}
#if 0
QSplitter *MainWindow::getCurTabSplitter() {
	auto docWidget = ui->tabWidget->currentWidget();
	if( docWidget == nullptr ) return nullptr;
	return docWidget->findChild<QSplitter*>();
}
#endif
DocWidget *MainWindow::getCurDocWidget() {
	//int ix = ui->tabWidget->currentIndex();
	//auto ptr = (DocWidget*)ui->tabWidget->widget(ix);
	return (DocWidget*)ui->tabWidget->currentWidget();
}
//DocWidget *MainWindow::findDocWidget(const QString& fullPath) {
//	for(int ix = 0; ix < ui->tabWidget->count(); ++ix) {
//	}
//}

void MainWindow::onAboutToShow_Insert() {
	QMenu *menu = qobject_cast<QMenu*>(sender());
    if (!menu) return;
    QList<QAction*> acts = menu->actions();
    if( acts.size() >= 3 ) {
		QString today = QDate::currentDate().toString("yyyy-MM-dd");
		acts[0]->setText("&1 " + today);
		today = QDate::currentDate().toString("MM-dd");
		acts[1]->setText("&2 " + today);
		today = QDate::currentDate().toString("dd-MMM-yyyy");
		acts[2]->setText("&3 " + today);
    }
}
void MainWindow::onAboutToShow_FavoriteFiles() {
	//ui->menu_FavoriteFiles->clear();
	QList<QAction*> actions = ui->menu_FavoriteFiles->actions();
	for (int i = 2; i < actions.size(); ++i) {
	    QAction* act = actions.at(i);
	    ui->menu_FavoriteFiles->removeAction(act);
	    delete act;
	}
	QSettings settings;
	QStringList favoriteFilePaths = settings.value(KEY_FAVORITE_FILES).toStringList();
	int k = 0;
	QString key;
	for(const QString &fullPath : favoriteFilePaths) {
		if (++k <= 10)
			key = QString::number(k % 10);
		else
			key = QChar(u'A' + k - 11);
		QAction *act = ui->menu_FavoriteFiles->addAction("&" + key + " " + fullPath);
		connect(act, &QAction::triggered, this, [this, fullPath]() {
			QSettings settings;
	        QStringList favoriteFilePaths = settings.value(KEY_FAVORITE_FILES).toStringList();
	        favoriteFilePaths.removeAll(fullPath);
			QString pathArg = fullPath;
			if( !do_open("", pathArg) ) {
				//	ファイル削除などで、ファイルオープンできなかった場合
			} else {
				//	Undone: オープンしたファイルをリスト先頭に移動
				favoriteFilePaths.push_front(fullPath);
			}
	        settings.setValue(KEY_FAVORITE_FILES, favoriteFilePaths);
		});
	}
}
void MainWindow::onAboutToShow_RecentFiles() {
	//qDebug() << "MainWindow::onAboutToShow_RecentFiles()";
	ui->menu_RecentFiles->clear();
	QSettings settings;
	QStringList recentFilePaths = settings.value(KEY_RECENT_FILES).toStringList();
	int k = 0;
	QString key;
	for(const QString &fullPathRaw : recentFilePaths) {
		if (++k <= 10)
			key = QString::number(k % 10);
		else
			key = QChar(u'A' + k - 11);
		QString fullPath = QDir::cleanPath(fullPathRaw);
		QAction *act = ui->menu_RecentFiles->addAction("&" + key + " " + fullPath);
		connect(act, &QAction::triggered, this, [this, fullPath]() {
			QString pathArg = fullPath;
			if( !do_open("", pathArg) ) {
				//	ファイル削除などで、ファイルオープンできなかった場合
				QSettings settings;
		        QStringList recentFilePaths = settings.value(KEY_RECENT_FILES).toStringList();
		        recentFilePaths.removeAll(fullPath);
		        settings.setValue(KEY_RECENT_FILES, recentFilePaths);
			}
		});
	}
}
void MainWindow::onAction_AddThisFavorite() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr || docWidget->m_fullPath.isEmpty() )
		return;
	QSettings settings;
	QStringList favoriteFilePaths = settings.value(KEY_FAVORITE_FILES).toStringList();
	int ix;
	while( (ix = favoriteFilePaths.indexOf(docWidget->m_fullPath)) >= 0 )
		favoriteFilePaths.removeAt(ix);
	favoriteFilePaths.push_front(docWidget->m_fullPath);
	while( favoriteFilePaths.size() > N_FAVORITE_FILES ) favoriteFilePaths.pop_back();
	settings.setValue(KEY_FAVORITE_FILES, favoriteFilePaths);
}
void MainWindow::onAction_Help() {
	qDebug() << "appDir = " << QCoreApplication::applicationDirPath();
	QDir dir = QCoreApplication::applicationDirPath();		//	viMarkdown/viMarkdown/x64/Debug or Release
//#ifdef	_DEBUG
	if( IsDebuggerPresent() ) {
		dir.cdUp();			//	viMarkdown/viMarkdown/x64
		dir.cdUp();			//	viMarkdown/viMarkdown
		dir.cdUp();			//	viMarkdown
	}
//#endif
	dir.cd("docs/ja");
	qDebug() << "helpdir = " << dir.path();
	do_open("", dir.path() + "/help.md", QString(), true);
}
void MainWindow::onAction_Settings() {
	Global g0 = g;
	SettingsDialog dlg(this);
	connect(&dlg, &SettingsDialog::settingsChanged, this, &MainWindow::onSettingsChanged);
	if (dlg.exec() == QDialog::Accepted) {
	    //qDebug() << "QDialog::Accepted";
	    //updateEditorFontSize(g.m_editorFontSize);
	    save_settings();
	} else {
		//load_settings();		//	g を元に戻す
		g = g0;
	    updateEditorFontSize(g.m_editorFontSize);
	}
}
void MainWindow::onSettingsChanged() {
	updateEditorFontSize(g.m_editorFontSize);
}
void MainWindow::onAction_Exit() {
	this->close(); // メインウィンドウを閉じる
}
void MainWindow::onAction_New() {
	qDebug() << "MainWindow::onAction_New()";
#if 1
	static int nwin = 1;
	MainWindow *newWin = new MainWindow();
    newWin->setAttribute(Qt::WA_DeleteOnClose);		// ウィンドウが閉じられたときにメモリを自動解放
    newWin->restoreGeometry(this->saveGeometry());
    QPoint newPos = newWin->pos() + QPoint(20*nwin, 20*nwin);
    ++nwin;
    newWin->move(newPos);
    newWin->show();
#else
	addTab(QString("無題-%1").arg(++m_tab_number));
#endif
}
void MainWindow::onAction_NewTab() {
	addTab(QString("vmd-%1").arg(++m_tab_number));
}
void MainWindow::addTab(const QString &title, const QString fullPath, const QString txt,
							bool withBOM, QStringConverter::Encoding encoding,
							bool readOnly)
{
	auto docWidget = newTabWidget(title, fullPath, readOnly);		//	新規タブ生成
	docWidget->m_withBOM = withBOM;
	docWidget->m_encoding = encoding;
	docWidget->m_readOnly = readOnly;
	int ix = ui->tabWidget->addTab(docWidget, title);		//	新規タブを追加
	ui->tabWidget->setCurrentIndex(ix);				//	新規タブをカレントに
	if( !fullPath.isEmpty() ) 
		ui->tabWidget->setTabToolTip(ix, fullPath);
	MarkdownEditor *mdEditor = getCurDocWidget()->m_editor;
	if( !txt.isEmpty() )
		mdEditor->setPlainText(txt);
	mdEditor->setFocus();
	addTopItemToTreeWidget(title, fullPath);
	appendToDocLoc(docWidget->m_title, docWidget->m_fullPath, mdEditor->textCursor().position());
	printDocLocHist();
}
void MainWindow::addTopItemToTreeWidget(const QString &title, const QString fullPath) {
	QTreeWidgetItem *item = new QTreeWidgetItem();
	item->setIcon(0, QIcon(":/MainWindow/images/markdown_48.png"));
	item->setText(0, title);
	item->setData(0, Qt::UserRole, fullPath);
	if( !fullPath.isEmpty() )
		item->setToolTip(0, fullPath);
	ui->treeWidget->addTopLevelItem(item);
}
void MainWindow::onAction_Open() {
	QString fullPath = QFileDialog::getOpenFileName(
		this,
		"open File",			// ダイアログのタイトル
		QDir::currentPath(),		// 初期ディレクトリ
		"markdown file (*.md)"	// フィルター
	);

	if (!fullPath.isEmpty()) {
		qDebug() << "path = " << fullPath;
		do_open("", fullPath);
	}
}
int MainWindow::tabIndexOf(const QString& title, const QString& fullPath) {
	for(int ix = 0; ix < ui->tabWidget->count(); ++ix) {
		DocWidget *docWidget = (DocWidget*)ui->tabWidget->widget(ix);
		if( fullPath.isEmpty() ) {
			if( docWidget->m_title == title )
				return ix;
		} else {
			if( docWidget->m_fullPath == fullPath )
				return ix;
		}
	}
	return -1;
}
void MainWindow::addToRecentFiles(const QString& fullPath) {
	QSettings settings;
	QStringList recentFilePaths = settings.value(KEY_RECENT_FILES).toStringList();
	int ix;
	while( (ix = recentFilePaths.indexOf(fullPath)) >= 0 )
		recentFilePaths.removeAt(ix);
	recentFilePaths.push_front(fullPath);
	while( recentFilePaths.size() > N_RECENT_FILES ) recentFilePaths.pop_back();
	settings.setValue(KEY_RECENT_FILES, recentFilePaths);
}
void MainWindow::do_load(const QString& fullPath) {
	int tix = tabIndexOf("", fullPath);
	if( tix < 0 ) return;
	DocWidget *docWidget = (DocWidget*)ui->tabWidget->widget(tix);
	if( docWidget->m_saving ) return;		//	保存中はリロードしない
	if( docWidget->m_hasSaved ) {
		docWidget->m_hasSaved = false;		//	保存直後もリロードしない
		return;
	}
	QFile file(fullPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(this, tr("エラー"), tr("ファイルが開けません:\n%1").arg(fullPath));
		return;
	}
	QString content = file.readAll();
	docWidget->m_editor->setPlainText(content);
	docWidget->setModified(false);
	ui->tabWidget->setTabText(tix, docWidget->m_title);
	close_empty_doc();
}
void MainWindow::removeTopLevelItem(DocWidget* docWidget) {
	QTreeWidgetItem *top = findTopLevelItemByFullPath(docWidget->m_title, docWidget->m_fullPath);
	if( top != nullptr ) {
		delete top;			//	TreeWidget から top アイテム以下をすべて削除
	}
}
void MainWindow::close_empty_doc() {
	for(int ix = 0; ix < ui->tabWidget->count(); ++ix) {
		DocWidget *docWidget = (DocWidget*)ui->tabWidget->widget(ix);
		if( docWidget != nullptr && docWidget->m_fullPath.isEmpty() && !docWidget->isModified()) {
			if( docWidget->m_editor->toPlainText().isEmpty() ) {
				removeHistoryEntries(docWidget->m_title, docWidget->m_fullPath,
										docWidget->m_editor->textCursor().position());		//	履歴から削除
				ui->tabWidget->removeTab(ix);		//	空の無名ドキュメントを削除
				--ix;
				removeTopLevelItem(docWidget);
			}
		}
	}
}
bool hasBOM(QFile &file) {
	QByteArray header = file.peek(3);
	return header.startsWith("\xEF\xBB\xBF") ||		//	UTF-8
			header.startsWith("\xFF\xFE") || header.startsWith("\xFE\xFF");		//	UTF-16 BE, LE
}
bool MainWindow::do_open(const QString& title0, const QString& fullPath, const QString name, bool readOnly) {
	qDebug() << "do_open(" << title0 << ", " << fullPath << ")";
	if (fullPath.startsWith("http://") || fullPath.startsWith("https://")) {
        QDesktopServices::openUrl(QUrl(fullPath));
        return true;
	}
	if( title0.isEmpty() && fullPath.isEmpty() ) {		//	現文書内ジャンプ
		if( !name.isEmpty() ) {
			DocWidget *docWidget = getCurDocWidget();;
			if( docWidget != nullptr ) {
				//appendToDocLoc(docWidget->m_title, docWidget->m_fullPath, docWidget->m_editor->linkClickedPos());	//	ジャンプ前位置
				docWidget->m_editor->jumpToHeading(name);
				appendToDocLoc(docWidget->m_title, docWidget->m_fullPath, docWidget->m_editor->textCursor().position());	//	ジャンプ後位置
				docWidget->m_editor->scrollToTop(docWidget->m_editor->textCursor());
				syncEditorPreviewScroll();
				printDocLocHist();
			}
		}
		return true;
	}
	int tix = tabIndexOf(title0, fullPath);
	if( tix >= 0 ) {		//	すでにオープン済み
		ui->tabWidget->setCurrentIndex(tix);
		if( !name.isEmpty() ) {
			DocWidget *docWidget = (DocWidget*)ui->tabWidget->widget(tix);
			docWidget->m_editor->jumpToHeading(name);
		}
		return true;
	}
	QFileInfo checkFile(fullPath);
	if( !checkFile.exists() ) {
		QString mess = QString("'%1'\nファイルが存在しません。\n新規作成しますか？").arg(fullPath);
		auto reply = QMessageBox::question(this, "confirmation", mess,
                                   QMessageBox::Yes | QMessageBox::No);
		if( reply != QMessageBox::Yes ) return false;
		//newTabWidget(fullPath, fullPath);
		QFileInfo fileInfo(fullPath);
		QString title = fileInfo.fileName();
		title.remove(QRegularExpression("\\.md$"));
		addTab(title, fullPath);
		return true;
	}
	m_opening_file = true;
	QFile file(fullPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(this, tr("エラー"), tr("ファイルが開けません:\n%1").arg(fullPath));
		return false;
	}
    bool withBOM = hasBOM(file);
	//QString content = file.readAll();
	QTextStream in(&file);
	in.setAutoDetectUnicode(true); 
    QString content = in.readAll();
    //bool bBOM = in.generateByteOrderMark();
    auto encoding = in.encoding();
    file.close();
	QFileInfo fileInfo(fullPath);
	QString title = fileInfo.fileName();
	title.remove(QRegularExpression("\\.md$"));
	addTab(title, fullPath, content, withBOM, encoding, readOnly);
	updateOutlineTree();
	QDir::setCurrent(fileInfo.path());
	m_watcher->addPath(fullPath);
	m_opening_file = false;

	DocWidget *docWidget = getCurDocWidget();
	if( docWidget != nullptr )
		docWidget->m_editor->jumpToHeading(name);
	addToRecentFiles(fullPath);
	close_empty_doc();
	return true;
}
void MainWindow::onAction_Save() {
	do_save();
}
void MainWindow::onAction_SaveAs() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	QTreeWidgetItem *top = findTopLevelItemByFullPath(docWidget->m_title, docWidget->m_fullPath);
	do_save(true);
	if( top != nullptr )
		top->setToolTip(0, docWidget->m_fullPath);
}
void MainWindow::do_save(bool fDialog) {
	int ix = ui->tabWidget->currentIndex();
	if( ix < 0 ) return;
	DocWidget *docWidget = (DocWidget*)ui->tabWidget->widget(ix);
	if( docWidget == nullptr ) return;
	QString fullPath = docWidget->m_fullPath;
	if( fDialog || fullPath.isEmpty() ) {		//	強制ダイアログ || フルパスを持っていない場合
		QString oldFullPath = fullPath;
		QString oldTitle = docWidget->m_title;
		QString path = QDir::currentPath();	  // カレントディレクトリ
		QString title = docWidget->getTitle();
		if( !title.isEmpty() )
			path += "/" + title;
		fullPath = QFileDialog::getSaveFileName(
						this,				   // 親ウィジェット（メインウィンドウがあれば this）
						"Save File",		 // ダイアログのタイトル
						title,
						"markdown (*.md)"  // ファイルフィルタ
						,nullptr // 選択されたフィルタを保存する変数（不要なら nullptr）
		                ,QFileDialog::DontUseNativeDialog
					);
		if( fullPath.isEmpty() ) return;
		QFileInfo fi(fullPath);
	    if (fi.suffix().isEmpty()) fullPath += ".md";
		docWidget->m_fullPath = fullPath;
		QFileInfo fileInfo(fullPath);
		QString title2 = fileInfo.fileName();
		title2.remove(QRegularExpression("\\.md$"));
		docWidget->m_title = title2;
		ui->tabWidget->setTabText(ix, docWidget->m_title);
		ui->tabWidget->setTabToolTip(ix, fullPath);
		QTreeWidgetItem *top = findTopLevelItemByFullPath(oldTitle, oldFullPath);
		top->setText(0, docWidget->m_title);
		if( !oldFullPath.isEmpty() && oldFullPath != fullPath )		//	ファイル名変更
			m_watcher->removePath(oldFullPath);
	}
	addToRecentFiles(fullPath);
	docWidget->m_saving = true;
	m_watcher->removePath(fullPath);		//	一旦ウォッチリストから外す
	QFile file(fullPath);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);
		out.setEncoding(docWidget->m_encoding); 
		out.setGenerateByteOrderMark(docWidget->m_withBOM);
		MarkdownEditor *mdEditor = docWidget->m_editor;
		out << mdEditor->toPlainText();
		file.close();
		//QMessageBox::information(nullptr, "成功", "ファイルが保存されました:\n" + fullPath);
		docWidget->setModified(false);		//	保存済み
		ui->tabWidget->setTabText(ix, docWidget->m_title);
		m_watcher->addPath(fullPath);
		docWidget->m_hasSaved = true;		//	保存済み for 自動更新
	} else {
		//QMessageBox::warning(nullptr, "エラー", "ファイルを開けませんでした。");
	}
	docWidget->m_saving = false;
	m_watcher->addPath(fullPath);
}
void MainWindow::onAction_ExportAsPDF() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	QString title;
	if( !docWidget->m_fullPath.isEmpty() ) title = docWidget->m_title;
	QString fileName = QFileDialog::getSaveFileName(this, "export as PDF", title, "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    docWidget->m_preview->print(&printer);
    statusBar()->showMessage(tr("PDF export completed."), 5000);
}
void MainWindow::onAction_Close() {
	qDebug() << "MainWindow::onAction_Close()";

	DocWidget *docWidget = getCurDocWidget();
	if (docWidget == nullptr) return;
	if( docWidget->isModified() ) {
		QMessageBox::StandardButton reply = QMessageBox::question(this,
								  "Confirm save",				 // タイトル
								  "The document has been modified.\nDo you want to save your changes?", // 本文
								  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel); // ボタンの種類

		if (reply == QMessageBox::Yes) {
			onAction_Save();
		} else if (reply == QMessageBox::Cancel) {
			return;
		}
	}
	int ix = ui->tabWidget->currentIndex();
	if (ix >= 0)
		ui->tabWidget->removeTab(ix);
	removeTopLevelItem(docWidget);
}
void MainWindow::onAction_Print() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle("print Markdown");
    if (dialog.exec() == QDialog::Accepted) {
        docWidget->m_preview->print(&printer);
    }
}
bool isCheckbox(const QString txt, int s) {
	return txt[s] == '[' && (txt[s+1] == ' ' || txt[s+1] == 'x' || txt[s+1] == 'X') && txt[s+2] == ']' && txt[s+3] == ' ';
}
int isCheckboxBlock(const QTextBlock& block) {		//	空白+ "- [{ xX}] " で始まるか？ return 0 for not List, 1以上 for 文字数
	const QString txt = block.text();
	int i = 0;
	while( i < txt.size() && txt[i] == ' ' ) ++i;
	if( !txt.mid(i).startsWith("- ") || txt.size() < 6) return 0;
	if( !isCheckbox(txt, 2) ) return 0;
	return i + 6;
}
int isListBlock(const QTextBlock& block) {		//	空白+ "- " で始まるか？ return 0 for not List, 1以上 for 文字数
	const QString txt = block.text();
	int i = 0;
	while( i < txt.size() && txt[i] == ' ' ) ++i;
	if( !txt.mid(i).startsWith("- ") ) return 0;
	return i + 2;
}
int isNumListBlock(const QTextBlock& block) {		//	空白+ "数字. " で始まるか？ return 0 for not NumList, 1以上 for 文字数
	const QString txt = block.text();
	int i = 0;
	while( i < txt.size() && txt[i] == ' ' ) ++i;
	if( i >= txt.size() || !txt[i].isNumber() ) return 0;
	if( !txt.mid(i+1).startsWith(". ") ) return 0;
	return i + 3;
}
void MainWindow::onAction_Indent() {
	MarkdownEditor *mdEditor = getCurDocWidget()->m_editor;
	QTextCursor cursor = mdEditor->textCursor();
	QTextDocument *doc = mdEditor->document();
	cursor.beginEditBlock();
	int startPos = cursor.selectionStart();
	int endPos = cursor.selectionEnd();
	// 範囲に含まれる最初のブロックと最後のブロックを取得
	QTextBlock startBlock = doc->findBlock(startPos);
	QTextBlock currentBlock = startBlock;
	QTextBlock endBlock = doc->findBlock(endPos);
	if (endPos > startPos && endPos == endBlock.position())
		endBlock = endBlock.previous();		//	最終ブロック修正
	while (currentBlock.isValid() && currentBlock.blockNumber() <= endBlock.blockNumber()) {
		cursor.setPosition(currentBlock.position());	//	行頭位置
		cursor.insertText("  ");
		currentBlock = currentBlock.next();		// 次のブロックへ
	}
	if( startBlock < endBlock ) {
		cursor.setPosition(startBlock.position());	//	行頭位置
		if( endBlock.next().isValid() ) {
			cursor.setPosition(endBlock.next().position(), QTextCursor::KeepAnchor);	//	行頭位置
		} else {
			cursor.setPosition(endBlock.position(), QTextCursor::KeepAnchor);
			cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		}
	} else {
		cursor.setPosition(startBlock.position());
		cursor.movePosition(QTextCursor::EndOfBlock);
	}
	mdEditor->setTextCursor(cursor);
	cursor.endEditBlock();
}
void MainWindow::onAction_UnIndent() {
	MarkdownEditor *mdEditor = getCurDocWidget()->m_editor;
	QTextCursor cursor = mdEditor->textCursor();
	QTextDocument *doc = mdEditor->document();
	cursor.beginEditBlock();
	int startPos = cursor.selectionStart();
	int endPos = cursor.selectionEnd();
	// 範囲に含まれる最初のブロックと最後のブロックを取得
	QTextBlock startBlock = doc->findBlock(startPos);
	QTextBlock currentBlock = startBlock;
	QTextBlock endBlock = doc->findBlock(endPos);
	if (endPos > startPos && endPos == endBlock.position())
		endBlock = endBlock.previous();		//	最終ブロック修正
	while (currentBlock.isValid() && currentBlock.blockNumber() <= endBlock.blockNumber()) {
		cursor.setPosition(currentBlock.position());	//	行頭位置
		if( currentBlock.text().startsWith("  ") ) {
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 2);
			cursor.removeSelectedText();
		}
		currentBlock = currentBlock.next();		// 次のブロックへ
	}
	if( startBlock < endBlock ) {
		cursor.setPosition(startBlock.position());	//	行頭位置
		if( endBlock.next().isValid() ) {
			cursor.setPosition(endBlock.next().position(), QTextCursor::KeepAnchor);	//	行頭位置
		} else {
			cursor.setPosition(endBlock.position(), QTextCursor::KeepAnchor);
			cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		}
	} else {
		cursor.setPosition(startBlock.position());
		cursor.movePosition(QTextCursor::EndOfBlock);
	}
	mdEditor->setTextCursor(cursor);
	cursor.endEditBlock();
}
void MainWindow::onAction_Checkbox() {
	MarkdownEditor *mdEditor = getCurDocWidget()->m_editor;
	QTextCursor cursor = mdEditor->textCursor();
	QTextDocument *doc = mdEditor->document();
	cursor.beginEditBlock();
	bool hadSelection = cursor.hasSelection();
	int startPos = cursor.selectionStart();
	int endPos = cursor.selectionEnd();
	QTextBlock startBlock = doc->findBlock(startPos);		// 範囲に含まれる最初のブロックと最後のブロックを取得
	QTextBlock currentBlock = startBlock;
	QTextBlock endBlock = doc->findBlock(endPos);
	bool remove_checkbox = isCheckboxBlock(currentBlock) != 0;
	if (endPos > startPos && endPos == endBlock.position())
		endBlock = endBlock.previous();		//	最終ブロック修正
	while (currentBlock.isValid() && currentBlock.blockNumber() <= endBlock.blockNumber()) {
		cursor.setPosition(currentBlock.position());	//	行頭位置
		if( remove_checkbox ) {
			int n = isCheckboxBlock(currentBlock);
			if( n != 0 ) {
				cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, n);
				cursor.removeSelectedText();
			}
		} else {
			if( !isCheckboxBlock(currentBlock) ) {
				if( currentBlock.text().startsWith("- ") ) {
					cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 2);
					cursor.insertText("[ ] ");
				} else
					cursor.insertText("- [ ] ");
			}
		}
		currentBlock = currentBlock.next();		// 次のブロックへ
	}
	cursor.setPosition(startBlock.position());	//	行頭位置
	if( !hadSelection ) {
		cursor.movePosition(QTextCursor::EndOfBlock);
	} else if( endBlock.next().isValid() ) {
		cursor.setPosition(endBlock.next().position(), QTextCursor::KeepAnchor);	//	行頭位置
	} else {
		cursor.setPosition(endBlock.position(), QTextCursor::KeepAnchor);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	}
	cursor.endEditBlock();
	mdEditor->setTextCursor(cursor);
	this->activateWindow();
	mdEditor->setFocus();
}
void MainWindow::onAction_List() {
	qDebug() << "MainWindow::onAction_List()";
	MarkdownEditor *mdEditor = getCurDocWidget()->m_editor;
	QTextCursor cursor = mdEditor->textCursor();
	QTextDocument *doc = mdEditor->document();
	cursor.beginEditBlock();
	bool hadSelection = cursor.hasSelection();
	int startPos = cursor.selectionStart();
	int endPos = cursor.selectionEnd();
	QTextBlock startBlock = doc->findBlock(startPos);		// 範囲に含まれる最初のブロックと最後のブロックを取得
	QTextBlock currentBlock = startBlock;
	QTextBlock endBlock = doc->findBlock(endPos);
	bool remove_list = isListBlock(currentBlock) != 0;
	if (endPos > startPos && endPos == endBlock.position())
		endBlock = endBlock.previous();		//	最終ブロック修正
	while (currentBlock.isValid() && currentBlock.blockNumber() <= endBlock.blockNumber()) {
		cursor.setPosition(currentBlock.position());	//	行頭位置
		if( remove_list ) {
			int n = isListBlock(currentBlock);
			if( n != 0 ) {
				cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, n);
				cursor.removeSelectedText();
			}
		} else {
			if( !isListBlock(currentBlock) ) {
				int n = isNumListBlock(currentBlock);	//	空白+ "数字. "
				if( n > 0 ) {
					cursor.setPosition(currentBlock.position() + n - 3);		//	3 for "数字. ".length()
					cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 3);
				}
				cursor.insertText("- ");
			}
		}
		currentBlock = currentBlock.next();		// 次のブロックへ
	}
	cursor.setPosition(startBlock.position());	//	行頭位置
	if( !hadSelection ) {
		cursor.movePosition(QTextCursor::EndOfBlock);
	} else if( endBlock.next().isValid() ) {
		cursor.setPosition(endBlock.next().position(), QTextCursor::KeepAnchor);	//	行頭位置
	} else {
		cursor.setPosition(endBlock.position(), QTextCursor::KeepAnchor);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	}
	cursor.endEditBlock();
	mdEditor->setTextCursor(cursor);
	this->activateWindow();
	mdEditor->setFocus();
}
void MainWindow::onAction_NumList() {
	qDebug() << "MainWindow::onAction_NumList()";
	MarkdownEditor *mdEditor = getCurDocWidget()->m_editor;
	QTextCursor cursor = mdEditor->textCursor();
	QTextDocument *doc = mdEditor->document();
	cursor.beginEditBlock();
	bool hadSelection = cursor.hasSelection();
	int startPos = cursor.selectionStart();
	int endPos = cursor.selectionEnd();
	QTextBlock startBlock = doc->findBlock(startPos);		// 範囲に含まれる最初のブロックと最後のブロックを取得
	QTextBlock currentBlock = startBlock;
	QTextBlock endBlock = doc->findBlock(endPos);
	bool remove_list = isNumListBlock(currentBlock) != 0;
	if (endPos > startPos && endPos == endBlock.position())
		endBlock = endBlock.previous();		//	最終ブロック修正
	while (currentBlock.isValid() && currentBlock.blockNumber() <= endBlock.blockNumber()) {
		cursor.setPosition(currentBlock.position());	//	行頭位置
		//cursor.movePosition(QTextCursor::StartOfBlock);			//	行頭移動　←　何故かうまく動作しない？？？
		if( remove_list ) {
			int n = isNumListBlock(currentBlock);
			if( n != 0 ) {
				cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, n);
				cursor.removeSelectedText();
			}
		} else {
			if( !isNumListBlock(currentBlock) ) {
				int n = isListBlock(currentBlock);	//	空白+ "- "
				if( n > 0 ) {
					cursor.setPosition(currentBlock.position() + n - 2);		//	2 for "- ".length()
					cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 2);
				}
				cursor.insertText("1. ");
			}
		}
		currentBlock = currentBlock.next();		// 次のブロックへ
	}
	cursor.setPosition(startBlock.position());	//	行頭位置
	if( !hadSelection ) {
		cursor.movePosition(QTextCursor::EndOfBlock);
	} else if( endBlock.next().isValid() ) {
		cursor.setPosition(endBlock.next().position(), QTextCursor::KeepAnchor);	//	行頭位置
	} else {
		cursor.setPosition(endBlock.position(), QTextCursor::KeepAnchor);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	}
	cursor.endEditBlock();
	mdEditor->setTextCursor(cursor);
	this->activateWindow();
	mdEditor->setFocus();
}
bool isWrappedWith(const QTextCursor &cursor, const QString &delimiter) {
    if (!cursor.hasSelection() || delimiter.isEmpty()) {
        return false;
    }
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    int delLen = delimiter.length();
    QTextDocument *doc = cursor.document();
    // 1. 境界チェック：前後にデリミタ分の長さがあるか
    if (start < delLen || (end + delLen) > doc->characterCount() - 1) {
        return false;
    }
    // 2. 直前の文字列チェック
    for (int i = 0; i < delLen; ++i) {
        // start - delLen から delLen 分の文字を比較
        if (doc->characterAt(start - delLen + i) != delimiter.at(i)) {
            return false;
        }
    }
    // 3. 直後の文字列チェック
    for (int i = 0; i < delLen; ++i) {
        // end から delLen 分の文字を比較
        if (doc->characterAt(end + i) != delimiter.at(i)) {
            return false;
        }
    }
    return true;
}
//	選択前後に delimiter（** 等）を挿入
//	ただし、選択前後に delimiter がある場合は削除
void MainWindow::insertInline(const QString& delimiter) {
	MarkdownEditor *mdEditor = getCurDocWidget()->m_editor;
	QTextCursor cursor = mdEditor->textCursor();
	if (cursor.hasSelection()) {
		// 2. 複数行にまたがっているかチェック
		QTextDocument *doc = mdEditor->document();
		// 選択範囲の「開始位置」と「終了位置」が属するブロック（行）を取得
		QTextBlock startBlock = doc->findBlock(cursor.selectionStart());
		QTextBlock endBlock   = doc->findBlock(cursor.selectionEnd());
		// ブロック番号が異なる場合＝複数行選択されている場合は無視
		if (startBlock.blockNumber() != endBlock.blockNumber())
			return;
		if( isWrappedWith(cursor, delimiter) ) {
			QString newText = cursor.selectedText();
		    int start = cursor.selectionStart() - delimiter.size();
		    int end = cursor.selectionEnd() + delimiter.size();
			cursor.setPosition(start);
			cursor.setPosition(end, QTextCursor::KeepAnchor);
			cursor.insertText(newText);
		} else {
			QString newText = delimiter + cursor.selectedText() + delimiter;
			cursor.insertText(newText);
		}
	} else {
		cursor.insertText(delimiter + delimiter);
		cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, delimiter.length());
	}
	mdEditor->setTextCursor(cursor);
}
void MainWindow::onAction_Undo() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->undo();
}
void MainWindow::onAction_Redo() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->redo();
}
void MainWindow::onAction_SelectAll() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->selectAll();
}
void MainWindow::onAction_Paste() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->paste();
}
void MainWindow::onAction_Heading() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	QTextCursor cursor = docWidget->m_editor->textCursor();
	const QString buf = cursor.block().text();
	cursor.movePosition(QTextCursor::StartOfBlock);
	if( !buf.startsWith("#") ) {	//	ヘッダ行でない場合
		cursor.insertText("# ");
	} else {
		int h = 0;
		while( ++h < buf.size() && buf[h] == '#' ) {}
		if( h < 6 ) {
			cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, h);
			cursor.insertText("#");
		} else {
			while( h < buf.size() && buf[h] == ' ' ) ++h;
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, h);
			cursor.deleteChar();
		}
	}
	docWidget->m_editor->setTextCursor(cursor);
}
void MainWindow::onAction_Bold() {
	insertInline("**");
}
void MainWindow::onAction_Italic() {
	insertInline("*");
}
void MainWindow::onAction_Strikethrough() {
	insertInline("~~");
}
void MainWindow::onAction_Link() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->make_link();
}
void MainWindow::onAction_AlignCenter() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->onAlignCenter();
}
void MainWindow::onAction_AlignLeft() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->onAlignLeft();
}
void MainWindow::onAction_AlignRight() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->onAlignRight();
}
void MainWindow::onAction_KeisenMode(bool checked) {
	m_keisenMode = checked;
	for(int ix = 0; ix < ui->tabWidget->count(); ++ix) {
		DocWidget *docWidget = (DocWidget*)ui->tabWidget->widget(ix);
		if( docWidget != nullptr )
			docWidget->m_editor->onKeisenMode(checked);
	}
}
void MainWindow::onAction_ThinKeisen(bool checked) {
	m_thickKeisen = !checked;
	updateThinThickCheck();
}
void MainWindow::onAction_ThickKeisen(bool checked) {
	m_thickKeisen = checked;
	updateThinThickCheck();
}
void MainWindow::updateThinThickCheck() {
	ui->action_ThinKeisen->setChecked(!m_thickKeisen);
	ui->action_ThickKeisen->setChecked(m_thickKeisen);
}
void MainWindow::onAction_OpenPrev() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->openPrev();
}
void MainWindow::onAction_OpenNext() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->openNext();
}
void MainWindow::onAction_HTML(bool checked) {
	//if( m_htmlMode ) return;
	m_htmlMode = checked;
	//updateHTMLModeCheck();
	updatePreview();
}
void MainWindow::onAction_Source(bool checked) {
	m_htmlMode = !checked;
	//updateHTMLModeCheck();
	updatePreview();
}
void MainWindow::onAction_TagJump() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_editor->tagJump();
}
void MainWindow::onAction_OutlineBar(bool checked) {
	ui->outlineBar->setVisible(checked);
}
void MainWindow::onAction_FocusOutline() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	ui->outlineBar->setVisible(true);
	ui->treeWidget->setFocus();
	QTreeWidgetItem* item = findTopLevelItemByFullPath(docWidget->m_title, docWidget->m_fullPath);
	if( item != nullptr )
		ui->treeWidget->setCurrentItem(item);
}
void MainWindow::onAction_NextTab() {
	if( ui->tabWidget->count() <= 1 ) return;
	int tix = (ui->tabWidget->currentIndex() + 1) % ui->tabWidget->count();
	ui->tabWidget->setCurrentIndex(tix);
}
void MainWindow::onAction_PrevTab() {
	if( ui->tabWidget->count() <= 1 ) return;
	int tix = ui->tabWidget->currentIndex() - 1;
	if( tix < 0 ) tix = ui->tabWidget->count() - 1;
	ui->tabWidget->setCurrentIndex(tix);
}
void MainWindow::onAction_ToggleFocus() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	if( docWidget->m_editor->hasFocus() )
		docWidget->m_preview->setFocus();
	else
		docWidget->m_editor->setFocus();
	docWidget->m_editor->viewport()->update();
	docWidget->m_preview->viewport()->update();
}
void MainWindow::onAction_SwitchToAltFile() {
	//auto fullPath = m_altFullPath;
	//m_altFullPath = m_curFullPath
	if( !m_altFullPath.isEmpty() )
		do_open("", m_altFullPath);
	else if( !m_altTitle.isEmpty() ) {
		//	m_altTitle 文書をアクティブに
		int ix = tabIndexOf(m_altTitle, m_altFullPath);
		if( ix >= 0 )
			ui->tabWidget->setCurrentIndex(ix);
	}
}
void MainWindow::onOutlineBarVisibilityChanged(bool v) {
	ui->action_OutlineBar->setChecked(v);
}
int MainWindow::treeItemToTabIndex(QTreeWidgetItem *current) {
	if( current == nullptr ) -1;
	auto top = current;
	while( top->parent() != nullptr ) top = top->parent();		//	親のトップレベルアイテムまで移動
	QString fullPath = top->data(0, Qt::UserRole).toString();
	return tabIndexOf(current->text(0), fullPath);
}
void MainWindow::onTreeCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
	//if( current == nullptr ) return;
	//auto top = current;
	//while( top->parent() != nullptr ) top = top->parent();		//	親のトップレベルアイテムまで移動
	//QString fullPath = top->data(0, Qt::UserRole).toString();
	//int tix = tabIndexOf(current->text(0), fullPath);
	int tix = treeItemToTabIndex(current);
	if( tix >= 0 ) {
		ui->tabWidget->setCurrentIndex(tix);
		DocWidget *docWidget = (DocWidget*)ui->tabWidget->widget(tix);
		if( current->parent() == nullptr ) {	//	トップレベルアイテムの場合
			statusBar()->showMessage(docWidget->m_fullPath, 5000);
		} else {	//	見出しアイテムの場合
			MarkdownEditor *mdEditor = docWidget->m_editor;
			int ln = current->data(0, Qt::UserRole).toInt();			//	行番号　0 org.
			//QTextBlock block = mdEditor->document()->findBlockByLineNumber(ln);
			QTextBlock block = mdEditor->document()->findBlockByNumber(ln);
			QTextCursor cursor = mdEditor->textCursor();
			cursor.setPosition(block.position());
			mdEditor->setTextCursor(cursor);
			//mdEditor->ensureCursorVisible();
			mdEditor->scrollToTop(cursor);
			//cursor = docWidget->m_preview->textCursor();
			//docWidget->m_preview->scrollToTop(cursor);
			syncEditorPreviewScroll();
		}
	}
}
void MainWindow::onTreeItemActivated(QTreeWidgetItem *current, int) {
	qDebug() << "MainWindow::onTreeItemActivated(QTreeWidgetItem *current, int)";
	int tix = treeItemToTabIndex(current);
	if( tix < 0 ) return;
	DocWidget *docWidget = (DocWidget*)ui->tabWidget->widget(tix);
	MarkdownEditor *mdEditor = docWidget->m_editor;
	mdEditor->setFocus();
}
void MainWindow::updatePreview() {
#if 0
	DocWidget *docWidget = getCurDocWidget();
	qDebug() << "docWidget = " << docWidget;
	MarkdownEditor *mdEditor = docWidget->m_editor;
	MarkdownPreview* textEdit = docWidget->m_preview;
	QScrollBar *vScrollBar = textEdit->verticalScrollBar();
	int currentPos = vScrollBar->value();
	const QString &htmlText = docWidget->m_htmlComvertor.getHtmlText();
	if( m_htmlMode ) {
		textEdit->setHtml(htmlText);
		//textEdit->setHtml(mdEditor->toPlainText());
	} else
		textEdit->setPlainText(htmlText);
	vScrollBar->setValue(currentPos);
#endif
}
QTreeWidgetItem* MainWindow::findTopLevelItemByFullPath(const QString& title, const QString fullPath) {
	QTreeWidget *treeWidget = ui->treeWidget;
	int topCount = treeWidget->topLevelItemCount();
	for (int i = 0; i < topCount; ++i) {
		QTreeWidgetItem* item = treeWidget->topLevelItem(i);
		QString itemPath = item->data(0, Qt::UserRole).toString();
		if( itemPath.isEmpty() ) {
			if( item->text(0) == title )
				return item;
		} else if( itemPath == fullPath )
			return item;
	}
	return nullptr;
}
void expandAllChildren(QTreeWidgetItem *item) {
	if (!item) return;
	item->setExpanded(true);  // まず自身を展開
	for (int i = 0; i < item->childCount(); ++i) {
		QTreeWidgetItem *child = item->child(i);
		expandAllChildren(child);  // 子に対して再帰呼び出し
	}
}
void MainWindow::updateOutlineTree() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	//	現文書に対応するトップレベルアイテムへのポインタ取得
	QTreeWidgetItem* item0 = findTopLevelItemByFullPath(docWidget->m_title, docWidget->m_fullPath);
	if( item0 == nullptr ) return;
	qDeleteAll(item0->takeChildren());
	vector<QTreeWidgetItem*> parents(10, nullptr);		//	各レベルごとの親アイテムリスト
	parents[0] = item0;
	const QStringList &lst = docWidget->m_preview->getHeadings();
	const vector<int>& hLineNum = docWidget->getSrcHeadingsBlocks();
	for(int i = 0; i != lst.size(); ++i) {
		QTreeWidgetItem *item2 = new QTreeWidgetItem();
		//bool ok;
		//int val = lst[i].toInt(&ok, 10);
		int val = lst[i][0].unicode() - '0';
		item2->setIcon(0, QIcon(":/MainWindow/images/small_square_48.png"));
		item2->setText(0, lst[i].mid(1));	//	lst[i][0] は見出しレベル（'1'-'6'）
		QString txt = lst[i].mid(1).remove(QRegularExpression("^ *[■●○◎★☆]\\s*"));
		item2->setText(0, txt);
		item2->setData(0, Qt::UserRole, hLineNum[i+1]);
		int k = val - 1;
		while( parents[k] == nullptr ) --k;
		parents[k]->addChild(item2);
		parents[val] = item2;
		while( ++val < 10 ) parents[val] = nullptr;
	}
	expandAllChildren(item0);
}
void MainWindow::onModificationChanged(bool b) {
	DocWidget *docWidget = getCurDocWidget();
	if (docWidget == nullptr) return;
	QString title = docWidget->m_title;
	if( b ) title += " *";
	ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), title);
}
void MainWindow::onSrcCursorPosChanged() {
}
void MainWindow::syncEditorPreviewScroll() {
	DocWidget *docWidget = getCurDocWidget();
	if (docWidget == nullptr) return;
	QRect srcRect = docWidget->m_editor->cursorRect();
    int srcCursorY = srcRect.y(); // ビューポート上端からの相対座標
    int srcViewHeight = docWidget->m_editor->viewport()->height();
    double ratio = (srcViewHeight > 0) ? (double)srcCursorY / srcViewHeight : 0.0;
    //
    QRect dstRect = docWidget->m_preview->cursorRect(); 
    int dstCursorY = dstRect.y(); // 現在のビューポート上端からの相対Y
    int dstViewHeight = docWidget->m_preview->viewport()->height();
    int targetYInView = dstViewHeight * ratio;
    int currentScroll = docWidget->m_preview->verticalScrollBar()->value();
    int diff = dstCursorY - targetYInView;
    if( abs(diff) >= 16 )
	    docWidget->m_preview->verticalScrollBar()->setValue(currentScroll + diff);	//	スクロール
}
void MainWindow::onMDTextChanged() {
	//qDebug() << "MainWindow::onMDTextChanged()";

	if( m_ignore_changed ) return;
	m_ignore_changed = true;
	DocWidget *docWidget = getCurDocWidget();
	//qDebug() << "docWidget = " << docWidget;
	MarkdownEditor *mdEditor = docWidget->m_editor;
	//m_plainText = mdEditor->toPlainText();
#if 1
	if( !mdEditor->isComposing() ) {
		int scrollPos = docWidget->m_preview->verticalScrollBar()->value();
		docWidget->m_editor->setProcessing(true);
		docWidget->m_preview->setMarkdown(mdEditor->document());
		docWidget->m_preview->verticalScrollBar()->setValue(scrollPos);
		docWidget->m_editor->setProcessing(false);
		docWidget->m_editor->syncEditorCursorFromPreview();
		//syncPreviewCursorWithEditor();
	}
#else
	auto &htmlComvertor = docWidget->m_htmlComvertor;
	htmlComvertor.convert(mdEditor->document());
#endif
#if 0
	const vector<char>& blockType = htmlComvertor.getBlockType();
	QTextCursor cursor(mdEditor->document()); 
	QTextCharFormat fmt_darkred, fmt_black;
	fmt_darkred.setForeground(QColor("darkred"));
	fmt_black.setForeground(QColor("black"));
	QTextBlock block = mdEditor->document()->firstBlock();
	for(int ln = 0; block.isValid() && ln < blockType.size(); ++ln) {
		cursor.setPosition(block.position());
		cursor.select(QTextCursor::BlockUnderCursor);
		if( blockType[ln] == '#' ) {
			cursor.mergeCharFormat(fmt_darkred);
		} else {
			cursor.mergeCharFormat(fmt_black);
		}
		block = block.next();
	}
#endif
	updatePreview();
	updateOutlineTree();
#if 0
	if( !m_opening_file ) {
		DocWidget *docWidget = getCurDocWidget();
		if( docWidget == nullptr ) return;
		docWidget->setModified(true);
		ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), docWidget->m_title + " *");
	}
#endif
	m_ignore_changed = false;
}
void MainWindow::onMdEditTabPressed() {
	onAction_Indent();
}
void MainWindow::onMdEditEscPressed() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	QTextCursor cursor = docWidget->m_preview->textCursor();
	if( cursor.hasSelection() ) {
		cursor.setPosition(cursor.position()); // 現在位置にセットし直すことで選択が消える
		docWidget->m_preview->setTextCursor(cursor);
	}
	ui->action_KeisenMode->setChecked(false);
}
void MainWindow::onMdEditCurPosChanged() {
#if 1
	MarkdownEditor *editor = (MarkdownEditor*)sender();
	QTextCursor cursor = editor->textCursor();
	int clmn = cursor.position() - cursor.block().position();
	m_lcLabel->setText(QString("%1:%2 (pos = %3)").arg(cursor.blockNumber()+1).arg(clmn+1).arg(cursor.position()));
#else
	qDebug() << "MainWindow::onMdEditCurPosChanged()";
	MarkdownEditor *mdEditor = (MarkdownEditor*)sender();
	QTextCursor cursor = mdEditor->textCursor();
	int bnum = cursor.blockNumber();
	
	//QString mess = QString("cursor.blockNumber = %1").arg(bnum);
	//mess += QString(", preview.blockNumber() = %1").arg(cursor.blockNumber());
	//statusBar()->showMessage(mess);
	int clmn = cursor.position() - cursor.block().position();
	m_lcLabel->setText(QString("%1:%2").arg(cursor.blockNumber()+1).arg(clmn+1));
	DocWidget *docWidget = getCurDocWidget();
	const auto v = docWidget->m_htmlComvertor.getBlockNumTohtmlLineNum();
	if( cursor.blockNumber() < v.size() ) {
		QString mess = QString("html line = %1").arg(v[cursor.blockNumber()]);
		statusBar()->showMessage(mess);
	}
#endif
}
void MainWindow::onAction_About() {
	qDebug() << "MainWindow::onAction_About()";

	QMessageBox::about(this, 
		"About viMarkdown", // タイトルバー
		
		"<p><big><b>viMarkdown</b></big> " + VER_STR + "</p>"
		"<p>The efficient visual Markdown editor"
		"<br>Copyright (C) 2025, 2026 by N.Tsuda"
		"<br>Powered by Qt 6 and C++</p>"
	);
}
//----------------------------------------------------------------------
