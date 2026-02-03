#include <QSettings>
#include <QColorDialog>
#include "MainWindow.h"
#include "SettingsDialog.h"


SettingsDialog::SettingsDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::SettingsDialogClass())
{
	ui->setupUi(this);
	QSettings settings;
	ui->editorFontSize->setValue(settings.value(KEY_EDITOR_FONT_SIZE).toInt());
	connect(ui->headingsColorPB, &QPushButton::clicked, this, &SettingsDialog::onHeadingColorButtonClicked);
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
void SettingsDialog::onHeadingColorButtonClicked() {
	QColor initialColor = QColor("#800000");
	QColor selectedColor = QColorDialog::getColor(initialColor, this, "Select Headings Color");
}

