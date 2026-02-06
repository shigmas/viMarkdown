#pragma once

#include <QDialog>
#include "ui_ReplaceDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ReplaceDialogClass; };
QT_END_NAMESPACE

class ReplaceDialog : public QDialog
{
	Q_OBJECT

public:
	ReplaceDialog(QWidget *parent = nullptr);
	~ReplaceDialog();

private:
	Ui::ReplaceDialogClass *ui;
};

