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

void animationClock(MainWindow* w) {
    long waitTime = 1000000.0/(MainWindow::updateRate * MainWindow::gameSpeed);
    while (MainWindow::running) {
        auto end = std::chrono::high_resolution_clock::now() + std::chrono::microseconds(waitTime);

        w->updateAnimations();
        MainWindow::updateCount++;

        while (std::chrono::high_resolution_clock::now() < end) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}

void gameClock(MainWindow* w, Samos* s) {
    long waitTime = 1000000.0/(MainWindow::frameRate * MainWindow::gameSpeed);
    while (MainWindow::running) {
        auto end = std::chrono::high_resolution_clock::now() + std::chrono::microseconds(waitTime);

        // Update FPS if it has to
        if ((std::chrono::high_resolution_clock::now() - MainWindow::lastFpsShown).count() > MainWindow::showFpsUpdateRate)
            MainWindow::fps = 1000000000 / (std::chrono::high_resolution_clock::now() - MainWindow::lastFrameTime).count();
        // And update the last frame time
        MainWindow::lastFrameTime = std::chrono::high_resolution_clock::now();

        // Update physics
        w->getInputs();
        w->updateSamos(s);
        w->updatePhysics();

        // Eventually render the game
        w->update();
        MainWindow::frameCount++;

        while (std::chrono::high_resolution_clock::now() < end) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
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
    std::future<void> anim = std::async(animationClock, &w);
    std::future<void> game = std::async(gameClock, &w, &s);
    return a.exec();
}
