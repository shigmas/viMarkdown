#include <QTextBlock>
#include "MainWindow.h"
#include "DocWidget.h"
#include "MarkdownEditor.h"
#include "MarkdownPreview.h"

extern Global g;

int getRepeatCount() {
	if( g.m_repeatCount == 0 ) return 1;
	return g.m_repeatCount;
}
void MainWindow::do_viCmd(QString cmd, QTextCursor& cursor) {
	if( cmd.isEmpty() ) return;
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	bool isEditor = cursor.document() == docWidget->m_editor->document();
	bool completed = true;
	int rcnt = getRepeatCount();
	QTextBlock block = cursor.block();
	switch( cmd[0].unicode() ) {
	case 'a':
		if( cursor.position() < block.position() + block.text().size() )	//	行末でない場合
			cursor.movePosition(QTextCursor::Right);
		//	下にスルー
	case 'i':
		g.m_viCmdMode = false;
		break;
	case 'A':
		cursor.setPosition(block.position() + block.text().size());
		g.m_viCmdMode = false;
		break;
	case 'I':
		cursor.movePosition(QTextCursor::StartOfBlock);
		for(;;) {
			QChar ch = cursor.document()->characterAt(cursor.position());
			if( ch != u' ' && ch != u'\t' ) break;
			cursor.movePosition(QTextCursor::Right);
		}
		g.m_viCmdMode = false;
		break;
	case 'o':
		cursor.setPosition(block.position() + block.text().size());
		cursor.insertText("\n");
		g.m_viCmdMode = false;
		break;
	case 'x':
		while( --rcnt >= 0 && cursor.position() < block.position() + block.text().size() )	//	行末でない場合
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		cursor.deleteChar();
		break;
	case 'X':
		while( --rcnt >= 0 && cursor.position() > block.position() )	//	行頭でない場合
			cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
		if( cursor.hasSelection() )
			cursor.deleteChar();
		break;
	case 'u':
		if( isEditor )
			docWidget->m_editor->undo();
		else
			docWidget->m_preview->undo();
		break;
	case 'U':
		if( isEditor )
			docWidget->m_editor->redo();
		else
			docWidget->m_preview->redo();
		break;
	case 'w':
		for(int i = 0; i < rcnt; ++i) {
			auto pos = cursor.position();
			if( isEditor )
				docWidget->m_editor->moveToNextWord(cursor, /*shift = */false);
			else
				docWidget->m_preview->moveToNextWord(cursor, /*shift = */false);
			if( cursor.position() == pos ) break;
		}
		break;
	case 'b':
		for(int i = 0; i < rcnt; ++i) {
			if( isEditor )
				docWidget->m_editor->moveToPrevWord(cursor, /*shift = */false);
			else
				docWidget->m_preview->moveToPrevWord(cursor, /*shift = */false);
			if( cursor.position() == 0 ) break;
		}
		break;
	case 'k':
		cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, rcnt);
		break;
	case 'j':
		cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, rcnt);
		break;
	case 'h': {
		rcnt = qMin(rcnt, cursor.position() - block.position());	//	行頭対応
		cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, rcnt);
		break;
	}
	case 'l':
	case ' ': {
		int pos = block.position() + block.text().size() - 1;
		if( cursor.position() < pos ) {
			rcnt = qMin(rcnt, pos - cursor.position());
			cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, rcnt);
		}
		break;
	}
	case '$':
		if( !block.text().isEmpty() ) {
			cursor.setPosition(block.position() + block.text().size() - 1);
		}
		break;
	case '\n':
	case '+':
		cursor.movePosition(QTextCursor::Down);
		//	下にスルー
	case '^':
		cursor.movePosition(QTextCursor::StartOfBlock);
		for(;;) {
			QChar ch = cursor.document()->characterAt(cursor.position());
			if( ch != u' ' && ch != u'\t' ) break;
			cursor.movePosition(QTextCursor::Right);	//	空白だけの行で行末まで行っちゃうけど、まあいいか・・・
		}
		break;
	case '0':
		if( g.m_repeatCount == 0 ) {
			cursor.movePosition(QTextCursor::StartOfBlock);
			break;
		}
		//	するすると下にスルーする
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		g.m_repeatCount = g.m_repeatCount * 10 + (cmd[0].unicode() - u'0');
		completed = false;
		break;
	default:
		completed = false;
	}
	if( completed ) {
		g.m_repeatCount = 0;
		if( docWidget != nullptr ) {
			docWidget->m_editor->viewport()->update();
			docWidget->m_preview->viewport()->update();
		}
	}
}
