#include "mainwindow.h"

#include <QApplication>
#include <QDir>
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
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "viMarkdown_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return app.exec();
}
