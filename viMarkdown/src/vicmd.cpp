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
	return ch == u' ' || ch == u'\t' || ch == u'\n'|| ch.unicode() == 0x2029;	//	0x2029: 改行コード
}
void hat(QTextCursor& cursor, QTextCursor::MoveMode moveMode = QTextCursor::MoveAnchor) {
	cursor.movePosition(QTextCursor::StartOfBlock);
	for(;;) {
		QChar ch = cursor.document()->characterAt(cursor.position());
		if( ch != u' ' && ch != u'\t' ) break;
		cursor.movePosition(QTextCursor::Right, moveMode);	//	空白だけの行で行末まで行っちゃうけど、まあいいか・・・
	}
}
void do_r(QChar ch, QTextCursor& cursor, int rcnt) {
	QTextBlock block = cursor.block();
    int available = block.position() + block.text().size() - cursor.position();
    if( rcnt > available ) return;   // 行末を超える場合は無視（viの仕様）
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, rcnt);
	cursor.insertText(QString(rcnt, ch));
	gvi.m_isEditCommand = true;
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
void do_swap_case(QTextCursor& cursor, int rcnt) {
	QString text;
	QTextBlock block = cursor.block();
	const QString buf = block.text();
	int ix = cursor.position() - block.position();
	int ix9 = qMin(ix + rcnt, buf.size());
	if( ix9 == ix ) return;
	for(int i = ix; i < ix9; ++i) {
		if( buf[i].isLower() ) text += buf[i].toUpper();
		else if( buf[i].isUpper() ) text += buf[i].toLower();
		else text += buf[i];
	}
	cursor.setPosition(block.position() + ix9, QTextCursor::KeepAnchor);
	cursor.insertText(text);
}
void do_cdy(QTextCursor& cursor) {
	if( gvi.m_operator == 'c' ) {
		if( cursor.hasSelection() )
			cursor.deleteChar();
		gvi.m_viCmdMode = false;
	} else if( gvi.m_operator == 'd' || gvi.m_operator == 'y' ) {	//	d<move> or y<move>
		if( cursor.hasSelection() ) {
			gvi.m_yankBuffer = cursor.selectedText();
			gvi.m_linewiseYanked = gvi.m_linewiseMoved;
			if( gvi.m_operator == 'd' )
				cursor.deleteChar();
			else
				cursor.clearSelection();
		}
	}
}
bool do_fFtT(QTextCursor& cursor, QChar cmd, QChar ch, int rcnt) {
	QTextBlock block = cursor.block();
	const QString buf = block.text();
	int ix = cursor.position() - block.position();		//	ブロック内インデックス
	int ix0 = ix;
	if( cmd== 'f' || cmd== 't' ) {		//	順方向検索
		for(int i = 0; i != rcnt; ++i) {
			int ix2 = buf.indexOf(ch, ix+1);
			if( ix2 < 0 ) break;	//	未発見
			ix = ix2;
		}
		if( cmd== 't' ) --ix;
	} else {		//	F T 逆方向検索
		for(int i = 0; i != rcnt; ++i) {
			int ix2 = buf.lastIndexOf(ch, ix-1);
			if( ix2 < 0 ) break;	//	未発見
			ix = ix2;
		}
		if( cmd== 'T' ) ++ix;
	}
	if( ix != ix0 ) {
		gvi.m_last_fFtT = cmd;
		gvi.m_last_fFtT_char = ch;
		cursor.setPosition(block.position() + ix);
		return true;
	} else
		return false;
}
void do_vi_change_line(QTextCursor& cursor) {
	cursor.beginEditBlock();	//	１文字削除とその後の文字挿入を１回でundo可能にするため
	g.m_editBlockOpen = true;
	hat(cursor);
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	if( cursor.hasSelection() ) {
		gvi.m_yankBuffer = cursor.selectedText();
		cursor.deleteChar();
	}
	cursor.endEditBlock();
	gvi.m_joinEditBlock = true;
	gvi.m_viCmdMode = false;
}
void MainWindow::do_vi_insert(QChar cmd, QTextCursor& cursor, int rcnt) {
	QTextBlock block = cursor.block();
	int eolpos = block.position() + block.text().size();
	switch( cmd.unicode() ) {
	case 'S':		//	行を消して挿入モードへ
		do_vi_change_line(cursor);
		break;
	case 's':
		cursor.beginEditBlock();	//	文字削除とその後の文字挿入を１回でundo可能にするため
		g.m_editBlockOpen = true;
		rcnt = qMin(rcnt, eolpos - cursor.position());
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, rcnt);
		if( cursor.hasSelection() ) {
			gvi.m_yankBuffer = cursor.selectedText();
			cursor.deleteChar();
			gvi.m_joinEditBlock = true;
		}
		cursor.endEditBlock();
		break;
	case 'a':
		if( cursor.position() < block.position() + block.text().size() )	//	行末でない場合
			cursor.movePosition(QTextCursor::Right);
		break;
	case 'i':
		break;
	case 'A':
		cursor.setPosition(block.position() + block.text().size());
		break;
	case 'I':
		cursor.movePosition(QTextCursor::StartOfBlock);
		for(;;) {
			QChar ch = cursor.document()->characterAt(cursor.position());
			if( ch != u' ' && ch != u'\t' ) break;
			cursor.movePosition(QTextCursor::Right);
		}
		break;
	case 'O':
		cursor.beginEditBlock();
		g.m_editBlockOpen = true;
		do_openline(cursor, true);
		cursor.endEditBlock();
		gvi.m_joinEditBlock = true;
		break;
	case 'o':
		cursor.beginEditBlock();
		g.m_editBlockOpen = true;
		do_openline(cursor, false);
		cursor.endEditBlock();
		gvi.m_joinEditBlock = true;
		break;
	case 'C':		//	カーソル位置から行末まで削除して挿入モードへ遷移
		cursor.beginEditBlock();
		g.m_editBlockOpen = true;
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		if( cursor.hasSelection() ) {
			gvi.m_yankBuffer = cursor.selectedText();
			gvi.m_linewiseYanked = false;
			cursor.deleteChar();
		}
		cursor.endEditBlock();
		gvi.m_joinEditBlock = true;
		break;
	default:
		return;
	}
	gvi.m_viCmdMode = false;
}
void MainWindow::do_vi_delete(QChar cmd, QTextCursor& cursor, int rcnt) {		//	x X D
	QTextBlock block = cursor.block();
	switch( cmd.unicode() ) {
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
	case 'D':
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		if( cursor.hasSelection() ) {
			gvi.m_yankBuffer = cursor.selectedText();
			gvi.m_linewiseYanked = false;
			cursor.deleteChar();
		}
		break;
	default:
		return;
	}
	gvi.m_isEditCommand = true;
}
void do_yank_line(QTextCursor& cursor, int rcnt) {
	cursor.movePosition(QTextCursor::StartOfBlock);
	cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, rcnt);
	if( cursor.hasSelection() ) {
		gvi.m_yankBuffer = cursor.selectedText();
		gvi.m_linewiseYanked = true;
		cursor.clearSelection();
	}
}
bool MainWindow::do_vi_operator(QChar cmd, QTextCursor& cursor, int rcnt) {		//	{c|d|y}<move>, gg
	if( gvi.m_operator == ' ' ) {
		gvi.m_operator = cmd;
		if( gvi.m_repeatCount != 0 )
			gvi.m_opCount = gvi.m_repeatCount;
		gvi.m_repeatCount = 0;
		return false;
	}
	if( gvi.m_operator == cmd ) {		//	cc dd yy gg << >> の場合
		switch( cmd.unicode() ) {
		case 'c':	//	cc
			do_vi_change_line(cursor);
#if 0
			cursor.beginEditBlock();
			g.m_editBlockOpen = true;
			cursor.movePosition(QTextCursor::StartOfBlock);
			cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, rcnt);
			cursor.deleteChar();
			cursor.endEditBlock();
			gvi.m_viCmdMode = false;
#endif
			break;
		case 'd':
			cursor.movePosition(QTextCursor::StartOfBlock);
			cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, rcnt);
			if( cursor.hasSelection() ) {
				gvi.m_yankBuffer = cursor.selectedText();
				gvi.m_linewiseYanked = true;
				cursor.deleteChar();
				gvi.m_isEditCommand = true;
			}
			break;
		case 'y':
			do_yank_line(cursor, rcnt);
			break;
		case 'g':
			cursor.movePosition(QTextCursor::Start);
			break;
		case '>':
			onAction_Indent();
			gvi.m_isEditCommand = true;
			break;
		case '<':
			onAction_UnIndent();
			gvi.m_isEditCommand = true;
			break;
		}
	}
	return true;
}
void do_vi_H(QTextCursor& cursor, int rcnt, DocWidget* docWidget) {
	QPoint topLeft(0, 0);	//	左上点
	if( cursor.document() == docWidget->m_editor->document() )
		cursor = docWidget->m_editor->cursorForPosition(topLeft);
	else
		cursor = docWidget->m_preview->cursorForPosition(topLeft);
	if( rcnt > 1 )
		cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, rcnt - 1);
	hat(cursor);
}
void do_vi_L(QTextCursor& cursor, int rcnt, DocWidget* docWidget) {
	QPoint bottomLeft(0, docWidget->m_editor->viewport()->height() - 1);	//	左下点
	if( cursor.document() == docWidget->m_editor->document() )
		cursor = docWidget->m_editor->cursorForPosition(bottomLeft);
	else
		cursor = docWidget->m_preview->cursorForPosition(bottomLeft);
	if( rcnt > 1 )
		cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, rcnt - 1);
	hat(cursor);
}
void do_vi_M(QTextCursor& cursor, DocWidget* docWidget) {
	int h, l;	//	H, L block number
	QPoint topLeft(0, 0);	//	左上点
	QPoint bottomLeft(0, docWidget->m_editor->viewport()->height() - 1);	//	左下点
	if( cursor.document() == docWidget->m_editor->document() ) {
		h = docWidget->m_editor->cursorForPosition(topLeft).block().blockNumber();;
		l = docWidget->m_editor->cursorForPosition(bottomLeft).block().blockNumber();;
	} else {
		h = docWidget->m_preview->cursorForPosition(topLeft).block().blockNumber();;
		l = docWidget->m_preview->cursorForPosition(bottomLeft).block().blockNumber();;
	}
	QTextBlock block = cursor.document()->findBlockByNumber((h+l)/2);
	cursor.setPosition(block.position());
	hat(cursor);
}
void MainWindow::do_vi_motion(QChar cmd, QTextCursor& cursor, int rcnt, DocWidget* docWidget) {		//	hjkl等
	gvi.m_linewiseMoved = false;
	bool isEditor = cursor.document() == docWidget->m_editor->document();
	auto moveMode = gvi.m_operator == ' ' ? QTextCursor::MoveAnchor : QTextCursor::KeepAnchor;
	QTextDocument *doc = cursor.document();
	QTextBlock block = cursor.block();
	switch( cmd.unicode() ) {
	case 'k':
		cursor.movePosition(QTextCursor::Up, moveMode, rcnt);
		gvi.m_linewiseMoved = true;
		break;
	case 'j':
		cursor.movePosition(QTextCursor::Down, moveMode, rcnt);
		gvi.m_linewiseMoved = true;
		break;
	case 'h': {
		rcnt = qMin(rcnt, cursor.position() - block.position());	//	行頭対応
		cursor.movePosition(QTextCursor::Left, moveMode, rcnt);
		break;
	}
	case 'l':
	case ' ': {
		int pos = block.position() + block.text().size() - 1;
		if( cursor.position() < pos ) {
			rcnt = qMin(rcnt, pos - cursor.position());
			cursor.movePosition(QTextCursor::Right, moveMode, rcnt);
		}
		break;
	}
	case 'w':
		for(int i = 0; i < rcnt; ++i) {
			auto pos = cursor.position();
			if( isEditor )
				docWidget->m_editor->moveToNextWord(cursor, /*select = */gvi.m_operator != ' ');
			else
				docWidget->m_preview->moveToNextWord(cursor, /*select = */gvi.m_operator != ' ');
			if( cursor.position() == pos ) break;
		}
		break;
	case 'W': {
		const int maxPos = doc->characterCount() - 1;
		for(int i = 0; i < rcnt; ++i) {
			while( cursor.position() < maxPos && !isSpaceChar(doc->characterAt(cursor.position())) )
				if( !cursor.movePosition(QTextCursor::Right) ) goto doneW;
			while( cursor.position() < maxPos && isSpaceChar(doc->characterAt(cursor.position())) )
				if( !cursor.movePosition(QTextCursor::Right) ) goto doneW;
			if( cursor.position() >= maxPos ) break;
		}
doneW:
		break;
	}
	case 'E': {
		const int maxPos = doc->characterCount() - 1;
		for(int i = 0; i < rcnt; ++i) {
			if( !cursor.movePosition(QTextCursor::Right) ) break;	// すでに末尾にいる場合はこれ以上進めないため、
			// 1. スペース文字をスキップ（次の単語の先頭に達するまで右へ）
			while( cursor.position() < maxPos && isSpaceChar(doc->characterAt(cursor.position())) )
				if( !cursor.movePosition(QTextCursor::Right) ) break;
			// 2. スペース以外の文字をスキップ（単語を通り過ぎるまで右へ）
			while( cursor.position() < maxPos && !isSpaceChar(doc->characterAt(cursor.position())) )
				if( !cursor.movePosition(QTextCursor::Right) ) break;
			// 単語を通り過ぎてスペース（または終端）の上にいるので、
			// 1文字左に戻ることで「単語の最後の文字の上」にカーソルを合わせます。
			cursor.movePosition(QTextCursor::Left);
			if( cursor.position() >= maxPos ) break;
		}
	    break;
	}
    case 'b':
		for(int i = 0; i < rcnt; ++i) {
			if( isEditor )
				docWidget->m_editor->moveToPrevWord(cursor, /*select = */gvi.m_operator != ' ');
			else
				docWidget->m_preview->moveToPrevWord(cursor, /*select = */gvi.m_operator != ' ');
			if( cursor.position() == 0 ) break;
		}
		break;
	case 'B':
		for(int i = 0; i < rcnt; ++i) {
			while( cursor.position() > 0 && isSpaceChar(doc->characterAt(cursor.position()-1)) )
				cursor.movePosition(QTextCursor::Left);
			while( cursor.position() > 0 && !isSpaceChar(doc->characterAt(cursor.position()-1)) )
				cursor.movePosition(QTextCursor::Left);
			if( cursor.position() == 0 ) break;
		}
		break;
	case '$':
		if( !block.text().isEmpty() ) {
			cursor.setPosition(block.position() + block.text().size() - 1, moveMode);
		}
		break;
	case '-':
		cursor.movePosition(QTextCursor::PreviousBlock, moveMode, rcnt);
		hat(cursor, moveMode);
		gvi.m_linewiseMoved = true;
		break;
	case '\n':
	case '+':
		cursor.movePosition(QTextCursor::NextBlock, moveMode, rcnt);
		hat(cursor, moveMode);
		gvi.m_linewiseMoved = true;
		break;
	case '^':
		hat(cursor, moveMode);
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
	case 'H':
		do_vi_H(cursor, rcnt, docWidget);
		break;
	case 'L':
		do_vi_L(cursor, rcnt, docWidget);
		break;
	case 'M':
		do_vi_M(cursor, docWidget);
		break;
	default:
		return;
	}
	do_cdy(cursor);
}
void MainWindow::do_viCmd(QChar cmd, QTextCursor& cursor) {
	//if( cmd.isEmpty() ) return;
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	bool isEditor = cursor.document() == docWidget->m_editor->document();
	bool completed = true;
	int rcnt = getRepeatCount();
	gvi.m_pendingCommand += cmd;
	if( gvi.m_fFtT == 'f' || gvi.m_fFtT == 'F' || gvi.m_fFtT == 't' || gvi.m_fFtT == 'T' ) {
		if( do_fFtT(cursor, gvi.m_fFtT, cmd, rcnt) )
			do_cdy(cursor);
	} else if( gvi.m_operator == 'r' ) {
		do_r(cmd, cursor, rcnt);
	} else if( cmd == 'i' || cmd == 'I' || cmd == 'a' || cmd == 'A' || cmd == 'o' || cmd == 'O' || cmd == 's' || cmd == 'S' || cmd == 'C' ) {
		do_vi_insert(cmd, cursor, rcnt);
	} else if( cmd == 'x' || cmd == 'X' || cmd == 'D' ) {
		do_vi_delete(cmd, cursor,rcnt);
	} else if( cmd == 'c' || cmd == 'd' || cmd == 'y' || cmd == 'g' || cmd == 'r' || cmd == '<' || cmd == '>' ) {
		completed = do_vi_operator(cmd, cursor, rcnt);
	} else if( cmd.unicode() >= '0' && cmd.unicode() <= '9' ) {
		if( cmd == u'0' && gvi.m_repeatCount == 0 ) {
			auto moveMode = gvi.m_operator == ' ' ? QTextCursor::MoveAnchor : QTextCursor::KeepAnchor;
			cursor.movePosition(QTextCursor::StartOfBlock, moveMode);
			do_cdy(cursor);
		} else {
			gvi.m_repeatCount = gvi.m_repeatCount * 10 + (cmd.unicode() - u'0');
			completed = false;
		}
	} else if( cmd == 'h' || cmd == 'j' || cmd == 'k' || cmd == 'l' || cmd == ' ' || 
	           cmd == 'w' || cmd == 'W' || cmd == 'b' || cmd == 'B' || cmd == 'e' || cmd == 'E' ||
	           cmd == '$' || cmd == '^' || cmd == '-' || cmd == '+' ||
	           cmd == '\n' || cmd == 'G' || cmd == 'H' || cmd == 'L' || cmd == 'M' )
	{
	    do_vi_motion(cmd, cursor, rcnt*gvi.m_opCount, docWidget);
	} else {
		auto moveMode = gvi.m_operator == ' ' ? QTextCursor::MoveAnchor : QTextCursor::KeepAnchor;
		QTextDocument *doc = cursor.document();
		QTextBlock block = cursor.block();
		QChar c;		//	for , ;
		QString buf;	//	for .
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
		case '~':
			do_swap_case(cursor, rcnt);		//	大文字・小文字反転
			gvi.m_isEditCommand = true;
			break;
		case 'Y':
			do_yank_line(cursor, rcnt);
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
		case ';':		//	順方向再検索
			switch( gvi.m_last_fFtT.unicode() ) {
			case 'f':
			case 'F':
				c = 'f';
				break;
			case 't':
			case 'T':
				c = 't';
				break;
			}
			do_fFtT(cursor, c, gvi.m_last_fFtT_char, rcnt);
			break;
		case ',':		//	逆方向再検索
			switch( gvi.m_last_fFtT.unicode()) {
			case 'f':
			case 'F':
				c = 'F';
				break;
			case 't':
			case 'T':
				c = 'T';
				break;
			}
			do_fFtT(cursor, c, gvi.m_last_fFtT_char, rcnt);
			break;
		case '.':
			if( gvi.m_redoing || gvi.m_lastEditCommand.isEmpty() ) break;
			qDebug() << "'.': gvi.m_lastEditCommand = " << gvi.m_lastEditCommand;
			gvi.m_redoing = true;
			buf = gvi.m_lastEditCommand;
			if( gvi.m_repeatCount != 0 ) {		//	<num>. の場合
				int i = 0;
				while( i < buf.size() && buf[i].isDigit() )
					++i;
				buf = QString::number(gvi.m_repeatCount) + buf.mid(i);
			}
			for(QChar ch: buf) {
				do_viCmd(ch, cursor);
			}
			gvi.m_redoing = false;
			break;
		default:	//	不正コマンド
			//	undone: エラー表示
			completed = true;
		}
	}
	if( completed ) {	//	コマンド完結
		gvi.m_opCount = 1;
		gvi.m_repeatCount = 0;
		gvi.m_operator = ' ';
		gvi.m_fFtT = ' ';
		if( gvi.m_isEditCommand && !gvi.m_redoing )
			gvi.m_lastEditCommand = gvi.m_pendingCommand;
		//else
		//	gvi.m_lastEditCommand.clear();
		gvi.m_pendingCommand.clear();
		gvi.m_isEditCommand = false;
		if( docWidget != nullptr ) {
			docWidget->m_editor->viewport()->update();
			docWidget->m_preview->viewport()->update();
		}
		qDebug() << "gvi.m_lastEditCommand = " << gvi.m_lastEditCommand;
	}
}
