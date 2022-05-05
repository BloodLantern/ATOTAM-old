#include "mainwindow.h"
#include "Entities/samos.h"

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
#include <fstream>

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

unsigned long frameCount = 0;

void gameClock(MainWindow* w) {
    double waitTime = 1.0/MainWindow::frameRate;
    waitTime /=2;
    while (MainWindow::running) {
        preciseSleep(waitTime);
        //preciseSleep(waitTime);
        w->updatePhysics();
        if (frameCount % 5 == 0)
            w->updateAnimations();
        w->update();
    }
    frameCount++;
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
    std::string variant = "Idle";
    nlohmann::json entJson = Entity::values["names"]["Samos"];
    nlohmann::json textureJson = Entity::values["textures"][entJson["texture"]];
    nlohmann::json variantJson = textureJson["variants"][variant];
    QImage fullImage(QString::fromStdString(std::string("../assets/textures/") + std::string(textureJson["variants"][variant]["file"])));
    QImage image = fullImage.copy(variantJson["x"], variantJson["y"], variantJson["width"], variantJson["height"]);
    Samos s(10, 10, 99, 5, 5, new CollisionBox(0, 0, 26, 43), &image, "Samos", 99, true, "Right", 1, "Samos", true);
    w.addRenderable(&s);
    w.update();
    //std::future<void> fobj1 = std::async(gameClock, &w);
    return a.exec();
}
