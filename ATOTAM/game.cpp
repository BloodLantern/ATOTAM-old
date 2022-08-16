#include "game.h"
#include <io.h>
#include <iostream>

nlohmann::json Game::loadJson(std::string fileName)
{
    std::ifstream keys_file(assetsPath + "/" + fileName + ".json");
    nlohmann::json temp;
    keys_file >> temp;
    return temp;
}

void Game::loadGeneral()
{
    //Loading the general settings of the game
    gameSpeed = Entity::values["general"]["gameSpeed"];
    updateRate = Entity::values["general"]["updateRate"];
    showFpsUpdateRate = Entity::values["general"]["showFpsUpdateRate"];
    Physics::gravity = Entity::values["general"]["gravity"];
    mapViewer = Entity::values["general"]["mapViewer"];
    currentMap = Map::loadMap(Entity::values["general"]["map"], assetsPath);
    mapViewerCameraSpeed = Entity::values["general"]["mapViewerCameraSpeed"];
    Physics::frameRate = params["frameRate"];
    resolution.first = params["resolution_x"];
    resolution.second = params["resolution_y"];
    language = params["language"];
    renderHitboxes = Entity::values["general"]["renderHitboxes"];
    showFps = params["showFps"];
    fullscreen = params["fullscreen"];
}

Game::Game(std::string assetsPath)
    : assetsPath(assetsPath)
    , running(true)
    , keyCodes(loadJson("inputs"))
    , currentMap(Map::loadMap("test", assetsPath))
    , stringsJson(loadJson("strings"))
    , isPaused(false)
    , resolution({1920,1080})
    , doorTransition("")
    , params(loadJson("params"))
    , fullscreen(false)
{

    loadGeneral();
}

template<typename ...StdStrings>
QString Game::translate(std::string text, StdStrings... subCategories)
{
    nlohmann::json json = stringsJson[language];
    for (std::string category : {subCategories...})
        json = json[category];
    return QString::fromStdString(json[text]);
}

void Game::updateMenu()
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

            menuArrowsTime += + 1 / Physics::frameRate;

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

            menuArrowsTime += + 1 / Physics::frameRate;

        } else if (!inputList["down"] && !inputList["up"] && inputList["left"] && !inputList["right"]) {
            if (menuArrowsTime == 0.0) {
                if (menuOptions[selectedOption].substr(0,8) == "< FPS : ") {
                    if (Physics::frameRate > 60.0) {
                        Physics::frameRate--;
                        params["Physics::frameRate"] = Physics::frameRate;
                        std::ofstream paramsfile(assetsPath + "/params.json");
                        paramsfile << params.dump(4);
                        paramsfile.close();
                    }
                } else if (menuOptions[selectedOption].substr(0,15) == "< Game speed : ") {
                    if (gameSpeed > 0.1) {
                        gameSpeed -= 0.1;
                    }
                }
            } else if (menuArrowsTime >= 5 * static_cast<double>(Entity::values["general"]["menuCoolDown"]) || (menuArrowsTime >= -1 && menuArrowsTime < 0.0)) {
                if (menuOptions[selectedOption].substr(0,8) == "< FPS : ") {
                    if (Physics::frameRate > 60.0) {
                        Physics::frameRate--;
                        params["Physics::frameRate"] = Physics::frameRate;
                        std::ofstream paramsfile(assetsPath + "/params.json");
                        paramsfile << params.dump(4);
                        paramsfile.close();
                    }
                } else if (menuOptions[selectedOption].substr(0,15) == "< Game speed : ") {
                    if (gameSpeed > 0.1) {
                        gameSpeed -= 0.1;
                    }
                }
                menuArrowsTime = -1 - static_cast<double>(Entity::values["general"]["menuCoolDown"]) / 2;
            }

            menuArrowsTime += + 1 / Physics::frameRate;

        } else if (!inputList["down"] && !inputList["up"] && !inputList["left"] && inputList["right"]) {
            if (menuArrowsTime == 0.0) {
                if (menuOptions[selectedOption].substr(0,8) == "< FPS : ") {
                    if (Physics::frameRate < 144.0) {
                        Physics::frameRate++;
                        params["Physics::frameRate"] = Physics::frameRate;
                        std::ofstream paramsfile(assetsPath + "/params.json");
                        paramsfile << params.dump(4);
                        paramsfile.close();
                    }
                } else if (menuOptions[selectedOption].substr(0,15) == "< Game speed : ") {
                    if (gameSpeed < 10.0) {
                        gameSpeed += 0.1;
                    }
                }
            } else if (menuArrowsTime >= 5 * static_cast<double>(Entity::values["general"]["menuCoolDown"]) || (menuArrowsTime >= -1 && menuArrowsTime < 0.0)) {
                if (menuOptions[selectedOption].substr(0,8) == "< FPS : ") {
                    if (Physics::frameRate < 144.0) {
                        Physics::frameRate++;
                        params["Physics::frameRate"] = Physics::frameRate;
                        std::ofstream paramsfile(assetsPath + "/params.json");
                        paramsfile << params.dump(4);
                        paramsfile.close();
                    }
                } else if (menuOptions[selectedOption].substr(0,15) == "< Game speed : ") {
                    if (gameSpeed < 10.0) {
                        gameSpeed += 0.1;
                    }
                }
                menuArrowsTime = -1 - static_cast<double>(Entity::values["general"]["menuCoolDown"]) / 2;
            }

            menuArrowsTime += + 1 / Physics::frameRate;

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
                running = false;
            else if (menuOptions[selectedOption] == "Back") {
                menu = "main";
                selectedOption = 0;
            } else if (menuOptions[selectedOption] == "Show FPS : ON") {
                showFps = false;
                params["showFps"] = showFps;
                std::ofstream paramsfile(assetsPath + "/params.json");
                paramsfile << params.dump(4);
                paramsfile.close();
            } else if (menuOptions[selectedOption] == "Show FPS : OFF") {
                showFps = true;
                params["showFps"] = showFps;
                std::ofstream paramsfile(assetsPath + "/params.json");
                paramsfile << params.dump(4);
                paramsfile.close();
            } else if (menuOptions[selectedOption] == "Show hitboxes : ON")
                renderHitboxes = false;
            else if (menuOptions[selectedOption] == "Show hitboxes : OFF")
                 renderHitboxes = true;
            else if (menuOptions[selectedOption] == "Max missiles")
                 s->setMissileCount(s->getMaxMissileCount());
            else if (menuOptions[selectedOption] == "Max grenades")
                 s->setGrenadeCount(s->getMaxGrenadeCount());
            else if (menuOptions[selectedOption] == "Max hp")
                 s->setHealth(s->getMaxHealth());
            else if (menuOptions[selectedOption] == "Reload entities.json") {
                Entity::values = Entity::loadValues(assetsPath);
                loadGeneral();
            } else if (menuOptions[selectedOption] == "Reload room") {

                std::cout << "a" << std::endl;
                clearEntities("Samos");
                std::cout << "b" << std::endl;
                addEntities(currentMap.loadRoom());
                std::cout << "c" << std::endl;
            } else if (menuOptions[selectedOption] == "Reload map") {
                int mapId = currentMap.getCurrentRoomId();
                currentMap = Map::loadMap(currentMap.getName(), assetsPath);
                currentMap.setCurrentRoomId(mapId);
            } else if (menuOptions[selectedOption] == "Map viewer mode : ON") {
                mapViewer = false;
                camera = cameraBeforeMapViewer;
            } else if (menuOptions[selectedOption] == "Map viewer mode : OFF") {
                mapViewer = true;
                cameraBeforeMapViewer = camera;
            } else if (menuOptions[selectedOption] == "Fullscreen : ON") {
                fullscreen = false;
                params["fullscreen"] = false;
                std::ofstream paramsfile(assetsPath + "/params.json");
                paramsfile << params.dump(4);
                paramsfile.close();
            } else if (menuOptions[selectedOption] == "Fullscreen : OFF") {
                fullscreen = true;
                params["fullscreen"] = true;
                std::ofstream paramsfile(assetsPath + "/params.json");
                paramsfile << params.dump(4);
                paramsfile.close();
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
            menuOptions.push_back(std::string("< FPS : ") + std::to_string(int(Physics::frameRate)) + " >");
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

void Game::updateDialogue()
{
    for (std::vector<NPC*>::iterator j = NPCs.begin(); j != NPCs.end(); j++) {
        if (Entity::checkCollision(s, s->getBox(), *j, (*j)->getBox())) {
            if (inputList["interact"] && inputTime["interact"] == 0.0) {
                if ((*j)->getNpcType() == "Talking") {
                    // Set maxInteractions here because npc.cpp cannot include Physics.h
                    if ((*j)->getMaxInteractions() == 0)
                        (*j)->setMaxInteractions(stringsJson[language]["dialogues"][(*j)->getName()].size());

                    bool increased = false;
                    if (!currentDialogue.isNull())
                        if (currentDialogue.getText().size() - 1 > currentDialogue.getTextAdvancement()) {
                            currentDialogue.setTextAdvancement(currentDialogue.getTextAdvancement() + 1);
                            increased = true;
                        }
                    if (!increased) {
                        // Set new Dialogue
                        if (stringsJson[language]["dialogues"][(*j)->getName()][std::min((*j)->getTimesInteracted(), (*j)->getMaxInteractions() - 1)]["talking"].is_null())
                          currentDialogue = Dialogue(
                              stringsJson[language]["dialogues"][(*j)->getName()][std::min((*j)->getTimesInteracted(), (*j)->getMaxInteractions() - 1)]["text"], *j);
                        else
                            currentDialogue = Dialogue(stringsJson[language]["dialogues"][(*j)->getName()][std::min((*j)->getTimesInteracted(),(*j)->getMaxInteractions())]["text"],
                                    *j, stringsJson[language]["dialogues"][(*j)->getName()][std::min((*j)->getTimesInteracted(),(*j)->getMaxInteractions())]["talking"]);
                    }
                }
                // Don't forget to increment the Dialogue advancement
                (*j)->setTimesInteracted((*j)->getTimesInteracted() + 1);
            }
        } else if (currentDialogue.getTalking() != nullptr)
            if (!Entity::checkCollision(s, s->getBox(), currentDialogue.getTalking(), currentDialogue.getTalking()->getBox()))
                // Reset Dialogue if the player went too far away
                currentDialogue = Dialogue();
    }
}

void Game::updateCamera()
{
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

void Game::updateMapViewer()
{
    if (inputList["enter"] && inputTime["enter"] == 0.0) {
        int mapId = currentMap.getCurrentRoomId();
        currentMap = Map::loadMap(currentMap.getName(), assetsPath);
        currentMap.setCurrentRoomId(mapId);
        clearEntities("Samos");
        addEntities(currentMap.loadRoom());
    }

    if (inputList["down"] && !inputList["up"]) {
        camera.setY(camera.y() + mapViewerCameraSpeed / Physics::frameRate);
    } else if (!inputList["down"] && inputList["up"]) {
        camera.setY(camera.y() - mapViewerCameraSpeed / Physics::frameRate);
    }
    if (inputList["left"] && !inputList["right"]) {
        camera.setX(camera.x() - mapViewerCameraSpeed / Physics::frameRate);
    } else if (!inputList["left"] && inputList["right"]) {
        camera.setX(camera.x() + mapViewerCameraSpeed / Physics::frameRate);
    }

}

void Game::addEntity(Entity *entity)
{
    entities.push_back(entity);
    if (entity->getEntType() == "Terrain") {
        Terrain* t = static_cast<Terrain*>(entity);
        terrains.push_back(t);
    } else if (entity->getEntType() == "Projectile") {
        Projectile* p = static_cast<Projectile*>(entity);
        projectiles.push_back(p);
    } else if (entity->getEntType() == "DynamicObj") {
        DynamicObj* d = static_cast<DynamicObj*>(entity);
        dynamicObjs.push_back(d);
    } else if (entity->getEntType() == "Monster") {
        Monster* m = static_cast<Monster*>(entity);
        monsters.push_back(m);
    } else if (entity->getEntType() == "Area") {
        Area* a = static_cast<Area*>(entity);
        areas.push_back(a);
    } else if (entity->getEntType() == "NPC") {
        NPC* n = static_cast<NPC*>(entity);
        NPCs.push_back(n);
    } else if (entity->getEntType() == "Samos") {
        s = static_cast<Samos*>(entity);
    }
}

void Game::addEntities(std::vector<Entity*> es)
{
    for (std::vector<Entity*>::iterator entity = es.begin(); entity != es.end(); entity++) {
        entities.push_back(*entity);
        if ((*entity)->getEntType() == "Terrain") {
            Terrain* t = static_cast<Terrain*>(*entity);
            terrains.push_back(t);
        } else if ((*entity)->getEntType() == "Projectile") {
            Projectile* p = static_cast<Projectile*>(*entity);
            projectiles.push_back(p);
        } else if ((*entity)->getEntType() == "DynamicObj") {
            DynamicObj* d = static_cast<DynamicObj*>(*entity);
            dynamicObjs.push_back(d);
        } else if ((*entity)->getEntType() == "Monster") {
            Monster* m = static_cast<Monster*>(*entity);
            monsters.push_back(m);
        } else if ((*entity)->getEntType() == "Area") {
            Area* a = static_cast<Area*>(*entity);
            areas.push_back(a);
        } else if ((*entity)->getEntType() == "NPC") {
            NPC* n = static_cast<NPC*>(*entity);
            NPCs.push_back(n);
        } else if ((*entity)->getEntType() == "Samos") {
            s = static_cast<Samos*>(*entity);
        }
    }
}

void Game::clearEntities(std::string excludedType, bool deleteEntities)
{
    std::vector<Entity*> nextRen;
    for (std::vector<Entity*>::iterator ent = entities.begin(); ent != entities.end(); ent ++)
        if ((*ent)->getEntType() != excludedType) {
            if (deleteEntities)
                delete *ent;
        } else
            nextRen.push_back(*ent);

    terrains = {};
    monsters = {};
    NPCs = {};
    projectiles = {};
    areas = {};
    dynamicObjs = {};
    entities = {};
    addEntities(nextRen);
}

void Game::removeOtherRoomsEntities()
{
    std::vector<Entity*> newentities;
    std::vector<Entity*> toDelete;
    for (std::vector<Entity*>::iterator ent = entities.begin(); ent != entities.end(); ent++)
        if ((*ent)->getRoomId() == currentMap.getCurrentRoomId())
            newentities.push_back(*ent);
        else
            toDelete.push_back(*ent);
    clearEntities("", false);
    addEntities(newentities);
    for (std::vector<Entity*>::iterator ent = toDelete.begin(); ent != toDelete.end(); ent++)
        delete *ent;
}

void Game::removeEntities(std::vector<Entity *> es)
{
    std::vector<Entity*> newRen;
    for (std::vector<Entity*>::iterator j = entities.begin(); j != entities.end(); j++) {
        bool td = false;
        for (std::vector<Entity*>::iterator i = es.begin(); i != es.end(); i++) {
            if (*j == *i) {
                td = true;
                break;
            }
        }
        if (!td)
            newRen.push_back(*j);
    }

    monsters = {};
    terrains = {};
    s = nullptr;
    dynamicObjs = {};
    areas = {};
    projectiles = {};
    NPCs = {};
    entities = {};
    addEntities(newRen);
    for (std::vector<Entity*>::iterator i = es.begin(); i != es.end(); i++)
        delete *i;
}

void Game::updateAnimations()
{
    unsigned long long uC = updateCount;
    for (std::vector<Entity*>::iterator ent = entities.begin(); ent != entities.end(); ent++) {
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

std::vector<Entity *> *Game::getEntities()
{
    return &entities;
}

void Game::setEntities(const std::vector<Entity *> &newRendering)
{
    entities = newRendering;
}

int Game::getSelectedOption() const
{
    return selectedOption;
}

void Game::setSelectedOption(int newSelectedOption)
{
    selectedOption = newSelectedOption;
}

const std::string &Game::getMenu() const
{
    return menu;
}

void Game::setMenu(const std::string &newMenu)
{
    menu = newMenu;
}

const std::vector<std::string> &Game::getMenuOptions() const
{
    return menuOptions;
}

void Game::setMenuOptions(const std::vector<std::string> &newMenuOptions)
{
    menuOptions = newMenuOptions;
}

QPoint Game::getCamera()
{
    return camera;
}

void Game::setCamera(QPoint newCamera)
{
    camera = newCamera;
}

std::vector<Terrain *>* Game::getTerrains()
{
    return &terrains;
}

void Game::setTerrains(const std::vector<Terrain *> &newTerrains)
{
    terrains = newTerrains;
}

std::vector<Monster *>* Game::getMonsters()
{
    return &monsters;
}

void Game::setMonsters(const std::vector<Monster *> &newMonsters)
{
    monsters = newMonsters;
}

std::vector<NPC *>* Game::getNPCs()
{
    return &NPCs;
}

void Game::setNPCs(const std::vector<NPC *> &newNPCs)
{
    NPCs = newNPCs;
}

std::vector<Projectile *>* Game::getProjectiles()
{
    return &projectiles;
}

void Game::setProjectiles(const std::vector<Projectile *> &newProjectiles)
{
    projectiles = newProjectiles;
}

std::vector<Area *>* Game::getAreas()
{
    return &areas;
}

void Game::setAreas(const std::vector<Area *> &newAreas)
{
    areas = newAreas;
}

std::vector<DynamicObj *>* Game::getDynamicObjs()
{
    return &dynamicObjs;
}

void Game::setDynamicObjs(const std::vector<DynamicObj *> &newDynamicObjs)
{
    dynamicObjs = newDynamicObjs;
}

Samos *Game::getS() const
{
    return s;
}

void Game::setS(Samos *newS)
{
    s = newS;
}

bool Game::getMapViewer() const
{
    return mapViewer;
}

void Game::setMapViewer(bool newMapViewer)
{
    mapViewer = newMapViewer;
}

bool Game::getRunning() const
{
    return running;
}

void Game::setRunning(bool newRunning)
{
    running = newRunning;
}

double Game::getUpdateRate() const
{
    return updateRate;
}

void Game::setUpdateRate(double newUpdateRate)
{
    updateRate = newUpdateRate;
}

double Game::getGameSpeed() const
{
    return gameSpeed;
}

void Game::setGameSpeed(double newGameSpeed)
{
    gameSpeed = newGameSpeed;
}

unsigned long long Game::getFrameCount() const
{
    return frameCount;
}

void Game::setFrameCount(unsigned long long newFrameCount)
{
    frameCount = newFrameCount;
}

unsigned long long Game::getUpdateCount() const
{
    return updateCount;
}

void Game::setUpdateCount(unsigned long long newUpdateCount)
{
    updateCount = newUpdateCount;
}

nlohmann::json &Game::getKeyCodes()
{
    return keyCodes;
}

void Game::setKeyCodes(const nlohmann::json &newKeyCodes)
{
    keyCodes = newKeyCodes;
}

Map &Game::getCurrentMap()
{
    return currentMap;
}

void Game::setCurrentMap(const Map &newCurrentMap)
{
    currentMap = newCurrentMap;
}

const nlohmann::json &Game::getStringsJson() const
{
    return stringsJson;
}

void Game::setStringsJson(const nlohmann::json &newStringsJson)
{
    stringsJson = newStringsJson;
}

std::map<std::string, bool>* Game::getInputList()
{
    return &inputList;
}

void Game::setInputList(const std::map<std::string, bool> &newInputList)
{
    inputList = newInputList;
}

std::map<std::string, double>* Game::getInputTime()
{
    return &inputTime;
}

void Game::setInputTime(const std::map<std::string, double> &newInputTime)
{
    inputTime = newInputTime;
}

std::chrono::system_clock::time_point Game::getLastFpsShown() const
{
    return lastFpsShown;
}

void Game::setLastFpsShown(std::chrono::system_clock::time_point newLastFpsShown)
{
    lastFpsShown = newLastFpsShown;
}

std::chrono::system_clock::time_point Game::getLastFrameTime() const
{
    return lastFrameTime;
}

void Game::setLastFrameTime(std::chrono::system_clock::time_point newLastFrameTime)
{
    lastFrameTime = newLastFrameTime;
}

unsigned int Game::getFps() const
{
    return fps;
}

void Game::setFps(unsigned int newFps)
{
    fps = newFps;
}

unsigned int Game::getShowFpsUpdateRate() const
{
    return showFpsUpdateRate;
}

void Game::setShowFpsUpdateRate(unsigned int newShowFpsUpdateRate)
{
    showFpsUpdateRate = newShowFpsUpdateRate;
}

bool Game::getIsPaused() const
{
    return isPaused;
}

void Game::setIsPaused(bool newIsPaused)
{
    isPaused = newIsPaused;
}

const std::pair<int, int> &Game::getResolution() const
{
    return resolution;
}

void Game::setResolution(const std::pair<int, int> &newResolution)
{
    resolution = newResolution;
}

double Game::getMapViewerCameraSpeed() const
{
    return mapViewerCameraSpeed;
}

void Game::setMapViewerCameraSpeed(double newMapViewerCameraSpeed)
{
    mapViewerCameraSpeed = newMapViewerCameraSpeed;
}

const std::string &Game::getAssetsPath() const
{
    return assetsPath;
}

const std::string &Game::getDoorTransition() const
{
    return doorTransition;
}

void Game::setDoorTransition(const std::string &newDoorTransition)
{
    doorTransition = newDoorTransition;
}

const nlohmann::json &Game::getParams() const
{
    return params;
}

void Game::setParams(const nlohmann::json &newParams)
{
    params = newParams;
}

const std::string &Game::getLanguage() const
{
    return language;
}

void Game::setLanguage(const std::string &newLanguage)
{
    language = newLanguage;
}

Dialogue &Game::getCurrentDialogue()
{
    return currentDialogue;
}

void Game::setCurrentDialogue(const Dialogue &newCurrentDialogue)
{
    currentDialogue = newCurrentDialogue;
}

QPoint Game::getCameraBeforeMapViewer() const
{
    return cameraBeforeMapViewer;
}

void Game::setCameraBeforeMapViewer(QPoint newCameraBeforeMapViewer)
{
    cameraBeforeMapViewer = newCameraBeforeMapViewer;
}

double Game::getMenuArrowsTime() const
{
    return menuArrowsTime;
}

void Game::setMenuArrowsTime(double newMenuArrowsTime)
{
    menuArrowsTime = newMenuArrowsTime;
}

bool Game::getRenderHitboxes() const
{
    return renderHitboxes;
}

void Game::setRenderHitboxes(bool newRenderHitboxes)
{
    renderHitboxes = newRenderHitboxes;
}

bool Game::getShowFps() const
{
    return showFps;
}

void Game::setShowFps(bool newShowFps)
{
    showFps = newShowFps;
}

bool Game::getFullscreen() const
{
    return fullscreen;
}

void Game::setFullscreen(bool newFullscreen)
{
    fullscreen = newFullscreen;
}
