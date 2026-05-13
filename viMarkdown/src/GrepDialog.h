#pragma once

#include <QDialog>
#include "ui_GrepDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GrepDialogClass; };
QT_END_NAMESPACE

class GrepDialog : public QDialog
{
	Q_OBJECT

public:
	GrepDialog(QWidget *parent = nullptr);
	~GrepDialog();

	const QString searchText() const { return ui->searchTextCB->currentText(); }
	const QString dirText() const { return ui->dirCB->currentText(); }

private:
	Ui::GrepDialogClass *ui;
};

