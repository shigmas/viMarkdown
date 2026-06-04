#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QLineEdit>
#include <QStatusBar>
#include <QRegularExpression>
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
	case 'O':
		cursor.beginEditBlock();
		g.m_editBlockOpen = true;
		//do_openline(cursor, true);
		cursor.movePosition(QTextCursor::StartOfBlock);
		cursor.insertText("\n");
		cursor.movePosition(QTextCursor::PreviousBlock);
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
	gvi.m_isEditCommand = true;
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
void MainWindow::do_prefix_cmd(QChar cmd, QTextCursor& cursor, int rcnt, DocWidget* docWidget) {		//	{g z r [ ]} cmd
	QTextBlock block = cursor.block();
	QTextDocument *doc = cursor.document();
	switch( gvi.m_prefix.unicode() ) {
	case 'g':
		switch( cmd.unicode() ) {
		case 'g':		//	gg
			cursor.movePosition(QTextCursor::Start);
			break;
		}
		break;
	case 'z':
		switch( cmd.unicode() ) {
		case 'a':		//	za
			if (!block.isValid()) break;
		    block.setVisible(false); // ブロックを非表示に設定
		    doc->markContentsDirty(block.position(), block.length());
			break;
		case 'c':		//	zc
			if (block.isValid()) {
		        block.setVisible(false); // ブロックを非表示に設定
		        doc->markContentsDirty(block.position(), block.length());
		    }
			break;
		case 'o':		//	zo
			if (block.isValid() && block.next().isValid() && !block.next().isVisible() ) {
		        block = block.next();
		        block.setVisible(true); // ブロックを表示
		        doc->markContentsDirty(block.position(), block.length());
			}
			break;
		}
		break;
	case '[':
		switch( cmd.unicode() ) {
		case '[': {	//	[[
			QTextBlock block = cursor.block().previous();
			while (block.isValid()) {
				if (block.userState() == US_HEADING) {
					if (--rcnt == 0) {
						cursor.setPosition(block.position());
						break;
					}
				}
				block = block.previous();
			}
			break;
		}
		}
		break;
	case ']':
		switch( cmd.unicode() ) {
		case ']': {	//	]]
			QTextBlock block = cursor.block().next();
			while (block.isValid()) {
				if (block.userState() == US_HEADING) {
					if (--rcnt == 0) {
						cursor.setPosition(block.position());
						break;
					}
				}
				block = block.next();
			}
			break;
		}
		}
		break;
	}
}
bool MainWindow::do_vi_operator(QChar cmd, QTextCursor& cursor, int rcnt, DocWidget* docWidget) {		//	{c d y < >}<move>
	if( gvi.m_operator == ' ' ) {
		if( cursor.hasSelection() ) {
			switch( cmd.unicode() ) {
			case 'c':
				gvi.m_yankBuffer = cursor.selectedText();
				cursor.beginEditBlock();
				cursor.deleteChar();
				cursor.endEditBlock();
				gvi.m_joinEditBlock = true;
				gvi.m_viCmdMode = false;
				return true;
			case 'd':
				gvi.m_yankBuffer = cursor.selectedText();
				cursor.deleteChar();
				return true;
			case 'y':
				gvi.m_yankBuffer = cursor.selectedText();
				cursor.setPosition(cursor.selectionStart());
				return true;
			}
		}
		gvi.m_operator = cmd;
		if( gvi.m_repeatCount != 0 )
			gvi.m_opCount = gvi.m_repeatCount;
		gvi.m_repeatCount = 0;
		return false;
	}
	QTextDocument *doc = cursor.document();
	QTextBlock block = cursor.block();
	if( gvi.m_operator == cmd ) {		//	cc dd yy gg zz << >> の場合
		switch( cmd.unicode() ) {
		case 'c':	//	cc
			do_vi_change_line(cursor);
			break;
		case 'd':	//	dd
#if 1
	cursor.movePosition(QTextCursor::StartOfBlock);
    if (cursor.blockNumber() + rcnt >= doc->blockCount()) {
        // 最終行を含む削除：行頭から EndOfBlock まで選択
        // ただし前の行の改行ごと削除するため、1つ前の行末から選択
        if (cursor.blockNumber() > 0) {
            cursor.movePosition(QTextCursor::PreviousBlock);
            cursor.movePosition(QTextCursor::EndOfBlock);
            cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        } else {
            // ドキュメント全体が対象
            cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        }
    } else {
        cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, rcnt);
    }
#else
			cursor.movePosition(QTextCursor::StartOfBlock);
			cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, rcnt);
#endif
			if( cursor.hasSelection() ) {
				gvi.m_yankBuffer = cursor.selectedText();
				gvi.m_linewiseYanked = true;
				cursor.deleteChar();
				gvi.m_isEditCommand = true;
			}
			break;
		case 'y':	//	yy
			do_yank_line(cursor, rcnt);
			break;
		//case 'g':	//	gg
		//	cursor.movePosition(QTextCursor::Start);
		//	break;
#if 0
		case ']': {	//	]]
			QTextBlock block = cursor.block().next();
			while (block.isValid()) {
				if (block.userState() == US_HEADING) {
					if (--rcnt == 0) {
						cursor.setPosition(block.position());
						break;
					}
				}
				block = block.next();
			}
			break;
		}
		case '[': {	//	[[
			QTextBlock block = cursor.block().previous();
			while (block.isValid()) {
				if (block.userState() == US_HEADING) {
					if (--rcnt == 0) {
						cursor.setPosition(block.position());
						break;
					}
				}
				block = block.previous();
			}
			break;
		}
		case 'z':	//	zz
			if( cursor.document() == docWidget->m_editor->document() )
				docWidget->m_editor->centerCursor();
			//else
			//	docWidget->m_preview->centerCursor();
			break;
#endif
		case '>':	//	>>
			cursor.beginEditBlock();
			for(int i = 0; i < gvi.m_opCount; ++i) {
				onAction_Indent();
				block = block.next();
				if( !block.isValid() ) break;
				cursor.setPosition(block.position());
				if( cursor.document() == docWidget->m_editor->document() )
					docWidget->m_editor->setTextCursor(cursor);
				else
					docWidget->m_preview->setTextCursor(cursor);
			}
			cursor.endEditBlock();
			gvi.m_isEditCommand = true;
			break;
		case '<':	//	<<
			cursor.beginEditBlock();
			for(int i = 0; i < gvi.m_opCount; ++i) {
				onAction_UnIndent();
				block = block.next();
				if( !block.isValid() ) break;
				cursor.setPosition(block.position());
				if( cursor.document() == docWidget->m_editor->document() )
					docWidget->m_editor->setTextCursor(cursor);
				else
					docWidget->m_preview->setTextCursor(cursor);
			}
			cursor.endEditBlock();
			gvi.m_isEditCommand = true;
			break;
		}
	} else {
		switch( gvi.m_operator.unicode() ) {
		case 'z':
			switch( cmd.unicode() ) {
			case '.':
				break;
			case 'a':		//	za: toggle fold
				if (block.isValid()) {
			        block.setVisible(false); // ブロックを表示に設定
			        doc->markContentsDirty(block.position(), block.length());
			    }
				break;
			}
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
void do_match_paren(QTextCursor& cursor) {
	QTextDocument *doc = cursor.document();
	QTextBlock block = cursor.block();
	int lineEnd = block.position() + block.text().size();
	const QString paren = "(){}[]";
	int pix;
	int pos = cursor.position();
	for(;;) {
		if( pos >= lineEnd ) return;
		if( (pix = paren.indexOf(doc->characterAt(pos))) >= 0 ) {
			break;
		}
		++pos;
	}
	int depth = 1;
	if( pix%2 == 0 ) {		//	openParen[pix] を発見（pix は偶数）
		const QString pp = paren.mid(pix, 2);
		while( ++pos < doc->characterCount() ) {
			int ix = pp.indexOf(doc->characterAt(pos));
			if( ix == 0 ) ++depth;
			else if( ix == 1 ) {
				if( --depth == 0 ) {
					cursor.setPosition(pos);
					return;
				}
			}
		}
	} else {				//	closeParen[pix] を発見（pix は奇数）
		const QString pp = paren.mid(pix-1, 2);
		while( --pos >= 0 ) {
			int ix = pp.indexOf(doc->characterAt(pos));
			if( ix == 1 ) ++depth;
			else if( ix == 0 ) {
				if( --depth == 0 ) {
					cursor.setPosition(pos);
					return;
				}
			}
		}
	}
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
	case 'e':
		for(int i = 0; i < rcnt; ++i) {
			auto pos = cursor.position();
			if( isEditor )
				docWidget->m_editor->moveToNextWordEnd(cursor, /*select = */gvi.m_operator != ' ');
			else
				docWidget->m_preview->moveToNextWordEnd(cursor, /*select = */gvi.m_operator != ' ');
			if( cursor.position() == pos ) break;
		}
		break;
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
	case '%':
		do_match_paren(cursor);
		break;
	case 'G':
		if( gvi.m_repeatCount == 0 ) {
			cursor.movePosition(QTextCursor::End);
			if( cursor.block().text().isEmpty() )
				cursor.movePosition(QTextCursor::PreviousBlock);
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
	case 'n':
		do_find();
		break;
	case 'N':
		do_find(true);
		break;
	default:
		return;
	}
	do_cdy(cursor);
}
void do_join(QTextCursor& cursor, int rcnt) {
	QTextDocument *doc = cursor.document();
	int joins = (rcnt <= 1) ? 1 : (rcnt - 1);
	cursor.beginEditBlock();
	int finalCursorPos = -1;
	for (int i = 0; i < joins; ++i) {
		cursor.movePosition(QTextCursor::EndOfBlock);
		QTextBlock nextBlock = cursor.block().next();
		if (!nextBlock.isValid()) break; // 次の行がない（ドキュメント末尾）場合は結合処理を終了
		QString currentText = cursor.block().text();
		QString nextText = nextBlock.text();
		int trailingSpaces = 0;
		while (trailingSpaces < currentText.size() && 
			   (currentText[currentText.size() - 1 - trailingSpaces] == ' ' || 
				currentText[currentText.size() - 1 - trailingSpaces] == '\t')) {
			trailingSpaces++;
		}
		int leadingSpaces = 0;
		while (leadingSpaces < nextText.size() && 
			   (nextText[leadingSpaces] == ' ' || nextText[leadingSpaces] == '\t')) {
			leadingSpaces++;
		}
		if (trailingSpaces > 0) {
			cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, trailingSpaces);
			cursor.removeSelectedText();
		}
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1 + leadingSpaces);
		cursor.removeSelectedText();
		bool needsSpace = true;
		if (currentText.size() == trailingSpaces || nextText.size() == leadingSpaces) {
			needsSpace = false;
		} else if (leadingSpaces < nextText.size() && nextText[leadingSpaces] == ')') {
			needsSpace = false;
		}
		
		if (needsSpace) {
			cursor.insertText(" ");
		}
		if (i == 0) {
			finalCursorPos = cursor.position();
			if (needsSpace && finalCursorPos > 0) {
				finalCursorPos--; // 挿入したスペースの上にカーソルを合わせるため1文字左へ
			}
		}
	}
	cursor.endEditBlock();
	if (finalCursorPos >= 0)
		cursor.setPosition(finalCursorPos);
}
void MainWindow::do_viCmd(QChar cmd, QTextCursor& cursor) {
	//if( cmd.isEmpty() ) return;
	DocWidget *docWidget = getCurDocWidget();
	if( docWidget == nullptr ) return;
	//bool isEditor = cursor.document() == docWidget->m_editor->document();
	bool completed = true;
	int rcnt = getRepeatCount();
	gvi.m_pendingCommand += cmd;
	if( gvi.m_pendingCommand == ":" || gvi.m_pendingCommand == "/"  || gvi.m_pendingCommand == "?" ) {
		gvi.m_cmdlineMode = true;
		gvi.m_prevFocusWidget = cursor.document() == docWidget->m_editor->document() ?
									(QWidget*)docWidget->m_editor : (QWidget*)docWidget->m_preview;
		m_cmdLine->setText(gvi.m_pendingCommand);
		m_cmdLine->show();
		m_cmdLine->setFocus();
		if( gvi.m_pendingCommand == ":" ) {
			gvi.m_exhist.push_front(":");
			while( gvi.m_exhist.size() > 64 ) gvi.m_exhist.pop_back();
			gvi.m_exhist_ix = 0;
		}
		return;
	} else if( gvi.m_fFtT == 'f' || gvi.m_fFtT == 'F' || gvi.m_fFtT == 't' || gvi.m_fFtT == 'T' ) {
		if( do_fFtT(cursor, gvi.m_fFtT, cmd, rcnt) )
			do_cdy(cursor);
	} else if( gvi.m_prefix == 'r' ) {
		do_r(cmd, cursor, rcnt);
	} else if( gvi.m_prefix != ' ' ) {
		do_prefix_cmd(cmd, cursor, rcnt, docWidget);		//	{g z r [ ]} cmd
	} else if( cmd == 'g' || cmd == 'z' || cmd == 'r' || cmd == '[' || cmd == ']' ) {
		gvi.m_prefix = cmd;
		completed = false;
	} else if( gvi.m_operator != ' ' && gvi.m_operator == cmd ) {		//	cc dd yy << >>
		completed = do_vi_operator(cmd, cursor, rcnt, docWidget);		//	op cmd
	} else if( cmd == 'c' || cmd == 'd' || cmd == 'y' || cmd == '<' || cmd == '>' ) {
		gvi.m_operator = cmd;
		completed = false;
	} else if( cmd == 'i' || cmd == 'I' || cmd == 'a' || cmd == 'A' || cmd == 'o' || cmd == 'O' || cmd == 's' || cmd == 'S' || cmd == 'C' ) {
		do_vi_insert(cmd, cursor, rcnt);
	} else if( cmd == 'x' || cmd == 'X' || cmd == 'D' ) {
		do_vi_delete(cmd, cursor,rcnt);
	}
	// else if( cmd == 'c' || cmd == 'd' || cmd == 'y' || cmd == 'g' || cmd == 'z' || cmd == 'r' ||
	//	cmd == '<' || cmd == '>' || cmd == '[' || cmd == ']' )
	//{
	//	completed = do_vi_operator(cmd, cursor, rcnt, docWidget);
	//}
	else if( cmd.unicode() >= '0' && cmd.unicode() <= '9' ) {
		if( cmd == u'0' && gvi.m_repeatCount == 0 ) {
			auto moveMode = gvi.m_operator == ' ' ? QTextCursor::MoveAnchor : QTextCursor::KeepAnchor;
			cursor.movePosition(QTextCursor::StartOfBlock, moveMode);
			do_cdy(cursor);
		} else {
			gvi.m_repeatCount = gvi.m_repeatCount * 10 + (cmd.unicode() - u'0');
			completed = false;
		}
	} else if( cmd == 'h' || cmd == 'j' || cmd == 'k' || cmd == 'l' || cmd == ' ' || cmd == '%' || 
	           cmd == 'w' || cmd == 'W' || cmd == 'b' || cmd == 'B' || cmd == 'e' || cmd == 'E' ||
	           cmd == '$' || cmd == '^' || cmd == '-' || cmd == '+' || cmd == 'n' || cmd == 'N' ||
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
				cursor.insertText(gvi.m_yankBuffer.repeated(rcnt));
			}
			break;
		case 'P':
			if( !gvi.m_yankBuffer.isEmpty() ) {
				if( gvi.m_linewiseYanked ) {
					cursor.movePosition(QTextCursor::StartOfBlock);
					//do_openline(cursor, true);
				}
				cursor.insertText(gvi.m_yankBuffer.repeated(rcnt));
			}
			break;
		case 'J':
			do_join(cursor, rcnt);
			break;
		case 'u':
			docWidget->m_editor->undo();
			break;
		case 'U':
			docWidget->m_editor->redo();
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
			//buf += gvi.m_insertedText;
			qDebug() << "redo buf = " << buf;
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
		if( gvi.m_redoing && !gvi.m_viCmdMode && !gvi.m_insertedText.isEmpty() ) {
			cursor.insertText(gvi.m_insertedText);
			gvi.m_viCmdMode = true;
		}
		gvi.m_opCount = 1;
		gvi.m_repeatCount = 0;
		gvi.m_operator = ' ';
		gvi.m_prefix = ' ';
		gvi.m_fFtT = ' ';
		if( gvi.m_isEditCommand && !gvi.m_redoing )
			gvi.m_lastEditCommand = gvi.m_pendingCommand;
		//else
		//	gvi.m_lastEditCommand.clear();
		if( !gvi.m_viCmdMode ) {
			gvi.m_recInsertedText = true;
			gvi.m_insertedText.clear();
		} else
			gvi.m_recInsertedText = false;
		gvi.m_pendingCommand.clear();
		gvi.m_isEditCommand = false;
		if( docWidget != nullptr ) {
			docWidget->m_editor->viewport()->update();
			docWidget->m_preview->viewport()->update();
		}
		qDebug() << "gvi.m_lastEditCommand = " << gvi.m_lastEditCommand;
	}
}
int do_search_line(const QString &text, int &i, int currentLine, const QTextDocument *doc) {
	QChar c = text[i++];
	QString pat;
	while( i < text.size() && text[i] != c ) {
		if( text[i] == '\\' ) pat += text[i++];
		pat += text[i++];
	}
	if( i == text.size() || pat.isEmpty() )		//	undone: // ?? の場合は再検索対応
		return -1;
	++i;
	QTextBlock block = doc->findBlockByNumber(currentLine - 1);
	QRegularExpression re(pat);
	if( c == '/' ) {	//	順方向検索
		if( !(block = block.next()).isValid() ) return -1;
		QTextCursor cur = doc->find(re, block.position());
		if( cur.isNull() ) return -1;	//	not found
		return cur.block().blockNumber() + 1;
	} else {	//	逆方向検索
		//if( !(block = block.next()).isValid() ) return -1;
		QTextCursor cur = doc->find(re, block.position(), QTextDocument::FindBackward);
		if( cur.isNull() ) return -1;	//	not found
		return cur.block().blockNumber() + 1;
	}
}
int parseLineSpec(const QString &text, int &i, int currentLine, int totalLines, const QTextDocument *doc) {
	if (i >= text.size()) return -1;
    int line = -1;
    QChar c = text.at(i);
    if (c.isDigit()) {
        int start = i;
        while (i < text.size() && text.at(i).isDigit()) {
            i++;
        }
        line = text.mid(start, i - start).toInt();
    } else if (c == '.') {
        line = currentLine;
        i++;
    } else if (c == '$') {
        line = totalLines;
        i++;
    } else if( c == '/' || c == '?' ) {
    	line = do_search_line(text, i, currentLine, doc);
    } else if( c == '+' || c == '-' ) {
        line = currentLine;
    } else {
        // 行指定の記号ではない（"w" や "q" などのコマンド文字に達した）
        return -1;
    }
    // 2. オプションのオフセット（例: "+3" や "-5"）の解析
    if (i < text.size()) {
        QChar sign = text.at(i);
        if (sign == '+' || sign == '-') {
            i++;
            int offset = 0;
            int start = i;
            while (i < text.size() && text.at(i).isDigit()) {
                i++;
            }
            if (i > start) {
                offset = text.mid(start, i - start).toInt();
            } else {
                offset = 1; // ".+" や "$-" のように数字が省略された場合は 1 とする
            }
            
            if (sign == '+') {
                line += offset;
            } else {
                line -= offset;
            }
        }
    }
    // ドキュメントの実際の範囲内にクランプ（丸め込み）
    if (line < 1) line = 1;
    if (line > totalLines) line = totalLines;

    return line;
}
//	cmd が pat にマッチするか？
//	pat は ( を含み、( 直前までマッチすればおｋ
//	( 以降は何文字マッチしてもおｋ、だが不一致は false を返す
//	pat: "q(uit" の場合、"q", "qu", "qui", "quit" でマッチ、"qx" は不一致
//	pat: "quit" の場合、"quit" のみがマッチ
bool is_match(const QString &cmd, const QString &pat) {
	int i = 0, k = 0;
	bool paren = false;		//	'(' フラグ
	while( i < cmd.size() && k < pat.size() ) {
		if( pat[k] == '(' ) {
			paren = true;
			++k;
			continue;
		}
		if( cmd[i++] != pat[k++] ) return false;	//	不一致
	}
	if( i != cmd.size() ) return false;	//	cmd の最後までマッチしていない
	return k == pat.size() || (k < pat.size() && pat[k] == '(') || paren;	//	次が '(' または既に発見
}
bool parse_subst(const QString &text, int &ix, QString &pat, QString &after, bool &global) {
	if (ix >= text.size()) {
		return false;
	}
	QChar delim = text[ix++]; // 開始デリミタを決定（通常は '/'）

	// 1. パターン（pat）の抽出とエスケープ解除
	pat.clear();
	while (ix < text.size() && text[ix] != delim) {
		if (text[ix] == '\\') {
			ix++;
			if (ix >= text.size()) {
				pat += '\\';
				break;
			}
			if (text[ix] == delim) {
				pat += text[ix++]; // デリミタのエスケープを解除してパターンに追加
			} else {
				pat += '\\';
				pat += text[ix++];
			}
		} else {
			pat += text[ix++];
		}
	}

	if (ix < text.size() && text[ix] == delim) {
		ix++; // 閉じデリミタをスキップ
	} else {
		return false; // デリミタが一致しない不正な構文
	}

	// --- 空パターンの場合は前回値を参照。新規パターンなら保存する処理を追加 ---
	if (pat.isEmpty()) {
		if (gvi.m_regexp.isEmpty()) {
			return false; // 前回パターンが存在しない場合はエラー（パース失敗）
		}
		pat = gvi.m_regexp; // 前回保存したパターンを再利用
	} else {
		gvi.m_regexp = pat; // 新しく指定されたパターンを保存
	}

	// 2. 置換後文字列（after）の抽出とエスケープ解除
	after.clear();
	while (ix < text.size() && text[ix] != delim) {
		if (text[ix] == '\\') {
			ix++;
			if (ix >= text.size()) {
				after += '\\';
				break;
			}
			if (text[ix] == delim || text[ix] == '\\') {
				after += text[ix++]; // デリミタやバックスラッシュのエスケープを解除
			} else {
				after += '\\';
				after += text[ix++];
			}
		} else {
			after += text[ix++];
		}
	}

	if (ix < text.size() && text[ix] == delim) {
		ix++; // 閉じデリミタをスキップ
	}

	// 3. フラグ（g）の解析
	global = false;
	while (ix < text.size()) {
		QChar flag = text[ix];
		if (flag == 'g') {
			global = true;
		} else {
			break;
		}
		ix++;
	}

	return true;
}
void MainWindow::do_subst(const QString &text, int ix) {
	QString pat;
	QString after;
	bool global = false;

	// パースの実行
	if (!parse_subst(text, ix, pat, after, global)) {
		statusBar()->showMessage(tr("Invalid substitute syntax."), 5000);
		return;
	}
	qDebug() << "pat = " << pat;
	qDebug() << "after = " << after;
	qDebug() << "global = " << global;
}
void MainWindow::on_cmdLine_enter() {
	qDebug() << "MainWindow::on_cmdLine_enter()";
	m_cmdLine->hide();
	QString text = m_cmdLine->text();
	qDebug() << "cmdline text = " << text;
	close_cmdLine();
    DocWidget *docWidget = getCurDocWidget();
    if( docWidget == nullptr ) return;
	QTextCursor cursor = gvi.m_prevFocusWidget == (QWidget*)docWidget->m_editor ?
							docWidget->m_editor->textCursor() : docWidget->m_preview->textCursor();
	QTextDocument *doc = cursor.document();
	int totalLines = doc->blockCount();
	if( doc->lastBlock().text().isEmpty() && totalLines > 1 ) --totalLines;
	int ix = 1;		//	skip ':'
	gvi.m_rangeStart = 1;
	if( ix < text.size() && text[ix] == '%' ) {
	    ++ix;
	    gvi.m_rangeStart = 1;
	    gvi.m_rangeEnd   = totalLines;
	} else {
		for(;;) {
			gvi.m_rangeEnd = parseLineSpec(text, ix, cursor.block().blockNumber()+1, totalLines, doc);
			qDebug() << "line = " << gvi.m_rangeEnd;
			if( ix == 1 ) break;	//	行番号無し
			if( gvi.m_rangeEnd < 0 ) return;
			if( ix >= text.size() || text[ix] != ',' ) break;
			++ix;
			gvi.m_rangeStart = gvi.m_rangeEnd;
		}
	}
	if( ix >= text.size() ) {	//	:{range} Enter の場合
		QTextBlock block = doc->findBlockByNumber(gvi.m_rangeEnd - 1);
		cursor.setPosition(block.position());
		hat(cursor);
		if( gvi.m_prevFocusWidget == (QWidget*)docWidget->m_editor )
			docWidget->m_editor->setTextCursor(cursor);
		else
			docWidget->m_preview->setTextCursor(cursor);
		gvi.m_exhist[0] = text;
		gvi.m_exhist_ix = 0;
			return;
	}
	if( gvi.m_rangeStart > gvi.m_rangeEnd )
		std::swap(gvi.m_rangeStart, gvi.m_rangeEnd);
	QString cmd;
	while( ix < text.size() && text[ix].isLetter() ) cmd += text[ix++];
	QChar nch = ix < text.size() ? text[ix] : u'\0';
	if( is_match(cmd, "q(uit") ) {
		do_close(nch == u'!');
	} else if( is_match(cmd, "w(rite") ) {
		onAction_Save();
	} else if( is_match(cmd, "d(elete") ) {
		QTextBlock startBlock = doc->findBlockByNumber(gvi.m_rangeStart - 1);
		QTextBlock endBlock = doc->findBlockByNumber(gvi.m_rangeEnd - 1);
		if( startBlock.isValid() && endBlock.isValid() ) {
			QTextCursor cursor(doc);
			cursor.beginEditBlock(); // Undo/Redo を1回にまとめる
			
			int startPos = startBlock.position();
			int endPos;
			
			if( endBlock.next().isValid() ) {
				// 次の行が存在する場合は、次の行の開始位置まで選択（対象行の末尾改行も削除に含める）
				endPos = endBlock.next().position();
			} else {
				// 最終行を削除する場合
				endPos = doc->characterCount() - 1;
				// 前の行が存在する場合は、前の行の末尾改行から削除対象に含める（不要な空行残りを防止）
				if( startBlock.previous().isValid() ) {
					startPos = startBlock.previous().position() + startBlock.previous().length() - 1;
				}
			}
			
			cursor.setPosition(startPos);
			cursor.setPosition(endPos, QTextCursor::KeepAnchor);
			cursor.removeSelectedText();
			cursor.endEditBlock();
		}
	} else if( is_match(cmd, "p(rint") ) {
		for(int ln = gvi.m_rangeStart; ln <= gvi.m_rangeEnd; ++ln) {
			QTextBlock block = doc->findBlockByNumber(ln - 1);
			if( !block.isValid() ) break;
			do_output(block.text() + "\n");
		}
	} else if( is_match(cmd, "P(RINT") || is_match(cmd, "nu(mber") ) {
		do_output("\n\"" + docWidget->m_fullPath + "\"\n");
		for(int ln = gvi.m_rangeStart; ln <= gvi.m_rangeEnd; ++ln) {
			QTextBlock block = doc->findBlockByNumber(ln - 1);
			if( !block.isValid() ) break;
			do_output(QString("%1:").arg(ln, 4) + block.text() + "\n");
		}
	} else if( is_match(cmd, "s(ubstitute") ) {
		do_subst(text, ix);
	} else {
		statusBar()->showMessage(tr("illegal command."), 5000);
	}
	gvi.m_exhist[0] = text;
	gvi.m_exhist_ix = 0;
}
void MainWindow::on_cmdLine_escape() {
	qDebug() << "MainWindow::on_cmdLine_escape()";
	close_cmdLine();
}
void MainWindow::on_cmdLine_up() {
	qDebug() << "MainWindow::on_cmdLine_up()";
	if( gvi.m_exhist_ix + 1 < gvi.m_exhist.size() ) {
		if( gvi.m_exhist_ix == 0 ) gvi.m_exhist[0] = m_cmdLine->text();
		m_cmdLine->setText(gvi.m_exhist[++gvi.m_exhist_ix]);
	}
}
void MainWindow::on_cmdLine_down() {
	qDebug() << "MainWindow::on_cmdLine_down()";
	if( gvi.m_exhist_ix > 0 ) {
		m_cmdLine->setText(gvi.m_exhist[--gvi.m_exhist_ix]);
	}
}
void MainWindow::close_cmdLine() {
	m_cmdLine->hide();
	gvi.m_cmdlineMode = false;
	gvi.m_pendingCommand.clear();
	gvi.m_prevFocusWidget->setFocus();
}
