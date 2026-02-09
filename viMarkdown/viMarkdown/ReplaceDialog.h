#pragma once

#include <QDialog>
#include "ui_ReplaceDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ReplaceDialogClass; };
QT_END_NAMESPACE

class ReplaceDialog : public QDialog
{
	Q_OBJECT

public:
	ReplaceDialog(QWidget *parent = nullptr);
	~ReplaceDialog();

protected:
	void	onSearchPrev();
	void	onSearchNext();
	void	onReplaceNext();
	void	onReplaceAll();
	void	onCheckStateChanged(Qt::CheckState state);

signals:
	void	do_search(const QString, bool backward);
	void	do_replace_next(const QString, const QString);
	void	do_replace_all(const QString, const QString);

private:
	Ui::ReplaceDialogClass *ui;
};

