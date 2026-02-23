test
```keisen
┏━━━━━━━━━━┓          ┏━━━━━━━━━━┓
┃   MainWindow       ┃          ┃     OutlineBar     ┃
┠──────────┨          ┠──────────┨
┃- m_editor          ┠────→┃- updateOutline()   ┃
┃- m_preview         ┃          ┃- onJumpToBlock()   ┃
┗━━━━┯━━━━━┛          ┗━━━━━━━━━━┛
          │        
          │test
          ↓        
┏━━━━━━━━━━┓          ┏━━━━━━━━━━┓
┃   MarkdownViewer   ┃          ┃   PreviewWindow    ┃
┃    (Logic)         ┃          ┃   (QTextEdit)      ┃
┠──────────┨          ┠──────────┨
┃+ do_heading()      ┠────→┃+ insertMarkdown()  ┃
┃+ do_list()         ┃          ┃+ insertFrame()     ┃
┃+ do_code()         ┃          ┃+ setBlockFormat()  ┃
┗━━━━━━━━━━┛          ┗━━━━━━━━━━┛
```
test
