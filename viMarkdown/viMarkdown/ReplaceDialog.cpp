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
	connect(ui->searchPrev, &QPushButton::clicked, this, &ReplaceDialog::onSearchPrev);
	connect(ui->searchNext, &QPushButton::clicked, this, &ReplaceDialog::onSearchNext);
	connect(ui->replaceNext, &QPushButton::clicked, this, &ReplaceDialog::onReplaceNext);
}

ReplaceDialog::~ReplaceDialog()
{
	delete ui;
}
void ReplaceDialog::onSearchPrev() {
	QString txt = ui->searchCB->currentText();
	if( txt.isEmpty() ) return;
	emit do_search(txt, true);
}
void ReplaceDialog::onSearchNext() {
	QString txt = ui->searchCB->currentText();
	if( txt.isEmpty() ) return;
	emit do_search(txt, false);
}
void ReplaceDialog::onReplaceNext() {
	QString src = ui->searchCB->currentText();
	QString dst = ui->replaceCB->currentText();
	if( src.isEmpty() ) return;
	emit do_replace_next(src, dst);
}

