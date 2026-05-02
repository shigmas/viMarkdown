#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class LangDialogClass; };
QT_END_NAMESPACE

class LangDialog  : public QDialog
{
	Q_OBJECT

public:
	LangDialog(QWidget *parent);
	~LangDialog();

private:
	Ui::LangDialogClass *ui;
};

