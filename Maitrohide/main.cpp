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

double frameRate = 60.0;

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

void test1(MainWindow* w) {
    double waitTime = 1.0/frameRate;
    waitTime /=2;
    for (int i = 0; i<2*frameRate; i++) {
        preciseSleep(waitTime);
        //preciseSleep(waitTime);
        for (Entity* ent : w->getRendering())
            ent->updateV(frameRate);
        w->update();
    }
    for (Entity* ent : w->getRendering())
        std::cout << ent->getX() << std::endl;
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
    nlohmann::json entJson = Entity::values["names"]["Samos"];
    nlohmann::json textureJson = Entity::values["textures"][entJson["texture"]];
    nlohmann::json variantJson = textureJson["variants"]["standing"];
    QImage fullImage(QString::fromStdString(std::string("../assets/textures/") + std::string(textureJson["file"])));
    QImage image = fullImage.copy(variantJson["x"], variantJson["y"], variantJson["width"], variantJson["height"]);
    Samos s(10, 10, 99, 5, 5, new CollisionBox(0, 0, 26, 43), &image, Entity::EntityType::Samos, 99, true, Entity::Direction::Right, "Samos");
    w.addRenderable(&s);
    //std::cout << w.getRendering()[0] << std::endl;
    w.update();
    //std::future<void> fobj1 = std::async(test1, &w);
    return a.exec();
}
