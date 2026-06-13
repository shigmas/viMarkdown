#pragma once

#include <QWidget>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTextEdit>

class SlideShow  : public QWidget
{
	Q_OBJECT

public:
	SlideShow(QWidget *parent);
	~SlideShow();

    void startPresentation() {    // スライドショー（フルスクリーン）を開始
	    showFullScreen();
	    raise();            // 最前面に移動
	    activateWindow();   // ウィンドウをアクティブ化してキー入力を受け付ける状態にする
	}
protected:
    // キーボード入力をフックする
    void keyPressEvent(QKeyEvent *event) override {
	    switch (event->key()) {
        case Qt::Key_Escape:    // Esc または 'q' でスライドショーを終了（閉じる）
        case Qt::Key_Q:
            close();
            break;
        default:
            QWidget::keyPressEvent(event);
            break;
	    }
    }
private:
    void setupScene();
    void nextSlide();
    void prevSlide();

	QGraphicsView  *m_view;
    QGraphicsScene *m_scene;
    QTextEdit      *m_textEdit;
};

