#include <QSettings>
#include <QColorDialog>
#include "MainWindow.h"
#include "SettingsDialog.h"

extern Global g;

SettingsDialog::SettingsDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::SettingsDialogClass())
{
	ui->setupUi(this);
	if (this->layout()) {
        this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    }
	QSettings settings;
	ui->editorFontSize->setValue(g.m_editorFontSize);
	updateColorButtons();
	//QColor color("#800000");
	connect(ui->headingsColorPB, &QPushButton::clicked, this, &SettingsDialog::onHeadingColorButtonClicked);
	connect(ui->tableHeaderPB, &QPushButton::clicked, this, &SettingsDialog::onTableHeaderColorButtonClicked);
	connect(ui->tableZebraPB, &QPushButton::clicked, this, &SettingsDialog::onTableZebraColorButtonClicked);
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}
void SettingsDialog::updateColorButtons() {
	ui->headingsColorPB->setStyleSheet(QString(
        "background-color: %1;" // 背景色をセット
        "border: 1px solid gray;" // 枠線をつける（色の判別をしやすくするため）
        "height: 20px;" // 必要に応じて高さを固定
    ).arg(g.m_headingsColor.name()));
	ui->tableHeaderPB->setStyleSheet(QString(
        "background-color: %1;" // 背景色をセット
        "border: 1px solid gray;" // 枠線をつける（色の判別をしやすくするため）
        "height: 20px;" // 必要に応じて高さを固定
    ).arg(g.m_tableHeaderColor.name()));
	ui->tableZebraPB->setStyleSheet(QString(
        "background-color: %1;" // 背景色をセット
        "border: 1px solid gray;" // 枠線をつける（色の判別をしやすくするため）
        "height: 20px;" // 必要に応じて高さを固定
    ).arg(g.m_tableZebraColor.name()));
}

void SettingsDialog::accept() {
	QSettings settings;
	settings.setValue(KEY_EDITOR_FONT_SIZE, ui->editorFontSize->value());
	QDialog::accept();
}
void SettingsDialog::onHeadingColorButtonClicked() {
	QColor selectedColor = QColorDialog::getColor(g.m_headingsColor, this, "Select Headings Color");
	if( selectedColor.isValid() ) {
		g.m_headingsColor = selectedColor;
		updateColorButtons();
	}
}
void SettingsDialog::onTableHeaderColorButtonClicked() {
	QColor selectedColor = QColorDialog::getColor(g.m_tableHeaderColor, this, "Select Headings Color");
	if( selectedColor.isValid() ) {
		g.m_tableHeaderColor = selectedColor;
		updateColorButtons();
	}
}
void SettingsDialog::onTableZebraColorButtonClicked() {
	QColor selectedColor = QColorDialog::getColor(g.m_tableZebraColor, this, "Select Headings Color");
	if( selectedColor.isValid() ) {
		g.m_tableZebraColor = selectedColor;
		updateColorButtons();
	}
}

