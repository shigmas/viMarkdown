# QTextDocument


- QTextEdit, QPlainTextEdit は MVアーキテクチャを採用
  - Doc-View アーキテクチャとも呼ばれる
- QTextDocument は Model（ドキュメント）を担う
  - QTextDocument：テキスト情報だけではなく、修飾情報も保持
  - QTextBlock でブロック情報を保持
- Editor にテキストを挿入する場合、QTextCursor を用いる
  - QTextCursor(QTextDocument *document) 等で生成可能
  - cousor.movePosition(...) でカーソル移動が可能
  - cousor.insertText(const QString &text)：テキスト挿入
- setPlainText(QString) で一括挿入