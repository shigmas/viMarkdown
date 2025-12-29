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
		QString buf = lst[ln];
		if( buf.startsWith('#') ) {
			do_heading(cursor, buf);
		} else {
			cursor.insertText(buf);
			QTextBlockFormat format;
			format.setAlignment(Qt::AlignJustify);
			cursor.mergeBlockFormat(format);
			QTextCharFormat charFormat;
			charFormat.setFontPointSize(12);
			//charFormat.clearProperty(QTextFormat::FontWeight);
		    charFormat.setFontWeight(QFont::Normal);
		    cursor.select(QTextCursor::BlockUnderCursor);
		    cursor.mergeCharFormat(charFormat);
		    cursor.clearSelection();
		}
		cursor.insertBlock();
	}
	qDebug() << "blockCount() = " << this->document()->blockCount();
#else
	setPlainText(mdtext);
#endif
}
int h_font_size[] = {12, 25, 21, 18, 16, 14, 12};

void MarkdownViewer::do_heading(QTextCursor& cursor, QString buf) {
	int i = 1;
	while( i < buf.size() && buf[i] == '#' ) ++i;
	int h = std::min(6, i);		//	[1, 6]
	while( i < buf.size() && buf[i] == ' ' ) ++i;
	cursor.insertText(buf.mid(i));
	//QTextBlock block = cursor.block();
	QTextBlockFormat format;
	format.setHeadingLevel(h); // Hx（見出しレベルx）に設定
	if( h == 1 )
		format.setAlignment(Qt::AlignCenter);
	cursor.mergeBlockFormat(format);

	QTextCharFormat charFormat;
	charFormat.setFontPointSize(h_font_size[h]);
    charFormat.setFontWeight(QFont::Bold);
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.mergeCharFormat(charFormat);
    cursor.clearSelection();
}
