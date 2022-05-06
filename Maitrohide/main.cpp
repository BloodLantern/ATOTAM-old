#include "mainwindow.h"
#include "Entities/samos.h"
#include "Entities/terrain.h"

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

#include <Entities/terrain.h>

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

void gameClock(MainWindow* w, Samos* s) {
    double waitTime = 1.0/MainWindow::frameRate;
    //waitTime /=2;
    while (MainWindow::running) {
        preciseSleep(waitTime);
        //preciseSleep(waitTime);
        MainWindow::getInputs();
        MainWindow::updateSamos(s);
        w->updatePhysics();
        if (MainWindow::frameCount % 4 == 0) {
            //std::cout << "a\n";
            w->updateAnimations();
            //std::cout << "b\n";
        }
        w->update();
        MainWindow::frameCount++;
    }
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
    //w.showFullScreen();
    nlohmann::json entJson = Entity::values["names"]["Samos"];
    nlohmann::json textureJson = Entity::values["textures"][entJson["texture"]];
    nlohmann::json variantJson = textureJson["Standing"];
    QImage fullImage(QString::fromStdString(std::string("../assets/textures/") + std::string(textureJson["Standing"]["file"])));
    QImage image = fullImage.copy(variantJson["x"], variantJson["y"], variantJson["width"], variantJson["height"]);
    //QImage img1("../assets/Image.png");
    Samos s(100, 10, 99, 5, 5, new CollisionBox(0, 0, 26*MainWindow::renderingMultiplier, 48*MainWindow::renderingMultiplier), &image, "Samos", 99, true, "Right", 1, "Samos", true);
    w.addRenderable(&s);
    /*QImage image = fullImage.copy(variantJson["x"], variantJson["y"], variantJson["width"], variantJson["height"]);
    Samos s(100, 10, 99, 5, 5, new CollisionBox(0, 0, 26*MainWindow::renderingMultiplier, 43*MainWindow::renderingMultiplier), &image, "Samos", 99, false, "Right", 1, "Samos", true);
    s.setState(variant);
    w.addRenderable(&s);*/
    QImage sol("../assets/sol.png");
    Terrain m1(50, 400, new CollisionBox(0, 0, 300*MainWindow::renderingMultiplier, 30*MainWindow::renderingMultiplier), &sol, "Terrain");
    w.addRenderable(&m1);
    w.update();
    std::future<void> fobj1 = std::async(gameClock, &w, &s);
    return a.exec();
}
