#pragma once
#include <QStringConverter>
#include "C:\Qt\6.10.0\msvc2022_64\include\QtWidgets\qwidget.h"
#include "markdowntohtmlconvertor.h"

//class QPlainTextEdit;
class MarkdownEditor;
class QTextEdit;
class MarkdownPreview;

enum {
    PCF_VISIBLE = 0,	// プレビューに表示される
    PCF_COMMENTED,		//	コメントアウトされた文字
    PCF_ESCAPE,			//	エスケープ文字
    PCF_HEADING,		//	タイトル・見出し行
    PCF_LIST_MARK,		// "- " などリストマーカー
    PCF_NUM_LIST,		//	"1. " 連番
    PCF_QUOTE,
    PCF_LINK,
    PCF_IMAGE,
    PCF_CSV,
    PCF_KEISEN,
    PCF_EMPHASIZED,		//	ボールド、イタリック等
};
class BlockData : public QTextBlockUserData
{
public:
    QByteArray m_charFlags;   // 1 char = 1 byte
};

BlockData* getBlockData(QTextBlock srcBlock, bool init = false /*, int length = 0*/);
const QByteArray& getCharFlags(QTextBlock srcBlock);
void printCharFlags(QTextBlock block);
void printCharFlags(const BlockData*);

bool parseCsvLine(QStringList &fields, const QString &line, bool inQuotes, bool &inComment, bool &commented, BlockData* = nullptr);

class DocWidget : public QWidget
{
public:
	DocWidget(const QString& title, const QString& fullPath, QWidget* parent = nullptr);

	bool	isModified() const;
	void	setModified(bool);
	QString	getTitle() const;
	int		previewPosToEditorPos(int pos);		//	プレビュー position をエディタの対応 position に変換
    const std::vector<int>&	getSrcHeadingsBlocks() const { return m_srcHeadingBlocks; }
    const std::vector<int>&	getPrvHeadingsBlocks() const { return m_prvHeadingBlocks; }		//	見出し行だけの行番号（0 org）リスト
    int		prvToSrcHeading(int blockNum);		//	プレビューの見出し行番号（0 org.）をエディタのそれに変換
    int		srcToPrvHeading(int blockNum);		//	エディタの見出し行番号（0 org.）をプレビューのそれに変換
    void	setEditorCurPos(int pos);

public:
	//bool	m_modified = false;		//	編集＆未保存状態
	bool	m_saving = false;
	bool	m_hasSaved = false;		//	保存直後
	bool	m_withBOM = true;		//	BOM付き
	bool	m_readOnly = false;
	QStringConverter::Encoding m_encoding = QStringConverter::Utf8;
	QString	m_title;				//	タブタイトル
	QString	m_fullPath;
	MarkdownEditor		*m_editor = nullptr;	//	マークダウンエディタへのポインタ
	MarkdownPreview		*m_preview = nullptr;	//	マークダウンプレビューワへのポインタ
	std::vector<int>	m_srcHeadingBlocks;		//	各見出し行 ブロック番号（0 org.）in マークダウンソース
	std::vector<int>	m_prvHeadingBlocks;		//	各見出し行 ブロック番号（0 org.）in マークダウンプレビューワ
    MarkdownToHtmlConvertor	m_htmlComvertor;
};

