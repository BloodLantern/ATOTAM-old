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
    long waitTime;
    unsigned long long prevCount;
    while (MainWindow::running) {
        waitTime = 1000000.0/(MainWindow::frameRate * MainWindow::gameSpeed);
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
    // Load map
    for (Entity* entity : Map::loadMap(Map("test")))
        w.addRenderable(entity);
    // Only instantiate samos if not in map viewer mode
    if (!MainWindow::mapViewer) {
        Samos s(500, 300, 99, 5, 5);
        w.addRenderable(&s);
        // Start the game update clock
        std::future<void> game = std::async(gameClock, &w, &s);
    } else
        // Start the game update clock
        std::future<void> game = std::async(gameClock, &w, nullptr);
    return a.exec();
}
