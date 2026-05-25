#include <QTextBlock>
#include "MainWindow.h"
#include "DocWidget.h"
#include "MarkdownEditor.h"
#include "MarkdownPreview.h"

extern Global g;
extern ViStatus gvi;

int getRepeatCount() {
	if( gvi.m_repeatCount == 0 ) return 1;
	return gvi.m_repeatCount;
}
bool isSpaceChar(QChar ch) {
	return ch == u' ' || ch == u'\t';
}
void hat(QTextCursor& cursor, QTextCursor::MoveMode moveMode = QTextCursor::MoveAnchor) {
	cursor.movePosition(QTextCursor::StartOfBlock);
	for(;;) {
		QChar ch = cursor.document()->characterAt(cursor.position());
		if( ch != u' ' && ch != u'\t' ) break;
		cursor.movePosition(QTextCursor::Right, moveMode);	//	空白だけの行で行末まで行っちゃうけど、まあいいか・・・
	}
}
void do_openline(QTextCursor& cursor, bool before) {
	if( before ) {
		cursor.movePosition(QTextCursor::StartOfBlock);
		cursor.insertText("\n");
		cursor.movePosition(QTextCursor::PreviousBlock);
	} else {
		cursor.setPosition(cursor.block().position() + cursor.block().text().size());
		cursor.insertText("\n");
	}
}
//void do_Word(QTextCursor& cursor, int rcnt) {
//}
void do_cdy(QTextCursor& cursor) {
	if( gvi.m_cdy == 'd' ) {	//	d<move>
		if( cursor.hasSelection() ) {
			gvi.m_yankBuffer = cursor.selectedText();
			gvi.m_linewiseYanked = true;
			cursor.deleteChar();
		}
	} else if( gvi.m_cdy == 'c' ) {
		if( cursor.hasSelection() )
			cursor.deleteChar();
		gvi.m_viCmdMode = false;
	}
}
bool do_fFtT(QTextCursor& cursor, QChar ch, int rcnt) {
	QTextBlock block = cursor.block();
	int EndBlockPos = block.position() + block.text().size();
	for(int i = 0; i != rcnt; ++i) {
		if( gvi.m_fFtT == 'f' ) {
			for(;;) {
				if( cursor.position() == EndBlockPos ) return false;
				cursor.movePosition(QTextCursor::Right);
				if( cursor.document()->characterAt(cursor.position()) == ch ) break;
			}
		} else if( gvi.m_fFtT == 'F' ) {
			for(;;) {
				if( cursor.position() == block.position() ) return false;
				cursor.movePosition(QTextCursor::Left);
				if( cursor.document()->characterAt(cursor.position()) == ch ) break;
			}
		}
	}
	return true;
}
void MainWindow::do_vi_insert(QChar cmd, QTextCursor& cursor) {
	QTextBlock block = cursor.block();
	switch( cmd.unicode() ) {
	case 's':
		cursor.beginEditBlock();
		g.m_editBlockOpen = true;
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		if( cursor.hasSelection() ) cursor.deleteChar();
		gvi.m_viCmdMode = false;
		break;
	case 'a':
		if( cursor.position() < block.position() + block.text().size() )	//	行末でない場合
			cursor.movePosition(QTextCursor::Right);
		//	下にスルー
	case 'i':
		gvi.m_viCmdMode = false;
		break;
	case 'A':
		cursor.setPosition(block.position() + block.text().size());
		gvi.m_viCmdMode = false;
		break;
	case 'I':
		cursor.movePosition(QTextCursor::StartOfBlock);
		for(;;) {
			QChar ch = cursor.document()->characterAt(cursor.position());
			if( ch != u' ' && ch != u'\t' ) break;
			cursor.movePosition(QTextCursor::Right);
		}
		gvi.m_viCmdMode = false;
		break;
	case 'O':
		cursor.beginEditBlock();
		g.m_editBlockOpen = true;
		do_openline(cursor, true);
		gvi.m_viCmdMode = false;
		break;
	case 'o':
		cursor.beginEditBlock();
		g.m_editBlockOpen = true;
		do_openline(cursor, false);
		gvi.m_viCmdMode = false;
		break;
	}
}
void MainWindow::do_viCmd(QChar cmd, QTextCursor& cursor) {
	//if( cmd.isEmpty() ) return;
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	bool isEditor = cursor.document() == docWidget->m_editor->document();
	bool completed = true;
	int rcnt = getRepeatCount();
	if( gvi.m_fFtT == 'f' || gvi.m_fFtT == 'F' || gvi.m_fFtT == 't' || gvi.m_fFtT == 'T' ) {
		if( do_fFtT(cursor, cmd, rcnt) )
			do_cdy(cursor);
	} else if( cmd == 'i' || cmd == 'I' || cmd == 'a' || cmd == 'A' || cmd == 'o' || cmd == 'O' || cmd == 's' || cmd == 'S' ) {
		do_vi_insert(cmd, cursor);
	} else {
		auto moveMode = gvi.m_cdy == ' ' ? QTextCursor::MoveAnchor : QTextCursor::KeepAnchor;
		QTextDocument *doc = cursor.document();
		QTextBlock block = cursor.block();
		g.m_pendingCommand += cmd;
		switch( cmd.unicode() ) {
		case 'f':
		case 'F':
		case 't':
		case 'T':
			if( gvi.m_fFtT == ' ' ) {
				gvi.m_fFtT = cmd.unicode();
				completed = false;
			}
			break;
		case 'G':
			if( gvi.m_repeatCount == 0 ) {
				cursor.movePosition(QTextCursor::End);
			} else {
				block = doc->findBlockByNumber(gvi.m_repeatCount - 1);
				cursor.setPosition(block.position());
				hat(cursor);
			}
			break;
		case 'g':
			if( gvi.m_cdy == 'g' ) {
				cursor.movePosition(QTextCursor::Start);
			} else if( gvi.m_cdy == ' ' ) {
				gvi.m_cdy = 'g';
				completed = false;
			}
			break;
		case 'c':
			if( gvi.m_cdy == 'c' ) {
				cursor.beginEditBlock();
				g.m_editBlockOpen = true;
				cursor.movePosition(QTextCursor::StartOfBlock);
				cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, rcnt);
				cursor.deleteChar();
				gvi.m_viCmdMode = false;
			} else if( gvi.m_cdy == ' ' ) {
				gvi.m_cdy = 'c';
				completed = false;
			}
			break;
		case 'D':
			cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
			if( cursor.hasSelection() ) {
				gvi.m_yankBuffer = cursor.selectedText();
				gvi.m_linewiseYanked = false;
				cursor.deleteChar();
			}
			break;
		case 'd':
			if( gvi.m_cdy == 'd' ) {	//	dd
				cursor.movePosition(QTextCursor::StartOfBlock);
				cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, rcnt);
				if( cursor.hasSelection() ) {
					gvi.m_yankBuffer = cursor.selectedText();
					gvi.m_linewiseYanked = true;
					cursor.deleteChar();
				}
			} else if( gvi.m_cdy == ' ' ) {
				gvi.m_cdy = 'd';
				completed = false;
			}
			break;
		case 'y':
			if( gvi.m_cdy == 'y' ) {	//	yy
				cursor.movePosition(QTextCursor::StartOfBlock);
				cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, rcnt);
				if( cursor.hasSelection() ) {
					gvi.m_yankBuffer = cursor.selectedText();
					gvi.m_linewiseYanked = true;
					cursor.clearSelection();
				}
			} else {
				gvi.m_cdy = 'y';
				completed = false;
			}
			break;
		case 'x':
			while( --rcnt >= 0 && cursor.position() < block.position() + block.text().size() )	//	行末でない場合
				cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
			if( cursor.hasSelection() ) {
				gvi.m_yankBuffer = cursor.selectedText();
				gvi.m_linewiseYanked = false;
				cursor.deleteChar();
			}
			break;
		case 'X':
			while( --rcnt >= 0 && cursor.position() > block.position() )	//	行頭でない場合
				cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
			if( cursor.hasSelection() ) {
				gvi.m_yankBuffer = cursor.selectedText();
				gvi.m_linewiseYanked = false;
				cursor.deleteChar();
			}
			break;
		case 'p':
			if( !gvi.m_yankBuffer.isEmpty() ) {
				if( gvi.m_linewiseYanked ) {
					cursor.movePosition(QTextCursor::NextBlock);
					//do_openline(cursor, false);
				} else
					cursor.movePosition(QTextCursor::Right);
				cursor.insertText(gvi.m_yankBuffer);
			}
			break;
		case 'P':
			if( !gvi.m_yankBuffer.isEmpty() ) {
				if( gvi.m_linewiseYanked ) {
					cursor.movePosition(QTextCursor::StartOfBlock);
					//do_openline(cursor, true);
				}
				cursor.insertText(gvi.m_yankBuffer);
			}
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
					docWidget->m_editor->moveToNextWord(cursor, /*select = */gvi.m_cdy != ' ');
				else
					docWidget->m_preview->moveToNextWord(cursor, /*select = */gvi.m_cdy != ' ');
				if( cursor.position() == pos ) break;
			}
			do_cdy(cursor);
			break;
		case 'W':
			//do_Word(cursor, rcnt);
			for(int i = 0; i < rcnt; ++i) {
				//if( cursor.position() == doc->characterCount() ) break;
				while( cursor.position() < doc->characterCount() && !isSpaceChar(doc->characterAt(cursor.position())) )
					cursor.movePosition(QTextCursor::Right);
				while( cursor.position() < doc->characterCount() && isSpaceChar(doc->characterAt(cursor.position())) )
					cursor.movePosition(QTextCursor::Right);
				if( cursor.position() == doc->characterCount() ) break;
			}
			do_cdy(cursor);
			break;
	#if 0
			for(int i = 0; i < rcnt; ++i) {
				auto pos = cursor.position();
				if( isEditor )
					docWidget->m_editor->moveToNextWord(cursor, /*select = */gvi.m_cdy != ' ');
				else
					docWidget->m_preview->moveToNextWord(cursor, /*select = */gvi.m_cdy != ' ');
				if( cursor.position() == pos ) break;
			}
	#endif
			do_cdy(cursor);
			break;
		case 'b':
			for(int i = 0; i < rcnt; ++i) {
				if( isEditor )
					docWidget->m_editor->moveToPrevWord(cursor, /*select = */gvi.m_cdy != ' ');
				else
					docWidget->m_preview->moveToPrevWord(cursor, /*select = */gvi.m_cdy != ' ');
				if( cursor.position() == 0 ) break;
			}
			do_cdy(cursor);
			break;
		case 'B':
			for(int i = 0; i < rcnt; ++i) {
				while( cursor.position() > 0 && isSpaceChar(doc->characterAt(cursor.position()-1)) )
					cursor.movePosition(QTextCursor::Left);
				while( cursor.position() > 0 && !isSpaceChar(doc->characterAt(cursor.position()-1)) )
					cursor.movePosition(QTextCursor::Left);
				if( cursor.position() == 0 ) break;
			}
			do_cdy(cursor);
			break;
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
			hat(cursor, moveMode);
			do_cdy(cursor);
			break;
		case '\n':
		case '+':
			cursor.movePosition(QTextCursor::NextBlock, moveMode, rcnt);
			hat(cursor, moveMode);
			do_cdy(cursor);
			break;
		case '^':
			hat(cursor, moveMode);
			do_cdy(cursor);
			break;
		case '0':
			if( gvi.m_repeatCount == 0 ) {
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
			gvi.m_repeatCount = gvi.m_repeatCount * 10 + (cmd.unicode() - u'0');
			completed = false;
			break;
		default:	//	不正コマンド
			//	undone: エラー表示
			completed = true;
		}
	}
	if( completed ) {	//	コマンド完結
		gvi.m_repeatCount = 0;
		gvi.m_cdy = ' ';
		g.m_lastCommand = g.m_pendingCommand;
		g.m_pendingCommand.clear();
		if( docWidget != nullptr ) {
			docWidget->m_editor->viewport()->update();
			docWidget->m_preview->viewport()->update();
		}
	}
}
