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

void gameClock(MainWindow* w, Samos* s) {
    long waitTime = 1000000.0/(MainWindow::frameRate * MainWindow::gameSpeed);
    unsigned long long prevCount;
    while (MainWindow::running) {
        auto end = std::chrono::high_resolution_clock::now() + std::chrono::microseconds(waitTime);

        // Update FPS if it has to
        if ((std::chrono::high_resolution_clock::now() - MainWindow::lastFpsShown).count() > MainWindow::showFpsUpdateRate)
            MainWindow::fps = 1000000000 / (std::chrono::high_resolution_clock::now() - MainWindow::lastFrameTime).count();
        // And update the last frame time
        MainWindow::lastFrameTime = std::chrono::high_resolution_clock::now();

        w->getInputs();

        if (!MainWindow::isPaused) {
            if (!MainWindow::mapViewer) {
                // Update physics
                w->updateSamos(s);
                w->updatePhysics();
            }

            prevCount = std::round(MainWindow::frameCount * 60.0 / MainWindow::frameRate);
            MainWindow::frameCount++;
            MainWindow::updateCount = std::round(MainWindow::frameCount * 60.0 / MainWindow::frameRate);
            if (prevCount != MainWindow::updateCount)
                w->updateAnimations();
        }

        w->updateMenu();

        // Eventually render the game
        w->update();


        while (std::chrono::high_resolution_clock::now() < end) {
            std::this_thread::sleep_for(std::chrono::microseconds(999));
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
    w.showFullScreen();
    Samos s(500, 300, 99, 5, 5);
    w.addRenderable(&s);
    Terrain s1(60, 400, "Ground");
    w.addRenderable(&s1);
    Terrain s2(60, 1020, "Ground");
    w.addRenderable(&s2);
    Terrain s3(660, 1020, "Ground");
    w.addRenderable(&s3);
    Terrain s4(1260, 1020, "Ground");
    w.addRenderable(&s4);
    Terrain s5(60, 0, "Ground");
    w.addRenderable(&s5);
    Terrain s6(660, 0, "Ground");
    w.addRenderable(&s6);
    Terrain s7(1260, 0, "Ground");
    w.addRenderable(&s7);
    Terrain s8(400, 625, "Ground");
    w.addRenderable(&s8);
    Terrain s9(200, 850, "Ground");
    w.addRenderable(&s9);
    Terrain s10(1060, 300, "Ground");
    w.addRenderable(&s10);
    Terrain m1(0, 0, "Wall");
    w.addRenderable(&m1);
    Terrain m2(0, 600, "Wall");
    w.addRenderable(&m2);
    Terrain m3(1860, 0, "Wall");
    w.addRenderable(&m3);
    Terrain m4(1860, 600, "Wall");
    w.addRenderable(&m4);
    Terrain m5(1600, 300, "Wall");
    w.addRenderable(&m5);
    std::future<void> game = std::async(gameClock, &w, &s);
    return a.exec();
}
