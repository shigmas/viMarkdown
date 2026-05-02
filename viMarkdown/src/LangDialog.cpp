#include "LangDialog.h"
#include "ui_LangDialog.h"

LangDialog::LangDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::LangDialogClass())
{
	ui->setupUi(this);
}

LangDialog::~LangDialog()
{
	delete ui;
}

