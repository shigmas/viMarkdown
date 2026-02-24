#include <QDir>
#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QString exeDir = QCoreApplication::applicationDirPath();
	QDir::setCurrent(exeDir);
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/MainWindow/images/viMarkdown.png"));
    QCoreApplication::setOrganizationName("VisualSoftLab");
    QCoreApplication::setApplicationName("viMarkdown");
    MainWindow window;
    if (app.arguments().count() > 1) {
        QString filePath = app.arguments().at(1);
        window.do_open("", filePath);
    }
    window.show();
    return app.exec(); 
}
