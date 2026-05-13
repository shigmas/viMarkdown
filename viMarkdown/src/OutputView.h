#pragma once

#include <QPlainTextEdit>

class OutputView  : public QPlainTextEdit
{
	Q_OBJECT

public:
	OutputView(QWidget *parent);
	~OutputView();

signals:
	void	do_open(const QString path, int ln);

protected:
    void	mouseDoubleClickEvent(QMouseEvent *e) override;
};

