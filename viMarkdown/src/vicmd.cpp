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
	case u'a':
		if( cursor.position() < block.position() + block.text().size() )	//	行末でない場合
			cursor.movePosition(QTextCursor::Right);
		//	下にスルー
	case u'i':
		g.m_viCmdMode = false;
		break;
	case u'x':
		if( rcnt > 1 ) {
			while( --rcnt >= 0 && cursor.position() < block.position() + block.text().size() )	//	行末でない場合
				cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		}
		cursor.deleteChar();
		break;
	case u'w':
		for(int i = 0; i < rcnt; ++i) {
			auto pos = cursor.position();
			if( isEditor )
				docWidget->m_editor->moveToNextWord(cursor, /*shift = */false);
			else
				docWidget->m_preview->moveToNextWord(cursor, /*shift = */false);
			if( cursor.position() == pos ) break;
		}
		break;
	case u'b':
		for(int i = 0; i < rcnt; ++i) {
			if( isEditor )
				docWidget->m_editor->moveToPrevWord(cursor, /*shift = */false);
			else
				docWidget->m_preview->moveToPrevWord(cursor, /*shift = */false);
			if( cursor.position() == 0 ) break;
		}
		break;
	case u'k':
		cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, getRepeatCount());
		break;
	case u'j':
		cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, getRepeatCount());
		break;
	case u'h': {
		rcnt = qMin(rcnt, cursor.position() - block.position());	//	行頭対応
		cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, rcnt);
		break;
	}
	case u'l':
	case u' ':
		//	undone: 行末対応
		cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, getRepeatCount());
		break;
	case u'0':
		if( g.m_repeatCount == 0 ) {
			cursor.movePosition(QTextCursor::StartOfBlock);
			break;
		}
		//	するすると下にスルーする
	case u'1':
	case u'2':
	case u'3':
	case u'4':
	case u'5':
	case u'6':
	case u'7':
	case u'8':
	case u'9':
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
