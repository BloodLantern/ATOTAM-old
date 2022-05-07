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
    double waitTime = 1.0/(MainWindow::frameRate * MainWindow::gameSpeed);
    //waitTime /=2;
    while (MainWindow::running) {
        preciseSleep(waitTime);
        //preciseSleep(waitTime);
        MainWindow::getInputs();
        w->updateSamos(s);
        w->updatePhysics();
        w->updateAnimations();
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

    MainWindow::loadGeneral();
    MainWindow w(&a);
    //w.show();
    Samos s(100, 300, 99, 5, 5);
    w.showFullScreen();
    /*nlohmann::json entJson = Entity::values["names"]["Samos"];
    nlohmann::json textureJson = Entity::values["textures"][entJson["texture"]];
    nlohmann::json variantJson = textureJson["Standing"];
    QImage fullImage(QString::fromStdString(std::string("../assets/textures/") + std::string(textureJson["Standing"]["file"])));
    QImage image = fullImage.copy(variantJson["x"], variantJson["y"], variantJson["width"], variantJson["height"]);
    //QImage img1("../assets/Image.png");
    Samos s(100, 10, 99, 5, 5, new CollisionBox(20, 0, 15*MainWindow::renderingMultiplier, 43*MainWindow::renderingMultiplier), &image, "Samos", 99, true, "Right", 1, "Samos", true);*/
    w.addRenderable(&s);
    QImage sol("../assets/sol.png");
    QImage mur("../assets/mur.png");
    Terrain s1(60, 400, new CollisionBox(0, 0, 300*MainWindow::renderingMultiplier, 30*MainWindow::renderingMultiplier), &sol, "Terrain");
    w.addRenderable(&s1);
    Terrain s2(60, 1020, new CollisionBox(0, 0, 300*MainWindow::renderingMultiplier, 30*MainWindow::renderingMultiplier), &sol, "Terrain");
    w.addRenderable(&s2);
    Terrain s3(660, 1020, new CollisionBox(0, 0, 300*MainWindow::renderingMultiplier, 30*MainWindow::renderingMultiplier), &sol, "Terrain");
    w.addRenderable(&s3);
    Terrain s4(1260, 1020, new CollisionBox(0, 0, 300*MainWindow::renderingMultiplier, 30*MainWindow::renderingMultiplier), &sol, "Terrain");
    w.addRenderable(&s4);
    Terrain s5(60, 0, new CollisionBox(0, 0, 300*MainWindow::renderingMultiplier, 30*MainWindow::renderingMultiplier), &sol, "Terrain");
    w.addRenderable(&s5);
    Terrain s6(660, 0, new CollisionBox(0, 0, 300*MainWindow::renderingMultiplier, 30*MainWindow::renderingMultiplier), &sol, "Terrain");
    w.addRenderable(&s6);
    Terrain s7(1260, 0, new CollisionBox(0, 0, 300*MainWindow::renderingMultiplier, 30*MainWindow::renderingMultiplier), &sol, "Terrain");
    w.addRenderable(&s7);
    Terrain s8(400, 625, new CollisionBox(0, 0, 300*MainWindow::renderingMultiplier, 30*MainWindow::renderingMultiplier), &sol, "Terrain");
    w.addRenderable(&s8);
    Terrain s9(200, 850, new CollisionBox(0, 0, 300*MainWindow::renderingMultiplier, 30*MainWindow::renderingMultiplier), &sol, "Terrain");
    w.addRenderable(&s9);
    Terrain s10(1060, 300, new CollisionBox(0, 0, 300*MainWindow::renderingMultiplier, 30*MainWindow::renderingMultiplier), &sol, "Terrain");
    w.addRenderable(&s10);
    Terrain m1(0, 0, new CollisionBox(0, 0, 30*MainWindow::renderingMultiplier, 300*MainWindow::renderingMultiplier), &mur, "Terrain");
    w.addRenderable(&m1);
    Terrain m2(0, 600, new CollisionBox(0, 0, 30*MainWindow::renderingMultiplier, 300*MainWindow::renderingMultiplier), &mur, "Terrain");
    w.addRenderable(&m2);
    Terrain m3(1860, 0, new CollisionBox(0, 0, 30*MainWindow::renderingMultiplier, 300*MainWindow::renderingMultiplier), &mur, "Terrain");
    w.addRenderable(&m3);
    Terrain m4(1860, 600, new CollisionBox(0, 0, 30*MainWindow::renderingMultiplier, 300*MainWindow::renderingMultiplier), &mur, "Terrain");
    w.addRenderable(&m4);
    Terrain m5(1600, 300, new CollisionBox(0, 0, 30*MainWindow::renderingMultiplier, 300*MainWindow::renderingMultiplier), &mur, "Terrain");
    w.addRenderable(&m5);
    std::future<void> fobj1 = std::async(gameClock, &w, &s);
    return a.exec();
}
