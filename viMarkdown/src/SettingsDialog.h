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

	void onEditorFontSizeChanged(int);
	void onPreviewFontSizeChanged(int);
	void onHeadingColorButtonClicked();
	void onActiveLineColorButtonClicked();
	void onInactiveLineColorButtonClicked();
	void onBoldItalicColorButtonClicked();
	void onBoldColorButtonClicked();
	void onItalicColorButtonClicked();
	void onStrikethroughColorButtonClicked();
	void onMatchColorButtonClicked();
	void onCSVHeaderColorButtonClicked();
	void onCSVZebraColor1ButtonClicked();
	void onCSVZebraColor2ButtonClicked();
	void onQuoteColorButtonClicked();
	void onCodeBlockColorButtonClicked();
	void onKeisenBlockColorButtonClicked();
	void	updateColorButtons();
	void	pickColor(QColor &targetColor, const QString &title);
	void onTreeItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

signals:
	void	settingsChanged();


private:
	Ui::SettingsDialogClass *ui;
};

