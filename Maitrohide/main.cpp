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
#include <chrono>
#include <thread>
#include <math.h>

void preciseSleep(double seconds) {

    double estimate = 5e-3;
    double mean = 5e-3;
    double m2 = 0;
    int64_t count = 1;

    while (seconds > estimate) {
        auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto end = std::chrono::high_resolution_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double delta = observed - mean;
        mean += delta / count;
        m2   += delta * (observed - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    auto start = std::chrono::high_resolution_clock::now();
    while ((std::chrono::high_resolution_clock::now() - start).count() / 1e9 < seconds);
}

bool arrete = true;
float frameRate = 144.0;

void test1(Renderable* renderable, MainWindow* w) {
    auto start = std::chrono::high_resolution_clock::now();
    double waitTime = 1.0/frameRate;
    //waitTime /=2;
    for (int i = 0; i<frameRate; i++) {
        preciseSleep(waitTime);
        //preciseSleep(waitTime);
        renderable->setX(i*1000/frameRate);
        w->update();
    }
    std::cout << (std::chrono::high_resolution_clock::now() - start).count();
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
