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
	//QColor color("#800000");
	ui->headingsColorPB->setStyleSheet(QString(
        "background-color: %1;" // 背景色をセット
        "border: 1px solid gray;" // 枠線をつける（色の判別をしやすくするため）
        "height: 20px;" // 必要に応じて高さを固定
    ).arg(g.m_headingsColor.name()));
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
	//QColor initialColor = QColor("#800000");
	QColor selectedColor = QColorDialog::getColor(g.m_headingsColor, this, "Select Headings Color");
	if( selectedColor.isValid() ) {
		g.m_headingsColor = selectedColor;
		ui->headingsColorPB->setStyleSheet(QString(
	        "background-color: %1;" // 背景色をセット
	        "border: 1px solid gray;" // 枠線をつける（色の判別をしやすくするため）
	        "height: 20px;" // 必要に応じて高さを固定
	    ).arg(g.m_headingsColor.name()));
	}
}

