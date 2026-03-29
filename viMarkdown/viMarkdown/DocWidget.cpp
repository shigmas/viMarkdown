#include <QTextBlock>
#include <QRegularExpression>
#include "MarkdownEditor.h"
#include "DocWidget.h"
#include "MarkdownPreview.h"

BlockData* getBlockData(QTextBlock srcBlock, bool init /*, int length*/) {
	assert( srcBlock.isValid() );
	int bn = srcBlock.blockNumber();	//	for Debug
	assert( bn >= 0 );
	QString txt = srcBlock.text();	//	for Debug
	BlockData* data = static_cast<BlockData*>(srcBlock.userData());
	//if( !length )
	int length = srcBlock.text().size();
	if (!data) {
	    data = new BlockData();
		srcBlock.setUserData(data);
		data->m_charFlags.resize(length);
		data->m_charFlags.fill(0);
	} else {
		if( init )
			data->m_charFlags.fill(0);
		else {
			auto len0 = data->m_charFlags.size();
			data->m_charFlags.resize(length);
			if( len0 < length ) {
				for(int i = len0; i < length; ++i)
					data->m_charFlags[i] = 0;
			}
		}
	}
	return data;
}
const QByteArray& getCharFlags(QTextBlock block) {
	const BlockData* data = getBlockData(block);
	return data->m_charFlags;
}
void printCharFlags(QTextBlock block) {
#ifdef _DEBUG
	qDebug() << "blockNumber = " << block.blockNumber();
	const BlockData* data = getBlockData(block);
	QString txt;
	for(int i = 0; i < data->m_charFlags.size(); ++i)
		txt += QString::number((int)data->m_charFlags[i]) + u' ';
	qDebug() << txt;
#endif
}
bool parseCsvLine(QStringList &fields, const QString &line, bool inQuotes, bool &inComment, bool &commented, BlockData* data) {
	//QStringList fields;
	if( data != nullptr ) {
		assert( line.size() == data->m_charFlags.size() );
		data->m_charFlags.fill(0);
		qDebug() << "data != nullptr";
	}
	commented = false;
	int i = 0;
	if( inComment ) {
		int ix = line.indexOf("-->");
		if( ix < 0 ) return inQuotes;
		inComment = false;
		if( (i = ix + 3) >= line.length() ) {	//	"-->" までスキップ
			commented = fields.isEmpty();
			return inQuotes;
		}

	}
	if( line.startsWith("```csv", Qt::CaseInsensitive) )
		return inQuotes;
	QString currentField;
	if( !inQuotes ) {
		fields.clear();
	} else {
		currentField = fields.back() + u'\n';
		fields.pop_back();
	}
	//bool inQuotes = false;
	int ix0 = i;
	for (; i < line.length(); ++i) {
		if( line.mid(i).startsWith("<!--") ) {
			int ix = line.indexOf("-->", i + 4);
			if( ix > 0 ) {	//	コメント終了を発見
				if( (i = ix + 3) >= line.length() ) {	//	"-->" までスキップ
					commented = fields.isEmpty();
					return inQuotes;
				}
			} else {
				inComment = true;
				return inQuotes;
			}
		}
		QChar c = line.at(i);
		if (c == '"') {
			if( data != nullptr )
				data->m_charFlags[i] = PCF_CSV;
			// ダブルクォートの中のダブルクォート（エスケープ）をチェック
			if (inQuotes && i + 1 < line.length() && line.at(i + 1) == '"') {
				currentField += '"';
				i++; // 1文字飛ばす
			} else if( currentField.isEmpty() || i + 1 == line.length() || line.at(i + 1) == ',') {
				inQuotes = !inQuotes; // クォート状態の反転
			} else {
				currentField += '"';
			}
		} else if (c == ',' && !inQuotes) {		// クォートの外にあるカンマはセパレータ
			fields.append(currentField.trimmed());
			currentField.clear();
			if( data != nullptr ) {
				data->m_charFlags[i] = PCF_CSV;
				for(int k = i+1; k < line.size() && line[k] == u' '; ++k)
					data->m_charFlags[k] = PCF_CSV;
				updateCharFlags(data, line, ix0, i, true);		//	true for エスケープ文字処理
			}
			ix0 = i + 1;
		} else { // 通常の文字
			if( c != ' ' || !currentField.isEmpty() )
				currentField += c;
		}
	}
	// 最後のフィールドを追加
	fields.append(currentField.trimmed());
	if( data != nullptr ) {
		updateCharFlags(data, line, ix0, i, true);		//	true for エスケープ文字処理
	}
	return inQuotes;
}
#if 0
bool isTableLine(const QString& lnStr0, const QString& lnStr, QList<QStringView> &tableTokens, BlockData *data) {
	assert( data == nullptr || data->m_charFlags.size() == lnStr0.size() );
	QStringView sv(lnStr);
	tableTokens.clear();
	int ix = 0;
	while( ix < sv.size() && sv[ix] == u' ' ) ++ix;		//	空白スキップ
	if (ix < sv.size() && sv[ix] == u'|') {
		if( data != nullptr )
			data->m_charFlags[ix] = PCF_TABLE;
		++ix;			//	先頭 '|' スキップ
	}
	while( ix < sv.size() ) {
		int ix0 = ix;
		while( ix < sv.size() && sv[ix] != u'|' ) {		//	'|' までループ
			if( ix+1 < sv.size() && sv[ix] == u'\\' ) ix += 2;
			else ++ix;
		}
		tableTokens.push_back(sv.mid(ix0, ix-ix0));
		if( ix < sv.size() && data != nullptr )
			data->m_charFlags[ix] = PCF_TABLE;
		++ix;			//	'|' スキップ
	}
	return tableTokens.size() > 1;
}
#endif
bool isTableLine(const QString& lnStr0, const QString& lnStr, QStringList &tableTokens, BlockData* data) {
	assert( data == nullptr || data->m_charFlags.size() == lnStr0.size() );
	QString sv(lnStr);
	tableTokens.clear();
	int ix = 0;
	while( ix < sv.size() && sv[ix] == u' ' ) {
		if( data != nullptr )
			data->m_charFlags[ix] = PCF_ESCAPE;	//	| 直後空白は非表示
		++ix;		//	空白スキップ
	}
	if (ix < sv.size() && sv[ix] == u'|') {
		if( data != nullptr )
			data->m_charFlags[ix] = PCF_TABLE;
		++ix;			//	先頭 '|' スキップ
	}
	while( ix < sv.size() ) {
		int ix0 = ix;
		while( ix < sv.size() && sv[ix] != u'|' ) {		//	'|' までループ
			if( ix+1 < sv.size() && sv[ix] == u'\\' ) ix += 2;
			else ++ix;
		}
		tableTokens.push_back(sv.mid(ix0, ix-ix0));
		if( data != nullptr ) {
			for(int i = ix; --i >= 0 && lnStr0[i] == u' '; )	//	| 直前空白は非表示
				data->m_charFlags[i] = PCF_ESCAPE;
			if( ix < sv.size() )
				data->m_charFlags[ix] = PCF_TABLE;
			updateCharFlags(data, lnStr0, ix0, ix, true);		//	true for エスケープ文字処理
		}
		++ix;			//	'|' スキップ
	}
	return tableTokens.size() > 1;
}
bool isTableHyphenLine(const QString& lnStr, std::vector<char> &tableAlign, BlockData *data) {
	tableAlign.clear();
	QStringView sv(lnStr);
	int ix = 0;
	while( ix < sv.size() && sv[ix] == u' ' ) ++ix;		//	空白スキップ
	if (ix < sv.size() && sv[ix] == u'|') ++ix;			//	先頭 '|' スキップ
	while( ix < sv.size() ) {
		char aln = 0;
		while( ix < sv.size() && sv[ix] == u' ' ) ++ix;		//	空白スキップ
		if( ix < sv.size() && sv[ix] == u':' ) { aln = ALIGHN_LEFT; ++ix; }
		while( ix < sv.size() && sv[ix] != u'|' ) {		//	次の'|' までループ
			if( ix+1 < sv.size() && sv[ix] == u'\\' ) ++ix;
			if( sv[ix] != u'-' && sv[ix] != u':' && sv[ix] != u' ' )
				return false;
			++ix;
		}
		int i = ix - 1;
		while( i >= 0 && sv[i] == u' ' ) --i;		//	空白スキップ
		if( i >= 0 && sv[i] == u':' )
			aln |= ALIGHN_RIGHT;
		tableAlign.push_back(aln);
		++ix;
	}
	if( tableAlign.size() > 1 ) {
		if( data != nullptr ) {
			assert( data->m_charFlags.size() == lnStr.size() );
			data->m_charFlags.fill(PCF_TABLE);
		}
		return true;
	} else
		return false;
}
bool isEscapedChar(QChar ch) {
	return ch.unicode() >= u' ' && ch.unicode() <= u'/' ||
			ch.unicode() >= u'[' && ch.unicode() <= u']' ||		//	[ \ ]
			ch.unicode() == u'>';
}
static QRegularExpression image_re(R"((?<!\\)!\[([^\]]+)\]\(([^)]+)\))");
static QRegularExpression link_re(R"((?<![!\\])\[([^\]]+)\]\(([^)]+)\))");
//	ボールド、イタリック、打ち消し線 部分に PCF_EMPHASIZED 設定
bool updateCharFlags(BlockData* data, const QString &buf, int ix, int ix9, bool esc) {
	bool modified = false;
	if( esc ) {
		int i = ix;
		while( (i = buf.indexOf(u'\\', i)) >= 0 && i < ix9 ) {
			if( i + 1 < ix9 && isEscapedChar(buf[i+1]) ) {
				modified = true;
				data->m_charFlags[i] = PCF_ESCAPE;
				i += 2;
			} else
				++i;
		}
	}
	while( ix < ix9 ) {
		if( ix + 1 < buf.size() && buf[ix] == u'\\' && isEscapedChar(buf[ix+1]) )
			ix += 2;
		else if( buf[ix] == u'*' || buf[ix] == u'_' || buf[ix] == u'~' && ix+1 < ix9 && buf[ix+1] == buf[ix]) {
			QString sym;
			if( buf[ix] == u'~' ) {
				sym = "~~";
			} else {
				if( ix+1 < ix9 && buf[ix+1] == buf[ix] ) {
					if(ix+2 < ix9 && buf[ix+2] == buf[ix] )
						sym = buf.mid(ix, 3);
					else
						sym = buf.mid(ix, 2);
				} else
					sym = buf[ix];
			}
			int ix2 = buf.indexOf(sym, ix+sym.size());
			if( ix2 >= 0 ) {	//	バランスしている場合
				for(int i = 0; i < sym.size(); ++i) {
					data->m_charFlags[ix+i] = PCF_EMPHASIZED;
					data->m_charFlags[ix2+i] = PCF_EMPHASIZED;
					modified = true;
				}
				if( updateCharFlags(data, buf, ix+sym.size(), ix2) )
					modified = true;
				ix = ix2 + sym.size();
			} else
				++ix;
		} else
			++ix;
	}
	return modified;
}
void updateCharFlags(QTextBlock srcBlock) {
	QString buf = srcBlock.text();
	BlockData *data = getBlockData(srcBlock, false);
	bool modified = false;
	auto match = image_re.match(buf);		//	![title](image.png) を含むか？
	while( match.hasMatch() ) {
		modified = true;
		int start = match.capturedStart(); // マッチした最初の位置
	    int length = match.capturedLength(); // マッチした全体の長さ
	    for(int i = start; i < start + length; ++i)
	    	data->m_charFlags[i] = PCF_IMAGE;
		match = image_re.match(buf, start + length);
	}
	match = link_re.match(buf);		//	[title](image.png) を含むか？
	while( match.hasMatch() ) {
		modified = true;
		int start = match.capturedStart(); // マッチした最初の位置（'['）
	    int length = match.capturedLength(); // マッチした全体の長さ
	    data->m_charFlags[start] = PCF_LINK;
	    int ix = buf.indexOf(']', start);
	    for(int i = ix; i < start + length && i < data->m_charFlags.size(); ++i)
	    	data->m_charFlags[i] = PCF_LINK;
		match = link_re.match(buf, start + length);
	}
	for(int i = 0; i < buf.size() && buf[i] == u' '; ++i)
		data->m_charFlags[i] = PCF_ESCAPE;
	for(int i = buf.size(); --i >= 0 && buf[i] == u' '; )
		data->m_charFlags[i] = PCF_ESCAPE;
	int ix = 0;
	while( (ix = buf.indexOf(u'\\', ix)) >= 0 ) {
		if( ix + 1 < buf.size() && isEscapedChar(buf[ix+1]) ) {
			modified = true;
			data->m_charFlags[ix] = PCF_ESCAPE;
			ix += 2;
		} else
			++ix;
	}
	if( updateCharFlags(data, buf, 0, buf.size()) )
		modified = true;
	if( modified ) {
		srcBlock.setUserData(data);
		qDebug() << "updateCharFlags(srcBlock)";
		printCharFlags(srcBlock);
	}
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
