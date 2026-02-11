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
	void onActiveLineColorButtonClicked();
	void onInactiveLineColorButtonClicked();
	void onBoldColorButtonClicked();
	void onMatchColorButtonClicked();
	void onCSVHeaderColorButtonClicked();
	void onCSVZebraColor1ButtonClicked();
	void onCSVZebraColor2ButtonClicked();
	void	updateColorButtons();

signals:
	void	settingsChanged();


private:
	Ui::SettingsDialogClass *ui;
};

