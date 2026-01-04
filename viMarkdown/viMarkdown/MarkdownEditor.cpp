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
	} else if( m_mainWindow->isKeisenMode() && (e->modifiers() & Qt::ControlModifier) != 0 ) {
		if (e->key() == Qt::Key_Right ) {
			do_keisen_right();
			return;
		} else if (e->key() == Qt::Key_Left) {
			do_keisen_left();
			return;
		} else if (e->key() == Qt::Key_Up) {
			do_keisen_up();
			return;
		} else if (e->key() == Qt::Key_Down) {
			do_keisen_down();
			return;
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
//
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
//
QString getUpSrcString(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'←' || txt[ix] == u'└' ) return "└";
		if( txt[ix] == u'→' || txt[ix] == u'┘' ) return "┘";
		if( txt[ix] == u'─' || txt[ix] == u'┴' ) return "┴";
		if( txt[ix] == u'┐' || txt[ix] == u'┤' ) return "┤";
		if( txt[ix] == u'┌' || txt[ix] == u'├' ) return "├";
		if( txt[ix] == u'┬' || txt[ix] == u'┼' ) return "┼";
	}
	return "│";
}
QString getUpDstString(const QString txt, int ix) {
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
}
void MarkdownEditor::do_keisen_up() {
	QTextCursor cursor = this->textCursor();
	int ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getUpSrcString(cursor.block().text(), ix));
	cursor.movePosition(QTextCursor::Left);
	int vc = getVisualColumn(cursor, this);
	cursor.movePosition(QTextCursor::Up);
	ix = cursor.positionInBlock();
	int vc2 = getVisualColumn(cursor, this);
	if( vc2 < vc ) cursor.insertText(QString(vc-vc2, u' '));
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc + 2 )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getUpDstString(cursor.block().text(), ix));
	cursor.movePosition(QTextCursor::Left);
	setTextCursor(cursor);
}
QString getDownSrcString(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'→' || txt[ix] == u'┐' ) return "┐";
		if( txt[ix] == u'←' || txt[ix] == u'┌' ) return "┌";
		if( txt[ix] == u'─' || txt[ix] == u'┬' ) return "┬";
		if( txt[ix] == u'┘' || txt[ix] == u'┤' ) return "┤";
		if( txt[ix] == u'└' || txt[ix] == u'├' ) return "├";
		if( txt[ix] == u'┴' || txt[ix] == u'┼' ) return "┼";
	}
	return "│";
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getDownDstString(const QString txt, int ix) {
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
}
void MarkdownEditor::do_keisen_down() {
	QTextCursor cursor = this->textCursor();
	if (cursor.block() == cursor.document()->lastBlock()) {		//	カーソルが最終行にいる場合
		QTextCursor tempCursor(document());
	    tempCursor.movePosition(QTextCursor::End);
	    tempCursor.insertBlock();		//	新規行作成
	}
	int ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	cursor.insertText(getDownSrcString(cursor.block().text(), ix));
	cursor.movePosition(QTextCursor::Left);
	int vc = getVisualColumn(cursor, this);
	cursor.movePosition(QTextCursor::Down);
	int vc2 = getVisualColumn(cursor, this);
	if( vc2 < vc ) cursor.insertText(QString(vc-vc2, u' '));		//	カーソル位置まで空白挿入
	ix = cursor.positionInBlock();
	while( !cursor.atBlockEnd() && getVisualColumn(cursor, this) < vc + 2 )
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	QString dst = getDownDstString(cursor.block().text(), ix);
	cursor.insertText(dst);
	cursor.movePosition(QTextCursor::Left);
	setTextCursor(cursor);
}
QString getLeftSrcString(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'↓' || txt[ix] == u'┘' ) return "┘";
		if( txt[ix] == u'↑' || txt[ix] == u'┐' ) return "┐";
		if( txt[ix] == u'│' || txt[ix] == u'┤' ) return "┤";
		if( txt[ix] == u'└' || txt[ix] == u'┴' ) return "┴";
		if( txt[ix] == u'┌' || txt[ix] == u'┬' ) return "┬";
		if( txt[ix] == u'├' || txt[ix] == u'┼' ) return "┼";
	}
	return "─";
}
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
QString getLeftDstString(const QString txt, int ix) {
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
}
void MarkdownEditor::do_keisen_left() {
	QTextCursor cursor = this->textCursor();
	if( cursor.atBlockStart() ) return;				//	行頭にいる場合は無視
	int vc = getVisualColumn(cursor, this);
	QString src = "─";
	if( !cursor.atBlockEnd() ) {
		int ix = cursor.positionInBlock();
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		src = getLeftSrcString(cursor.block().text(), ix);
		cursor.clearSelection();	//	選択解除、カーソル位置は movePosition() 後の位置、つまり選択末尾
	}
	while( !cursor.atBlockStart() && getVisualColumn(cursor, this) > vc - 2 )
		cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
	QString dst = getLeftDstString(cursor.block().text(), cursor.positionInBlock());
	cursor.insertText(dst+src);
	cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 2);
	setTextCursor(cursor);
}
//
//┌┬┐┌─→
//├┼┤│
//└┴┘↓
//
QString getRightSrcString(const QString txt, int ix) {
	if( ix < txt.size() ) {
		if( txt[ix] == u'↑' || txt[ix] == u'┌' ) return "┌";
		if( txt[ix] == u'↓' || txt[ix] == u'└' ) return "└";
		if( txt[ix] == u'│' || txt[ix] == u'├' ) return "├";
		if( txt[ix] == u'┘' || txt[ix] == u'┴' ) return "┴";
		if( txt[ix] == u'┐' || txt[ix] == u'┬' ) return "┬";
		if( txt[ix] == u'┤' || txt[ix] == u'┼' ) return "┼";
	}
	return "─";
}
QString getRightDstString(const QString txt, int ix) {
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
}
void MarkdownEditor::do_keisen_right() {
	QTextCursor cursor = this->textCursor();
	QString str = "─";
	int ix = cursor.positionInBlock();
	if( !cursor.atBlockEnd() ) {
		int vc = getVisualColumn(cursor, this);
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while( getVisualColumn(cursor, this) < vc + 2);
		str = getRightSrcString(cursor.block().text(), ix);
	}
	QString str2 = "→";
	if (!cursor.atBlockEnd()) {
		int ix = cursor.positionInBlock();;
		int vc = getVisualColumn(cursor, this);
		do {
			cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
		} while( getVisualColumn(cursor, this) < vc + 2);
		str2 = getRightDstString(cursor.block().text(), ix);
	}
	cursor.insertText(str + str2);
	cursor.movePosition(QTextCursor::Left);
	setTextCursor(cursor);
}
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
