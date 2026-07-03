//----------------------------------------------------------------------
//
//			File:			"diff.cpp"
//			Created:		04-7-2026
//			Author:			vivisuke
//			Description:
//
//----------------------------------------------------------------------

#include <QTextDocument>
#include <QTextBlock>
#include "dtl/dtl.hpp"
#include "MainWindow.h"
#include "DocWidget.h"
#include "MarkdownEditor.h"
#include "MarkdownPreview.h"

std::vector<QString> extractLinesFromDocument(const QTextDocument *doc) {
    std::vector<QString> lines;
    if (!doc) return lines;

    // メモリ確保のオーバーヘッドを減らすため大まかにリザーブ
    lines.reserve(doc->blockCount());

    // QTextBlock を走査することで、巨大なテキストでも分割時のメモリ消費を最小限に抑えます
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        lines.push_back(block.text());
    }
    return lines;
}
void MainWindow::onDiffViewChanged() {
	qDebug() << "MainWindow::onDiffViewChanged()";
	DocWidget *docWidget = getCurDocWidget();
	if (docWidget == nullptr || !docWidget->m_diffMode)
		return;
	QTextDocument *doc1 = docWidget->m_editor->document();
	QTextDocument *doc2 = docWidget->m_diffview->document();
	std::vector<QString> lines1 = extractLinesFromDocument(doc1);
    std::vector<QString> lines2 = extractLinesFromDocument(doc2);

    dtl::Diff<QString, std::vector<QString>> d(lines1, lines2);
    d.compose();

    auto ses = d.getSes().getSequence();
    for (const auto& item : ses) {
#if 0
    	qDebug().noquote()
			        << item.first.beforeIdx
			        << item.first.afterIdx
			        << item.second.type
			        << "'" << item.second.elem << "'";
#endif
        const QString& line = item.first;
        dtl::elemInfo info = item.second;
        switch (info.type) {
        case dtl::SES_COMMON:
            // 両方に共通する行（通常の行）
            do_output("= '" + line + "'\n");
            break;
        case dtl::SES_DELETE:
            // 左側（doc1）にのみ存在し、右側（doc2）で削除された行
            do_output("- '" + line + "'\n");
            break;
        case dtl::SES_ADD:
            // 右側（doc2）で新しく追加された行
            do_output("+ '" + line + "'\n");
            break;
        }
    }
}
