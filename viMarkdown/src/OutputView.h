#pragma once

#include <QPlainTextEdit>

class OutputView  : public QPlainTextEdit
{
	Q_OBJECT

public:
	OutputView(QWidget *parent);
	~OutputView();
};

