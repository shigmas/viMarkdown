#include <QSettings>
#include "ReplaceDialog.h"

ReplaceDialog::ReplaceDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::ReplaceDialogClass())
{
	ui->setupUi(this);
	QSettings settings;
	QStringList history = settings.value("search/history").toStringList();
    ui->searchCB->clear();
	ui->searchCB->addItems(history);
}

ReplaceDialog::~ReplaceDialog()
{
	delete ui;
}

