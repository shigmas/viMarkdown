#include <QTextBlock>
#include "OutputView.h"

OutputView::OutputView(QWidget *parent)
	: QPlainTextEdit(parent)
{
	qDebug() << "OutputView::OutputView()";
}

OutputView::~OutputView()
{}

void OutputView::mouseDoubleClickEvent(QMouseEvent *e) {
	QTextCursor cursor = cursorForPosition(e->pos());
	QTextBlock block = cursor.block();
	qDebug() << "text = " << block.text();
	int ln = block.text().section(':', 0, 0).trimmed().toInt();
	qDebug() << "ln = " << ln;
	if( ln <= 0 ) return;
	do {
		if( !(block = block.previous()).isValid() ) return;
	} while( !block.text().startsWith('"') );
	QString path = block.text();
	path = path.mid(1, path.size() - 2);		//	前後の " を削除
	qDebug() << "path = " << path;
	emit do_open(path, ln-1);
}
