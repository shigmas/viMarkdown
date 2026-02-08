#include <QSettings>
#include "ReplaceDialog.h"
#include "MainWindow.h"

extern Global g;

ReplaceDialog::ReplaceDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::ReplaceDialogClass())
{
	ui->setupUi(this);
	QSettings settings;
	QStringList history = settings.value("search/history").toStringList();
    ui->searchCB->clear();
	ui->searchCB->addItems(history);
	ui->ignoreCase->setCheckState(g.m_ignoreCase ? Qt::Checked : Qt::Unchecked);
	connect(ui->searchPrev, &QPushButton::clicked, this, &ReplaceDialog::onSearchPrev);
	connect(ui->searchNext, &QPushButton::clicked, this, &ReplaceDialog::onSearchNext);
	connect(ui->replaceNext, &QPushButton::clicked, this, &ReplaceDialog::onReplaceNext);
	connect(ui->ignoreCase, &QCheckBox::checkStateChanged, this, &ReplaceDialog::onCheckStateChanged);
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
void ReplaceDialog::onCheckStateChanged(Qt::CheckState state) {
	g.m_ignoreCase = state == Qt::Checked;
	qDebug() << "g.m_ignoreCase = " << g.m_ignoreCase;
}
