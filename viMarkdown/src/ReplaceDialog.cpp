#include <QSettings>
#include <QLineEdit>
#include <QShortcut>
#include "ReplaceDialog.h"
#include "MainWindow.h"

extern Global g;

ReplaceDialog::ReplaceDialog(const QStringList &hist, const QStringList &replace, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::ReplaceDialogClass())
{
	ui->setupUi(this);
	//QSettings settings;
	//QStringList history = settings.value("search/history").toStringList();
    ui->searchCB->clear();
	ui->searchCB->addItems(hist);
	ui->searchCB->lineEdit()->selectAll();
    ui->replaceCB->clear();
	ui->replaceCB->addItems(replace);
	ui->ignoreCase->setCheckState(g.m_ignoreCase ? Qt::Checked : Qt::Unchecked);
	connect(ui->searchPrev, &QPushButton::clicked, this, &ReplaceDialog::onSearchPrev);
	connect(ui->searchNext, &QPushButton::clicked, this, &ReplaceDialog::onSearchNext);
	connect(ui->replaceNext, &QPushButton::clicked, this, &ReplaceDialog::onReplaceNext);
	connect(ui->replaceAll, &QPushButton::clicked, this, &ReplaceDialog::onReplaceAll);
	connect(ui->undoPB, &QPushButton::clicked, this, &ReplaceDialog::onUndo);
	connect(ui->redoPB, &QPushButton::clicked, this, &ReplaceDialog::onRedo);
	connect(ui->ignoreCase, &QCheckBox::checkStateChanged, this, &ReplaceDialog::onCheckStateChanged);
	QShortcut *undoShortcut = new QShortcut(QKeySequence::Undo, this);
		connect(undoShortcut, &QShortcut::activated, this, [this]() {
		    emit do_undo(); 
		});
	QShortcut *redoShortcut = new QShortcut(QKeySequence::Redo, this);
		connect(redoShortcut, &QShortcut::activated, this, [this]() {
		    emit do_redo();
		});
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
void ReplaceDialog::onReplaceAll() {
	QString src = ui->searchCB->currentText();
	QString dst = ui->replaceCB->currentText();
	if( src.isEmpty() ) return;
	emit do_replace_all(src, dst);
}
void ReplaceDialog::onCheckStateChanged(Qt::CheckState state) {
	g.m_ignoreCase = state == Qt::Checked;
	qDebug() << "g.m_ignoreCase = " << g.m_ignoreCase;
}
void ReplaceDialog::onUndo() {
	emit do_undo();
}
void ReplaceDialog::onRedo() {
	emit do_redo();
}
