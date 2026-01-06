#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextBlock>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include "MarkdownEditor.h"
#include "MainWindow.h"

class MarkdownHighlighter : public QSyntaxHighlighter {
public:
    MarkdownHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
    {
    	boldFormat.setFontWeight(QFont::Bold);
    	boldRegex = QRegularExpression(R"(\*\*([^\*]+)\*\*)");
    }

protected:
    void highlightBlock(const QString &text) override {
        if (text.startsWith("#")) {
            QTextCharFormat fmt_darkred;
            fmt_darkred.setForeground(QColor("darkred"));
            setFormat(0, text.length(), fmt_darkred);
        } else {
            // デフォルトの色（黒）
            setFormat(0, text.length(), QColor("black"));
            auto it = boldRegex.globalMatch(text);
	        while (it.hasNext()) {
	            QRegularExpressionMatch match = it.next();
	            // マッチした範囲（開始位置と長さ）にフォーマットを適用
	            setFormat(match.capturedStart(), match.capturedLength(), boldFormat);
	        }
        }
    }
private:
    QTextCharFormat boldFormat;
    QRegularExpression boldRegex;
};
//----------------------------------------------------------------------
MarkdownEditor::MarkdownEditor(const MainWindow* mainWindow, QWidget *parent)
	: m_mainWindow(mainWindow), QPlainTextEdit(parent)
{
	m_highlighter = new MarkdownHighlighter(this->document());
#if 1
	QTextCursor cursor = this->textCursor();
    QTextBlockFormat format;
    format.setLineHeight(150, QTextBlockFormat::ProportionalHeight); // 1.5倍
    cursor.setBlockFormat(format);
#endif
}
void MarkdownEditor::keyPressEvent(QKeyEvent *e) {
	//static QRegularExpression re(R"(^\d[\.\)] )");
	static QRegularExpression re(R"(^\d\. )");
	static QRegularExpression re2(R"(^\d\) )");
	QTextCursor cursor = this->textCursor();
	if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        QTextBlock currentBlock = cursor.block();
        if( (e->modifiers() & Qt::ShiftModifier) == 0 &&		//	Shift + Enter でない
        	cursor.position() != currentBlock.position())		//	行頭にいない場合
        {
	        QString text = currentBlock.text();
	        int n = 0;
	        while( n < text.length() && text[n].isSpace() ) ++n;
	        QString atxt = text.left(n);
	        const QString mtxt = text.mid(n);
	        if( mtxt.startsWith("- [ ] ") )
				atxt += "- [ ] ";
	        else if( mtxt.startsWith("- [x] ") )
				atxt += "- [x] ";
	        else if( mtxt.startsWith("- [X] ") )
				atxt += "- [X] ";
	        else if( mtxt.startsWith("- ") )
				atxt += "- ";
	        else if( re.match(mtxt).hasMatch())
				atxt += "1. ";
	        else if( re2.match(mtxt).hasMatch())
				atxt += "1) ";
	        else if( mtxt.startsWith("> ") )
				atxt += "> ";
	        //cursor.insertText("\n" + atxt);
	        QPlainTextEdit::keyPressEvent(e);		//	改行挿入
	        cursor.insertText(atxt);
	        // カーソル位置を画面内に維持
	        this->ensureCursorVisible();
			return;
        } else {
	        cursor.insertText("\n");
			return;
        }
	} else if (e->key() == Qt::Key_Tab ) {
		emit tab_pressed();
		return;
	} else if (e->key() == Qt::Key_Escape ) {
		emit esc_pressed();
		return;
	} else if( m_mainWindow->isKeisenMode() ) {
		bool erase = (e->modifiers() & Qt::ShiftModifier) != 0;
		if( (e->modifiers() & Qt::ControlModifier) != 0 /*|| erase*/ ) {
			if (e->key() == Qt::Key_Right ) {
				do_keisen_right(erase);
				return;
			} else if (e->key() == Qt::Key_Left) {
				do_keisen_left(erase);
				return;
			} else if (e->key() == Qt::Key_Up) {
				do_keisen_up(erase);
				return;
			} else if (e->key() == Qt::Key_Down) {
				do_keisen_down(erase);
				return;
			}
		}
	}
    QPlainTextEdit::keyPressEvent(e);	// Enter 以外のキーは通常通りの処理
}
int getVisualColumn(QTextCursor cursor, QPlainTextEdit *editor) {
    // 1. 行頭から現在のカーソル位置までのテキストを取得
    QString text = cursor.block().text().left(cursor.positionInBlock());
    // 2. フォントの計測準備
    QFontMetrics fm(editor->font());
    // 3. 半角文字（例: 'A'）1文字分の幅を取得
    int halfWidth = fm.horizontalAdvance("A"); 
    // 4. 行頭からカーソルまでの全テキストの幅を取得
    int fullWidth = fm.horizontalAdvance(text);
    // 5. 割り算で「半角何文字分か」を出す
    return fullWidth / halfWidth;
}
QString getUpSrcString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'←' || txt[ix] == u'└' ) return "└";
			if( txt[ix] == u'→' || txt[ix] == u'┘' ) return "┘";
			if( txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┐' || txt[ix] == u'┤' ) return "┤";
			if( txt[ix] == u'┌' || txt[ix] == u'├' ) return "├";
			if( txt[ix] == u'┬' || txt[ix] == u'┼' ) return "┼";
			if( txt[ix] == u'─' )
				if( ix < 2 )		//	undone: 左に文字がある場合
					return "└";
				else
					return "┴";
		}
		return "│";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'─' || txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' )
				return txt[ix];		//	変化無し
			if( txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' )
				return "─";
			if( txt[ix] == u'┼' ) return "┬";
			if( txt[ix] == u'├' ) return "┌";
	        if( txt[ix] == u'┤' ) return "┐";
		}
		return "  ";
	}
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getUpDstString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'│' || txt[ix] == u'┐' || txt[ix] == u'┌' || 
				txt[ix] == u'┬' || txt[ix] == u'├' || txt[ix] == u'┤'|| txt[ix] == u'┼' )
			{
				return txt[ix];
			}
			if( txt[ix] == u'┘' ) return "┤";
			if( txt[ix] == u'└' ) return "├";
			if( txt[ix] == u'┴' ) return "┼";
			if( txt[ix] == u'─' ) {
				if( ix > 0 && (txt[ix-1] == u'─' || txt[ix-1] == u'←') )
					return "┬";
				else
					return "┌";
			}
		}
		return "↑";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'─' || txt[ix] == u'┘' || txt[ix] == u'└' || txt[ix] == u'┴' )
				return txt[ix];		//	変化無し
			if( txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'├' ) return "─";
			if( txt[ix] == u'┼' ) return "┴";
	        if( txt[ix] == u'├' ) return "└";
	        if( txt[ix] == u'┤' ) return "┘";
		}
		return "  ";
	}
}
void MarkdownEditor::do_keisen_up(bool erase) {
	QTextCursor cursor = this->textCursor();
	if( cursor.blockNumber() == 0 ) return;		//	１行目では実行不可
	cursor.beginEditBlock();
	int vc0 = getVisualColumn(cursor, this); // 開始位置の表示列(VC)を保存
	int ix = cursor.positionInBlock();

// 1. 移動元（現在地）の置換
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 + 2 )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getUpSrcString(erase, cursor.block().text(), ix));
	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);

	// 2. 上の行へ移動し、表示列(vc0)に正確に合わせる
	cursor.movePosition(QTextCursor::Up);
	cursor.movePosition(QTextCursor::StartOfBlock); // 行頭から辿るのが全角混じりでも最も確実
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 ) cursor.movePosition(QTextCursor::Right);
	
	// 足りなければスペース補完（これで確実に vc0 に到達する）
	int currentVc = getVisualColumn(cursor, this);
	if( currentVc < vc0 ) cursor.insertText(QString(vc0 - currentVc, u' '));

	// 3. 移動先の置換
	ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() ) cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor); // 1文字だけ選択
	cursor.insertText(getUpDstString(erase, cursor.block().text(), ix));	
	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);
	cursor.endEditBlock();
	setTextCursor(cursor);
}
QString getDownSrcString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'→' || txt[ix] == u'┐' ) return "┐";
			if( txt[ix] == u'←' || txt[ix] == u'┌' ) return "┌";
			if( txt[ix] == u'┬' ) return "┬";
			if( txt[ix] == u'┘' || txt[ix] == u'┤' ) return "┤";
			if( txt[ix] == u'└' || txt[ix] == u'├' ) return "├";
			if( txt[ix] == u'┴' || txt[ix] == u'┼' ) return "┼";
			if( txt[ix] == u'─' )
				if( ix < 2 )		//	undone: 左に文字がある場合
					return "┌";
				else
					return "┬";
		}
		return "│";
	} else {
		if( ix < txt.size() ) {
			// 変化無し（すでに下に線がない）
			if( txt[ix] == u'─' || txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' || txt[ix] == u'←' || txt[ix] == u'→' ) return txt[ix];
			// 縦要素が消えて横棒だけ残る
			if( txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' ) return "─";
			// Ｔ字・角に格下げ
			if( txt[ix] == u'┼' ) return "┴";
			if( txt[ix] == u'├' ) return "└";
			if( txt[ix] == u'┤' ) return "┘";
			if( txt[ix] == u'│' ) return "  ";
		}
		return "  ";
	}
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getDownDstString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'│' || txt[ix] == u'┘' || txt[ix] == u'└' || 
				/*txt[ix] == u'┬' ||*/ txt[ix] == u'├' || txt[ix] == u'┤'|| txt[ix] == u'┼' )
			{
				return txt[ix];
			}
			if( txt[ix] == u'─' ) return "┴";
			if( txt[ix] == u'┐' ) return "┤";
			if( txt[ix] == u'┌' ) return "├";
			if( txt[ix] == u'┴' ) return "┼";		//	undone: この下も参照？
			if( txt[ix] == u'┬' ) return "┼";
		}
		return "↓";
	} else {
		if( ix < txt.size() ) {
			// 変化無し（すでに上に線がない）
			if( txt[ix] == u'─' || txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' || txt[ix] == u'←' || txt[ix] == u'→' ) return txt[ix];
			// 縦要素が消えて横棒だけ残る
			if( txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' ) return "─";
			// Ｔ字・角に格下げ
			if( txt[ix] == u'┼' ) return "┬";
			if( txt[ix] == u'├' ) return "┌";
			if( txt[ix] == u'┤' ) return "┐";
		}
		return "  ";
	}
}
void MarkdownEditor::do_keisen_down(bool erase) {
	QTextCursor cursor = this->textCursor();
	cursor.beginEditBlock();
	int vc0 = getVisualColumn(cursor, this);
	int ix = cursor.positionInBlock();

	if (cursor.block() == cursor.document()->lastBlock()) {		//	カーソルが最終行にいる場合
	    cursor.movePosition(QTextCursor::End);
	    cursor.insertBlock();							//	新規行作成
		cursor.movePosition(QTextCursor::Left);			//	新規行作成で下に移動したカーソルを元の行に戻す
		while( getVisualColumn(cursor, this) > vc0 )
			cursor.movePosition(QTextCursor::Left);
	}
	// 1. 移動元（現在地）の置換
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 + 2 ) 
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	QString src = getDownSrcString(erase, cursor.block().text(), ix);
	cursor.insertText(src);
	cursor.movePosition(QTextCursor::Left);


	// 2. 下の行の準備（なければ作成）
	if (cursor.block() == cursor.document()->lastBlock()) {
		cursor.movePosition(QTextCursor::EndOfBlock);
		cursor.insertBlock(); // これでカーソル自体が新行へ移動する
	} else {
		cursor.movePosition(QTextCursor::Down);
	}

	// 3. 表示列(vc0)に正確に合わせる
	cursor.movePosition(QTextCursor::StartOfBlock);
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 )
		cursor.movePosition(QTextCursor::Right);
	int curVc = getVisualColumn(cursor, this);
	if( curVc < vc0 ) cursor.insertText(QString(vc0 - curVc, u' ')); // 足りない分を補完

	// 4. 移動先（下）の置換
	ix = cursor.positionInBlock(); // 位置が確定してからインデックス取得
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc0 + 2 )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getDownDstString(erase, cursor.block().text(), ix));

	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);
	cursor.endEditBlock();
	setTextCursor(cursor);
}
QString getLeftSrcString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'↓' || txt[ix] == u'┘' ) return "┘";
			if( txt[ix] == u'↑' || txt[ix] == u'┐' ) return "┐";
			if( txt[ix] == u'│' || txt[ix] == u'┤' ) return "┤";
			if( txt[ix] == u'└' || txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┌' || txt[ix] == u'┬' ) return "┬";
			if( txt[ix] == u'├' || txt[ix] == u'┼' ) return "┼";
		}
		return "─";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'│' || txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' )
				return txt[ix];		//	変化無し
			if( txt[ix] == u'┘' || txt[ix] == u'┐' || txt[ix] == u'┤' )
				return "│";
			if( txt[ix] == u'┼' ) return "├";
		}
		return "  ";
	}
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getLeftDstString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'─' || txt[ix] == u'┌' || txt[ix] == u'┼')
				return txt[ix];
			if( txt[ix] == u'│' || txt[ix] == u'├' ) return "├";
			if( txt[ix] == u'↑' || txt[ix] == u'└' ) return "└";
			if( txt[ix] == u'↓' || txt[ix] == u'┌' ) return "┌";
			if( txt[ix] == u'┘' || txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┐' || txt[ix] == u'┬' ) return "┬";
			if( txt[ix] == u'┤' ) return "┼";
		}
		return "←";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'┐' || txt[ix] == u'┘' || txt[ix] == u'┤' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' ) return "│";
			if( txt[ix] == u'┼' ) return "┤";
		}
		return "  ";
	}
}
void MarkdownEditor::do_keisen_left(bool erase) {
	QTextCursor cursor = this->textCursor();
	if( cursor.atBlockStart() ) return;				//	行頭にいる場合は無視
	int vc = getVisualColumn(cursor, this);
	QString src = "─";
	if( !cursor.atBlockEnd() ) {
		int ix = cursor.positionInBlock();
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		src = getLeftSrcString(erase, cursor.block().text(), ix);
		cursor.clearSelection();	//	選択解除、カーソル位置は movePosition() 後の位置、つまり選択末尾
	}
	while( !cursor.atBlockStart() && getVisualColumn(cursor, this) > vc - 2 )
		cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
	QString dst = getLeftDstString(erase, cursor.block().text(), cursor.positionInBlock());
	cursor.insertText(dst+src);
	cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 2);
	setTextCursor(cursor);
}
QString getRightSrcString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'↑' || txt[ix] == u'┌' ) return "┌";
			if( txt[ix] == u'↓' || txt[ix] == u'└' ) return "└";
			if( txt[ix] == u'│' || txt[ix] == u'├' ) return "├";
			if( txt[ix] == u'┘' || txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┐' || txt[ix] == u'┬' ) return "┬";
			if( txt[ix] == u'┤' || txt[ix] == u'┼' ) return "┼";
		}
		return "─";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'│' || txt[ix] == u'┘' || txt[ix] == u'┐' || txt[ix] == u'┤' )
				return txt[ix];		//	変化無し
			if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' )
				return "│";
			if( txt[ix] == u'┼' ) return "┤";
		}
		return "  ";
	}
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getRightDstString(bool erase, const QString txt, int ix) {
	if( !erase ) {
		if( ix < txt.size() ) {
			if( txt[ix] == u'─' || txt[ix] == u'┐' || txt[ix] == u'┼')
				return txt[ix];
			if( txt[ix] == u'│' || txt[ix] == u'┤' ) return "┤";
			if( txt[ix] == u'↑' || txt[ix] == u'┐' ) return "┐";
			if( txt[ix] == u'↓' || txt[ix] == u'┘' ) return "┘";
			if( txt[ix] == u'└' || txt[ix] == u'┴' ) return "┴";
			if( txt[ix] == u'┌' || txt[ix] == u'┬' ) return "┬";
			if( txt[ix] == u'├' ) return "┼";
		}
		return "→";
	} else {
		if( ix < txt.size() ) {
			if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' ) return txt[ix];		//	変化無し
			if( txt[ix] == u'┐' || txt[ix] == u'┘' || txt[ix] == u'┤' ) return "│";
			if( txt[ix] == u'┼' ) return "├";
		}
		return "  ";
	}
}
void MarkdownEditor::do_keisen_right(bool erase) {
	QTextCursor cursor = this->textCursor();
	QString str = "─";
	int ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() ) {
		int vc = getVisualColumn(cursor, this);
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while( getVisualColumn(cursor, this) < vc + 2);
		str = getRightSrcString(erase, cursor.block().text(), ix);
	}
	QString str2 = "→";
	if (!cursor.atBlockEnd()) {
		int ix = cursor.positionInBlock();;
		int vc = getVisualColumn(cursor, this);
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc + 2);
		str2 = getRightDstString(erase, cursor.block().text(), ix);
	}
	cursor.insertText(str + str2);
	cursor.movePosition(QTextCursor::Left);
	setTextCursor(cursor);
}
#if 0
QString getRightSrcString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'│' || txt[ix] == u'┘' || txt[ix] == u'┐' || txt[ix] == u'┤' )
			return txt[ix];		//	変化無し
		if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' )
			return "│";
		if( txt[ix] == u'┼' ) return "┤";
	}
	return "  ";
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getRightDstString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' ) return txt[ix];		//	変化無し
		if( txt[ix] == u'┐' || txt[ix] == u'┘' || txt[ix] == u'┤' ) return "│";
		if( txt[ix] == u'┼' ) return "├";
	}
	return "  ";
}
void MarkdownEditor::do_keisen_right_erase() {
	QTextCursor cursor = this->textCursor();
	int vc0 = getVisualColumn(cursor, this);
	QString str = "  ";
	int ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() ) {
		int vc = getVisualColumn(cursor, this);
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while( getVisualColumn(cursor, this) < vc + 2);
		str = getRightSrcString_erase(cursor.block().text(), ix);
	}
	QString str2 = "  ";
	if (!cursor.atBlockEnd()) {
		int ix = cursor.positionInBlock();;
		int vc = getVisualColumn(cursor, this);
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while( getVisualColumn(cursor, this) < vc + 2);
		str2 = getRightDstString_erase(cursor.block().text(), ix);
	}
	cursor.insertText(str + str2);
	while( getVisualColumn(cursor, this) > vc0 + 2)
		cursor.movePosition(QTextCursor::Left);
	setTextCursor(cursor);
}
QString getLeftSrcString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'│' || txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' )
			return txt[ix];		//	変化無し
		if( txt[ix] == u'┘' || txt[ix] == u'┐' || txt[ix] == u'┤' )
			return "│";
		if( txt[ix] == u'┼' ) return "├";
	}
	return "  ";
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getLeftDstString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'┐' || txt[ix] == u'┘' || txt[ix] == u'┤' ) return txt[ix];		//	変化無し
		if( txt[ix] == u'└' || txt[ix] == u'┌' || txt[ix] == u'├' ) return "│";
		if( txt[ix] == u'┼' ) return "┤";
	}
	return "  ";
}
void MarkdownEditor::do_keisen_left_erase() {
	QTextCursor cursor = this->textCursor();
	if( cursor.atBlockStart() ) return;				//	行頭にいる場合は無視
	int vc = getVisualColumn(cursor, this);
	QString src = "  ";
	if( !cursor.atBlockEnd() ) {
		int ix = cursor.positionInBlock();
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while (getVisualColumn(cursor, this) < vc + 2);
		src = getLeftSrcString_erase(cursor.block().text(), ix);
		cursor.clearSelection();	//	選択解除、カーソル位置は movePosition() 後の位置、つまり選択末尾
	}
	while( !cursor.atBlockStart() && getVisualColumn(cursor, this) > vc - 2 )
		cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
	QString dst = getLeftDstString_erase(cursor.block().text(), cursor.positionInBlock());
	cursor.insertText(dst+src);
	while (!cursor.atBlockStart() && getVisualColumn(cursor, this) > vc - 2)
		cursor.movePosition(QTextCursor::Left);
	setTextCursor(cursor);
}
QString getUpSrcString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'─' || txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' )
			return txt[ix];		//	変化無し
		if( txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' )
			return "─";
		if( txt[ix] == u'┼' ) return "┬";
		if( txt[ix] == u'├' ) return "┌";
        if( txt[ix] == u'┤' ) return "┐";
	}
	return "  ";
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getUpDstString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'─' || txt[ix] == u'┘' || txt[ix] == u'└' || txt[ix] == u'┴' )
			return txt[ix];		//	変化無し
		if( txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'├' ) return "─";
		if( txt[ix] == u'┼' ) return "┴";
        if( txt[ix] == u'├' ) return "└";
        if( txt[ix] == u'┤' ) return "┘";
	}
	return "  ";
}
void MarkdownEditor::do_keisen_up_erase() {
	QTextCursor cursor = this->textCursor();
	cursor.beginEditBlock();
	int vc0 = getVisualColumn(cursor, this);
	int ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getUpSrcString_erase(cursor.block().text(), ix));
	cursor.movePosition(QTextCursor::Left);
	int vc = getVisualColumn(cursor, this);
	cursor.movePosition(QTextCursor::Up);
	ix = cursor.positionInBlock();
	int vc2 = getVisualColumn(cursor, this);
	if( vc2 < vc ) cursor.insertText(QString(vc-vc2, u' '));
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc + 2 )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getUpDstString_erase(cursor.block().text(), ix));
	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);
	cursor.endEditBlock();
	setTextCursor(cursor);
}
QString getDownSrcString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		// 変化無し（すでに下に線がない）
		if( txt[ix] == u'─' || txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' || txt[ix] == u'←' || txt[ix] == u'→' ) return txt[ix];
		// 縦要素が消えて横棒だけ残る
		if( txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' ) return "─";
		// Ｔ字・角に格下げ
		if( txt[ix] == u'┼' ) return "┴";
		if( txt[ix] == u'├' ) return "└";
		if( txt[ix] == u'┤' ) return "┘";
		if( txt[ix] == u'│' ) return "  ";
	}
	return "  ";
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getDownDstString_erase(const QString txt, int ix) {
	if( ix < txt.size() ) {
		// 変化無し（すでに上に線がない）
		if( txt[ix] == u'─' || txt[ix] == u'┌' || txt[ix] == u'┐' || txt[ix] == u'┬' || txt[ix] == u'←' || txt[ix] == u'→' ) return txt[ix];
		// 縦要素が消えて横棒だけ残る
		if( txt[ix] == u'└' || txt[ix] == u'┘' || txt[ix] == u'┴' ) return "─";
		// Ｔ字・角に格下げ
		if( txt[ix] == u'┼' ) return "┬";
		if( txt[ix] == u'├' ) return "┌";
		if( txt[ix] == u'┤' ) return "┐";
	}
	return "  ";
}
void MarkdownEditor::do_keisen_down_erase() {
	QTextCursor cursor = this->textCursor();
	cursor.beginEditBlock();
	int vc0 = getVisualColumn(cursor, this);
	if (cursor.block() == cursor.document()->lastBlock()) {		//	カーソルが最終行にいる場合
		QTextCursor tempCursor(document());
	    tempCursor.movePosition(QTextCursor::End);
	    tempCursor.insertBlock();		//	新規行作成
	}
	int ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getDownSrcString_erase(cursor.block().text(), ix));
	cursor.movePosition(QTextCursor::Left);
	int vc = getVisualColumn(cursor, this);
	cursor.movePosition(QTextCursor::Down);
	int vc2 = getVisualColumn(cursor, this);
	if( vc2 < vc ) cursor.insertText(QString(vc-vc2, u' '));		//	カーソル位置まで空白挿入
	ix = cursor.positionInBlock();
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc + 2 )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	QString dst = getDownDstString_erase(cursor.block().text(), ix);
	cursor.insertText(dst);
	while( getVisualColumn(cursor, this) > vc0 )
		cursor.movePosition(QTextCursor::Left);
	cursor.endEditBlock();
	setTextCursor(cursor);
}
#endif
//void MarkdownEditor::scrollToTop(int ln) {		//	ln: 0 org.
//}
void MarkdownEditor::scrollToTop(const QTextCursor &cursor) {
	int visualLineNum = getVisualLineNumber(cursor);
    this->scrollToTop(visualLineNum);
}
int MarkdownEditor::getVisualLineNumber(const QTextCursor &cursor) const {
	int visualLineNum = 0;
    QTextBlock targetBlock = cursor.block();

    // 1. カーソルがあるブロックより前の全ブロックの表示行数を合計する
    for (QTextBlock block = document()->begin(); block != targetBlock; block = block.next()) {
        if (block.isValid()) {
            // block.layout() から、そのブロックが何行に折り返されているかを取得
            visualLineNum += block.layout()->lineCount();
        }
    }

    // 2. カーソルがある現在のブロック内で、カーソルが「何行目の折り返し」にいるかを取得
    // cursor.positionInBlock() はブロック先頭からの文字数
    int relativePos = cursor.position() - targetBlock.position();
    
    // layout()->lineForTextPosition(n) で、文字位置 n が含まれる QTextLine を取得できる
    int lineInBlock = targetBlock.layout()->lineForTextPosition(relativePos).lineNumber();

    return visualLineNum + lineInBlock;
}
