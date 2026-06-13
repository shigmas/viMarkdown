#include <QBoxLayout>
#include <QGraphicsProxyWidget>
#include "SlideShow.h"

SlideShow::SlideShow(QWidget *parent)
	: QWidget(parent)
	, m_view(nullptr)
    , m_scene(nullptr)
    , m_textEdit(nullptr)
{
    // 枠なしのウィンドウに設定し、タスクバーやメニューバーを完全に隠すフラグを設定
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    // スライドショーの背景色を黒、文字を白にする（スタイルシートはお好みで）
    setStyleSheet("background-color: black; color: white;");
    // フォーカスをこのWidgetに強制し、キーイベントを受け取れるようにする
    setFocusPolicy(Qt::StrongFocus);

	QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_view = new QGraphicsView(this);
    m_view->setStyleSheet("background: transparent; border: none;"); // 枠を完全に消す
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);    // スクロールバー非表示
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setRenderHint(QPainter::Antialiasing);                  // アンチエイリアス有効
    layout->addWidget(m_view);
    setupScene();
}
SlideShow::~SlideShow()
{}

void SlideShow::setupScene() {
	const int slideWidth = 1920;
    const int slideHeight = 1080;

    m_scene = new QGraphicsScene(0, 0, slideWidth, slideHeight, this);
    m_view->setScene(m_scene);
    m_view->setSceneRect(0, 0, slideWidth, slideHeight);

    // 4. スライドの内容を表示する QTextEdit を作成
    m_textEdit = new QTextEdit();
    m_textEdit->setFrameStyle(QFrame::NoFrame);                      // 枠線なし
    m_textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 右のスクロールバー非表示
    m_textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 下のスクロールバー非表示
    m_textEdit->setReadOnly(true);                                   // プレゼン中は読み取り専用

    // 5. QTextEdit の見栄えを整える（お好みでカスタマイズしてください）
    m_textEdit->setStyleSheet(
        "background-color: #1e1e1e; " // スライドの背景色
        "color: #ffffff; "             // 文字色
        "font-family: 'Helvetica Neue', Arial, sans-serif; "
        "font-size: 48px; "            // スライドに適した大きな文字
        "padding: 80px;"               // スライドの周囲に余白をとる
    );

    // テキスト（HTMLによるマークアップ）の設定例
    m_textEdit->setHtml(R"(
        <h1 align="center" style="color: #4CAF50; font-size: 72px; margin-bottom: 30px;">マークダウンエディタ viMarkdown に<br>
SVG ブロックを実装してみたら<br>図が意外と簡単に描けるようになった話</h1>
    )");
#if 0
    m_textEdit->setHtml(R"(
        <h1 style="color: #4CAF50; font-size: 72px; margin-bottom: 30px;">Slide Title</h1>
        <p>This is a slide rendered with <b>QTextEdit</b> inside a <b>QGraphicsScene</b>.</p>
        <ul style="margin-top: 20px; line-height: 1.5;">
            <li>No.1: Extremely smooth scaling and rotation!</li>
            <li>No.2: Built using Qt's Graphics View Framework.</li>
            <li>No.3: Scaled automatically to fit any projector resolution.</li>
        </ul>
    )");
#endif

    // 6. QTextEdit をシーンに追加（自動的に QGraphicsProxyWidget にラップされます）
    QGraphicsProxyWidget *proxy = m_scene->addWidget(m_textEdit);
    
    // スライド全面（1920x1080）に広げて配置
    proxy->setGeometry(QRectF(0, 0, slideWidth, slideHeight));
}
