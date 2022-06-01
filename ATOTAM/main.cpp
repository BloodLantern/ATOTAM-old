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

#include <Easing/Cubic.h>
#include <Easing/Sine.h>

void gameClock(MainWindow* w, Samos* s) {
    long waitTime;
    unsigned long long prevCount;
    double timeLeftCameraMove;
    QPoint* startingCameraPos = nullptr;
    QPoint startingSamosPos;
    const double cameraMoveTime = 0.75;
    const int samosDoorMove = 100;
    while (MainWindow::running) {
        waitTime = 1000000.0/(MainWindow::frameRate * MainWindow::gameSpeed);
        auto end = std::chrono::high_resolution_clock::now() + std::chrono::microseconds(waitTime);

        // Update FPS if it has to
        if ((std::chrono::high_resolution_clock::now() - MainWindow::lastFpsShown).count() > MainWindow::showFpsUpdateRate)
            MainWindow::fps = 1000000000 / (std::chrono::high_resolution_clock::now() - MainWindow::lastFrameTime).count();
        // And update the last frame time
        MainWindow::lastFrameTime = std::chrono::high_resolution_clock::now();

        if (MainWindow::doorTransition == "") {
            w->getInputs();

            if (!MainWindow::isPaused) {
                if (!MainWindow::mapViewer) {
                    // Update physics
                    if (s != nullptr)
                        w->updateSamos(s);
                    w->updatePhysics();
                } else {
                    w->updateMapViewer();
                }



                prevCount = std::round(MainWindow::frameCount * 60.0 / MainWindow::frameRate);
                MainWindow::frameCount++;
                MainWindow::updateCount = std::round(MainWindow::frameCount * 60.0 / MainWindow::frameRate);
                if (prevCount != MainWindow::updateCount)
                    w->updateAnimations();
            }

            w->updateMenu();
        } else {
            if (s != nullptr) {
                // Make sure we can't pause while changing room
                MainWindow::isPaused = false;

                // Set starting values
                if (startingCameraPos == nullptr) {
                    startingCameraPos = new QPoint(w->getCamera());
                    startingSamosPos = QPoint(s->getX(), s->getY());
                    timeLeftCameraMove = cameraMoveTime;
                } else
                    // Set time left
                    timeLeftCameraMove -= 1 / MainWindow::frameRate;

                // Set camera position
                QPoint camera = w->getCamera();
                if (MainWindow::doorTransition == "Right")
                    camera.setX(Cubic::easeInOut(cameraMoveTime - timeLeftCameraMove, startingCameraPos->x(), 1920, cameraMoveTime));
                else if (MainWindow::doorTransition == "Left")
                    camera.setX(Cubic::easeInOut(cameraMoveTime - timeLeftCameraMove, startingCameraPos->x(), -1920, cameraMoveTime));
                else if (MainWindow::doorTransition == "Up")
                    camera.setY(Cubic::easeInOut(cameraMoveTime - timeLeftCameraMove, startingCameraPos->y(), /*Remember that Y axis is reversed*/ -1080, cameraMoveTime));
                else if (MainWindow::doorTransition == "Down")
                    camera.setY(Cubic::easeInOut(cameraMoveTime - timeLeftCameraMove, startingCameraPos->y(), /*See above*/ 1080, cameraMoveTime));
                w->setCamera(camera);

                // Set Samos position
                int samosPos = 0;
                if (MainWindow::doorTransition == "Right") {
                    samosPos = Sine::easeOut(cameraMoveTime - timeLeftCameraMove, startingSamosPos.x(), samosDoorMove, cameraMoveTime);
                    s->setX(samosPos);
                } else if (MainWindow::doorTransition == "Left") {
                    samosPos = Sine::easeOut(cameraMoveTime - timeLeftCameraMove, startingSamosPos.x(), -samosDoorMove, cameraMoveTime);
                    s->setX(samosPos);
                } else if (MainWindow::doorTransition == "Up") {
                    samosPos = Sine::easeOut(cameraMoveTime - timeLeftCameraMove, startingSamosPos.y(), -samosDoorMove, cameraMoveTime);
                    s->setY(samosPos);
                } else if (MainWindow::doorTransition == "Down") {
                    samosPos = Sine::easeOut(cameraMoveTime - timeLeftCameraMove, startingSamosPos.y(), samosDoorMove, cameraMoveTime);
                    s->setY(samosPos);
                }

                // When the move is over
                if (timeLeftCameraMove <= 0) {
                    delete startingCameraPos;
                    startingCameraPos = nullptr;
                    MainWindow::doorTransition = "";

                    // Set it to delete everything
                    w->clearRendering("Samos");

                    w->addRenderable(MainWindow::currentMap.loadRoom());

                    w->updateAnimations();
                }
            }
        }

        // Eventually render the game
        w->update();


        while (std::chrono::high_resolution_clock::now() < end) {
            std::this_thread::sleep_for(std::chrono::microseconds(999));
        }
    }
    // Just in case but at this point 'startingCameraPos' should be a nullptr anyway
    delete startingCameraPos;
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

    Entity::values = Entity::loadNames(MainWindow::assetsPath);
    MainWindow::loadGeneral();
    MainWindow w(&a);
    if (MainWindow::fullscreen)
        w.showFullScreen();
    else
        w.show();

    // Only instantiate samos if not in map viewer mode
    Samos* sp = nullptr;
    if (!MainWindow::mapViewer) {
        sp = new Samos(500, 300, 99, 5, 5);
        w.addRenderable(sp);
    }

    // Load map
    for (Entity* entity : MainWindow::currentMap.loadRoom())
        w.addRenderable(entity);

    // Start the game update clock
    std::future<void> game = std::async(gameClock, &w, sp);
    return a.exec();
}
