#pragma once

#include <QDialog>
#include "ui_LangDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LangDialogClass; };
QT_END_NAMESPACE

class LangDialog : public QDialog
{
	Q_OBJECT

public:
	LangDialog(QWidget *parent = nullptr);
	~LangDialog();

protected:
	void accept() override;

private:
	Ui::LangDialogClass *ui;
};

