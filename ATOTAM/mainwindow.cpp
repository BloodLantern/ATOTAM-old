#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <Entities/area.h>
#include <Entities/door.h>

bool MainWindow::running = true;
double MainWindow::frameRate;
double MainWindow::updateRate;
double MainWindow::gameSpeed;
bool MainWindow::renderHitboxes;
bool MainWindow::mapViewer;
int MainWindow::renderingMultiplier;
unsigned long long MainWindow::frameCount;
unsigned long long MainWindow::updateCount;
std::chrono::system_clock::time_point MainWindow::lastFpsShown;
std::chrono::system_clock::time_point MainWindow::lastFrameTime;
unsigned int MainWindow::fps;
bool MainWindow::showFps;
unsigned int MainWindow::showFpsUpdateRate;
double MainWindow::gravity;
std::map<std::string, bool> MainWindow::inputList;
std::map<std::string, double> MainWindow::inputTime;
const std::string MainWindow::assetsPath = "../ATOTAM/assets";
QImage MainWindow::errorTexture(QString::fromStdString(assetsPath + "/textures/error.png"));
QImage MainWindow::emptyTexture(QString::fromStdString(assetsPath + "/textures/empty.png"));
bool MainWindow::showUI = true;
bool MainWindow::isPaused = false;
bool MainWindow::fullscreen = false;
std::pair<int,int> MainWindow::resolution = {1920,1080};
Map MainWindow::currentMap = Map::loadMap("test");
double MainWindow::mapViewerCameraSpeed;
std::string MainWindow::doorTransition = "";
std::string MainWindow::language;

MainWindow::MainWindow(QApplication *app)
    : ui(new Ui::MainWindow)
    , m_qApp(app)
    //, rendering()
{
    ui->setupUi(this);
    setFixedSize(resolution.first, resolution.second);
}

MainWindow::~MainWindow()
{
    delete ui;
}

nlohmann::json MainWindow::loadJson(std::string fileName)
{
    std::ifstream keys_file(assetsPath + "/" + fileName + ".json");
    nlohmann::json temp;
    keys_file >> temp;
    return temp;
}

nlohmann::json MainWindow::params = loadJson("params");
nlohmann::json MainWindow::keyCodes = loadJson("inputs");
nlohmann::json MainWindow::stringsJson = loadJson("strings");

bool MainWindow::canChangeBox(Entity *e, CollisionBox *b)
{
    Entity ne = Entity(e->getX(), e->getY(), new CollisionBox(*b), nullptr, e->getEntType(), e->getIsAffectedByGravity(), e->getFacing(), e->getFrictionFactor(), e->getName(), e->getIsMovable());
    for (std::vector<Entity*>::iterator i = rendering.begin(); i != rendering.end(); i++) {
        if ((*i)->getEntType() == "Terrain" || (*i)->getEntType() == "DynamicObj") {
            if (Entity::checkCollision(&ne, b, *i, (*i)->getBox())) {
                Entity::calcCollisionReplacement(&ne, *i);
            }
        }
    }
    for (std::vector<Entity*>::iterator i = rendering.begin(); i != rendering.end(); i++) {
        if ((*i)->getEntType() == "Terrain" || (*i)->getEntType() == "DynamicObj") {
            if (Entity::checkCollision(&ne, b, *i, (*i)->getBox())) {
                return false;
            }
        }
    }

    return true;
}

void MainWindow::loadGeneral()
{
    //Loading the general settings of the game
    gameSpeed = Entity::values["general"]["gameSpeed"];
    updateRate = Entity::values["general"]["updateRate"];
    showFpsUpdateRate = Entity::values["general"]["showFpsUpdateRate"];
    gravity = Entity::values["general"]["gravity"];
    renderingMultiplier = Entity::values["general"]["renderingMultiplier"];
    renderHitboxes = Entity::values["general"]["renderHitboxes"];
    mapViewer = Entity::values["general"]["mapViewer"];
    currentMap = Map::loadMap(Entity::values["general"]["map"]);
    mapViewerCameraSpeed = Entity::values["general"]["mapViewerCameraSpeed"];

    frameRate = params["frameRate"];
    showFps = params["showFps"];
    fullscreen = params["fullscreen"];
    resolution.first = params["resolution_x"];
    resolution.second = params["resolution_y"];
    language = params["language"];
}

template<typename ...StdStrings>
QString MainWindow::translate(std::string text, StdStrings... subCategories)
{
    nlohmann::json json = stringsJson[language];
    for (std::string category : {subCategories...})
        json = json[category];
    return QString::fromStdString(json[text]);
}


void MainWindow::getInputs()
{
    for (std::map<std::string, bool>::iterator i = inputList.begin(); i != inputList.end(); i ++) {
        if (i->second)
            inputTime[i->first] += 1 / frameRate;
        else
            inputTime[i->first] = 0;
    }

    //Only listen for inputs if the window is currently selected
    if (isActiveWindow()) {
        //Check every key
        for (nlohmann::json::iterator i = keyCodes.begin(); i != keyCodes.end(); i++) {
            inputList[i.key()] = GetKeyState(i.value()) & 0x8000;
        }
    } else
        //Reset the keys if the window is not selected
        for (nlohmann::json::iterator i = keyCodes.begin(); i != keyCodes.end(); i++) {
            inputList[i.key()] = 0;
        }
}

bool MainWindow::updateProjectile(Projectile *p)
{
    p->setLifeTime(p->getLifeTime() - (1 / frameRate));
    if (p->getLifeTime() <= 0.0) {
        p->timeOut();
    }
    if (p->getState() == "Hit" && p->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"][p->getName()]["texture"]]["Hit"]["count"]) - 1))
        return true;
    else
        return false;
}

void MainWindow::updateMenu()
{
    if (isPaused) {
        if (inputList["menu"] && inputTime["menu"] == 0.0) {
            if (menu == "main")
                isPaused = false;
            else {
                menu = "main";
                selectedOption = 0;
            }
        }

        if (inputList["down"] && !inputList["up"] && !inputList["left"] && !inputList["right"]) {
            if (menuArrowsTime == 0.0) {
                selectedOption += 1;
                if (selectedOption >= static_cast<int>(menuOptions.size()))
                    selectedOption = 0;
            } else if (menuArrowsTime >= 5 * static_cast<double>(Entity::values["general"]["menuCoolDown"]) || (menuArrowsTime >= -1 && menuArrowsTime < 0.0)) {
                selectedOption += 1;
                if (selectedOption >= static_cast<int>(menuOptions.size()))
                    selectedOption = 0;
                menuArrowsTime = -1 - static_cast<double>(Entity::values["general"]["menuCoolDown"]);
            }

            menuArrowsTime += + 1 / frameRate;

        } else if (!inputList["down"] && inputList["up"] && !inputList["left"] && !inputList["right"]) {
            if (menuArrowsTime == 0.0) {
                selectedOption -= 1;
                if (selectedOption < 0)
                    selectedOption = menuOptions.size() - 1;
            } else if (menuArrowsTime >= 5 * static_cast<double>(Entity::values["general"]["menuCoolDown"]) || (menuArrowsTime >= -1 && menuArrowsTime < 0.0)) {
                selectedOption -= 1;
                if (selectedOption < 0)
                    selectedOption = menuOptions.size() - 1;
                menuArrowsTime = -1 - static_cast<double>(Entity::values["general"]["menuCoolDown"]);
            }

            menuArrowsTime += + 1 / frameRate;

        } else if (!inputList["down"] && !inputList["up"] && inputList["left"] && !inputList["right"]) {
            if (menuArrowsTime == 0.0) {
                if (menuOptions[selectedOption].substr(0,8) == "< FPS : ") {
                    if (frameRate > 60.0) {
                        frameRate--;
                        params["frameRate"] = frameRate;
                        std::ofstream paramsfile(assetsPath + "/params.json");
                        paramsfile << params;
                        paramsfile.close();
                    }
                } else if (menuOptions[selectedOption].substr(0,15) == "< Game speed : ") {
                    if (gameSpeed > 0.1) {
                        gameSpeed -= 0.1;
                    }
                }
            } else if (menuArrowsTime >= 5 * static_cast<double>(Entity::values["general"]["menuCoolDown"]) || (menuArrowsTime >= -1 && menuArrowsTime < 0.0)) {
                if (menuOptions[selectedOption].substr(0,8) == "< FPS : ") {
                    if (frameRate > 60.0) {
                        frameRate--;
                        params["frameRate"] = frameRate;
                        std::ofstream paramsfile(assetsPath + "/params.json");
                        paramsfile << params;
                        paramsfile.close();
                    }
                } else if (menuOptions[selectedOption].substr(0,15) == "< Game speed : ") {
                    if (gameSpeed > 0.1) {
                        gameSpeed -= 0.1;
                    }
                }
                menuArrowsTime = -1 - static_cast<double>(Entity::values["general"]["menuCoolDown"]) / 2;
            }

            menuArrowsTime += + 1 / frameRate;

        } else if (!inputList["down"] && !inputList["up"] && !inputList["left"] && inputList["right"]) {
            if (menuArrowsTime == 0.0) {
                if (menuOptions[selectedOption].substr(0,8) == "< FPS : ") {
                    if (frameRate < 144.0) {
                        frameRate++;
                        params["frameRate"] = frameRate;
                        std::ofstream paramsfile(assetsPath + "/params.json");
                        paramsfile << params;
                        paramsfile.close();
                    }
                } else if (menuOptions[selectedOption].substr(0,15) == "< Game speed : ") {
                    if (gameSpeed < 10.0) {
                        gameSpeed += 0.1;
                    }
                }
            } else if (menuArrowsTime >= 5 * static_cast<double>(Entity::values["general"]["menuCoolDown"]) || (menuArrowsTime >= -1 && menuArrowsTime < 0.0)) {
                if (menuOptions[selectedOption].substr(0,8) == "< FPS : ") {
                    if (frameRate < 144.0) {
                        frameRate++;
                        params["frameRate"] = frameRate;
                        std::ofstream paramsfile(assetsPath + "/params.json");
                        paramsfile << params;
                        paramsfile.close();
                    }
                } else if (menuOptions[selectedOption].substr(0,15) == "< Game speed : ") {
                    if (gameSpeed < 10.0) {
                        gameSpeed += 0.1;
                    }
                }
                menuArrowsTime = -1 - static_cast<double>(Entity::values["general"]["menuCoolDown"]) / 2;
            }

            menuArrowsTime += + 1 / frameRate;

        } else
            menuArrowsTime = 0.0;

        if (inputList["enter"] && inputTime["enter"] == 0.0) {
            if (menuOptions[selectedOption] == "Resume")
                isPaused = false;
            else if (menuOptions[selectedOption] == "Options") {
                menu = "options";
                selectedOption = 0;
            } else if (menuOptions[selectedOption] == "Debug") {
                menu = "debug";
                selectedOption = 0;
            } else if (menuOptions[selectedOption] == "Quit")
                close();
            else if (menuOptions[selectedOption] == "Back") {
                menu = "main";
                selectedOption = 0;
            } else if (menuOptions[selectedOption] == "Show FPS : ON") {
                showFps = false;
                params["showFps"] = showFps;
                std::ofstream paramsfile(assetsPath + "/params.json");
                paramsfile << params;
                paramsfile.close();
            } else if (menuOptions[selectedOption] == "Show FPS : OFF") {
                showFps = true;
                params["showFps"] = showFps;
                std::ofstream paramsfile(assetsPath + "/params.json");
                paramsfile << params;
                paramsfile.close();
            } else if (menuOptions[selectedOption] == "Show hitboxes : ON")
                renderHitboxes = false;
            else if (menuOptions[selectedOption] == "Show hitboxes : OFF")
                 renderHitboxes = true;
            else if (menuOptions[selectedOption] == "Max missiles")
                 for (std::vector<Entity*>::iterator i = rendering.begin(); i != rendering.end(); i++) {
                     if ((*i)->getEntType() == "Samos") {
                         Samos* s = static_cast<Samos*>(*i);
                         s->setMissileCount(s->getMaxMissileCount());
                     }
                 }
            else if (menuOptions[selectedOption] == "Max grenades")
                 for (std::vector<Entity*>::iterator i = rendering.begin(); i != rendering.end(); i++) {
                     if ((*i)->getEntType() == "Samos") {
                         Samos* s = static_cast<Samos*>(*i);
                         s->setGrenadeCount(s->getMaxGrenadeCount());
                     }
                 }
            else if (menuOptions[selectedOption] == "Max health")
                 for (std::vector<Entity*>::iterator i = rendering.begin(); i != rendering.end(); i++) {
                     if ((*i)->getEntType() == "Samos") {
                         Samos* s = static_cast<Samos*>(*i);
                         s->setHealth(s->getMaxHealth());
                     }
                 }
            else if (menuOptions[selectedOption] == "Reload entities.json") {
                Entity::values = Entity::loadValues(assetsPath);
                loadGeneral();
            } else if (menuOptions[selectedOption] == "Reload room") {
                clearRendering("Samos");
                addRenderable(currentMap.loadRoom());
            } else if (menuOptions[selectedOption] == "Reload map") {
                int mapId = currentMap.getCurrentRoomId();
                currentMap = Map::loadMap(currentMap.getName());
                currentMap.setCurrentRoomId(mapId);
            } else if (menuOptions[selectedOption] == "Map viewer mode : ON")
                mapViewer = false;
            else if (menuOptions[selectedOption] == "Map viewer mode : OFF")
                mapViewer = true;
            else if (menuOptions[selectedOption] == "Fullscreen : ON") {
                fullscreen = false;
                params["fullscreen"] = false;
                std::ofstream paramsfile(assetsPath + "/params.json");
                paramsfile << params;
                paramsfile.close();
                showNormal();
            } else if (menuOptions[selectedOption] == "Fullscreen : OFF") {
                fullscreen = true;
                params["fullscreen"] = true;
                std::ofstream paramsfile(assetsPath + "/params.json");
                paramsfile << params;
                paramsfile.close();
                showFullScreen();
            }
        }


        if (menu == "main") {
            menuOptions.clear();
            menuOptions.push_back("Resume");
            menuOptions.push_back("Options");
            menuOptions.push_back("Debug");
            menuOptions.push_back("Quit");
        } else if (menu == "options") {
            menuOptions.clear();
            menuOptions.push_back("Back");
            menuOptions.push_back(std::string("< FPS : ") + std::to_string(int(frameRate)) + " >");
            menuOptions.push_back(std::string("Show FPS : ") + (showFps ? "ON" : "OFF"));
            menuOptions.push_back(std::string("Fullscreen : ") + (fullscreen ? "ON" : "OFF"));
        } else if (menu == "debug") {
            menuOptions.clear();
            menuOptions.push_back("Back");
            menuOptions.push_back(std::string("< Game speed : ") + std::to_string(gameSpeed).substr(0,4) + " >");
            menuOptions.push_back("Max missiles");
            menuOptions.push_back("Max grenades");
            menuOptions.push_back("Max hp");
            menuOptions.push_back(std::string("Show hitboxes : ") + (renderHitboxes ? "ON" : "OFF"));
            menuOptions.push_back("Reload entities.json");
            menuOptions.push_back(std::string("Map viewer mode : ") + (mapViewer ? "ON" : "OFF"));
            menuOptions.push_back("Reload room");
            menuOptions.push_back("Reload map");
        }

    } else
        if (inputList["menu"] && inputTime["menu"] == 0.0) {
            isPaused = true;
            menu = "main";
            selectedOption = 0;
        }
}

std::vector<Entity*> MainWindow::handleCollision(Entity *obj1, Entity *obj2)
{//{Null, Terrain, Samos, Monster, Area, DynamicObj, NPC, Projectile};

    //Long if else to decide what to do between two entities after a collision
    if (obj1->getEntType() == "Terrain") {
        if (obj2->getEntType() == "Samos") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Monster") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "NPC") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Projectile") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Projectile* p = static_cast<Projectile*>(obj2);
                p->hitting(obj1);
            }
        }

    } else if (obj1->getEntType() == "Samos") {
        if (obj2->getEntType() == "Terrain") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Monster") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

            // TODO hit
        } else if (obj2->getEntType() == "Area") {
            // TODO
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Area* a = static_cast<Area*>(obj2);
                if (a->getAreaType() == "Door") {
                    Door* d = static_cast<Door*>(a);
                    currentMap.setCurrentRoomId(d->getEndingRoom());
                    std::vector<Entity*> nextRen = currentMap.loadRoom();
                    if (d->getState().find("Right") != std::string::npos)
                        doorTransition = "Right";
                    else if (d->getState().find("Left") != std::string::npos)
                        doorTransition = "Left";
                    else if (d->getState().find("Up") != std::string::npos)
                        doorTransition = "Up";
                    else if (d->getState().find("Down") != std::string::npos)
                        doorTransition = "Down";
                    return nextRen;
                }
            }
        } else if (obj2->getEntType() == "DynamicObj") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "NPC") {
            // TODO
        } else if (obj2->getEntType() == "Projectile") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Projectile* p = static_cast<Projectile*>(obj2);
                p->hitting(obj1);
            }
        }

    } else if (obj1->getEntType() == "Monster") {
        if (obj2->getEntType() == "Terrain") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Samos") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

            // TODO hit
        } else if (obj2->getEntType() == "Monster") {
            // TODO

        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "NPC") {
            // TODO

        } else if (obj2->getEntType() == "Projectile") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Projectile* p = static_cast<Projectile*>(obj2);
                p->hitting(obj1);
            }
        }

    } else if (obj1->getEntType() == "Area") {
        if (obj2->getEntType() == "Terrain") {
            // TODO

        } else if (obj2->getEntType() == "Samos") {
            // TODO
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Area* a = static_cast<Area*>(obj1);
                if (a->getAreaType() == "Door") {
                    Door* d = static_cast<Door*>(a);
                    currentMap.setCurrentRoomId(d->getEndingRoom());
                    std::vector<Entity*> nextRen = currentMap.loadRoom();
                    if (d->getState().find("Right") != std::string::npos)
                        doorTransition = "Right";
                    else if (d->getState().find("Left") != std::string::npos)
                        doorTransition = "Left";
                    else if (d->getState().find("Up") != std::string::npos)
                        doorTransition = "Up";
                    else if (d->getState().find("Down") != std::string::npos)
                        doorTransition = "Down";
                    return nextRen;
                }
            }
        } else if (obj2->getEntType() == "Monster") {
            // TODO

        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            // TODO

        } else if (obj2->getEntType() == "NPC") {
            // TODO

        } else if (obj2->getEntType() == "Projectile") {
            // TODO
        }

    } else if (obj1->getEntType() == "DynamicObj") {
        if (obj2->getEntType() == "Terrain") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Samos") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Monster") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "NPC") {
            // TODO

        } else if (obj2->getEntType() == "Projectile") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Projectile* p = static_cast<Projectile*>(obj2);
                p->hitting(obj1);
            }
        }

    } else if (obj1->getEntType() == "NPC") {
        if (obj2->getEntType() == "Terrain") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Samos") {
            // TODO

        } else if (obj2->getEntType() == "Monster") {
            // TODO

        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            // TODO

        } else if (obj2->getEntType() == "NPC") {
            // TODO

        } else if (obj2->getEntType() == "Projectile") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Projectile* p = static_cast<Projectile*>(obj2);
                p->hitting(obj1);
            }
        }

    } else if (obj1->getEntType() == "Projectile") {
        if (obj2->getEntType() == "Terrain") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Entity::calcCollisionReplacement(obj1, obj2);
                Projectile* p = static_cast<Projectile*>(obj1);
                p->hitting(obj2);
            }
        } else if (obj2->getEntType() == "Samos") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Entity::calcCollisionReplacement(obj1, obj2);
                Projectile* p = static_cast<Projectile*>(obj1);
                p->hitting(obj2);
            }
        } else if (obj2->getEntType() == "Monster") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Entity::calcCollisionReplacement(obj1, obj2);
                Projectile* p = static_cast<Projectile*>(obj1);
                p->hitting(obj2);
            }
        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Entity::calcCollisionReplacement(obj1, obj2);
                Projectile* p = static_cast<Projectile*>(obj1);
                p->hitting(obj2);
            }
        } else if (obj2->getEntType() == "NPC") {
            // TODO

        } else if (obj2->getEntType() == "Projectile") {
            // TODO
        }

    } else
        //If getEntType() has been given a wrong value
        throw Entity::unknownEntityType;

    return {};
}

void MainWindow::updateSamos(Samos *s)
{
    if (s->getState() == "MorphBallStop" || s->getState() == "MorphBallSlow" || s->getState() == "MorphBallSuperSlow")
        s->setState("MorphBall");

    nlohmann::json samosJson = Entity::values["names"]["Samos"];

    if (s->getOnGround() || !s->getIsAffectedByGravity()) {
        if ((s->getState() == "Jumping") || (s->getState() == "SpinJump") || (s->getState() == "Falling") || (s->getState() == "JumpEnd") || (s->getState() == "WallJump")
                || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
            if ((s->getState() == "Falling") || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                s->setY(s->getY() - static_cast<double>(samosJson["height"]) - static_cast<int>(samosJson["offset_y"]) +
                        static_cast<double>(samosJson["fallingHitbox_height"]) + static_cast<int>(samosJson["fallingHitbox_offset_y"]));
            } else if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
                s->setY(s->getY() - static_cast<double>(samosJson["height"]) - static_cast<int>(samosJson["offset_y"]) +
                        static_cast<double>(samosJson["spinJumpHitbox_height"]) + static_cast<int>(samosJson["spinJumpHitbox_offset_y"]));
            }
        }
    }

    CollisionBox* spinBox = new CollisionBox(samosJson["spinJumpHitbox_offset_x"], samosJson["spinJumpHitbox_offset_y"],
                  samosJson["spinJumpHitbox_width"], samosJson["spinJumpHitbox_height"]);
    bool canSpin = canChangeBox(s, spinBox);
    CollisionBox* morphBox = new CollisionBox(samosJson["morphBallHitbox_offset_x"], samosJson["morphBallHitbox_offset_y"],
                  samosJson["morphBallHitbox_width"], samosJson["morphBallHitbox_height"]);
    bool canMorph = canChangeBox(s, morphBox);
    CollisionBox* fallBox = new CollisionBox(samosJson["fallingHitbox_offset_x"], samosJson["fallingHitbox_offset_y"],
                samosJson["fallingHitbox_width"], samosJson["fallingHitbox_height"]);
    bool canFall = canChangeBox(s, fallBox);
    CollisionBox* crouchBox = new CollisionBox(samosJson["crouchHitbox_offset_x"], samosJson["crouchHitbox_offset_y"],
                  samosJson["crouchHitbox_width"], samosJson["crouchHitbox_height"]);
    bool canCrouch = canChangeBox(s, crouchBox);
    CollisionBox* standBox = new CollisionBox(samosJson["offset_x"], samosJson["offset_y"], samosJson["width"], samosJson["height"]);
    bool canStand = canChangeBox(s, standBox);


    if (s->getState() == "MorphBalling" && s->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"]["Samos"]["texture"]]["MorphBalling"]["count"]) - 1)) {
        s->setIsInAltForm(true);
        s->setState("MorphBall");
    } else if (s->getState() == "UnMorphBalling" && s->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"]["Samos"]["texture"]]["UnMorphBalling"]["count"]) - 1)) {
        if (s->getOnGround() || !s->getIsAffectedByGravity()) {
            s->setState("IdleCrouch");
            s->setY(s->getY() - static_cast<double>(samosJson["crouchHitbox_height"]) - static_cast<int>(samosJson["crouchHitbox_offset_y"]) +
                    static_cast<double>(samosJson["morphBallHitbox_height"]) + static_cast<int>(samosJson["morphBallHitbox_offset_y"]));
        } else
            s->setState("Falling");
    }

    if (s->getIsInAltForm()) {
        if (((inputList["morph"] && inputTime["morph"] == 0.0) || (inputList["up"] && inputTime["up"] == 0.0)) && canCrouch) {
            s->setState("UnMorphBalling");
            s->setIsInAltForm(false);
        } else {
            bool wallL = false;
            bool wallR = false;
            for (std::vector<Entity*>::iterator j = rendering.begin(); j!= rendering.end(); j++) {
                if ((*j)->getEntType() == "Terrain" || (*j)->getEntType() == "DynamicObj") {
                    if (Entity::checkCollision(s ,s->getWallBoxL(), *j, (*j)->getBox())) {
                        wallL = true;
                    }
                    if (Entity::checkCollision(s, s->getWallBoxR(), *j, (*j)->getBox())) {
                        wallR = true;
                    }
                }
            }
            if (s->getOnGround() || !s->getIsAffectedByGravity()) {
                if (inputList["left"] && !inputList["right"]) {
                    if (!wallL) {
                        if (s->getVX() > (static_cast<double>(samosJson["morphGroundAcceleration"]) / frameRate - static_cast<double>(samosJson["morphGroundMaxSpeed"]))) {
                            s->setVX(s->getVX() - static_cast<double>(samosJson["morphGroundAcceleration"]) / frameRate);
                        } else if (s->getVX() < (static_cast<double>(samosJson["morphGroundAcceleration"]) / frameRate - static_cast<double>(samosJson["morphGroundMaxSpeed"]))
                                   && s->getVX() > -static_cast<double>(samosJson["morphGroundMaxSpeed"])) {
                            s->setVX(-static_cast<double>(samosJson["morphGroundMaxSpeed"]));
                        }
                        if (s->getVX() < 0)
                            s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                        else
                            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                    }
                    s->setFacing("Left");
                    if (inputList["jump"] && s->getJumpTime() == -1) {
                        s->setVY(-static_cast<double>(samosJson["morphJumpPower"]));
                        s->setJumpTime(0);
                    }
                } else if (!inputList["left"] && inputList["right"]) {
                    if (!wallR) {
                        if (s->getVX() < (static_cast<double>(samosJson["morphGroundMaxSpeed"]) - static_cast<double>(samosJson["morphGroundAcceleration"]) / frameRate)) {
                            s->setVX(s->getVX() + static_cast<double>(samosJson["morphGroundAcceleration"]) / frameRate);
                        } else if (s->getVX() > (static_cast<double>(samosJson["morphGroundMaxSpeed"]) - static_cast<double>(samosJson["morphGroundAcceleration"]) / frameRate)
                                   && s->getVX() < static_cast<double>(samosJson["morphGroundMaxSpeed"])) {
                            s->setVX(static_cast<double>(samosJson["morphGroundMaxSpeed"]));
                        }
                        if (s->getVX() > 0)
                            s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                        else
                            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                    }
                    s->setFacing("Right");
                    if (inputList["jump"] && s->getJumpTime() == -1) {
                        s->setVY(-static_cast<double>(samosJson["morphJumpPower"]));
                        s->setJumpTime(0);
                    }
                } else {
                    s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                    if (inputList["jump"] && s->getJumpTime() == -1) {
                        s->setVY(-static_cast<double>(samosJson["morphJumpPower"]));
                        s->setJumpTime(0);
                    }
                    if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
                        s->setVX(0);
                }
                if (!inputList["jump"]) {
                    s->setJumpTime(-1);
                }
            } else {
                if (inputList["left"] && !inputList["right"]) {
                    if (s->getVX() > (static_cast<double>(samosJson["morphAirAcceleration"]) / frameRate - static_cast<double>(samosJson["morphAirMaxSpeed"]))) {
                        s->setVX(s->getVX() - static_cast<double>(samosJson["morphAirAcceleration"]) / frameRate);
                    } else if (s->getVX() < (static_cast<double>(samosJson["morphAirAcceleration"]) / frameRate - static_cast<double>(samosJson["morphAirMaxSpeed"]))
                               && s->getVX() > -static_cast<double>(samosJson["morphAirMaxSpeed"])) {
                        s->setVX(-static_cast<double>(samosJson["morphAirMaxSpeed"]));
                    }
                    if (s->getVX() < 0)
                        s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                    else
                        s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                    s->setFacing("Left");
                    if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["morphJumpTimeMax"]) && s->getJumpTime() >= 0) {
                        s->setVY(s->getVY() - static_cast<double>(samosJson["morphPostJumpBoost"]) / frameRate);
                        s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                    } else {
                        s->setJumpTime(static_cast<double>(samosJson["morphJumpTimeMax"]));
                    }
                } else if (!inputList["left"] && inputList["right"]) {
                    if (s->getVX() < (static_cast<double>(samosJson["morphAirMaxSpeed"]) - static_cast<double>(samosJson["morphAirAcceleration"]) / frameRate)) {
                        s->setVX(s->getVX() + static_cast<double>(samosJson["morphAirAcceleration"]) / frameRate);
                    } else if (s->getVX() > (static_cast<double>(samosJson["morphAirMaxSpeed"]) - static_cast<double>(samosJson["morphAirAcceleration"]) / frameRate)
                               && s->getVX() < static_cast<double>(samosJson["morphAirMaxSpeed"])) {
                        s->setVX(static_cast<double>(samosJson["morphAirMaxSpeed"]));
                    }
                    if (s->getVX() > 0)
                        s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                    else
                        s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                    s->setFacing("Right");
                    if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["morphJumpTimeMax"]) && s->getJumpTime() >= 0) {
                        s->setVY(s->getVY() - static_cast<double>(samosJson["morphPostJumpBoost"]) / frameRate);
                        s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                    }else {
                        s->setJumpTime(static_cast<double>(samosJson["morphJumpTimeMax"]));
                    }
                } else {
                    s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                    if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["morphJumpTimeMax"]) && s->getJumpTime() >= 0) {
                        s->setVY(s->getVY() - static_cast<double>(samosJson["morphPostJumpBoost"]) / frameRate);
                        s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                    } else {
                        s->setJumpTime(static_cast<double>(samosJson["morphJumpTimeMax"]));
                    }
                    if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
                        s->setVX(0);
                }
            }
        }
    } else {
        if (inputList["morph"] && inputTime["morph"] == 0.0 && canMorph) {
            s->setState("MorphBalling");
            if (s->getOnGround() || !s->getIsAffectedByGravity()) {
                if ((s->getState() == "Falling") || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag") || (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                    s->setY(s->getY() - static_cast<int>(samosJson["morphBallHitbox_height"]) - static_cast<int>(samosJson["morphBallHitbox_offset_y"]) +
                            static_cast<int>(samosJson["fallingHitbox_height"]) + static_cast<int>(samosJson["fallingHitbox_offset_y"]));
                } else if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
                    s->setY(s->getY() - static_cast<int>(samosJson["morphBallHitbox_height"]) - static_cast<int>(samosJson["morphBallHitbox_offset_y"]) +
                            static_cast<int>(samosJson["spinJumpHitbox_height"]) + static_cast<int>(samosJson["spinJumpHitbox_offset_y"]));
                } else if (s->getState() == "IdleCrouch" || s->getState() == "Crouching" || s->getState() == "UnCrouching") {
                    s->setY(s->getY() - static_cast<int>(samosJson["morphBallHitbox_height"]) - static_cast<int>(samosJson["morphBallHitbox_offset_y"]) +
                            static_cast<int>(samosJson["crouchHitbox_height"]) + static_cast<int>(samosJson["crouchHitbox_offset_y"]));
                } else
                    s->setY(s->getY() - static_cast<int>(samosJson["morphBallHitbox_height"]) - static_cast<int>(samosJson["morphBallHitbox_offset_y"]) +
                            static_cast<int>(samosJson["height"]) + static_cast<int>(samosJson["offset_y"]));
           }
        } else if (inputList["aim"] && s->getState() != "MorphBalling") {
            if (s->getOnGround() || !s->getIsAffectedByGravity()) {
                if ((s->getState() == "Jumping") || (s->getState() == "SpinJump") || (s->getState() == "Falling") || (s->getState() == "JumpEnd") || (s->getState() == "WallJump")
                        || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                    if (canStand)
                        s->setState("Landing");
                    else if (canCrouch)
                        s->setState("IdleCrouch");
                    else if (canMorph)
                        s->setState("MorphBalling");
                } else if (inputList["jump"] && s->getJumpTime() == -1 && canStand) {
                    s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                    s->setJumpTime(0);
                    s->setState("Jumping");
                } else if ((s->getState() == "Crouching" || s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag"
                           || s->getState() == "UnMorphBalling") && canCrouch) {
                    s->setState("IdleCrouch");
                } else {
                    if (canStand)
                        s->setState("Standing");
                    else if (canCrouch)
                        s->setState("IdleCrouch");
                    else if (canMorph)
                        s->setState("MorphBalling");
                }
                if (!inputList["jump"])
                    s->setJumpTime(-1);
            }
            if ((!s->getOnGround() && s->getIsAffectedByGravity()) || s->getState() == "Jumping") {
                if (inputList["jump"] && ((s->getJumpTime() == -1 && s->getState() == "WallJump") || ((s->getJumpTime() == -2 || s->getJumpTime() == -3) && s->getState() == "SpinJump"))) {
                    if (s->getFacing() == "Left") {
                        s->setVX(-static_cast<double>(samosJson["wallJumpPower_x"]));
                    } else {
                        s->setVX(static_cast<double>(samosJson["wallJumpPower_x"]));
                    }
                    s->setVY(-static_cast<double>(samosJson["wallJumpPower_y"]));
                    s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                } else if (s->getJumpTime() == -3) {
                    s->setJumpTime(-2);
                } else if (s->getJumpTime() == -2) {
                    s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                }

                if (canFall)
                    s->setState("Falling");
                else if (canSpin)
                    s->setState("SpinJump");
                else if (canMorph)
                    s->setState("MorphBalling");

                if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0) {
                    s->setVY(s->getVY() - static_cast<double>(samosJson["postJumpBoost"]) / frameRate);
                    s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                } else {
                    s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                }
            }

            if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
                s->setVX(0);
            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
        } else {
            bool wallL = false;
            bool wallR = false;
            for (std::vector<Entity*>::iterator j = rendering.begin(); j!= rendering.end(); j++) {
                if ((*j)->getEntType() == "Terrain" || (*j)->getEntType() == "DynamicObj") {
                    if (Entity::checkCollision(s , s->getWallBoxL(), *j, (*j)->getBox())) {
                        wallL = true;
                    }
                    if (Entity::checkCollision(s, s->getWallBoxR(), *j, (*j)->getBox())) {
                        wallR = true;
                    }
                }
            }
            if (s->getOnGround() || !s->getIsAffectedByGravity()) {
                if ((s->getState() == "Jumping") || (s->getState() == "SpinJump") || (s->getState() == "Falling") || (s->getState() == "JumpEnd") || (s->getState() == "WallJump")
                        || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                    if (canStand)
                        s->setState("Landing");
                    else if (canCrouch)
                        s->setState("IdleCrouch");
                    else if (canMorph)
                        s->setState("MorphBalling");
                } else if (inputList["left"] && !inputList["right"]) {
                    if (!wallL && canStand) {
                        if (s->getVX() > (static_cast<double>(samosJson["groundAcceleration"]) / frameRate - static_cast<double>(samosJson["groundMaxSpeed"]))) {
                            s->setVX(s->getVX() - static_cast<double>(samosJson["groundAcceleration"]) / frameRate);
                        } else if (s->getVX() < (static_cast<double>(samosJson["groundAcceleration"]) / frameRate - static_cast<double>(samosJson["groundMaxSpeed"]))
                                   && s->getVX() > -static_cast<double>(samosJson["groundMaxSpeed"])) {
                            s->setVX(-static_cast<double>(samosJson["groundMaxSpeed"]));
                        }
                        if (s->getVX() < 0)
                            s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                        else
                            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                    }
                    s->setFacing("Left");
                    if (inputList["jump"] && s->getJumpTime() == -1 && canSpin) {
                        s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                        s->setJumpTime(0);
                        s->setState("SpinJump");
                    } else if (s->getState() != "MorphBalling" && s->getState() != "UnMorphBalling" && canStand) {
                        if (!wallL) {
                            if (inputList["down"] && !inputList["up"])
                                s->setState("WalkingAimDown");
                            else if (!inputList["down"] && inputList["up"])
                                s->setState("WalkingAimUp");
                            else
                                s->setState("Walking");
                        } else {
                            s->setState("Standing");
                        }
                    }
                } else if (!inputList["left"] && inputList["right"]) {
                    if (!wallR && canStand) {
                        if (s->getVX() < (static_cast<double>(samosJson["groundMaxSpeed"]) - static_cast<double>(samosJson["groundAcceleration"]) / frameRate)) {
                            s->setVX(s->getVX() + static_cast<double>(samosJson["groundAcceleration"]) / frameRate);
                        } else if (s->getVX() > (static_cast<double>(samosJson["groundMaxSpeed"]) - static_cast<double>(samosJson["groundAcceleration"]) / frameRate)
                                   && s->getVX() < static_cast<double>(samosJson["groundMaxSpeed"])) {
                            s->setVX(static_cast<double>(samosJson["groundMaxSpeed"]));
                        }
                        if (s->getVX() > 0)
                            s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                        else
                            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                    }
                    s->setFacing("Right");
                    if (inputList["jump"] && s->getJumpTime() == -1 && canSpin) {
                        s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                        s->setJumpTime(0);
                        s->setState("SpinJump");
                    } else if (s->getState() != "MorphBalling" && s->getState() != "UnMorphBalling") {
                        if (!wallR) {
                            if (inputList["down"] && !inputList["up"])
                                s->setState("WalkingAimDown");
                            else if (!inputList["down"] && inputList["up"])
                                s->setState("WalkingAimUp");
                            else
                                s->setState("Walking");
                        } else {
                            s->setState("Standing");
                        }
                    }
                } else {
                    s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                    if (inputList["jump"] && s->getJumpTime() == -1 && canStand) {
                        s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                        s->setJumpTime(0);
                        s->setState("Jumping");
                    } else if (s->getState() != "MorphBalling" && s->getState() != "UnMorphBalling") {
                        if (inputList["down"] && !inputList["up"] && inputTime["down"] == 0.0 && canMorph && s->getState() == "IdleCrouch")
                            s->setState("MorphBalling");
                        else {
                            if (canCrouch) {
                                if (!inputList["up"] && inputList["down"] && inputTime["down"] == 0.0 && (s->getState() == "Standing" || s->getState() == "UnCrouching"))
                                    s->setState("Crouching");
                                else if (s->getState() == "Crouching" && s->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"]["Samos"]["texture"]]["Crouching"]["count"]) - 1))
                                    s->setState("IdleCrouch");
                            }
                            if (canStand) {
                                if (inputList["up"] && !inputList["down"] && inputTime["up"] == 0.0 && (s->getState() == "IdleCrouch" || s->getState() == "Crouching") && s->getState() != "UnMorphBalling")
                                    s->setState("UnCrouching");
                                else if (s->getState() == "UnCrouching" && s->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"]["Samos"]["texture"]]["UnCrouching"]["count"]) - 1))
                                    s->setState("Standing");

                                if (s->getState() != "IdleCrouch" && s->getState() != "Crouching" && s->getState() != "UnCrouching" && s->getState() != "CrouchAimUp" && s->getState() != "CrouchAimUpDiag" && s->getState() != "UnMorphBalling")
                                    s->setState("Standing");
                            }
                        }
                    }
                    if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
                        s->setVX(0);
                }
                if (!inputList["jump"]) {
                    s->setJumpTime(-1);
                }
            } else {
                if (canSpin && inputList["jump"] && ((s->getJumpTime() == -1 && s->getState() == "WallJump") || ((s->getJumpTime() == -2 || s->getJumpTime() == -3) && s->getState() == "SpinJump"))) {
                    if (s->getFacing() == "Left") {
                        s->setVX(-static_cast<double>(samosJson["wallJumpPower_x"]));
                    } else {
                        s->setVX(static_cast<double>(samosJson["wallJumpPower_x"]));
                    }
                    s->setVY(-static_cast<double>(samosJson["wallJumpPower_y"]));
                    s->setState("SpinJump");
                    s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                } else if (s->getJumpTime() == -3) {
                    s->setJumpTime(-2);
                } else if (s->getJumpTime() == -2) {
                    s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                }  else if (inputList["left"] && !inputList["right"]) {
                    if (s->getVX() > (static_cast<double>(samosJson["airAcceleration"]) / frameRate - static_cast<double>(samosJson["airMaxSpeed"]))) {
                        s->setVX(s->getVX() - static_cast<double>(samosJson["airAcceleration"]) / frameRate);
                    } else if (s->getVX() < (static_cast<double>(samosJson["airAcceleration"]) / frameRate - static_cast<double>(samosJson["airMaxSpeed"]))
                               && s->getVX() > -static_cast<double>(samosJson["airMaxSpeed"])) {
                        s->setVX(-static_cast<double>(samosJson["airMaxSpeed"]));
                    }
                    if (s->getVX() < 0)
                        s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                    else
                        s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                    s->setFacing("Left");
                    if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0) {
                        s->setVY(s->getVY() - static_cast<double>(samosJson["postJumpBoost"]) / frameRate);
                        s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                    } else if (s->getState() == "Jumping" && (!inputList["jump"] || s->getJumpTime() >= static_cast<double>(samosJson["jumpTimeMax"]))) {
                        s->setState("JumpEnd");
                        s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                    } else {
                        s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                    }
                    if (s->getState() != "SpinJump" && s->getState() != "WallJump" && s->getState() != "Jumping" && s->getState() != "JumpEnd"
                            && s->getState() != "MorphBalling" && s->getState() != "UnMorphBalling" && canFall) {
                        s->setState("Falling");
                    }
                } else if (!inputList["left"] && inputList["right"]) {
                    if (s->getVX() < (static_cast<double>(samosJson["airMaxSpeed"]) - static_cast<double>(samosJson["airAcceleration"]) / frameRate)) {
                        s->setVX(s->getVX() + static_cast<double>(samosJson["airAcceleration"]) / frameRate);
                    } else if (s->getVX() > (static_cast<double>(samosJson["airMaxSpeed"]) - static_cast<double>(samosJson["airAcceleration"]) / frameRate)
                               && s->getVX() < static_cast<double>(samosJson["airMaxSpeed"])) {
                        s->setVX(static_cast<double>(samosJson["airMaxSpeed"]));
                    }
                    if (s->getVX() > 0)
                        s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                    else
                        s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                    s->setFacing("Right");
                    if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0) {
                        s->setVY(s->getVY() - static_cast<double>(samosJson["postJumpBoost"]) / frameRate);
                        s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                    } else if (s->getState() == "Jumping" && (!inputList["jump"] || s->getJumpTime() >= static_cast<double>(samosJson["jumpTimeMax"]))) {
                        s->setState("JumpEnd");
                        s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                    } else {
                        s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                    }
                    if (s->getState() != "SpinJump" && s->getState() != "WallJump" && s->getState() != "Jumping" && s->getState() != "JumpEnd"
                            && s->getState() != "MorphBalling" && s->getState() != "UnMorphBalling" && canFall) {
                        s->setState("Falling");
                    }
                } else {
                    s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                    if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0) {
                        s->setVY(s->getVY() - static_cast<double>(samosJson["postJumpBoost"]) / frameRate);
                        s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                    } else if (s->getState() == "Jumping" && (!inputList["jump"] || s->getJumpTime() >= static_cast<double>(samosJson["jumpTimeMax"]))) {
                        s->setState("JumpEnd");
                        s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                    } else {
                        s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                    }
                    if (s->getState() != "SpinJump" && s->getState() != "WallJump" && s->getState() != "Jumping" && s->getState() != "JumpEnd"
                            && s->getState() != "MorphBalling" && s->getState() != "UnMorphBalling" && canFall) {
                        s->setState("Falling");
                    }

                    if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
                        s->setVX(0);
                }
            }
        }
    }

    if (s->getShootTime() > 0.0) {
        s->setShootTime(s->getShootTime() - 1 / frameRate);
    }

    if (s->getShootTime() <= 0.0 && inputList["shoot"]) {
        if (s->getState() == "Crouching" || s->getState() == "UnMorphBalling")
            s->setState("IdleCrouch");
        else if (s->getState() == "Uncrouching" || s->getState() == "Landing")
            s->setState("Standing");
        else if (s->getState() == "JumpEnd" || s->getState() == "Jumping" || s->getState() == "SpinJump" || s->getState() == "WallJump")
            s->setState("Falling");
        else if (s->getState() == "Walking")
            s->setState("WalkingAimForward");
    }

    if (s->getShootTime() > 0 && (s->getState() == "Walking" || s->getState() == "WalkingAimForward"))
        s->setState("WalkingAimForward");

    bool changedBox = false;
    if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
        if ((*s->getBox()) != *spinBox) {
            s->setBox(spinBox);
            changedBox = true;
        } else
            delete spinBox;
        delete morphBox;
        delete fallBox;
        delete crouchBox;
        delete standBox;
    } else if (s->getState() == "MorphBall" || s->getState() == "MorphBalling" || s->getState() == "UnMorphBalling") {
        if ((*s->getBox()) != *morphBox) {
            s->setBox(morphBox);
            changedBox = true;
        } else
            delete morphBox;
        delete spinBox;
        delete fallBox;
        delete crouchBox;
        delete standBox;
    } else if (s->getState() == "Falling" || s->getState() == "FallingAimUp" || s->getState() == "FallingAimUpDiag" || s->getState() == "FallingAimDownDiag" || s->getState() == "FallingAimDown") {
        if ((*s->getBox()) != *fallBox) {
            s->setBox(fallBox);
            changedBox = true;
        } else
            delete fallBox;
        delete spinBox;
        delete morphBox;
        delete crouchBox;
        delete standBox;
    } else if (s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag" || s->getState() == "UnCrouching" || s->getState() == "Crouching") {
        if ((*s->getBox()) != *crouchBox) {
            s->setBox(crouchBox);
            changedBox = true;
        } else
            delete crouchBox;
        delete spinBox;
        delete morphBox;
        delete fallBox;
        delete standBox;
    } else {
        if ((*s->getBox()) != *standBox) {
            s->setBox(standBox);
            changedBox = true;
        } else
            delete standBox;
        delete spinBox;
        delete morphBox;
        delete fallBox;
        delete crouchBox;
    }
    if (changedBox) {
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 2));
        s->setWallBoxR(new CollisionBox(s->getBox()->getX() + s->getBox()->getWidth(), s->getBox()->getY(), 2, s->getBox()->getHeight()));
        s->setWallBoxL(new CollisionBox(s->getBox()->getX() - 2, s->getBox()->getY(), 2, s->getBox()->getHeight()));
        for (std::vector<Entity*>::iterator i = rendering.begin(); i != rendering.end(); i++) {
            if ((*i)->getEntType() == "Terrain" || (*i)->getEntType() == "DynamicObj") {
                if (Entity::checkCollision(s, s->getBox(), *i, (*i)->getBox())) {
                    Entity::calcCollisionReplacement(s, *i);
                }
            }
        }
    }

    if (!s->getOnGround() && !inputList["aim"] && !inputList["shoot"] && s->getShootTime() <= 0 && !s->getIsInAltForm() && s->getState() != "MorphBalling" && canSpin) {
        std::string wallJump;
        for (std::vector<Entity*>::iterator j = rendering.begin(); j!= rendering.end(); j++) {
            if ((*j)->getEntType() == "Terrain" || (*j)->getEntType() == "DynamicObj") {
                if (Entity::checkCollision(s, s->getWallBoxL(), *j, (*j)->getBox())) {
                    wallJump = "Left";
                } else if (Entity::checkCollision(s, s->getWallBoxR(), *j, (*j)->getBox())) {
                    wallJump = "Right";
                }
            }
        }

        if (wallJump == "Right" && (inputList["right"] || s->getState() == "WallJump" || s->getState() == "SpinJump")) {
            s->setFacing("Left");
            s->setState("WallJump");
            if (!inputList["jump"])
                s->setJumpTime(-1);
            if (s->getVY() > 0)
                s->setVY(1 / ((1 / s->getVY()) + (static_cast<double>(samosJson["wallFriction"]) / frameRate)));
            else if (s->getVY() < 0)
                s->setVY(1 / ((1 / s->getVY()) - (static_cast<double>(samosJson["wallFriction"]) / frameRate)));
        } else if (wallJump == "Left" && (inputList["left"] || s->getState() == "WallJump" || s->getState() == "SpinJump")) {
            s->setFacing("Right");
            s->setState("WallJump");
            if (!inputList["jump"])
                s->setJumpTime(-1);
            if (s->getVY() > 0)
                s->setVY(1 / ((1 / s->getVY()) + (static_cast<double>(samosJson["wallFriction"]) / frameRate)));
            else if (s->getVY() < 0)
                s->setVY(1 / ((1 / s->getVY()) - (static_cast<double>(samosJson["wallFriction"]) / frameRate)));
        } else if (s->getState() == "WallJump") {
            s->setState("SpinJump");
            if (s->getJumpTime() == -1)
                s->setJumpTime(-3);
        }
    }

    if (s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag") {
        if (inputList["left"] && !inputList["right"]) {
            s->setFacing("Left");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("CrouchAimUpDiag");
                s->setCanonDirection("UpLeft");
            } else {
                s->setState("IdleCrouch");
                s->setCanonDirection("Left");
            }
        } else if (inputList["right"] && !inputList["left"]) {
            s->setFacing("Right");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("CrouchAimUpDiag");
                s->setCanonDirection("UpRight");
            } else {
                s->setState("IdleCrouch");
                s->setCanonDirection("Right");
            }
        } else {
            if (inputList["up"] && !inputList["down"]) {
                s->setState("CrouchAimUp");
                s->setCanonDirection("Up");
            } else {
                s->setState("IdleCrouch");
                if (s->getFacing() == "Left")
                    s->setCanonDirection("Left");
                else
                    s->setCanonDirection("Right");
            }
        }
    } else if (s->getState() == "Standing" || s->getState() == "StandingAimUpDiag" || s->getState() == "StandingAimDownDiag" || s->getState() == "StandingAimUp") {
        if (inputList["left"] && !inputList["right"]) {
            s->setFacing("Left");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("StandingAimUpDiag");
                s->setCanonDirection("UpLeft");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setState("StandingAimDownDiag");
                s->setCanonDirection("DownLeft");
            } else {
                s->setState("Standing");
                s->setCanonDirection("Left");
            }
        } else if (inputList["right"] && !inputList["left"]) {
            s->setFacing("Right");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("StandingAimUpDiag");
                s->setCanonDirection("UpRight");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setState("StandingAimDownDiag");
                s->setCanonDirection("DownRight");
            } else {
                s->setState("Standing");
                s->setCanonDirection("Right");
            }
        } else {
            if (inputList["up"] && !inputList["down"]) {
                s->setState("StandingAimUp");
                s->setCanonDirection("Up");
            } else {
                s->setState("Standing");
                if (s->getFacing() == "Left")
                    s->setCanonDirection("Left");
                else
                    s->setCanonDirection("Right");
            }
        }
    } else if (s->getState() == "Falling" || s->getState() == "FallingAimUpDiag" || s->getState() == "FallingAimDownDiag" || s->getState() == "FallingAimUp" || s->getState() == "FallingAimDown") {
        if (inputList["left"] && !inputList["right"]) {
            s->setFacing("Left");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("FallingAimUpDiag");
                s->setCanonDirection("UpLeft");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setState("FallingAimDownDiag");
                s->setCanonDirection("DownLeft");
            } else {
                s->setState("Falling");
                s->setCanonDirection("Left");
            }
        } else if (inputList["right"] && !inputList["left"]) {
            s->setFacing("Right");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("FallingAimUpDiag");
                s->setCanonDirection("UpRight");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setState("FallingAimDownDiag");
                s->setCanonDirection("DownRight");
            } else {
                s->setState("Falling");
                s->setCanonDirection("Right");
            }
        } else {
            if (inputList["up"] && !inputList["down"]) {
                s->setState("FallingAimUp");
                s->setCanonDirection("Up");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setState("FallingAimDown");
                s->setCanonDirection("Down");
            } else {
                s->setState("Falling");
                if (s->getFacing() == "Left")
                    s->setCanonDirection("Left");
                else
                    s->setCanonDirection("Right");
            }
        }
    } else if (s->getState() == "Walking" || s->getState() == "WalkingAimForward") {
        if (s->getFacing() == "Left")
            s->setCanonDirection("Left");
        else
            s->setCanonDirection("Right");
    } else if (s->getState() == "WalkingAimUp") {
        if (s->getFacing() == "Left")
            s->setCanonDirection("UpLeft");
        else
            s->setCanonDirection("UpRight");
    } else if (s->getState() == "WalkingAimDown") {
        if (s->getFacing() == "Left")
            s->setCanonDirection("DownLeft");
        else
            s->setCanonDirection("DownRight");
    } else if (s->getState() == "MorphBall" || s->getState() == "MorphBalling" || s->getState() == "UnMorphBalling") {
        if (s->getFacing() == "Left")
            s->setCanonDirection("Left");
        else
            s->setCanonDirection("Right");
    } else if (s->getState() == "SpinJump" || s->getState() == "WallJump" || s->getState() == "Jumping" || s->getState() == "JumpEnd") {
        if (inputList["left"] && !inputList["right"]) {
            if (inputList["up"] && !inputList["down"]) {
                s->setCanonDirection("UpLeft");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setCanonDirection("DownLeft");
            } else {
                s->setCanonDirection("Left");
            }
        } else if (inputList["right"] && !inputList["left"]) {
            if (inputList["up"] && !inputList["down"]) {
                s->setCanonDirection("UpRight");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setCanonDirection("DownRight");
            } else {
                s->setCanonDirection("Right");
            }
        } else {
            if (inputList["up"] && !inputList["down"]) {
                s->setCanonDirection("Up");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setCanonDirection("Down");
            } else {
                if (s->getFacing() == "Left")
                    s->setCanonDirection("Left");
                else
                    s->setCanonDirection("Right");
            }
        }
    }

    if (inputList["weapon"]) {
        if (s->getSwitchDelay() == 0.0) {
            s->nextWeapon();
        } else if (s->getSwitchDelay() >= 3 * static_cast<double>(samosJson["switchDelay"]) || (s->getSwitchDelay() >= -1 && s->getSwitchDelay() < 0.0)) {
            s->nextWeapon();
            s->setSwitchDelay(-1 - static_cast<double>(samosJson["switchDelay"]));
        }

        s->setSwitchDelay(s->getSwitchDelay() + 1 / frameRate);

    } else
        s->setSwitchDelay(0.0);

    if (s->getShootTime() <= 0.0 && inputList["shoot"]) {
        Projectile* p = nullptr;
        if (s->getIsInAltForm())
            p = s->shoot("Bomb");
        else if (s->getState() != "MorphBalling")
            p = s->shoot(s->getSelectedWeapon());
        if (p != nullptr) {
            p->setX(p->getX() + s->getVX() / frameRate);
            p->setY(p->getY() + s->getVY() / frameRate);
            addRenderable(p);
            s->setShootTime(static_cast<double>(samosJson["shootTime"]));
        }
    }

    if (s->getState() == "MorphBall") {
        if (std::abs(s->getVX()) < 1)
            s->setState("MorphBallStop");
        else if (std::abs(s->getVX()) < 100)
            s->setState("MorphBallSuperSlow");
        else if (std::abs(s->getVX()) < 200)
            s->setState("MorphBallSlow");
    }

    // Camera
    nlohmann::json mapJson = currentMap.getJson()["rooms"][std::to_string(currentMap.getCurrentRoomId())];

    int roomS_x = mapJson["position"][0];
    int roomS_y = mapJson["position"][1];
    int roomE_x = mapJson["size"][0];
    int roomE_y = mapJson["size"][1];
    roomE_x += roomS_x;
    roomE_y += roomS_y;

    int cam_dist_x = s->getX() + static_cast<int>(Entity::values["general"]["camera_rx"]) - camera.x();
    int cam_dist_y = s->getY() + static_cast<int>(Entity::values["general"]["camera_ry"]) - camera.y();
    camera.setX(camera.x() + cam_dist_x);
    camera.setY(camera.y() + cam_dist_y);
    if (camera.x() < roomS_x)
        camera.setX(roomS_x);
    else if (camera.x() + 1920 > roomE_x)
        camera.setX(roomE_x - 1920);
    if (camera.y() < roomS_y)
        camera.setY(roomS_y);
    else if (camera.y() + 1080 > roomE_y)
        camera.setY(roomE_y - 1080);
}

void MainWindow::updateMapViewer()
{
    if (inputList["enter"] && inputTime["enter"] == 0.0) {
        int mapId = currentMap.getCurrentRoomId();
        currentMap = Map::loadMap(currentMap.getName());
        currentMap.setCurrentRoomId(mapId);
        clearRendering("Samos");
        addRenderable(currentMap.loadRoom());
    }

    if (inputList["down"] && !inputList["up"]) {
        camera.setY(camera.y() + mapViewerCameraSpeed / frameRate);
    } else if (!inputList["down"] && inputList["up"]) {
        camera.setY(camera.y() - mapViewerCameraSpeed / frameRate);
    }
    if (inputList["left"] && !inputList["right"]) {
        camera.setX(camera.x() - mapViewerCameraSpeed / frameRate);
    } else if (!inputList["left"] && inputList["right"]) {
        camera.setX(camera.x() + mapViewerCameraSpeed / frameRate);
    }

}

void MainWindow::closeEvent(QCloseEvent *)
{
    running = false;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    Samos* s = nullptr;

    //Draw every entity in the rendering list
    for (std::vector<Entity*>::iterator ent = rendering.begin(); ent != rendering.end(); ent++) {

        //If the texture is null, set it to the error texture
        if ((*ent)->getTexture() == nullptr) {
            (*ent)->setTexture(&emptyTexture);
        }

        //Try to draw the textrue, if it fails, set it to the error texture and try again
        try {
            painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - camera.x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - camera.y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                              *(*ent)->getTexture());
        } catch (...) {
            (*ent)->setTexture(&errorTexture);
            painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - camera.x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - camera.y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                              *(*ent)->getTexture());
        }

        if ((*ent)->getEntType() == "Samos")
            s = static_cast<Samos*>((*ent));
    }

    //Draw hitboxes if necessary
    if (renderHitboxes) {
        for (std::vector<Entity*>::iterator ent = rendering.begin(); ent != rendering.end(); ent++) {
            painter.setPen(QColor("blue"));
            // Select another color for specific entity types
            if ((*ent)->getEntType() == "Monster")
                painter.setPen(QColor("red"));
            else if ((*ent)->getEntType() == "Area")
                painter.setPen(QColor("gray"));
            else if ((*ent)->getEntType() == "Samos")
                painter.setPen(QColor("orange"));
            if ((*ent)->getBox() != nullptr)
                painter.drawRect((*ent)->getX() + (*ent)->getBox()->getX() - camera.x(), (*ent)->getY() + (*ent)->getBox()->getY() - camera.y(),
                                 (*ent)->getBox()->getWidth(), (*ent)->getBox()->getHeight());

            //Ground boxes
            if ((*ent)->getEntType() == "Samos" || (*ent)->getEntType() == "Monster" || (*ent)->getEntType() == "NPC" || (*ent)->getEntType() == "DynamicObj") {
                Living* liv = static_cast<Living*>((*ent));
                painter.setPen(QColor("green"));
                if (liv->getGroundBox() != nullptr)
                    painter.drawRect(liv->getX() + liv->getGroundBox()->getX() - camera.x(), liv->getY() + liv->getGroundBox()->getY() - camera.y(),
                                    liv->getGroundBox()->getWidth(), liv->getGroundBox()->getHeight());

                //Wall boxes
                if ((*ent)->getEntType() == "Samos") {
                    Samos* s = static_cast<Samos*>(liv);
                    if (s->getWallBoxL() != nullptr)
                        painter.drawRect(s->getX() + s->getWallBoxL()->getX() - camera.x(), s->getY() + s->getWallBoxL()->getY() - camera.y(),
                                        s->getWallBoxL()->getWidth(), s->getWallBoxL()->getHeight());
                    if (s->getWallBoxR() != nullptr)
                        painter.drawRect(s->getX() + s->getWallBoxR()->getX() - camera.x(), s->getY() + s->getWallBoxR()->getY() - camera.y(),
                                        s->getWallBoxR()->getWidth(), s->getWallBoxR()->getHeight());
                }
            }
        }
        nlohmann::json mapJson = currentMap.getJson()["rooms"][std::to_string(currentMap.getCurrentRoomId())];

        painter.setPen(QColor("magenta"));
        painter.drawRect(static_cast<int>(mapJson["position"][0]) - camera.x(), static_cast<int>(mapJson["position"][1]) - camera.y(), mapJson["size"][0], mapJson["size"][1]);
    }

    // Draw fps if necessary
    if (showFps) {
        if ((std::chrono::high_resolution_clock::now() - lastFpsShown).count() > showFpsUpdateRate) {
            lastFpsShown = std::chrono::high_resolution_clock::now();
        }
        painter.setPen(QColor("black"));
        painter.drawText(QPoint(2, 12), QString::fromStdString(std::to_string(fps) + " FPS"));
    }

    QFont f = painter.font();
    f.setPointSize(f.pointSize() * 2);
    painter.setFont(f);

    //UI
    if (s != nullptr && showUI) {

        painter.setPen(QColor("black"));

        //Selected weapon
        painter.fillRect(QRect(70,15,100,30), QColor("white"));
        painter.drawRect(QRect(70,15,100,30));
        painter.drawText(QPoint(80, 40), translate(s->getSelectedWeapon(), "ui", "selectedWeapon"));

        //Missile count
        painter.fillRect(QRect(200,15,70,30), QColor("white"));
        painter.drawRect(QRect(200,15,70,30));
        painter.drawText(QPoint(210, 40), QString::fromStdString("M : " + std::to_string(s->getMissileCount())));

        //Grenade count
        painter.fillRect(QRect(290,15,70,30), QColor("white"));
        painter.drawRect(QRect(290,15,70,30));
        painter.drawText(QPoint(300, 40), QString::fromStdString("G : " + std::to_string(s->getGrenadeCount())));

        //Health count
        painter.fillRect(QRect(380,15,80,30), QColor("white"));
        painter.drawRect(QRect(380,15,80,30));
        painter.drawText(QPoint(390, 40), QString::fromStdString("H : " + std::to_string(s->getHealth())));
    }

    //Menu
    if (isPaused) {
        painter.fillRect(QRect(0,0,size().width(),size().height()), QBrush(QColor(0,0,0,200)));
        for (unsigned int i = 0; i < menuOptions.size(); i++) {
            painter.setPen(QColor("white"));
            if (selectedOption == (int) i)
                painter.setPen(QColor("cyan"));
            painter.drawText(0, size().height() / 2 - 15 * menuOptions.size() + 30 * i, size().width(), 50, Qt::AlignHCenter, QString::fromStdString(menuOptions[i]));
        }
    }

    f.setPointSize(f.pointSize() / 2);
    painter.setFont(f);
}

void MainWindow::addRenderable(Entity *entity)
{
    rendering.push_back(entity);
}

void MainWindow::addRenderable(std::vector<Entity*> entities)
{
    for (std::vector<Entity*>::iterator entity = entities.begin(); entity != entities.end(); entity++)
        rendering.push_back(*entity);
}

void MainWindow::clearRendering(std::string excludedType)
{
    std::vector<Entity*> nextRen;
    for (std::vector<Entity*>::iterator ent = rendering.begin(); ent != rendering.end(); ent ++)
        if ((*ent)->getEntType() != excludedType)
            delete *ent;
        else
            nextRen.push_back(*ent);
    rendering = nextRen;
}

void MainWindow::updatePhysics()
{
    //Physics settings
    nlohmann::json paramJson = Entity::values["general"];
    double speedcap = static_cast<double>(paramJson["speedcap"]);
    //Ground objects list
    std::vector<Entity*> solidList;
    //Living objects list
    std::vector<Living*> livingList;
    //Deletion list
    std::vector<Entity*> toDel;

    nlohmann::json mapJson = currentMap.getJson()["rooms"][std::to_string(currentMap.getCurrentRoomId())];

    int roomS_x = mapJson["position"][0];
    int roomS_y = mapJson["position"][1];
    int roomE_x = mapJson["size"][0];
    int roomE_y = mapJson["size"][1];
    roomE_x += roomS_x;
    roomE_y += roomS_y;

    for (std::vector<Entity*>::iterator ent = rendering.begin(); ent != rendering.end(); ent++) {

        if ((*ent)->getEntType() == "Samos" || (*ent)->getEntType() == "Monster" || (*ent)->getEntType() == "NPC" || (*ent)->getEntType() == "DynamicObj") {
            Living* liv = static_cast<Living*>((*ent));
            livingList.push_back(liv);
            //Calc earth's attraction's acceleration if the (*ent)ity is affected
            if (liv->getIsAffectedByGravity() && !liv->getOnGround() && liv->getIsMovable()) {
                liv->setVY(liv->getVY() + gravity / frameRate);
            }
            //Calc frictions
            if (liv->getOnGround() && liv->getIsMovable()) {
                //Grounded frictions
                if (std::abs(liv->getVX()) < speedcap) {
                    if (liv->getVX() > 0)
                        liv->setVX(1 / ((1 / liv->getVX()) + (static_cast<double>(paramJson["groundFriction"]) * liv->getFrictionFactor() / frameRate)));
                    else if (liv->getVX() < 0)
                        liv->setVX(1 / ((1 / liv->getVX()) - (static_cast<double>(paramJson["groundFriction"]) * liv->getFrictionFactor() / frameRate)));
                //Speedcap
                } else {
                    if (liv->getVX() > 0)
                        liv->setVX(speedcap);
                    else
                        liv->setVX(-speedcap);
                }
            } else if (liv->getIsMovable()) {
                //Air friction
                if (std::abs(liv->getVX()) < speedcap) {
                    if (liv->getVX() > 0)
                        liv->setVX(1 / ((1 / liv->getVX()) + (static_cast<double>(paramJson["airFriction"]) * liv->getFrictionFactor() / frameRate)));
                    else if (liv->getVX() < 0)
                        liv->setVX(1 / ((1 / liv->getVX()) - (static_cast<double>(paramJson["airFriction"]) * liv->getFrictionFactor() / frameRate)));
                }
            }
        } else {
            //Non-living objects can't be grounded
            if ((*ent)->getIsAffectedByGravity() && (*ent)->getIsMovable())
                (*ent)->setVY((*ent)->getVY() + gravity / frameRate);
            if ((*ent)->getIsMovable()) {
                if (std::abs((*ent)->getVX()) < speedcap) {
                    if ((*ent)->getVX() > 0)
                        (*ent)->setVX(1 / ((1 / (*ent)->getVX()) + (static_cast<double>(paramJson["airFriction"]) * (*ent)->getFrictionFactor() / frameRate)));
                    else if ((*ent)->getVX() < 0)
                        (*ent)->setVX(1 / ((1 / (*ent)->getVX()) - (static_cast<double>(paramJson["airFriction"]) * (*ent)->getFrictionFactor() / frameRate)));
                //Speedcap
                } else {
                    if ((*ent)->getVX() > 0)
                        (*ent)->setVX(speedcap);
                    else
                        (*ent)->setVX(-speedcap);
                }
            }
        }
        //Livings can only stand on terrain or dynamic objects
        if ((*ent)->getEntType() == "Terrain" || (*ent)->getEntType() == "DynamicObj") {
            solidList.push_back((*ent));
        }
        //Speedcap
        if (std::abs((*ent)->getVY()) > speedcap) {
            if ((*ent)->getVY() > 0)
                (*ent)->setVY(speedcap);
            else
                (*ent)->setVY(-speedcap);
        }
        //Move entities
        if ((*ent)->getIsMovable()) {
            if ((*ent)->getEntType() == "Projectile") {
                Projectile* p = static_cast<Projectile*>((*ent));
                if (p->getLifeTime() != Entity::values["names"][p->getName()]["lifeTime"])
                    (*ent)->updateV(frameRate);
                if (updateProjectile(p))
                    toDel.push_back((*ent));
            } else
                (*ent)->updateV(frameRate);
        }
    } //{Null, Terrain, Samos, Monster, Area, DynamicObj, NPC, Projectile};

    std::vector<Entity*> toAdd;
    //Check for collisions and handle them
    for (std::vector<Entity*>::iterator i = rendering.begin(); i != rendering.end(); i++) {
        for (std::vector<Entity*>::iterator j = i+1; j!= rendering.end(); j++) {
            for (Entity* ent : handleCollision(*i,*j))
                toAdd.push_back(ent);
        }
        if ((*i)->getIsMovable()) {
            if ((*i)->getX() + (*i)->getBox()->getX() + (*i)->getBox()->getWidth() > roomE_x) {
                (*i)->setX(roomE_x - (*i)->getBox()->getX() - (*i)->getBox()->getWidth());
                if ((*i)->getEntType() == "Projectile") {
                    Projectile* p = static_cast<Projectile*>(*i);
                    p->timeOut();
                }
            } else if ((*i)->getX() + (*i)->getBox()->getX() < roomS_x) {
                (*i)->setX(roomS_x - (*i)->getBox()->getX());
                if ((*i)->getEntType() == "Projectile") {
                    Projectile* p = static_cast<Projectile*>(*i);
                    p->timeOut();
                }
            } if ((*i)->getY() + (*i)->getBox()->getY() + (*i)->getBox()->getHeight() > roomE_y) {
                (*i)->setY(roomE_y - (*i)->getBox()->getY() - (*i)->getBox()->getHeight());
                if ((*i)->getEntType() == "Projectile") {
                    Projectile* p = static_cast<Projectile*>(*i);
                    p->timeOut();
                }
            } else if ((*i)->getY() + (*i)->getBox()->getY() < roomS_y) {
                (*i)->setY(roomS_y - (*i)->getBox()->getY());
                if ((*i)->getEntType() == "Projectile") {
                    Projectile* p = static_cast<Projectile*>(*i);
                    p->timeOut();
                }
            }
        }
    }

    addRenderable(toAdd);

    //Update the grounded state of livings
    for (std::vector<Living*>::iterator i = livingList.begin(); i != livingList.end(); i++) {
        (*i)->setOnGround(false);
        for (std::vector<Entity*>::iterator j = solidList.begin(); j!= solidList.end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getGroundBox(), *j, (*j)->getBox())) {
                (*i)->setOnGround(true);
                (*i)->setVY(0);
                break;
            }
        }
        if ((*i)->getY() + (*i)->getGroundBox()->getY() + (*i)->getGroundBox()->getHeight() > roomE_y)
            (*i)->setOnGround(true);
    }

    for (std::vector<Entity*>::iterator i = toDel.begin(); i != toDel.end(); i++) {
        std::vector<Entity*> newRen;
        for (std::vector<Entity*>::iterator j = rendering.begin(); j != rendering.end(); j++) {
            if (*j != *i)
                newRen.push_back(*j);
        }
        rendering = newRen;
        delete *i;
    }
}

void MainWindow::updateAnimations()
{
    unsigned long long uC = updateCount;
    for (std::vector<Entity*>::iterator ent = rendering.begin(); ent != rendering.end(); ent++) {
        std::string state = (*ent)->getState();
        std::string facing = (*ent)->getFacing();
        // Every 'refreshRate' frames
        if (!Entity::values["textures"][(*ent)->getName()][state]["refreshRate"].is_null())
            if (uC % static_cast<int>(Entity::values["textures"][(*ent)->getName()][state]["refreshRate"]) == 0)
                // If the animation index still exists
                if ((*ent)->getCurrentAnimation().size() > (*ent)->getFrame())
                    // Increment the animation index
                    (*ent)->setFrame((*ent)->getFrame() + 1);

        // If the (*ent) state is different from the last frame
        if (state != (*ent)->getLastFrameState()
                || facing != (*ent)->getLastFrameFacing()) {
            // Update the QImage array representing the animation
            (*ent)->setCurrentAnimation((*ent)->updateAnimation(state));
            // If the animation should reset the next one
            if (!Entity::values["textures"]
                    [Entity::values["names"][(*ent)->getName()]["texture"]]
                    [(*ent)->getLastFrameState()]
                    ["dontReset"])
                // Because the animation changed, reset it
                (*ent)->setFrame(0);
            else
                // Else, make sure not to end up with a too high index
                if ((*ent)->getFrame() >= (*ent)->getCurrentAnimation().size())
                    (*ent)->setFrame(0);
        }

        // Every 'refreshRate' frames
        if (!Entity::values["textures"][(*ent)->getName()][state]["refreshRate"].is_null())
            if (uC % static_cast<int>(Entity::values["textures"][(*ent)->getName()][state]["refreshRate"]) == 0)
                // If the animation has to loop
                if (!Entity::values["textures"][(*ent)->getName()][state]["loop"].is_null()) {
                    if (Entity::values["textures"][(*ent)->getName()][state]["loop"]) {
                        // If the animation index still exists
                        if ((*ent)->getCurrentAnimation().size() - 1 < (*ent)->getFrame())
                            // Reset animation
                            (*ent)->setFrame(0);
                    } else
                        // If the animation index still exists
                        if ((*ent)->getCurrentAnimation().size() - 1 < (*ent)->getFrame())
                            // If the animation doesn't loop, make sure it stays on its last frame
                            (*ent)->setFrame((*ent)->getFrame() - 1);
                }

        // Update the texture with the animation index
        (*ent)->updateTexture();

        // Make sure to update these values for the next frame
        (*ent)->setLastFrameState(state);
        (*ent)->setLastFrameFacing(facing);
    }
}

const std::vector<Entity *> &MainWindow::getRendering() const
{
    return rendering;
}

void MainWindow::setRendering(const std::vector<Entity *> &newRendering)
{
    rendering = newRendering;
}

int MainWindow::getSelectedOption() const
{
    return selectedOption;
}

void MainWindow::setSelectedOption(int newSelectedOption)
{
    selectedOption = newSelectedOption;
}

const std::string &MainWindow::getMenu() const
{
    return menu;
}

void MainWindow::setMenu(const std::string &newMenu)
{
    menu = newMenu;
}

const std::vector<std::string> &MainWindow::getMenuOptions() const
{
    return menuOptions;
}

void MainWindow::setMenuOptions(const std::vector<std::string> &newMenuOptions)
{
    menuOptions = newMenuOptions;
}

QPoint MainWindow::getCamera() const
{
    return camera;
}

void MainWindow::setCamera(QPoint newCamera)
{
    camera = newCamera;
}
