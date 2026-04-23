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
#include <QRegularExpression>
#include "ver.h"
#include "MainWindow.h"
#include "DocWidget.h"
#include "MarkdownEditor.h"
#include "MarkdownPreview.h"
#include "SettingsDialog.h"
#include "ReplaceDialog.h"

using namespace std;

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
bool ASSERT(bool actual, int ln) {
	++g_tested_count;
	if( actual ) return true;
	++g_failed_count;
	g_result += QString("%1: true expected. but false\n").arg(ln+1);
	return false;
}
//	undone: テンプレート関数化
bool ASSERT_EQ(bool expected, bool actual, int ln) {
	++g_tested_count;
	if( actual == expected ) return true;
	++g_failed_count;
	return false;
}
bool ASSERT_EQ(int expected, int actual, int ln) {
	++g_tested_count;
	if( actual == expected ) return true;
	++g_failed_count;
	g_result += QString("%1: %2 expected. but %3\n").arg(ln+1).arg(expected).arg(actual);
	return false;
}
bool ASSERT_EQ(int expected, int actual, int ln, QChar ch, int ix, int type) {
	++g_tested_count;
	if( actual == expected ) return true;
	++g_failed_count;
	if( type == TEST_EtoP_CUR_SYNC )
		g_result += QString("%1: E [%2] '%3' -> P ix: %4 expected. but %5\n").arg(ln+1).arg(ix).arg(ch).arg(expected).arg(actual);
	else
		g_result += QString("%1: P [%2] '%3' -> E ix: %4 expected. but %5\n").arg(ln+1).arg(ix).arg(ch).arg(expected).arg(actual);
	return false;
}
bool ASSERT_EQ(const QChar expected, const QChar actual, int ln) {
	++g_tested_count;
	if( actual == expected ) return true;
	++g_failed_count;
	g_result += QString("%1: '%2' expected. but '%3'\n").arg(ln+1).arg(expected).arg(actual);
	return false;
}
bool ASSERT_EQ(const QString &expected, const QString &actual, int ln) {
	++g_tested_count;
	if( actual == expected ) return true;
	++g_failed_count;
	g_result += QString("%1: '%2' expected. but '%3'\n").arg(ln+1).arg(expected).arg(actual);
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
	g_result = "\n# Test Result:\n\n";
	if( (type & TEST_CHAR_FLAGS) != 0 ) {
		g_tested_count = 0;
		g_failed_count = 0;
		test_charFlags(docWidget);			//	m_charFlags[] テスト
		g_result += QString("\nTest char flags: %1 failed / %2 tested.\n\n").arg(g_failed_count).arg(g_tested_count);
		total_tested += g_tested_count;
		total_failed += g_failed_count;
	}
	if( (type & TEST_CONTEXT_AT) != 0 ) {
		g_tested_count = 0;
		g_failed_count = 0;
		test_contextAt(docWidget);			//	MarkdownEditor::contextAt テスト
		g_result += QString("\nTest MarkdownEditor::contextAt: %1 failed / %2 tested.\n\n").arg(g_failed_count).arg(g_tested_count);
		total_tested += g_tested_count;
		total_failed += g_failed_count;
	}
	if( (type & TEST_LINE_CRSP) != 0 ) {
		g_tested_count = 0;
		g_failed_count = 0;
		docWidget->m_editor->setPlainText(QA_MD_TEXT_2);
		do_test(docWidget, TEST_LINE_CRSP);
		g_result += QString("\nTest Line Corresponding: %1 failed / %2 tested.\n\n").arg(g_failed_count).arg(g_tested_count);
		total_tested += g_tested_count;
		total_failed += g_failed_count;
	}
	if( (type & TEST_EtoP_CUR_SYNC) != 0 ) {
		g_tested_count = 0;
		g_failed_count = 0;
		do_test(docWidget, TEST_EtoP_CUR_SYNC);		//	EtoP 行内表示文字一致テスト
		//g_tested_count -= n_testted;	//	重複数分
		//g_failed_count -= n_failed;
		g_result += QString("\nTest EtoP CurSync: %1 failed / %2 tested.\n\n").arg(g_failed_count).arg(g_tested_count);
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
		g_result += QString("\nTest PtoE CurSync: %1 failed / %2 tested.\n\n").arg(g_failed_count).arg(g_tested_count);
		total_tested += g_tested_count;
		total_failed += g_failed_count;
	}
	//do_test(docWidget, PATH_3);		//	PtoE 行内表示文字一致テスト
	//QString mess = QString("Total: %1 failed / %2 tested. (Fail:%3%)")
	//				.arg(total_failed).arg(total_tested).arg(total_failed*100.0/total_tested, 0, 'f', 1);
	QString mess = QString("Total: %1 failed / %2 tested. (Success:%3%)")
					.arg(total_failed).arg(total_tested).arg(100.0 - total_failed*100.0/total_tested, 0, 'f', 1);
	statusBar()->showMessage(mess);
	g_result += "\n" + mess;
	qDebug() << "test result:\n" << g_result;
	QTextCursor cursor = docWidget->m_editor->textCursor();
	cursor.movePosition(QTextCursor::End);
	cursor.insertText(g_result);
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
		while( block2.userState() == US_TABLE ) {		//	テーブル前後ダミーブロックはスキップ
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
		//if( block1.userState() == US_TABLE && block2.userState() != US_TABLE )		//	block2 には何故かダミー？がある
		//	block2 = block2.next();
		ASSERT(block2.isValid(), block1.blockNumber());
		while( (block1.userState() == US_CODE_BLOCK || block1.userState() == US_CODE_BLOCK_END) &&
			block1.text().startsWith("```") )
		{
			block1 = block1.next();
		}
		if( block1.userState() == US_KEISEN_BEGIN || block1.userState() == US_KEISEN_BLOCK ) {
			while( block1.userState() == US_KEISEN_BEGIN || block1.userState() == US_KEISEN_BLOCK )
				block1 = block1.next();
			block2 = block2.next();
			continue;
		}
		while( /*block1.userState() == US_KEISEN_BLOCK &&*/ block1.text().startsWith("```") ) {
			block1 = block1.next();
		}
#if 0
		if( block1.userState() == US_TABLE) {
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
		while( block2.userState() == US_TABLE ) {		//	テーブル前後ダミーブロックはスキップ
			block2 = block2.next();
		}
		qDebug() << "block1: " << block1.blockNumber() << ", block2: " << block2.blockNumber();
		qDebug() << "charAt(block2) = " << docWidget->m_preview->document()->characterAt(block2.position());
		QString buf1 = block1.text();
		QString buf2 = block2.text();
		int offset = 0;
		if( listStrings.isEmpty() && block2.userState() == US_LIST ) {
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
		//qDebug() << block.blockNumber() << ": " << block.userState() << ", " << block.text();
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
		//qDebug() << block.blockNumber() << ": " << block.userState() << ", " << block.text();
		//txt += QString::number((int)block.blockNumber()) + ": '" + block.userState();
		txt += QString("%1: %2 '%3'\n").arg(block.blockNumber()+1).arg(block.userState()).arg(block.text());
		block = block.next();
	}
	txt += "```\n";
	txt += "\n## userStates of Preview Blocks\n\n```\n";
	block = docWidget->m_preview->document()->firstBlock();
	while( block.isValid() ) {
		//qDebug() << block.blockNumber() << ": " << block.userState() << ", " << block.text();
		//txt += QString::number((int)block.blockNumber()) + ": '" + block.userState();
		txt += QString("%1: %2 '%3'\n").arg(block.blockNumber()+1).arg(block.userState()).arg(block.text());
		block = block.next();
	}
	txt += "```\n";

	QTextCursor cursor = docWidget->m_editor->textCursor();
	cursor.movePosition(QTextCursor::End);
	cursor.insertText(txt);
	docWidget->m_editor->setTextCursor(cursor);
}
