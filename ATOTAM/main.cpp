#include "mainwindow.h"
#include "Entities/samos.h"
#include "physics.h"

#include <QApplication>
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

void gameClock(MainWindow* w) {
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
        if ((*g->getInputList())["SPECIAL_fastForward"])
            waitTime /= 4;
        else if ((*g->getInputList())["SPECIAL_slowForward"])
            waitTime *= 4;
        auto end = std::chrono::high_resolution_clock::now() + std::chrono::microseconds(waitTime);

        if (g->getTasToolEnabled()) {
            w->getSpecialInputs();
            g->updateSpecialInputs();
            if ((!((*g->getInputList())["SPECIAL_frameAdvance"]
                    && (*g->getInputTime())["SPECIAL_frameAdvance"] == 0)
                    && g->getFrameAdvance())
                    && !(*g->getInputList())["SPECIAL_slowForward"]
                    && !(*g->getInputList())["SPECIAL_fastForward"]) {
                // Wait between frames
                while (std::chrono::high_resolution_clock::now() < end) {
                    std::this_thread::sleep_for(std::chrono::microseconds(999));
                }
                continue;
            }

            g->updateTas();
        }
        if (!g->getTas()) {
            w->getInputs();
        }

        // Update FPS if it has to
        if ((std::chrono::high_resolution_clock::now() - g->getLastFpsShown()).count() > g->getShowFpsUpdateRate())
            g->setFps(1000000000 / (std::chrono::high_resolution_clock::now() - g->getLastFrameTime()).count());

        // And update the last frame time
        g->setLastFrameTime(std::chrono::high_resolution_clock::now());

        prevCount = std::round(g->getFrameCount() * 60.0 / Physics::frameRate);
        g->setFrameCount(g->getFrameCount() + 1);
        g->setUpdateCount(std::round(g->getFrameCount() * 60.0 / Physics::frameRate));

        if (g->getDoorTransition() == "") {

            g->updateAsyncRoomLoading();

            if (!g->getIsPaused()) {
                if (!g->getInInventory() && !g->getInMap()) {
                    if (!g->getMapViewer()) {
                        // Update physics
                        if (g->getS() != nullptr) {
                            g->addEntities(Physics::updateSamos(g));
                            g->updateNPCs();
                            g->updateCamera();
                        }
                        std::tuple<std::string, std::vector<Entity*>, std::vector<Entity*>, Map, Save> physicsOutput
                                = Physics::updatePhysics(g);
                        g->setDoorTransition(std::get<0>(physicsOutput));
                        g->addEntities(std::get<1>(physicsOutput));
                        g->removeEntities(std::get<2>(physicsOutput));
                        g->setCurrentMap(std::get<3>(physicsOutput));
                        g->setCurrentProgress(std::get<4>(physicsOutput));

                        if (g->getS()->getHealth() <= 0) {
                            g->die();
                        }
                    } else {
                        g->updateMapViewer();
                    }

                    if (prevCount != g->getUpdateCount())
                        g->updateAnimations();
                }

                g->updateInventory();
            }
            if (!g->getInInventory() && !g->getInMap())
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
            if (g->getS() != nullptr) {
                // Make sure we can't pause while changing room
                g->setInInventory(false);
                g->setInMap(false);
                g->setIsPaused(false);

                nlohmann::json roomJson = (*g->getCurrentMap().getJson())["rooms"][g->getCurrentMap().getCurrentRoomId()];

                int roomS_x = roomJson["position"][0];
                int roomS_y = roomJson["position"][1];
                int roomE_x = roomJson["size"][0];
                int roomE_y = roomJson["size"][1];
                roomE_x += roomS_x;
                roomE_y += roomS_y;

                // Set starting values

                if (startingCameraPos == nullptr) {
                    startingCameraPos = new QPoint(g->getCamera());
                    startingSamosPos = QPoint(g->getS()->getX(), g->getS()->getY());
                    timeLeftCameraMove = cameraMoveTime;
                    if (g->getDoorTransition() == "Right") {
                        cameraDist.setX(roomS_x);
                        cameraDist.setY(g->getS()->getY() + static_cast<int>(Entity::values["general"]["camera_ry"]));
                    } else if (g->getDoorTransition() == "Left") {
                        cameraDist.setX(roomE_x - g->getCameraSize().first);
                        cameraDist.setY(g->getS()->getY() + static_cast<int>(Entity::values["general"]["camera_ry"]));
                    } else if (g->getDoorTransition() == "Up") {
                        cameraDist.setX(g->getS()->getX() + static_cast<int>(Entity::values["general"]["camera_rx"]));
                        cameraDist.setY(roomE_y - g->getCameraSize().second);
                    } else if (g->getDoorTransition() == "Down") {
                        cameraDist.setX(g->getS()->getX() + static_cast<int>(Entity::values["general"]["camera_rx"]));
                        cameraDist.setY(roomS_y);
                    }
                    if (cameraDist.x() < roomS_x)
                        cameraDist.setX(roomS_x);
                    else if (cameraDist.x() + g->getCameraSize().first > roomE_x)
                        cameraDist.setX(roomE_x - g->getCameraSize().first);
                    if (cameraDist.y() < roomS_y)
                        cameraDist.setY(roomS_y);
                    else if (cameraDist.y() + g->getCameraSize().second > roomE_y)
                        cameraDist.setY(roomE_y - g->getCameraSize().second);

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
                    g->getS()->setX(samosPos);
                } else if (g->getDoorTransition() == "Left") {
                    samosPos = Cubic::easeOut(cameraMoveTime - timeLeftCameraMove, startingSamosPos.x(), -samosDoorMove, cameraMoveTime);
                    g->getS()->setX(samosPos);
                } else if (g->getDoorTransition() == "Up") {
                    samosPos = Cubic::easeOut(cameraMoveTime - timeLeftCameraMove, startingSamosPos.y(), -1.5*samosDoorMove, cameraMoveTime);
                    g->getS()->setY(samosPos);
                } else if (g->getDoorTransition() == "Down") {
                    samosPos = Cubic::easeOut(cameraMoveTime - timeLeftCameraMove, startingSamosPos.y(), 1.25*samosDoorMove, cameraMoveTime);
                    g->getS()->setY(samosPos);
                }

                // When the move is over
                if (timeLeftCameraMove <= 0) {
                    delete startingCameraPos;
                    startingCameraPos = nullptr;
                    g->setDoorTransition("");
                    g->getS()->setRoomId(g->getCurrentMap().getCurrentRoomId());

                    g->addRoomDiscovered(g->getCurrentMap().getName(), g->getCurrentMap().getCurrentRoomId());

                    // Stop updating the last room
                    g->removeOtherRoomsEntities();
                    g->updateLoadedRooms();

                    g->updateAnimations();
                }
            }
        }

        // Only paint if not in ultra fast forward or every 60 frames
        if (!g->getUltraFastForward() || g->getFrameCount() % 60 == 0) {
            // Eventually render the game
            if (!w->getCopyingToDraw()) {
                w->setRender(false);
                w->setupToDraw();
            }
            w->setRender(true);
            w->update();
        }

        // Only wait if not in ultra fast forward
        if (!g->getUltraFastForward())
            // Wait between frames
            while (std::chrono::high_resolution_clock::now() < end) {
                std::this_thread::sleep_for(std::chrono::microseconds(999));
            }
    }
    w->setRender(true);

    // Close the window
    if (w != nullptr) {
        w->getGame()->clearEntities();
        w->close();
    }

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

    // Start the game update clock
    std::future<void> game = std::async(gameClock, &w);
    return a.exec();
}
