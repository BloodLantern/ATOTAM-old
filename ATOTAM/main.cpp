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

void gameClock(MainWindow* w, Samos* s) {
    long waitTime;
    unsigned long long prevCount;
    double timeLeftCameraMove;
    QPoint* startingCameraPos = nullptr;
    QPoint startingSamosPos;
    QPoint cameraDist;
    const double cameraMoveTime = 0.75;
    const int samosDoorMove = 100;

    Game* g = w->getGame();
    while (g->getRunning()) {
        waitTime = 1000000.0/(Physics::frameRate * g->getGameSpeed());
        auto end = std::chrono::high_resolution_clock::now() + std::chrono::microseconds(waitTime);

        // Update FPS if it has to
        if ((std::chrono::high_resolution_clock::now() - g->getLastFpsShown()).count() > g->getShowFpsUpdateRate())
            g->setFps(1000000000 / (std::chrono::high_resolution_clock::now() - g->getLastFrameTime()).count());
        // And update the last frame time
        g->setLastFrameTime(std::chrono::high_resolution_clock::now());

        if (g->getDoorTransition() == "") {
            w->getInputs();

            if (!g->getIsPaused()) {
                if (!g->getMapViewer()) {
                    // Update physics
                    if (s != nullptr) {
                        g->addEntities(Physics::updateSamos(s, g->getTerrains(), g->getDynamicObjs(), *g->getInputList(), *g->getInputTime()));
                        g->updateDialogue();
                        g->updateCamera();
                    }
                    std::tuple<std::string, std::vector<Entity*>, std::vector<Entity*>, Map> physicsOutput = Physics::updatePhysics(s,
                                                                                                                               g->getTerrains(),
                                                                                                                               g->getDynamicObjs(),
                                                                                                                               g->getMonsters(),
                                                                                                                               g->getAreas(),
                                                                                                                               g->getNPCs(),
                                                                                                                               g->getProjectiles(),
                                                                                                                               g->getCurrentMap());
                    g->setDoorTransition(std::get<0>(physicsOutput));
                    g->addEntities(std::get<1>(physicsOutput));
                    g->removeEntities(std::get<2>(physicsOutput));
                    g->setCurrentMap(std::get<3>(physicsOutput));
                } else {
                    g->updateMapViewer();
                }



                prevCount = std::round(g->getFrameCount() * 60.0 / Physics::frameRate);
                g->setFrameCount(g->getFrameCount() + 1);
                g->setUpdateCount(std::round(g->getFrameCount() * 60.0 / Physics::frameRate));
                if (prevCount != g->getUpdateCount())
                    g->updateAnimations();
            }

            g->updateMenu();

            // Fullscreen update
            if (w->isFullScreen()) {
                if (!g->getFullscreen())
                    w->showNormal();
            } else {
                if (g->getFullscreen())
                    w->showFullScreen();
            }
        } else {
            if (s != nullptr) {
                // Make sure we can't pause while changing room
                g->setIsPaused(false);

                nlohmann::json mapJson = g->getCurrentMap().getJson()["rooms"][std::to_string(g->getCurrentMap().getCurrentRoomId())];

                int roomS_x = mapJson["position"][0];
                int roomS_y = mapJson["position"][1];
                int roomE_x = mapJson["size"][0];
                int roomE_y = mapJson["size"][1];
                roomE_x += roomS_x;
                roomE_y += roomS_y;

                // Set starting values

                if (startingCameraPos == nullptr) {
                    startingCameraPos = new QPoint(g->getCamera());
                    startingSamosPos = QPoint(s->getX(), s->getY());
                    timeLeftCameraMove = cameraMoveTime;
                    if (g->getDoorTransition() == "Right") {
                        cameraDist.setX(roomS_x);
                        cameraDist.setY(s->getY() + static_cast<int>(Entity::values["general"]["camera_ry"]));
                    } else if (g->getDoorTransition() == "Left") {
                        cameraDist.setX(roomE_x - 1920);
                        cameraDist.setY(s->getY() + static_cast<int>(Entity::values["general"]["camera_ry"]));
                    } else if (g->getDoorTransition() == "Up") {
                        cameraDist.setX(s->getX() + static_cast<int>(Entity::values["general"]["camera_rx"]));
                        cameraDist.setY(roomE_y - 1080);
                    } else if (g->getDoorTransition() == "Down") {
                        cameraDist.setX(s->getX() + static_cast<int>(Entity::values["general"]["camera_rx"]));
                        cameraDist.setY(roomS_y);
                    }
                    if (cameraDist.x() < roomS_x)
                        cameraDist.setX(roomS_x);
                    else if (cameraDist.x() + 1920 > roomE_x)
                        cameraDist.setX(roomE_x - 1920);
                    if (cameraDist.y() < roomS_y)
                        cameraDist.setY(roomS_y);
                    else if (cameraDist.y() + 1080 > roomE_y)
                        cameraDist.setY(roomE_y - 1080);

                    cameraDist.setX(cameraDist.x() - startingCameraPos->x());
                    cameraDist.setY(cameraDist.y() - startingCameraPos->y());
                } else
                    // Set time left
                    timeLeftCameraMove -= 1 / Physics::frameRate;

                // Set camera position
                QPoint camera = g->getCamera();

                camera.setX(Cubic::easeInOut(cameraMoveTime - timeLeftCameraMove, startingCameraPos->x(), cameraDist.x(), cameraMoveTime));
                camera.setY(Cubic::easeInOut(cameraMoveTime - timeLeftCameraMove, startingCameraPos->y(), cameraDist.y(), cameraMoveTime));

                g->setCamera(camera);

                // Set Samos position
                int samosPos = 0;
                if (g->getDoorTransition() == "Right") {
                    samosPos = Cubic::easeOut(cameraMoveTime - timeLeftCameraMove, startingSamosPos.x(), samosDoorMove, cameraMoveTime);
                    s->setX(samosPos);
                } else if (g->getDoorTransition() == "Left") {
                    samosPos = Cubic::easeOut(cameraMoveTime - timeLeftCameraMove, startingSamosPos.x(), -samosDoorMove, cameraMoveTime);
                    s->setX(samosPos);
                } else if (g->getDoorTransition() == "Up") {
                    samosPos = Cubic::easeOut(cameraMoveTime - timeLeftCameraMove, startingSamosPos.y(), -1.5*samosDoorMove, cameraMoveTime);
                    s->setY(samosPos);
                } else if (g->getDoorTransition() == "Down") {
                    samosPos = Cubic::easeOut(cameraMoveTime - timeLeftCameraMove, startingSamosPos.y(), 1.25*samosDoorMove, cameraMoveTime);
                    s->setY(samosPos);
                }

                // When the move is over
                if (timeLeftCameraMove <= 0) {
                    delete startingCameraPos;
                    startingCameraPos = nullptr;
                    g->setDoorTransition("");
                    s->setRoomId(g->getCurrentMap().getCurrentRoomId());

                    // Unload the last room
                    g->removeOtherRoomsEntities();

                    g->updateAnimations();
                }
            }
        }

        // Eventually render the game
        w->setRender(true);
        w->update();

        while (std::chrono::high_resolution_clock::now() < end) {
            std::this_thread::sleep_for(std::chrono::microseconds(999));
        }
    }

    // Close the window
    if (w != nullptr)
        w->close();

    // Just in case but at this point 'startingCameraPos' should be a nullptr anyway
    delete startingCameraPos;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set the random seed
    std::srand(time(NULL));
    std::string assetsPath = "../ATOTAM/assets";
    Entity::values = Entity::loadValues(assetsPath);
    MainWindow w(&a, assetsPath);
    if (w.getGame()->getFullscreen())
        w.showFullScreen();
    else
        w.show();

    // Only instantiate samos if not in map viewer mode
    Samos* sp = nullptr;
    if (!w.getGame()->getMapViewer()) {
        sp = new Samos(500, 300, 99, 5, 5);
        w.getGame()->addEntity(sp);
    }

    // Load map
    for (Entity* entity : w.getGame()->getCurrentMap().loadRoom())
        w.getGame()->addEntity(entity);

    // Start the game update clock
    std::future<void> game = std::async(gameClock, &w, sp);
    return a.exec();
}
