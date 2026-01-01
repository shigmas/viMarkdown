```
┏━━━━━━━━━━┓          ┏━━━━━━━━━━┓
┃   MainWindow       ┃          ┃     OutlineBar     ┃
┠──────────┨          ┠──────────┨
┃- m_editor          ┠────→┃- updateOutline()   ┃
┃- m_preview         ┃          ┃- onJumpToBlock()   ┃
┗━━━━┯━━━━━┛          ┗━━━━━━━━━━┛
          │        
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
