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
#include <QRegularExpression>
#include <QStatusBar>
#include "ver.h"
#include "MainWindow.h"
#include "DocWidget.h"
#include "MarkdownEditor.h"
#include "MarkdownPreview.h"
#include "SettingsDialog.h"
#include "ReplaceDialog.h"

using namespace std;

extern Global g;
extern ViStatus gvi;

uchar blockType(const QTextBlock &block);
void setBlockType(QTextBlock block, uchar type);

QChar g_flag_char[] = {
	u'v',	//	PCF_VISIBLE = 0,	// プレビューに表示される
	u'-',	//	PCF_COMMENTED,		//	コメントアウトされた文字
	u'E',	//	PCF_ESCAPE,			//	エスケープ文字
	u'H',	//	PCF_HEADING,		//	タイトル・見出し行
	u'L',	//	PCF_LIST_MARK,		// "- " などリストマーカー
	u'N',	//	PCF_NUM_LIST,		//	"1. " 連番
	u'Q',	//	PCF_QUOTE,
	u'K',	//	PCF_LINK,
	u'I',	//	PCF_IMAGE,
	u'C',	//	PCF_CODE,			// ```
	u'S',	//	PCF_CSV,
	u'T',	//	PCF_TABLE,			//	マークダウン表要素
	u'K',	//	PCF_KEISEN,
	u'=',	//	PCF_EMPHASIZED,		//	ボールド、イタリック等
};
enum {
	TEST_CHAR_FLAGS = 1,
	TEST_CONTEXT_AT = 2,
	TEST_LINE_CRSP = 4,			//	対応行テキストチェック
	TEST_EtoP_CUR_SYNC = 8,		//	対応行カーソル位置同期チェック
	TEST_PtoE_CUR_SYNC = 16,	//	対応行カーソル位置同期チェック
	TEST_ALL = TEST_CHAR_FLAGS|TEST_LINE_CRSP /*|TEST_EtoP_CUR_SYNC*/ |TEST_PtoE_CUR_SYNC,
};

const QString QA_MD_TEXT_2 =
#if 0
	"```CSV\n"	//	CSVブロック開始
	"abc, xy, z123\n"
	"id, , h3\n"
	"id, h*h*h2, **h3**\n"
	"69, ""hasshi"", h\\*o\\*ge\n"
	",,\n"
	"\"\",\"\",\n"	//	途中のセルが ""
	",,\"\"\n"	//	最後のセルが ""
	"```\n"		//	CSVブロック終了
	"text\n"
	//"```CSV\n"
	//",,,\n"
	//"```\n"
	//"1. item1\n"
	//"1. *italic*\n"
	//"\n"
	//"![v](url)\n"	//	画像
	//"\n"
#endif
#if 0
	"<!-- comment -->\n"
	"# title\n"
	"hoge<!-- -->\n"
	"<!-- -->fuga\n"
	"text\n"
#endif
	//"\\> quote-5\n"
	//"item2**bold**\n"
	//"- item2**bold**\n"
	//"fuga\\hoge\n"
#if 0
	"- item1\n"
	"  hoge fuga\n"
	"  abc xyzzzzy\n"
	"\n"
	//"\\- item3\n"
#endif
#if 0
	"```CSV\n"	//	CSVブロック開始
	"id, hhh2, h3\n"
	"69, ""hasshi"", hoge\n"
	"```\n"		//	CSVブロック終了
	"\n"
	"text\n"
#endif
#if 0
	"|he**ad**er|h|\n"
	"|-|-|\n"
	"|3|1415|\n"
	"|f\\*o\\*o|bar|\n"
	"\n"
	"text\n"
	"\n"
#endif
#if 1
	"TEST\n"
	"<!-- comment -->\n"
	"# title\n"
	"hoge<!-- -->\n"
	"<!-- -->fuga\n"
	"text\n"
	" text\n"
	"text \n"
	"abc \\xyzzz\n"		//	x はエスケープされず \x と表示される
	"hoge*fuga*foo\n"
	"hoge\\*fuga\\*foo\n"
	"hoge**fuga**foo\n"
	"hoge**fuga** **fuga**\n"
	"hoge***fuga***foo\n"
	"hoge~~fuga~~foo\n"
	"h'a\\*bc*xyz*abc'x\n"
	"[v](url)\n"	//	リンク
	"x[v](url)y\n"	//	リンク
	"![v](url)\n"	//	画像
	"x![v](url)y\n"	//	画像
	"x![v](url)y![v](url)z\n"	//	画像が複数
	"x![v](url)![v](url)z\n"	//	画像が連続
#if 1
	"## list\n"
	"text\n"
	"- item1\n"
	"  - item1.2\n"
	"- item2**bold**\n"
	"- item3\n"
	"hoge fuga\n"
	"xyz\n"
	"\n"
	"\\- item3\n"
	"\n"
	"1. item1\n"
	"1. *italic*\n"
	"1. item3\n"
	"1. **bold** *italic* ~~strike~~ \n"
	"1. [link](url) \n"
	"\n"
	"\\1. item\n"
	"\n"
	"- [ ] checkbox\n"
	"- [x] checkbox\n"
	"- [ ] checkbox\n"
	"\n"
	"\\- [ ] checkbox\n"
	"\n"
	"text\n"
	"> quote-1\n"
	"> q*uot*e-2\n"
	"> quote-3\n"
	"> quote-4\n"
	"\n"
	"\\> quote-5\n"
	"## block\n"
	"text\n"
	"```\n"		//	コードブロック開始
	"int main() { return 0; }\n"
	"```\n"		//	コードブロック終了
	"text\n"
	"```keisen\n"	//	罫線ブロック開始
	"┌───┐\n"
	"│      │\n"
	"└───┘\n"
	"```\n"		//	罫線ブロック終了
	"text\n"
	"```SVG\n"	//	SVGブロック開始
	"<svg width='200' height='100'>\n"
	"  <rect x='10' y='10' width='180' height='80' rx='5' fill='green' />\n"
	"  <circle cx='100' cy='50' r='35' fill='yellow' />\n"
	"</svg>\n"
	"```\n"		//	SVGブロック終了
	"text\n"
#if 1
	"## table\n"
	"```CSV\n"	//	CSVブロック開始
	"abc, xy, z123\n"
	"id, , h3\n"
	"id, h*h*h2, **h3**\n"
	"69, ""hasshi"", h\\*o\\*ge\n"
	",,\n"
	"\"\",\"\",\n"	//	途中のセルが ""
	",,\"\"\n"	//	最後のセルが ""
	"```\n"		//	CSVブロック終了
	"text\n"
#endif
#if 1
	"|he**ad**er|h|\n"
	"|-|-|\n"
	"|3|1415|\n"
	"| a |xyz|\n"
	"|a|\\[xyz\\]|\n"
	"|f\\*o\\*o|bar|\n"
#endif
	"text\n"
#endif
#endif
	"\n"
	"";
//const short CODE_IMAGE = 0xfffc;		//	プレビュー：画像アイコン
int g_tested_count = 0;
int g_failed_count = 0;
QString g_result;
bool MainWindow::ASSERT(bool actual, int ln) {
	++g_tested_count;
	if( actual ) return true;
	++g_failed_count;
	do_output(QString("%1: true expected. but false\n").arg(ln+1));
	return false;
}
//	undone: テンプレート関数化
bool MainWindow::ASSERT_EQ(bool expected, bool actual, int ln) {
	++g_tested_count;
	if( actual == expected ) return true;
	++g_failed_count;
	return false;
}
bool MainWindow::ASSERT_EQ(int expected, int actual, int ln) {
	++g_tested_count;
	if( actual == expected ) return true;
	++g_failed_count;
	do_output(QString("%1: %2 expected. but %3\n").arg(ln+1).arg(expected).arg(actual));
	return false;
}
bool MainWindow::ASSERT_EQ(int expected, int actual, int ln, QChar ch, int ix, int type) {
	++g_tested_count;
	if( actual == expected ) return true;
	++g_failed_count;
	if( type == TEST_EtoP_CUR_SYNC )
		do_output(QString("%1: E [%2] '%3' -> P ix: %4 expected. but %5\n").arg(ln+1).arg(ix).arg(ch).arg(expected).arg(actual));
	else
		do_output(QString("%1: P [%2] '%3' -> E ix: %4 expected. but %5\n").arg(ln+1).arg(ix).arg(ch).arg(expected).arg(actual));
	return false;
}
bool MainWindow::ASSERT_EQ(const QChar expected, const QChar actual, int ln) {
	++g_tested_count;
	if( actual == expected ) return true;
	++g_failed_count;
	do_output(QString("%1: '%2' expected. but '%3'\n").arg(ln+1).arg(expected).arg(actual));
	return false;
}
bool MainWindow::ASSERT_EQ(const QString &expected, const QString &actual, int ln) {
	++g_tested_count;
	if( actual == expected ) return true;
	++g_failed_count;
	do_output(QString("%1: '%2' expected. but '%3'\n").arg(ln+1).arg(expected).arg(actual));
	return false;
}
bool isCommentOuted(const BlockData* data) {
	if( data->m_charFlags.isEmpty() ) return false;			//	空行
	for(int i = 0; i < data->m_charFlags.size(); ++i) {
		if( data->m_charFlags[i] != PCF_COMMENTED ) return false;		//	表示文字があった
	}
	return true;
}
//enum { PATH_1 = 1, PATH_2, PATH_3, };
void MainWindow::onAction_Test() {
}
void MainWindow::onAction_TestCharFlags() {
	do_test(TEST_CHAR_FLAGS);
}
void MainWindow::onAction_TestContextAt() {
	do_test(TEST_CONTEXT_AT);
}
void MainWindow::onAction_TestLineCrsp() {
	do_test(TEST_LINE_CRSP);
}
void MainWindow::onAction_TestEtoPCurSync() {
	do_test(TEST_EtoP_CUR_SYNC);
}
void MainWindow::onAction_TestAll() {
	do_test(TEST_ALL);
}

void MainWindow::do_test(int type) {
	addTab(QString("QA-%1").arg(++m_QA_tab_number));
	DocWidget *docWidget = getCurDocWidget();;
	if( docWidget == nullptr ) return;
	int total_tested = 0;
	int total_failed = 0;
	do_output("\n# Test Result:\n\n");
	if( (type & TEST_CHAR_FLAGS) != 0 ) {
		g_tested_count = 0;
		g_failed_count = 0;
		test_charFlags(docWidget);			//	m_charFlags[] テスト
		do_output(QString("\nTest char flags: %1 failed / %2 tested.\n\n").arg(g_failed_count).arg(g_tested_count));
		total_tested += g_tested_count;
		total_failed += g_failed_count;
	}
	if( (type & TEST_CONTEXT_AT) != 0 ) {
		g_tested_count = 0;
		g_failed_count = 0;
		test_contextAt(docWidget);			//	MarkdownEditor::contextAt テスト
		do_output(QString("\nTest MarkdownEditor::contextAt: %1 failed / %2 tested.\n\n").arg(g_failed_count).arg(g_tested_count));
		total_tested += g_tested_count;
		total_failed += g_failed_count;
	}
	if( (type & TEST_LINE_CRSP) != 0 ) {
		g_tested_count = 0;
		g_failed_count = 0;
		docWidget->m_editor->setPlainText(QA_MD_TEXT_2);
		do_test(docWidget, TEST_LINE_CRSP);
		do_output(QString("\nTest Line Corresponding: %1 failed / %2 tested.\n\n").arg(g_failed_count).arg(g_tested_count));
		total_tested += g_tested_count;
		total_failed += g_failed_count;
	}
	if( (type & TEST_EtoP_CUR_SYNC) != 0 ) {
		g_tested_count = 0;
		g_failed_count = 0;
		do_test(docWidget, TEST_EtoP_CUR_SYNC);		//	EtoP 行内表示文字一致テスト
		//g_tested_count -= n_testted;	//	重複数分
		//g_failed_count -= n_failed;
		do_output(QString("\nTest EtoP CurSync: %1 failed / %2 tested.\n\n").arg(g_failed_count).arg(g_tested_count));
		total_tested += g_tested_count;
		total_failed += g_failed_count;
	}
	if( (type & TEST_PtoE_CUR_SYNC) != 0 ) {
		docWidget->m_preview->setFocus();
		g_tested_count = 0;
		g_failed_count = 0;
		do_test(docWidget, TEST_PtoE_CUR_SYNC);		//	PtoE 行内表示文字一致テスト
		//g_tested_count -= n_testted;	//	重複数分
		//g_failed_count -= n_failed;
		do_output(QString("\nTest PtoE CurSync: %1 failed / %2 tested.\n\n").arg(g_failed_count).arg(g_tested_count));
		total_tested += g_tested_count;
		total_failed += g_failed_count;
	}
	//do_test(docWidget, PATH_3);		//	PtoE 行内表示文字一致テスト
	//QString mess = QString("Total: %1 failed / %2 tested. (Fail:%3%)")
	//				.arg(total_failed).arg(total_tested).arg(total_failed*100.0/total_tested, 0, 'f', 1);
	QString mess = QString("\nTotal: %1 failed / %2 tested. (Success:%3%)")
					.arg(total_failed).arg(total_tested).arg(100.0 - total_failed*100.0/total_tested, 0, 'f', 1);
	statusBar()->showMessage(mess);
	//g_result += mess;
	do_output(mess);
#if 0
	qDebug() << "test result:\n" << g_result;
	QTextCursor cursor = docWidget->m_editor->textCursor();
	cursor.movePosition(QTextCursor::End);
	cursor.insertText(g_result);
#endif
}
void MainWindow::do_test(DocWidget *docWidget, int type) {
	QTextBlock block1 = docWidget->m_editor->document()->firstBlock();
	QTextBlock block2 = docWidget->m_preview->document()->firstBlock();
	//bool inTable = false;
	QStringList listStrings;
	int prevLen = 0;
	while( block1.isValid() && block2.isValid() ) {
		QCoreApplication::processEvents();		//	溜まっているイベント処理
#if 0
		while( blockType(block2) == BT_TABLE ) {		//	テーブル前後ダミーブロックはスキップ
			block2 = block2.next();
		}
		if (!block2.isValid()) break;
#endif
		int n1 = block1.blockNumber();
		int n2 = block2.blockNumber();
		QString t1 = block1.text();
		QString t2 = block2.text();
		assert( block2.isValid() );
		if( isCommentOuted(getBlockData(block1)) ) {		//	（非空行で、）１行全てコメントアウトされている場合はスキップ
			block1 = block1.next();
			continue;
		}
		std::vector<char> tableAlign;
		if( isTableHyphenLine(block1.text(), tableAlign) ) {		//	GFM表のハイフン行はスキップ
			block1 = block1.next();
			continue;
		}
		//if( blockType(block1) == BT_TABLE && blockType(block2) != BT_TABLE )		//	block2 には何故かダミー？がある
		//	block2 = block2.next();
		ASSERT(block2.isValid(), block1.blockNumber());
		while( (blockType(block1) == BT_CODE_BLOCK || blockType(block1) == BT_CODE_BLOCK_END) &&
			block1.text().startsWith("```") )
		{
			block1 = block1.next();
		}
		if( blockType(block1) == BT_KEISEN_BEGIN || blockType(block1) == BT_KEISEN_BLOCK ) {
			while( blockType(block1) == BT_KEISEN_BEGIN || blockType(block1) == BT_KEISEN_BLOCK )
				block1 = block1.next();
			block2 = block2.next();
			continue;
		}
		while( /*blockType(block1) == BT_KEISEN_BLOCK &&*/ block1.text().startsWith("```") ) {
			block1 = block1.next();
		}
#if 0
		if( blockType(block1) == BT_TABLE) {
			if( !inTable ) {
				block2 = block2.next();		//	GFMテーブル最初はテーブル全体のためのブロックなのでスキップ
			}
			inTable = true;
		} else {
			if( inTable ) {
				block2 = block2.next();		//	GFMテーブル最後もスキップ
			}
			inTable = false;
		}
#endif
		while( blockType(block2) == BT_TABLE ) {		//	テーブル前後ダミーブロックはスキップ
			block2 = block2.next();
		}
		qDebug() << "block1: " << block1.blockNumber() << ", block2: " << block2.blockNumber();
		qDebug() << "charAt(block2) = " << docWidget->m_preview->document()->characterAt(block2.position());
		QString buf1 = block1.text();
		QString buf2 = block2.text();
		int offset = 0;
		if( listStrings.isEmpty() && blockType(block2) == BT_LIST ) {
			listStrings = buf2.split(QChar(LINE_SEPARATOR));
			//if( listStrings.size() > 1 )	//	継続行がある場合
			//	offset = strlen("- ");
			prevLen = 0;
		}
		if( !listStrings.isEmpty() )
			buf2 = listStrings.front();
		int lineStartPos = block2.position();		//	行先頭位置
		QTextTable *table = QTextCursor(block2).currentTable();
		if( table != nullptr ) {	//	CSV・GFM テーブル中の場合
			for(int i = 0; i < table->columns() - 1; ++i) {
				block2 = block2.next();
				assert( block2.isValid() );
				buf2 += " " + block2.text().trimmed();		//	セル文字列は半角空白j区切り
			}
			//block2 = block2.next();
			assert( block2.isValid() );
			//inTable = true;
		} else {
			//inTable = false;
		}
		BlockData *data = getBlockData(block1);
		int k = 0;
		for(int i = 0; i < data->m_charFlags.size(); ++i) {		//	buf1: 非表示部分を削除
			if( data->m_charFlags[i] == PCF_VISIBLE ) {
				buf1[k++] = buf1[i];
			} else if(data->m_charFlags[i] == PCF_CELL_SEPARATOR) {
				buf1[k++] = u' ';
			}
		}
		buf1.resize(k);
		buf2.remove(QChar(CODE_IMAGE));
		static QRegularExpression allspc("^ +$");
		if( !allspc.match(buf1).hasMatch() )	//	空白以外を含んでいる
			buf1 = buf1.trimmed();
		ASSERT_EQ( buf1, buf2, block1.blockNumber());
		//if( ASSERT_EQ( buf1.trimmed(), buf2, block1.blockNumber()) )	//	表示テキストが一致した場合
		{
			if( type == TEST_EtoP_CUR_SYNC ) {
				//	エディタ → プレビュー カーソル同期テスト
				//		![v](url) の場合、m_charFlags[] = {I, I, I, ... I}
				//		column: 0 -> 0, 1 ～ 9 -> 1 （画像があるため１ずれる）
				QTextDocument *document = docWidget->m_editor->document();
				QTextCursor cursor(block1);
				const BlockData *data = getBlockData(block1);
				int nvcnt = 0;	//	非表示文字数
				for(int i = 0; i < block1.text().size(); ++i) {
					int k1 = i - nvcnt;			//	k1: 期待されるカーソル位置
					if( i != 0 && data->m_charFlags[i-1] == PCF_IMAGE && data->m_charFlags[i] == PCF_IMAGE )
						++k1;
					if( data->m_charFlags[i] != PCF_VISIBLE ) ++nvcnt;
					docWidget->m_editor->setTextCursor(cursor);
					QTextCursor cur2 = docWidget->m_preview->textCursor();		//	プレビューカーソル
					int k2 = cur2.position() - cur2.block().position();			//	k2: プレビューカーソルカラム
					QChar ch = document->characterAt(cursor.position());
					if( !ASSERT_EQ( k2, k1, block1.blockNumber(), ch, i, TEST_EtoP_CUR_SYNC ) ) {
						qDebug() << "ch = " << ch;
					}
					cursor.movePosition(QTextCursor::Right);
				}
			}
			if( type == TEST_PtoE_CUR_SYNC ) {
				//	プレビュー → エディタ カーソル同期テスト
				QTextDocument *document = docWidget->m_preview->document();
				//QTextCursor cur1(block1);		//	エディタカーソル
				QTextCursor cur2(block2);		//	プレビューカーソル
				cur2.setPosition(lineStartPos);
				//QTextCursor cur2 = docWidget->m_preview->textCursor();		//	プレビューカーソル
				const BlockData *data = getBlockData(block1);
				//int nvcnt = 0;	//	非表示文字数
				int k = 0;		//	エディタカーソルインデックス
				for(int i = 0; i <= buf2.size(); ++i) {		//	１行分のテキストについてチェック
					docWidget->m_preview->setTextCursor(cur2);
					QCoreApplication::processEvents();		//	溜まっているイベント処理
					int k0 = k;
					while( k < data->m_charFlags.size() && data->m_charFlags[k] >= PCF_NOT_VISIBLE &&
						data->m_charFlags[k] != PCF_CELL_SEPARATOR )	//	次の表示文字を探す
					{
						++k;
					}
					QChar ch1 = k < block1.text().size() ? block1.text()[k] : u'\n';
					QTextCursor cur1 = docWidget->m_editor->textCursor();
					int k1 = cur1.position() - cur1.block().position();		//	実際のエディタカーソルインデックス
					QChar ch = document->characterAt(cur2.position());
					//	"** " の様な場合は、カーソルは "** " 先頭位置（k0）を期待
					if( !ASSERT_EQ( (ch1 == u' ' || ch1 == u'\n' ? k0 : k)+offset-prevLen, k1, block1.blockNumber() , ch, i, TEST_PtoE_CUR_SYNC) ) {
						qDebug() << "ch1 = " << ch1;
					}
					cur2.movePosition(QTextCursor::Right);		//	カーソル右移動
					++k;
				}
			}
		}
		block1 = block1.next();
		if( !listStrings.isEmpty() ) {
			prevLen += listStrings.front().size() + 1;
			listStrings.pop_front();
		}
		if( listStrings.isEmpty() ) {
			prevLen = 0;
			block2 = block2.next();
		}
	}
	ASSERT( !block2.isValid(), block1.blockNumber());	//	同行数のはず
}
const QStringList QA_TEXT_FLAGS = {
	"text",
	"vvvv",
	" text",
	"-vvvv",
	"text ",
	"vvvv-",
	"<!--text-->",
	"-----------",
	"xyz<!--text-->",
	"vvv-----------",
	"<!--text-->xyz",
	"-----------vvv",
	"![i](png)",
	"IIIIIIIII",
	"x![i](png)x",
	"vIIIIIIIIIv",
	"[i](url)",
	"KvKKKKKK",
	"x[i](url)x",
	"vKvKKKKKKv",
	"text",
	"vvvv",
	"*i*",
	"=v=",
	"\\*i\\*",
	"=vv=v",
	"**b**",
	"==v==",
	"*i* **b** ***bi*** ~~s~~",
	"=v= ==v== ===vv=== ==v==",
	"h`a\\*bc*xyz*abc`x",
	"v=vvvvvvvvvvvvv=v",
	"",
	"",
	"1. hoge",
	"NNNvvvv",
	"1. h*og*e",
	"NNNv=vv=v",
	"1. h**og**e",
	"NNNv==vv==v",
	"",
	"",
	"- text",
	"LLvvvv",
	"- t*ex*t",
	"LLv=vv=v",
	"- t**ex**t",
	"LLv==vv==v",
	"",
	"",
	"- [ ] text",
	"LLLLLLvvvv",
	"- [x] t*ex*t",
	"LLLLLLv=vv=v",
	"- [X] t**ex**t",
	"LLLLLLv==vv==v",
	"",
	"",
	"```",
	"CCC",
	"abcdef",
	"vvvvvv",
	"a*bcde*f",
	"vvvvvvvv",
	"```",
	"CCC",
	"",
	"",
	"```keisen",
	"KKKKKKKKK",
	"───",
	"vvv",
	"a*bcde*f",
	"vvvvvvvv",
	"───",
	"vvv",
	"```",
	"KKK",
	"",
	"",
#if 1
	"```CSV",
	"SSSSSS",
	"h1,  h22,  h3*33",
	"vvSSSvvvSSSvvvvv",
	"ID,  9,  h*o*ge",
	"vvSSSvSSSv=v=vv",
	"ID,  9,  h\\*o\\*ge",
	"vvSSSvSSSv-vv-vvv",
	",,",
	"SS",
	"```",
	"SSS",
	"",
	"",
#endif
	"hdr|hdr2",
	"vvvTvvvv",
	"---|---:",
	"TTTTTTTT",
	"hac*k|69",
	"vvvvvTvv",
	"h*a*ck|69",
	"v=v=vvTvv",
	"69|h*a*ck",
	"vvTv=v=vv",
	"69|h\\*a\\*ck",
	"vvTv-vv-vvv",
	" a |xyz",
	"-v--vvv",
	"",
	"",
};
#if 0
const QString QA_MD_TEXT_1 =
	"text\n"
	"*i*\n"
	"**b**\n"
	"*i* **b** ***bi*** ~~s~~\n"
	"\n"
	"1. hoge\n"
	"1. h*og*e\n"
	"\n";
const QStringList QA_MD_FLAGS = {
	"vvvv",
	"=v=",
	"==v==",
	"=v= ==v== ===vv=== ==v==",
	"",
	"LLLvvvv",
	"LLLv=vv=v",
	"",
};
#endif
void MainWindow::test_charFlags(DocWidget *docWidget) {
	QString buf;
	for(int i = 0; i < QA_TEXT_FLAGS.size(); i += 2 )
		buf += QA_TEXT_FLAGS[i] + "\n";
	docWidget->m_editor->setPlainText(buf);
	QTextBlock block1 = docWidget->m_editor->document()->firstBlock();
	QTextBlock block2 = docWidget->m_preview->document()->firstBlock();
	//for(auto flags: QA_MD_FLAGS) {
	for(int i = 1; i < QA_TEXT_FLAGS.size(); i += 2 ) {
		QString buf1 = block1.text();
		QString flags = QA_TEXT_FLAGS[i];
		BlockData *data = getBlockData(block1);
		ASSERT_EQ( (int)flags.size(), (int)data->m_charFlags.size(), block1.blockNumber());
		for(int i = 0; i < flags.size(); ++i) {
			++g_tested_count;
			if( (flags[i] == u'v' || flags[i] == u' ') && data->m_charFlags[i] != PCF_VISIBLE ||
				!(flags[i] == u'v' || flags[i] == u' ') && data->m_charFlags[i] == PCF_VISIBLE )
			{
				g_result += QString("%1: flags[%2] is NOT correct.\n").arg(block1.blockNumber()+1).arg(i);
				QString f = "m_charFlags[] = {";
				int len = f.size();
				for(int k = 0; k < flags.size(); ++k) {
					f += g_flag_char[data->m_charFlags[k]];
				}
				g_result += f + "}\n";
				g_result += QString(len + i, QChar(0x00a0)) + "^\n";
				++g_failed_count;
			}
		}
		block1 = block1.next();
	}
}
const QStringList QA_CONTEXT_AT = {
	"# title",
	"text",
	"",
};
struct TestCaseContextAt {
	int		m_position;
	QChar	m_anchorChar;
	int		m_offset;
	int		m_nth;
} g_testCaseContextAt[] = {
	{0, u't', 0, 1},	//	[#] title
	{1, u't', 0, 1},
	{2, u't', 0, 1},	//	# [t]itle
	{3, u'i', 0, 1},
	{4, u't', 0, 2},	//	# title		２つめの t
	{5, u'l', 0, 1},	//	# title
	{6, u'e', 0, 1},	//	# title
	{7, ETX, 0, 1},		//	# title
};
void MainWindow::test_contextAt(DocWidget *docWidget) {
	QString buf;
	for(int i = 0; i < QA_CONTEXT_AT.size(); ++i)
		buf += QA_CONTEXT_AT[i] + "\n";
	docWidget->m_editor->setPlainText(buf);
	QTextCursor cursor = docWidget->m_editor->textCursor();
	for(auto tc: g_testCaseContextAt) {
		cursor.setPosition(tc.m_position);
		PosContext pc = docWidget->m_editor->contextAt(tc.m_position);
		ASSERT_EQ( tc.m_anchorChar, pc.m_anchorChar, cursor.block().blockNumber() );
	}
}
void MainWindow::onAction_DumpCharFlags() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	QTextBlock block = docWidget->m_editor->document()->firstBlock();
	QString txt = "\n# Dump charFlags[]\n\n```\n";
	while( block.isValid() ) {
		//qDebug() << block.blockNumber() << ": " << blockType(block) << ", " << block.text();
		//printCharFlags(block);
		txt += QString::number((int)block.blockNumber()) + ": '" + block.text() + "' ";
		const BlockData* data = getBlockData(block);
		for(int i = 0; i < data->m_charFlags.size(); ++i)
			txt += QString::number((int)data->m_charFlags[i]) + u' ';
		txt += "\n";
		block = block.next();
	}
	txt += "```\n";
	QTextCursor cursor = docWidget->m_editor->textCursor();
	cursor.movePosition(QTextCursor::End);
	cursor.insertText(txt);
	docWidget->m_editor->setTextCursor(cursor);
}
void MainWindow::onAction_DumpBlockUserStates() {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	QString txt = "\n## userStates of Editor Blocks\n\n```\n";
	QTextBlock block = docWidget->m_editor->document()->firstBlock();
	while( block.isValid() ) {
		//qDebug() << block.blockNumber() << ": " << blockType(block) << ", " << block.text();
		//txt += QString::number((int)block.blockNumber()) + ": '" + blockType(block);
		//txt += QString("%1: %2 '%3'\n").arg(block.blockNumber()+1).arg(blockType(block)).arg(block.text());
		txt += QString("%1: 0x%2 '%3'\n").arg(block.blockNumber()+1).arg((unsigned int)block.userState(), 4, 16, QChar('0')).arg(block.text());
		block = block.next();
	}
	txt += "```\n";
	txt += "\n## userStates of Preview Blocks\n\n```\n";
	block = docWidget->m_preview->document()->firstBlock();
	while( block.isValid() ) {
		//qDebug() << block.blockNumber() << ": " << blockType(block) << ", " << block.text();
		//txt += QString::number((int)block.blockNumber()) + ": '" + blockType(block);
		//txt += QString("%1: %2 '%3'\n").arg(block.blockNumber()+1).arg(blockType(block)).arg(block.text());
		txt += QString("%1: 0x%2 '%3'\n").arg(block.blockNumber()+1).arg((unsigned int)block.userState(), 4, 16, QChar('0')).arg(block.text());
		block = block.next();
	}
	txt += "```\n";

	QTextCursor cursor = docWidget->m_editor->textCursor();
	cursor.movePosition(QTextCursor::End);
	cursor.insertText(txt);
	docWidget->m_editor->setTextCursor(cursor);
}
static QString g_script_1 = R"(
	TYPE "hoge"
	#CRLF
	#TYPE "xyzzzz"
	#CRLF
	#UP
)";
void MainWindow::onAction_RunPTS() {
	run_previewTestScript(g_script_1);
}
void MainWindow::run_previewTestScript(const QString &script) {
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	docWidget->m_preview->setFocus();
	QStringList lst = script.split('\n', Qt::SkipEmptyParts);
	for (const QString& line : lst) {
		QCoreApplication::processEvents();		//	溜まっているイベント処理
		QTextCursor cursor = docWidget->m_preview->textCursor();
		int pos = cursor.position();
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty() || trimmed.startsWith("//") || trimmed.startsWith("#")) continue; // 空行やコメントをスキップ

        // コマンドと引数に分割 (最初のスペースで分ける)
        int ix = trimmed.indexOf(' ');
        QString cmd = trimmed.left(ix).toUpper();
        QString arg = (ix != -1) ? trimmed.mid(ix + 1) : "";

        // 引数のダブルクォーテーションを外す (もしあれば)
        if (arg.startsWith('"') && arg.endsWith('"')) {
            arg = arg.mid(1, arg.length() - 2);
        }
        if( cmd == "TYPE" ) {
        	if( !arg.isEmpty() ) cursor.insertText(arg);
        } else if( cmd == "CRLF" ) {
        	cursor.insertText("\n");
        } else if( cmd == "UP" ) {
        	cursor.movePosition(QTextCursor::Up);
        }
		docWidget->m_preview->setTextCursor(cursor);
	}
}
//----------------------------------------------------------------------
// viコマンド自動テスト用のテストケース構造体
struct ViTestCase {
    QString m_name;			// テスト名
    QString m_initialText;	// 初期テキスト（'|' がカーソル位置）
    QStringList m_steps;	//	{viコマンド列, コマンド実行後テキスト（'|' がカーソル位置）}...
};

const QList<ViTestCase> viTestCases = {
	//{ "Move cursor right",	"h@ello\n", {"l", "he@llo\n", "l", "hel@lo\n", "l", "hell@o\n", } },
	//{ "Move cursor left",	"h@ello\n", {"h", "@hello\n", "h", "@hello\n", } },
#if 0
    { "Basic ex command",
        "li┃ne1\nline2\nline3\n",
        {
            ":2", "line1\n┃line2\nline3\n", // 2行目行頭に移動
        }
    },
#endif
#if 0
	// 1. 下移動 (j) の基本動作と最終行での境界制御
    { "Move cursor down (j)",
        "a┃bc\ndef\nghi\n",
        {
            "j", "abc\nd┃ef\nghi\n", // 2行目の同じ列に移動
            "j", "abc\ndef\ng┃hi\n", // 3行目の同じ列に移動
            "j", "abc\ndef\ng┃hi\n"  // 最終行のため下移動しない
        }
    },
    // 2. 上移動 (k) の基本動作と先頭行での境界制御
    { "Move cursor up (k)",
        "abc\ndef\ng┃hi\n",
        {
            "k", "abc\nd┃ef\nghi\n", // 2行目の同じ列に移動
            "k", "a┃bc\ndef\nghi\n", // 1行目の同じ列に移動
            "k", "a┃bc\ndef\nghi\n"  // 先頭行のため上移動しない
        }
    },
    // 3. 短い行を通過する際の「列位置の記憶（カラムメモリ）」の検証
    // ※ 1行目の4文字目（index 3）からスタートし、短い2行目を通過して、3行目で元の4文字目に復帰するかをテストします。
    { "Move vertically through short lines (Column Memory)",
        "lin┃e1\nab\nline3\n",
        {
            "j", "line1\na┃b\nline3\n",  // 2行目が短いため、一時的に2行目の末尾（index 1）に移動
            "j", "line1\nab\nlin┃e3\n",  // 3行目に降りると、元の列位置（index 3）に復帰する
            "k", "line1\na┃b\nline3\n",  // 再び2行目の末尾に制限される
            "k", "lin┃e1\nab\nline3\n"   // 1行目に戻ると、元の列位置（index 3）に完全復帰する
        }
    },
    // 4. h と l が行境界（改行）を越えて回り込まないかの検証
    { "h and l boundaries (No wrapping)",
        "abc\n┃def\nghi\n",
        {
            "h", "abc\n┃def\nghi\n", // 行頭のため、h を押しても前の行の末尾に回り込まない
            "l", "abc\nd┃ef\nghi\n",
            "l", "abc\nde┃f\nghi\n", // 'f' はこの行の実質的な末尾（改行 \n の直前）
            "l", "abc\nde┃f\nghi\n"  // 行末のため、l を押しても次の行の先頭に回り込まない
        }
    },
    // 5. カウント付き右移動 (nl) と行末でのクランプ処理
    { "Move right with count (nl)",
        "h┃ello world\n", // 'e' (index 1) からスタート
        {
            "3l", "hell┃o world\n", // 3文字右へ -> 'o' (index 4)
            "5l", "hello wor┃ld\n", // 5文字右へ -> 'l' (index 9)
            "5l", "hello worl┃d\n"  // 5文字右へ -> 行末の 'd' (index 10) でクランプされて止まる
        }
    },
    // 6. カウント付き左移動 (nh) と行頭でのクランプ処理
    { "Move left with count (nh)",
        "hello wor┃ld\n", // 'l' (index 9) からスタート
        {
            "4h", "hello┃ world\n", // 4文字左へ -> 半角スペース (index 5)
            "10h", "┃hello world\n"  // 10文字左へ -> 行頭の 'h' (index 0) でクランプされて止まる
        }
    },
    // 7. カウント付き下移動 (nj) と最終行でのクランプ処理
    { "Move down with count (nj)",
        "a┃bc\ndef\nghi\njkl\n", // 1行目の 'b' (line 0, index 1) からスタート
        {
            "2j", "abc\ndef\ng┃hi\njkl\n", // 2行下へ -> 3行目の 'h' (line 2, index 1)
            "5j", "abc\ndef\nghi\nj┃kl\n"  // 5行下へ -> 最終行の 'k' (line 3, index 1) でクランプされて止まる
        }
    },
    // 8. カウント付き上移動 (nk) と先頭行でのクランプ処理
    { "Move up with count (nk)",
        "abc\ndef\nghi\nj┃kl\n", // 4行目の 'k' (line 3, index 1) からスタート
        {
            "2k", "abc\nd┃ef\nghi\njkl\n", // 2行上へ -> 2行目の 'e' (line 1, index 1)
            "5k", "a┃bc\ndef\nghi\njkl\n"  // 5行上へ -> 先頭行の 'b' (line 0, index 1) でクランプされて止まる
        }
    },
    { "Move forward word (w) - Basic spacing",
        "┃abc def ghi",
        {
            "w", "abc ┃def ghi", // 次の単語 "def" の先頭へ移動
            "w", "abc def ┃ghi", // 次の単語 "ghi" の先頭へ移動
            "w", "abc def gh┃i"  // これ以上単語がないため、ファイル末尾の文字 'i' で停止
        }
    },
    { "Move forward word (w) - Across lines and empty lines",
        "┃abc\n  def\n\nghi",
        {
            "w", "abc\n  ┃def\n\nghi", // 改行し、先頭のインデントをスキップして "def" の先頭へ
            "w", "abc\n  def\n\n┃ghi", // 空行を飛び越えて "ghi" の先頭へ
            "w", "abc\n  def\n\ngh┃i"  // ファイル末尾の文字 'i' で停止
        }
    },
    { "Move forward word (w) - Punctuation boundaries",
        "┃abc.def!ghi",
        {
            "w", "abc┃.def!ghi", // 記号 "." の先頭で停止
            "w", "abc.┃def!ghi", // 次の英数字単語 "def" の先頭へ
            "w", "abc.def┃!ghi", // 記号 "!" の先頭で停止
            "w", "abc.def!┃ghi", // 次の英数字単語 "ghi" の先頭へ
            "w", "abc.def!gh┃i"  // ファイル末尾の文字 'i' で停止
        }
    },
    { "Move forward multiple words (num w) - Basic counts",
        "┃abc def ghi jkl mno",
        {
            "2w", "abc def ┃ghi jkl mno", // 2単語分移動して "ghi" の先頭へ
            "2w", "abc def ghi jkl ┃mno", // さらに2単語分移動して "mno" の先頭へ
            "3w", "abc def ghi jkl mn┃o"  // 3単語分移動（足りないためファイル末尾の文字 'o' で停止）
        }
    },
    { "Move forward word (w) - Japanese word classes",
        "┃日本語ひらがなカタカナ。漢字",
        {
            "w", "日本語┃ひらがなカタカナ。漢字", // 漢字「日本語」を飛び越え、ひらがな「ひ」の先頭へ
            "w", "日本語ひらがな┃カタカナ。漢字", // ひらがな「ひらがな」を飛び越え、カタカナ「カ」の先頭へ
            "w", "日本語ひらがなカタカナ┃。漢字", // カタカナ「カタカナ」を飛び越え、記号「。」の先頭へ
            "w", "日本語ひらがなカタカナ。┃漢字", // 記号「。」を飛び越え、次の漢字「漢」の先頭へ
            "w", "日本語ひらがなカタカナ。漢┃字"  // 次の単語がないため、ファイル末尾の文字 '字' で停止
        }
    },
    { "Move forward word (w) - Japanese mixed with alphanumeric and spaces",
        "┃日本語 abc カタカナ 123。ひらがな",
        {
            "w", "日本語 ┃abc カタカナ 123。ひらがな", // スペースをスキップし、半角英字 "abc" の先頭へ
            "w", "日本語 abc ┃カタカナ 123。ひらがな", // スペースをスキップし、カタカナ "カタカナ" の先頭へ
            "w", "日本語 abc カタカナ ┃123。ひらがな", // スペースをスキップし、半角数字 "123" の先頭へ
            "w", "日本語 abc カタカナ 123┃。ひらがな", // スペース無しでの文字種変更を検知し、記号 "。" の先頭へ
            "w", "日本語 abc カタカナ 123。┃ひらがな", // スペース無しでの文字種変更を検知し、ひらがな "ひらがな" の先頭へ
            "w", "日本語 abc カタカナ 123。ひらが┃な"  // 次の単語がないため、末尾の文字 'な' で停止
        }
    },
    { "Delete character under cursor (x) - Basic",
        "a┃bc\n",
        {
            "x", "a┃c\n", // 'b' を削除、カーソルは右隣の文字 'c' へ移動
            "x", "┃a\n"   // 'c' を削除。行末の文字だったため、カーソルは左の 'a' に後退（\nには乗らない）
        }
    },
    { "Delete multiple characters (num x) - With count",
        "┃abcdef\n",
        {
            "3x", "┃def\n", // 3文字（a, b, c）を削除、カーソルは 'd' へ
            "5x", "┃\n"     // 残り3文字に対して5文字削除を指定。行を越えて削除せず、空行（┃\n）に
        }
    },
    { "Anchor test",
        "┃abcdef\n",
        {
            "vl", "┣a┃b┫def\n",	// a, b 選択、カーソルは b 位置
            "x", "┃cdef\n"     		// 被選択 a, b が削除される
        }
    },
    { "Delete character before cursor (X) - Basic",
        "abc┃d\n",
        {
            "X", "ab┃d\n", // 'c' を削除、カーソルはそのまま 'd' を維持
            "X", "a┃d\n",  // 'b' を削除、カーソルはそのまま 'd' を維持
            "X", "┃d\n",   // 'a' を削除。これ以上左に文字がないため、カーソルは 'd'（行頭）へ
            "X", "┃d\n"    // 行頭での実行。削除されず、状態とカーソル位置を維持（0158の検証）
        }
    },
    { "Delete character before cursor (X) - Japanese",
        "あいう┃え\n",
        {
            "X", "あい┃え\n", // 'う' を削除、カーソルはそのまま 'え' を維持
            "X", "あ┃え\n",  // 'い' を削除、カーソルはそのまま 'え' を維持
            "X", "┃え\n",   // 'あ' を削除。行頭になったため、カーソルは 'え'（行頭）へ
            "X", "┃え\n"    // 行頭での実行。日本語でも何も起こらないことを確認
        }
    },
    { "Delete character before cursor (X) - Count",
        "abcde┃f\n",
        {
            "2X", "abc┃f\n", // 'd', 'e' の2文字を削除、カーソルはそのまま 'f' を維持
            "5X", "┃f\n"     // 5文字の削除を試みるが、存在する3文字のみ削除して行頭に留まる
        }
    },
#endif
    { "Ex Range - Absolute Line Number (:num)",
        "line 1\nli┃ne 2\nline 3\nline 4\n",
        {
            ":3", "line 1\nline 2\n┃line 3\nline 4\n", // 3行目の行頭へジャンプ
            ":1", "┃line 1\nline 2\nline 3\nline 4\n"  // 1行目の行頭へジャンプ
        }
    },
#if 0
    { "Ex Range - Current Line (:.)",
        "line 1\nli┃ne 2\nline 3\n",
        {
            ":.", "line 1\n┃line 2\nline 3\n" // カレント行（2行目）の行頭へジャンプ
        }
    },
    { "Ex Range - End of File (:$ and :%)",
        "line 1\nli┃ne 2\nline 3\n",
        {
            ":$", "line 1\nline 2\n┃line 3\n", // 最終行（3行目）へジャンプ
            ":%", "line 1\nline 2\n┃line 3\n"  // 全行。範囲のみ指定時は、最後の行（3行目）へジャンプ
        }
    },
    { "Ex Range - Relative Offsets (:+-)",
        "line 1\nline 2\nli┃ne 3\nline 4\nline 5\n",
        {
            ":+2", "line 1\nline 2\nline 3\nline 4\n┃line 5\n", // '.' 省略の相対移動。現在行(3) + 2 = 5行目へ
            ":-3", "line 1\n┃line 2\nline 3\nline 4\nline 5\n", // 現在行(5) - 3 = 2行目へ
            ":-",  "┃line 1\nline 2\nline 3\nline 4\nline 5\n"  // 数値省略時は -1。現在行(2) - 1 = 1行目へ
        }
    },
    { "Ex Range - Forward Pattern Search (:/pat/)",
        "line 1\nline 2 (TODO)\nli┃ne 3\nline 4 (TODO)\nline 5\n",
        {
            // 3行目以降で、最初にマッチする行（4行目）へジャンプ
            ":/TODO/", "line 1\nline 2 (TODO)\nline 3\n┃line 4 (TODO)\nline 5\n" 
        }
    },
    { "Ex Range - Backward Pattern Search (:?pat?)",
        "line 1\nline 2 (TODO)\nline 3\nli┃ne 4 (TODO)\nline 5\n",
        {
            // 4行目より前で、最初にマッチする行（2行目）へジャンプ
            ":?TODO?", "line 1\n┃line 2 (TODO)\nline 3\nline 4 (TODO)\nline 5\n" 
        }
    },
    { "Ex Range - Semicolon Separator (:;)",
        "line 1\nli┃ne 2\nline 3\nline 4\nline 5\n",
        {
            // セミコロン区切り：'1' で一度カレント行が1行目に更新され、そこから '+2' されて3行目へジャンプ
            ":1;+2", "line 1\nline 2\n┃line 3\nline 4\nline 5\n" 
        }
    },
#endif
#if 0
    { "Ex Delete - Current Line (:d)",
        "line 1\nli┃ne 2\nline 3\n",
        {
            ":d\n", "line 1\n┃line 3\n" // 範囲省略時はカレント行（2行目）を削除。カーソルは次の行の先頭へ
        }
    },
    { "Ex Delete - Specific Line (:1d)",
        "line 1\nli┃ne 2\nline 3\n",
        {
            ":1d\n", "┃line 2\nline 3\n" // 指定した1行目を削除。カーソルは新しい1行目（元2行目）の先頭へ
        }
    },
    { "Ex Delete - Range (:1,2d)",
        "line 1\nline 2\nli┃ne 3\n",
        {
            ":1,2d\n", "┃line 3\n" // 1〜2行目を一括削除。カーソルは残った3行目の先頭へ
        }
    },
    { "Ex Delete - Whole Document (:%d)",
        "line 1\nli┃ne 2\nline 3\n",
        {
            ":%d\n", "┃\n" // バッファ全体のすべての行を削除。Vimの仕様として、中身のない空行が1行だけ残る
        }
    },
    { "Ex Delete - To EOF (:.,$d)",
        "line 1\nli┃ne 2\nline 3\n",
        {
            ":.,$d\n", "┃line 1\n" // カレント行（2行目）から最終行（3行目）まで削除。下に行がないため、カーソルは1行目へ
        }
    },
#endif
};
QString removeCursor(const QString &src, int &pos) {
	QString dst;
	int p = 0;
	for(auto ch: src) {
		if( ch == u'┃' )
			pos = p;
		else {
			dst += ch;
			++p;
		}
	}
	return dst;
}
void MainWindow::onAction_TestViCommands() {
	qDebug() << "MainWindow::onAction_TestViCommands()";
	addTab(QString("QA-%1").arg(++m_QA_tab_number));
	DocWidget *docWidget = getCurDocWidget();;
	if( docWidget == nullptr ) return;
	MarkdownEditor *editor = docWidget->m_editor;
	int total_tested = 0;
	int total_failed = 0;
	do_output("\n");
	do_output("Test vi commands...\n\n");
	int pos;
	for(int i = 0; i < viTestCases.size(); ++i) {
		do_output(viTestCases[i].m_name + ": ");
		QTextCursor cursor = editor->textCursor();
		cursor.movePosition(QTextCursor::Start);
		cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
		cursor.insertText(removeCursor(viTestCases[i].m_initialText, pos));
		cursor.setPosition(pos);
		editor->setTextCursor(cursor);
		const QStringList &steps = viTestCases[i].m_steps;
		for(int k = 0; k < steps.size(); k+=2) {
			++total_tested;
			do_output(".");
			const QString cmd_text = steps[k];
			for(int i = 0; i < cmd_text.size(); ++i) {
				do_viCmd(cmd_text[i], cursor);
				if( gvi.m_cmdlineMode ) {
					if( cmd_text[i] == u':' )
						do_exCmd(cmd_text.mid(i));
					break;
				}
			}
			QCoreApplication::processEvents();		//	溜まっているイベント処理
			int cpos1 = cursor.position();
			QString exp = removeCursor(steps[k+1], pos);
			//cursor = editor->textCursor();
			//int cpos2 = cursor.position();
			if( cursor.position() != pos ) {
				//do_output(QString("wrong cursor position. pos = %1 expected, but %2\n").arg(pos).arg(cursor.position()));
				QString exp = steps[k+1];
				exp.replace('\n', "\\n");
				QString act = cursor.document()->toPlainText();
				act.insert(cursor.position(), u'┃');
				act.replace('\n', "\\n");
				++total_failed;
				do_output(QString("\n[FAILED #%1] wrong cursor position.\nExpected: '%2'\nActual:   '%3'\n").arg(total_failed).arg(exp).arg(act));
			}
			if( cursor.document()->toPlainText() != exp ) {
				QString exp = steps[k+1];
				exp.replace('\n', "\\n");
				QString act = cursor.document()->toPlainText();
				act.insert(cursor.position(), u'┃');
				act.replace('\n', "\\n");
				++total_failed;
				do_output(QString("\n[FAILED #%1] wrong document text.\n").arg(total_failed));
				do_output("Commands: '" + cmd_text + "'\n");
				do_output(QString("Expected: '%1'\nActual:   '%2'\n").arg(exp).arg(act));
				//do_output("expected:\n'" + exp + "', but:\n'" + cursor.document()->toPlainText() + "'\n");
			}
		}
		do_output("\n");
	}
	QString mess = QString("\nTotal: %1 failed / %2 tested. (Success:%3%)")
					.arg(total_failed).arg(total_tested).arg(100.0 - total_failed*100.0/total_tested, 0, 'f', 1);
	statusBar()->showMessage(mess);
	do_output(mess);
}
