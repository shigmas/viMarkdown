#include <QDir>
#include "GrepDialog.h"
#include "MainWindow.h"

extern Global g;

GrepDialog::GrepDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::GrepDialogClass())
{
	ui->setupUi(this);
	QString currentPath = QDir::currentPath();
	ui->dirCB->addItem(currentPath);
	ui->dirCB->setCurrentText(currentPath);
	ui->ignoreCase->setCheckState(g.m_ignoreCase ? Qt::Checked : Qt::Unchecked);
	connect(ui->ignoreCase, &QCheckBox::checkStateChanged, this, &GrepDialog::onCheckStateChanged);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

GrepDialog::~GrepDialog()
{
	delete ui;
}

void GrepDialog::onCheckStateChanged(Qt::CheckState state) {
	g.m_ignoreCase = state == Qt::Checked;
	qDebug() << "g.m_ignoreCase = " << g.m_ignoreCase;
}
