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
	ReplaceDialog(const QStringList &hist, const QStringList &replace, QWidget *parent = nullptr);
	~ReplaceDialog();

protected:
	void	onSearchPrev();
	void	onSearchNext();
	void	onReplaceNext();
	void	onReplaceAll();
	void	onCheckStateChanged(Qt::CheckState state);
	void	onUndo();
	void	onRedo();

signals:
	void	do_search(const QString, bool backward);
	void	do_replace_next(const QString, const QString);
	void	do_replace_all(const QString, const QString);
	void	do_undo();
	void	do_redo();

private:
	Ui::ReplaceDialogClass *ui;
};

