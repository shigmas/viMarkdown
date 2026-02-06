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
	connect(ui->activeLineColorPB, &QPushButton::clicked, this, &SettingsDialog::onActiveLineColorButtonClicked);
	connect(ui->inactiveLineColorPB, &QPushButton::clicked, this, &SettingsDialog::onInactiveLineColorButtonClicked);
	connect(ui->boldColorPB, &QPushButton::clicked, this, &SettingsDialog::onBoldColorButtonClicked);
	connect(ui->CSVHeaderPB, &QPushButton::clicked, this, &SettingsDialog::onCSVHeaderColorButtonClicked);
	connect(ui->CSVZebra1PB, &QPushButton::clicked, this, &SettingsDialog::onCSVZebraColor1ButtonClicked);
	connect(ui->CSVZebra2PB, &QPushButton::clicked, this, &SettingsDialog::onCSVZebraColor2ButtonClicked);
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}
void SettingsDialog::updateColorButtons() {
	ui->headingsColorPB->setStyleSheet(QString(
        "background-color: %1;" // 背景色をセット
        "border: 1px solid gray;" // 枠線をつける
        "height: 20px;" // 必要に応じて高さを固定
    ).arg(g.m_headingsColor.name()));
	ui->activeLineColorPB->setStyleSheet(QString(
        "background-color: %1;" // 背景色をセット
        "border: 1px solid gray;" // 枠線をつける
        "height: 20px;" // 必要に応じて高さを固定
    ).arg(g.m_activeLnColor.name()));
	ui->inactiveLineColorPB->setStyleSheet(QString(
        "background-color: %1;" // 背景色をセット
        "border: 1px solid gray;" // 枠線をつける
        "height: 20px;" // 必要に応じて高さを固定
    ).arg(g.m_inactiveLnColor.name()));
	ui->boldColorPB->setStyleSheet(QString(
        "background-color: %1;" // 背景色をセット
        "border: 1px solid gray;" // 枠線をつける
        "height: 20px;" // 必要に応じて高さを固定
    ).arg(g.m_boldColor.name()));
	ui->CSVHeaderPB->setStyleSheet(QString(
        "background-color: %1;" // 背景色をセット
        "border: 1px solid gray;" // 枠線をつける
        "height: 20px;" // 必要に応じて高さを固定
    ).arg(g.m_CSVHeaderColor.name()));
	ui->CSVZebra1PB->setStyleSheet(QString(
        "background-color: %1;" // 背景色をセット
        "border: 1px solid gray;" // 枠線をつける
        "height: 20px;" // 必要に応じて高さを固定
    ).arg(g.m_CSVZebraColor1.name()));
	ui->CSVZebra2PB->setStyleSheet(QString(
        "background-color: %1;" // 背景色をセット
        "border: 1px solid gray;" // 枠線をつける
        "height: 20px;" // 必要に応じて高さを固定
    ).arg(g.m_CSVZebraColor2.name()));
}

void SettingsDialog::accept() {
	QSettings settings;
	settings.setValue(KEY_EDITOR_FONT_SIZE, ui->editorFontSize->value());
	QDialog::accept();
}
void SettingsDialog::onActiveLineColorButtonClicked() {
	QColor selectedColor = QColorDialog::getColor(g.m_activeLnColor, this, "Select Headings Color");
	if (selectedColor.isValid()) {
		g.m_activeLnColor = selectedColor;
		updateColorButtons();
		emit settingsChanged();
	}
}
void SettingsDialog::onInactiveLineColorButtonClicked() {
	QColor selectedColor = QColorDialog::getColor(g.m_inactiveLnColor, this, "Select Headings Color");
	if (selectedColor.isValid()) {
		g.m_inactiveLnColor = selectedColor;
		updateColorButtons();
		emit settingsChanged();
	}
}
void SettingsDialog::onHeadingColorButtonClicked() {
	QColor selectedColor = QColorDialog::getColor(g.m_headingsColor, this, "Select Headings Color");
	if( selectedColor.isValid() ) {
		g.m_headingsColor = selectedColor;
		updateColorButtons();
		emit settingsChanged();
	}
}
void SettingsDialog::onBoldColorButtonClicked() {
	QColor selectedColor = QColorDialog::getColor(g.m_boldColor, this, "Select Headings Color");
	if (selectedColor.isValid()) {
		g.m_boldColor = selectedColor;
		updateColorButtons();
		emit settingsChanged();
	}
}
void SettingsDialog::onCSVHeaderColorButtonClicked() {
	QColor selectedColor = QColorDialog::getColor(g.m_CSVHeaderColor, this, "Select Headings Color");
	if( selectedColor.isValid() ) {
		g.m_CSVHeaderColor = selectedColor;
		updateColorButtons();
		emit settingsChanged();
	}
}
void SettingsDialog::onCSVZebraColor1ButtonClicked() {
	QColor selectedColor = QColorDialog::getColor(g.m_CSVZebraColor1, this, "Select Headings Color");
	if( selectedColor.isValid() ) {
		g.m_CSVZebraColor1 = selectedColor;
		updateColorButtons();
		emit settingsChanged();
	}
}
void SettingsDialog::onCSVZebraColor2ButtonClicked() {
	QColor selectedColor = QColorDialog::getColor(g.m_CSVZebraColor2, this, "Select Headings Color");
	if( selectedColor.isValid() ) {
		g.m_CSVZebraColor2 = selectedColor;
		updateColorButtons();
		emit settingsChanged();
	}
}

