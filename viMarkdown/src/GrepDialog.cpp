#include <QDir>
#include "GrepDialog.h"
#include "MainWindow.h"

extern Global g;

GrepDialog::GrepDialog(const QStringList &hist, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::GrepDialogClass())
{
	ui->setupUi(this);
	QString currentPath = QDir::currentPath();
    ui->searchTextCB->clear();
	ui->searchTextCB->addItems(hist);
	ui->dirCB->addItem(currentPath);
	ui->dirCB->setCurrentText(currentPath);
	ui->ignoreCase->setCheckState(g.m_ignoreCase ? Qt::Checked : Qt::Unchecked);
	ui->regexp->setCheckState(g.m_regexp ? Qt::Checked : Qt::Unchecked);
	connect(ui->ignoreCase, &QCheckBox::checkStateChanged, this, &GrepDialog::onIgnoreCaseCheckStateChanged);
	connect(ui->regexp, &QCheckBox::checkStateChanged, this, &GrepDialog::onRegexpCheckStateChanged);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

GrepDialog::~GrepDialog()
{
	delete ui;
}

void GrepDialog::onIgnoreCaseCheckStateChanged(Qt::CheckState state) {
	g.m_ignoreCase = state == Qt::Checked;
	qDebug() << "g.m_ignoreCase = " << g.m_ignoreCase;
}
void GrepDialog::onRegexpCheckStateChanged(Qt::CheckState state) {
	g.m_regexp = state == Qt::Checked;
	qDebug() << "g.m_regexp = " << g.m_regexp;
}
