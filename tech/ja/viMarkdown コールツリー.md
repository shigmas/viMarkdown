# viMarkdown コールツリー

## ■ エディタカーソル移動時
###【Editor クラス】
- onCursorPosChanged()    //  cursorPositionChanged シグナル処理スロット
  - syncEditorCursorFromPreview()
    - context = contextAt(cursor.position());   // pos 位置を検索するための情報取得
    - emit posContextChanged(context,...);  → 【MainWindow へ渡す】
### 【MainWindow クラス（仲介）】
- onSrcPosContextChanged()    // posContextChanged シグナル処理スロット
  - preview->setCursorByContext(const PosContext,...)  → 【Preview へ渡す】
### 【Preview クラス】
- setCursorByContext(context)   // context 位置にカーソル設定
  - pos = findPosition(context);  // context 対応位置取得
  - cursor.setPosition(pos);    // カーソル位置設定

## ■ エディタでの編集時
### 【MainWindow クラス】
- onMDTextChanged()    // Editor::textChanged シグナル処理スロット
  - preview->setMarkdownText(editor->document())


## ■ プレビューでの編集時
### 【Preview クラス】
- onContentsChanged(int position, int charsRemoved, int charsAdded)
  - emit textInserted(addedStr);  → 【MainWindow へ渡す】
### 【MainWindow クラス（仲介）】
- onTextInsertedAtPreview(QString txt)
  - editCursor.insertText(txt);
