#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextBlock>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include "MarkdownEditor.h"

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
MarkdownEditor::MarkdownEditor(QWidget *parent) : QPlainTextEdit(parent)
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
	if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
		QTextCursor cursor = this->textCursor();
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
	}
    QPlainTextEdit::keyPressEvent(e);	// Enter 以外のキーは通常通りの処理
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
