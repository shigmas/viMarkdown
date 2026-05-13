#include "OutputView.h"

OutputView::OutputView(QWidget *parent)
	: QPlainTextEdit(parent)
{
	qDebug() << "OutputView::OutputView()";
}

OutputView::~OutputView()
{}

