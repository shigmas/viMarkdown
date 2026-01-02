#include <QTextBlock>
#include "MarkdownEditor.h"
#include "DocWidget.h"
#include "MarkdownViewer.h"

DocWidget::DocWidget(const QString& title, const QString& fullPath, QWidget *parent)
	: m_title(title)
	, m_fullPath(fullPath)
	, QWidget(parent)
{
}
bool DocWidget::isModified() const {
	return m_mdEditor->document()->isModified();
}
void DocWidget::setModified(bool b) {
	m_mdEditor->document()->setModified(b);
}
QString DocWidget::getTitle() const {
	QString title;
	auto hln = m_markdownViewer->getHeadingsLineNum();
	if( !hln.empty() && hln[0] == 0 ) {		//	1行目が見出し行
		title = m_markdownViewer->document()->begin().text();
	}
	return title;
}
