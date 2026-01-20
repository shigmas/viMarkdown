# オイラが知らなかった Qt6 の便利な機能


## QString::remove(const QRegularExpression &re)

- re にマッチするテキストを削除してくれる。
- [例] 先頭の数字列を削除
  - str.remove(QRegularExpression("^[0-9]+"));

## QStringView

- QString と同じように利用できる
- mid(), left() を実行してもメモリアロケーションを行わず光速



