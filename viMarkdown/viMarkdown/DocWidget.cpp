#include <QTextBlock>
#include "MarkdownEditor.h"
#include "DocWidget.h"
#include "MarkdownPreview.h"

DocWidget::DocWidget(const QString& title, const QString& fullPath, QWidget *parent)
	: m_title(title)
	, m_fullPath(fullPath)
	, QWidget(parent)
{
}
bool DocWidget::isModified() const {
	return m_editor->document()->isModified();
}
void DocWidget::setModified(bool b) {
	m_editor->document()->setModified(b);
}
QString DocWidget::getTitle() const {
	QString title;
#if 0
	auto hbn = m_preview->getHeadingsBlockNum();
	if( !hbn.empty() && hbn[0] == 0 ) {		//	（コメントを無視した）最初の行が見出し行
		title = m_preview->document()->begin().text();
	}
#else
	int ln = 0;
	QTextDocument *doc = m_editor->document();
	QTextBlock block = doc->begin();
	while( block.isValid() && (block.text().startsWith("<!--") || block.text().isEmpty()) ) {
		++ln;
		block = block.next();
	}
	auto hln = m_preview->getSrcHeadingsBlocks();
	if( !hln.empty() && hln[0] == ln ) {		//	コメントを除いた最初の行が見出し行
		title = block.text();
		int i = 0;
		while( i < title.size() && title[i] == u'#' ) ++i;
		while( i < title.size() && title[i] == u' ' ) ++i;
		title = title.mid(i);
	}
#endif
	return title;
}
int DocWidget::previewPosToEditorPos(int pos) {
	return pos;
}
