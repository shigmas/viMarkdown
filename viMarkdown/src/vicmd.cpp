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
void do_cdy(QTextCursor& cursor) {
	if( g.m_cdy == 'd' ) {
		if( cursor.hasSelection() )
			cursor.deleteChar();
	} else if( g.m_cdy == 'c' ) {
		if( cursor.hasSelection() )
			cursor.deleteChar();
		g.m_viCmdMode = false;
	}
}
void MainWindow::do_viCmd(QString cmd, QTextCursor& cursor) {
	if( cmd.isEmpty() ) return;
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	bool isEditor = cursor.document() == docWidget->m_editor->document();
	bool completed = true;
	int rcnt = getRepeatCount();
	auto moveMode = g.m_cdy == ' ' ? QTextCursor::MoveAnchor : QTextCursor::KeepAnchor;
	QTextBlock block = cursor.block();
	g.m_pendingCommand += cmd[0];
	switch( cmd[0].unicode() ) {
	case 'c':
		if( g.m_cdy == 'c' ) {
			cursor.beginEditBlock();
			g.m_editBlockOpen = true;
			cursor.movePosition(QTextCursor::StartOfBlock);
			cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, rcnt);
			cursor.deleteChar();
			g.m_viCmdMode = false;
		} else if( g.m_cdy == ' ' ) {
			g.m_cdy = 'c';
			completed = false;
		}
		break;
	case 'd':
		if( g.m_cdy == 'd' ) {
			cursor.movePosition(QTextCursor::StartOfBlock);
			cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, rcnt);
			cursor.deleteChar();
		} else if( g.m_cdy == ' ' ) {
			g.m_cdy = 'd';
			completed = false;
		}
		break;
	case 's':
		cursor.beginEditBlock();
		g.m_editBlockOpen = true;
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		if( cursor.hasSelection() ) cursor.deleteChar();
		g.m_viCmdMode = false;
		break;
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
		cursor.beginEditBlock();
		g.m_editBlockOpen = true;
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
				docWidget->m_editor->moveToNextWord(cursor, /*select = */g.m_cdy != ' ');
			else
				docWidget->m_preview->moveToNextWord(cursor, /*select = */g.m_cdy != ' ');
			if( cursor.position() == pos ) break;
		}
		do_cdy(cursor);
		break;
	case 'b':
		for(int i = 0; i < rcnt; ++i) {
			if( isEditor )
				docWidget->m_editor->moveToPrevWord(cursor, /*select = */g.m_cdy != ' ');
			else
				docWidget->m_preview->moveToPrevWord(cursor, /*select = */g.m_cdy != ' ');
			if( cursor.position() == 0 ) break;
		}
		do_cdy(cursor);
		break;
	case 'k':
		cursor.movePosition(QTextCursor::Up, moveMode, rcnt);
		do_cdy(cursor);
		break;
	case 'j':
		cursor.movePosition(QTextCursor::Down, moveMode, rcnt);
		do_cdy(cursor);
		break;
	case 'h': {
		rcnt = qMin(rcnt, cursor.position() - block.position());	//	行頭対応
		cursor.movePosition(QTextCursor::Left, moveMode, rcnt);
		do_cdy(cursor);
		break;
	}
	case 'l':
	case ' ': {
		int pos = block.position() + block.text().size() - 1;
		if( cursor.position() < pos ) {
			rcnt = qMin(rcnt, pos - cursor.position());
			cursor.movePosition(QTextCursor::Right, moveMode, rcnt);
		}
		do_cdy(cursor);
		break;
	}
	case '$':
		if( !block.text().isEmpty() ) {
			cursor.setPosition(block.position() + block.text().size() - 1, moveMode);
		}
		do_cdy(cursor);
		break;
	case '-':
		cursor.movePosition(QTextCursor::PreviousBlock, moveMode, rcnt);
		goto hat;
	case '\n':
	case '+':
		cursor.movePosition(QTextCursor::NextBlock, moveMode, rcnt);
		goto hat;
	case '^':
hat:
		cursor.movePosition(QTextCursor::StartOfBlock);
		for(;;) {
			QChar ch = cursor.document()->characterAt(cursor.position());
			if( ch != u' ' && ch != u'\t' ) break;
			cursor.movePosition(QTextCursor::Right, moveMode);	//	空白だけの行で行末まで行っちゃうけど、まあいいか・・・
		}
		do_cdy(cursor);
		break;
	case '0':
		if( g.m_repeatCount == 0 ) {
			cursor.movePosition(QTextCursor::StartOfBlock, moveMode);
			do_cdy(cursor);
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
	if( completed ) {	//	コマンド完結
		g.m_repeatCount = 0;
		g.m_cdy = ' ';
		g.m_lastCommand = g.m_pendingCommand;
		g.m_pendingCommand.clear();
		if( docWidget != nullptr ) {
			docWidget->m_editor->viewport()->update();
			docWidget->m_preview->viewport()->update();
		}
	}
}
