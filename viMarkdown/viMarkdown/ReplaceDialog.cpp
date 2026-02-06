#include "ReplaceDialog.h"

ReplaceDialog::ReplaceDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::ReplaceDialogClass())
{
	ui->setupUi(this);
}

ReplaceDialog::~ReplaceDialog()
{
	delete ui;
}

