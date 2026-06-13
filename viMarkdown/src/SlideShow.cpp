#include "SlideShow.h"

SlideShow::SlideShow(QWidget *parent)
	: QWidget(parent)
{
    // 枠なしのウィンドウに設定し、タスクバーやメニューバーを完全に隠すフラグを設定
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    
    // スライドショーの背景色を黒、文字を白にする（スタイルシートはお好みで）
    setStyleSheet("background-color: black; color: white;");
    
    // フォーカスをこのWidgetに強制し、キーイベントを受け取れるようにする
    setFocusPolicy(Qt::StrongFocus);
}
SlideShow::~SlideShow()
{}

