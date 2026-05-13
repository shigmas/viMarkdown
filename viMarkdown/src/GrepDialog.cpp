#include <QDir>
#include "GrepDialog.h"

GrepDialog::GrepDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::GrepDialogClass())
{
	ui->setupUi(this);
	QString currentPath = QDir::currentPath();
	ui->dirCB->addItem(currentPath);
	ui->dirCB->setCurrentText(currentPath);
	//
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

GrepDialog::~GrepDialog()
{
	delete ui;
}

