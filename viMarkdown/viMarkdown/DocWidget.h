#pragma once
#include <QStringConverter>
#include "C:\Qt\6.10.0\msvc2022_64\include\QtWidgets\qwidget.h"
#include "markdowntohtmlconvertor.h"

//class QPlainTextEdit;
class MarkdownEditor;
class QTextEdit;
class MarkdownPreview;

class DocWidget : public QWidget
{
public:
	DocWidget(const QString& title, const QString& fullPath, QWidget* parent = nullptr);

	bool	isModified() const;
	void	setModified(bool);
	QString	getTitle() const;
	int		previewPosToEditorPos(int pos);		//	プレビュー position をエディタの対応 position に変換

public:
	//bool	m_modified = false;		//	編集＆未保存状態
	bool	m_saving = false;
	bool	m_hasSaved = false;		//	保存直後
	bool	m_withBOM = true;		//	BOM付き
	QStringConverter::Encoding m_encoding = QStringConverter::Utf8;
	QString	m_title;				//	タブタイトル
	QString	m_fullPath;
	MarkdownEditor		*m_editor = nullptr;	//	マークダウンエディタへのポインタ
	MarkdownPreview		*m_preview = nullptr;	//	マークダウンプレビューワへのポインタ
	std::vector<int>	m_srcHeadingBlocks;		//	各見出し行 ブロック番号（0 org.）in マークダウンソース
	std::vector<int>	m_prvHeadingBlocks;		//	各見出し行 ブロック番号（0 org.）in マークダウンプレビューワ
    MarkdownToHtmlConvertor	m_htmlComvertor;
};

