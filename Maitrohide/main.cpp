#include "mainwindow.h"
#include "Rendering/renderable.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QImage>
#include <iostream>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Maitrohide_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w(&a);
    w.show();
    Renderable renderable1(new QImage("../assets/Image.png"), 10, 10);
    w.addRenderable(&renderable1);
    return a.exec();
}
