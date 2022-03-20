#include "mainwindow.h"
#include "Rendering/renderable.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QImage>
#include <unistd.h>
#include <future>
#include <iostream>
#include <time.h>

bool arrete = true;

void test1(Renderable* renderable, MainWindow* w) {
    int a = std::time(0);
    for (int i = 0; i<720; i++) {
        usleep(6000);
        renderable->setX(i);
        w->update();
    }
    std::cout << std::time(0) - a;
    arrete = false;
}

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
    Renderable renderable1(new QImage("../assets/Image.png"), 0, 10);
    w.addRenderable(&renderable1);
    std::future<void> fobj1 = std::async(test1,&renderable1, &w);
    return a.exec();
}
