#include "mainwindow.h"
#include "Rendering/renderer.h"

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
    Renderer renderer(&w);
    Renderable renderable(new QImage("assets/Image.png"), 300, 300);
    renderer.render(&renderable);
    return a.exec();
}
