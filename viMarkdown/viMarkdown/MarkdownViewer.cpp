#include <QTextDocument>
#include <QTextBlock>
#include "MarkdownViewer.h"

void MarkdownViewer::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton) {
	    // 1. クリックされた位置のカーソルオブジェクトを取得
	    // viewport()->mapFromGlobal(e->globalPos()) ではなく e->pos() でOKです
	    QTextCursor cursor = cursorForPosition(e->pos());

	    // 2. その位置のブロック番号を取得（0ベース）
	    int blockNumber = cursor.blockNumber();

	    // 3. シグナルを発行
	    emit lineClicked(blockNumber);
	}
    QTextEdit::mousePressEvent(e);
}
void MarkdownViewer::setMarkdown(QTextDocument *doc) {
	const QString mdtext = doc->toPlainText();
#if 1
	this->clear();
	QTextCursor cursor(this->document());
	cursor.movePosition(QTextCursor::Start);
	QStringList lst = mdtext.split(u'\n');
	for(int ln = 0; ln < lst.size(); ++ln) {
		const QString &buf = lst[ln];
		cursor.insertText(buf);
		cursor.insertBlock();
	}
	qDebug() << "blockCount() = " << this->document()->blockCount();
#else
	setPlainText(mdtext);
#endif
}
