#include <QSettings>
#include "MainWindow.h"
#include "SettingsDialog.h"


SettingsDialog::SettingsDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::SettingsDialogClass())
{
	ui->setupUi(this);
	QSettings settings;
	ui->editorFontSize->setValue(settings.value(KEY_EDITOR_FONT_SIZE).toInt());
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::accept() {
	QSettings settings;
	settings.setValue(KEY_EDITOR_FONT_SIZE, ui->editorFontSize->value());
	QDialog::accept();
}

