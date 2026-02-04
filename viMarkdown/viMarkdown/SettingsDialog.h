#pragma once

#include <QDialog>
#include "ui_SettingsDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SettingsDialogClass; };
QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	SettingsDialog(QWidget *parent = nullptr);
	~SettingsDialog();

protected:
	void accept() override;

	void onHeadingColorButtonClicked();
	void onBoldColorButtonClicked();
	void onCSVHeaderColorButtonClicked();
	void onCSVZebraColor1ButtonClicked();
	void onCSVZebraColor2ButtonClicked();
	void	updateColorButtons();

private:
	Ui::SettingsDialogClass *ui;
};

