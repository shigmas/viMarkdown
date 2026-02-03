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

private:
	Ui::SettingsDialogClass *ui;
};

