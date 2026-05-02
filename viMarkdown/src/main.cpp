#include <QDir>
#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QString exeDir = QCoreApplication::applicationDirPath();
	QDir::setCurrent(exeDir);
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/MainWindow/images/viMarkdown.png"));
    QCoreApplication::setOrganizationName("VisualSoftLab");
    QCoreApplication::setApplicationName("viMarkdown");

    QTranslator translator;
    QLocale systemLocale = QLocale::system();
    if (systemLocale.language() == QLocale::Japanese) {		//	日本語環境の場合
	    if (translator.load(":/MainWindow/viMarkdown_ja.qm")) {
	        app.installTranslator(&translator);
	    }
    }
    
    MainWindow window;
    if (app.arguments().count() > 1) {
        QString filePath = app.arguments().at(1);
        window.do_open("", filePath);
    }
    window.show();
    qDebug() << "OK";
    return app.exec(); 
}
