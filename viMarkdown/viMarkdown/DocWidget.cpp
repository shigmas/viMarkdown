#include <QTextBlock>
#include "MarkdownEditor.h"
#include "DocWidget.h"
#include "MarkdownPreview.h"

BlockData* getBlockData(QTextBlock srcBlock) {
	BlockData* data = static_cast<BlockData*>(srcBlock.userData());
	if (!data) {
	    data = new BlockData();
		srcBlock.setUserData(data);
		data->m_charFlags.resize(srcBlock.text().size());
		data->m_charFlags.fill(0);
	} else
		data->m_charFlags.resize(srcBlock.text().size());	//	念のため
	return data;
}
//----------------------------------------------------------------------
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
	auto hln = getSrcHeadingsBlocks();
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
int DocWidget::prvToSrcHeading(int blockNum) {
	assert( m_prvHeadingBlocks.size() == m_srcHeadingBlocks.size() );
	auto it = std::lower_bound(m_prvHeadingBlocks.begin(), m_prvHeadingBlocks.end(), blockNum);
	if (it != m_prvHeadingBlocks.end()) {
		size_t ix = std::distance(m_prvHeadingBlocks.begin(), it);
		return m_srcHeadingBlocks[ix];
	} else
		return 0;
}
int DocWidget::srcToPrvHeading(int blockNum) {
	assert( m_prvHeadingBlocks.size() == m_srcHeadingBlocks.size() );
	auto it = std::lower_bound(m_srcHeadingBlocks.begin(), m_srcHeadingBlocks.end(), blockNum);
	if (it != m_srcHeadingBlocks.end()) {
		size_t ix = std::distance(m_srcHeadingBlocks.begin(), it);
		return m_prvHeadingBlocks[ix];
	} else
		return 0;
}
void DocWidget::setEditorCurPos(int pos) {
	QTextCursor cursor = m_editor->textCursor();
	cursor.setPosition(pos);
	m_editor->setTextCursor(cursor);
	m_editor->ensureCursorVisible();
}
