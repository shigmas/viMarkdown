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
	connect(ui->italicColorPB, &QPushButton::clicked, this, &SettingsDialog::onItalicColorButtonClicked);
	connect(ui->strikethroughColorPB, &QPushButton::clicked, this, &SettingsDialog::onStrikethroughColorButtonClicked);
	connect(ui->matchColorPB, &QPushButton::clicked, this, &SettingsDialog::onMatchColorButtonClicked);
	connect(ui->CSVHeaderPB, &QPushButton::clicked, this, &SettingsDialog::onCSVHeaderColorButtonClicked);
	connect(ui->CSVZebra1PB, &QPushButton::clicked, this, &SettingsDialog::onCSVZebraColor1ButtonClicked);
	connect(ui->CSVZebra2PB, &QPushButton::clicked, this, &SettingsDialog::onCSVZebraColor2ButtonClicked);
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}
static void setColorButtonStyle(QPushButton* button, const QColor& color)
{
    button->setStyleSheet(QString(
        "background-color: %1;"		// 背景色
        "border: 1px solid gray;"	// 枠線
        "height: 20px;"
    ).arg(color.name()));
}
void SettingsDialog::updateColorButtons() {
#if 1
	setColorButtonStyle(ui->headingsColorPB,     g.m_headingsColor);
    setColorButtonStyle(ui->activeLineColorPB,   g.m_activeLnColor);
    setColorButtonStyle(ui->inactiveLineColorPB, g.m_inactiveLnColor);
    setColorButtonStyle(ui->boldColorPB,         g.m_boldColor);
    setColorButtonStyle(ui->italicColorPB,       g.m_italicColor);
    setColorButtonStyle(ui->strikethroughColorPB, g.m_strikethroughColor);
    setColorButtonStyle(ui->matchColorPB,        g.m_matchColor);
    setColorButtonStyle(ui->CSVHeaderPB,         g.m_CSVHeaderColor);
    setColorButtonStyle(ui->CSVZebra1PB,         g.m_CSVZebraColor1);
    setColorButtonStyle(ui->CSVZebra2PB,         g.m_CSVZebraColor2);
#else
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
	ui->matchColorPB->setStyleSheet(QString(
        "background-color: %1;" // 背景色をセット
        "border: 1px solid gray;" // 枠線をつける
        "height: 20px;" // 必要に応じて高さを固定
    ).arg(g.m_matchColor.name()));
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
#endif
}

void SettingsDialog::accept() {
	QSettings settings;
	settings.setValue(KEY_EDITOR_FONT_SIZE, ui->editorFontSize->value());
	QDialog::accept();
}
void SettingsDialog::pickColor(QColor &targetColor, const QString &title) {
	QColor selectedColor = QColorDialog::getColor(targetColor, this, QString("Select %1 Color").arg(title));
	if (selectedColor.isValid()) {
		targetColor = selectedColor;
		updateColorButtons();
		emit settingsChanged();
	}
}
void SettingsDialog::onActiveLineColorButtonClicked() {
	pickColor(g.m_activeLnColor, "Active Line");
}
void SettingsDialog::onInactiveLineColorButtonClicked() {
	pickColor(g.m_inactiveLnColor, "Inactive Line");
}
void SettingsDialog::onHeadingColorButtonClicked() {
	pickColor(g.m_headingsColor, "Heading");
}
void SettingsDialog::onBoldColorButtonClicked() {
	pickColor(g.m_boldColor, "Bold");
}
void SettingsDialog::onItalicColorButtonClicked() {
	pickColor(g.m_italicColor, "Italic");
}
void SettingsDialog::onStrikethroughColorButtonClicked() {
	pickColor(g.m_strikethroughColor, "Strikethrough");
}
void SettingsDialog::onMatchColorButtonClicked() {
	pickColor(g.m_matchColor, "Match");
}
void SettingsDialog::onCSVHeaderColorButtonClicked() {
	pickColor(g.m_CSVHeaderColor, "CSV Header");
}
void SettingsDialog::onCSVZebraColor1ButtonClicked() {
	pickColor(g.m_CSVZebraColor1, "CSV Zebra-1 ");
}
void SettingsDialog::onCSVZebraColor2ButtonClicked() {
	pickColor(g.m_CSVZebraColor2, "CSV Zebra-2 ");
}

