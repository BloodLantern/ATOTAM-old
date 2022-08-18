#ifndef GAME_H
#define GAME_H

#define JSON_DIAGNOSTICS 1 // Json extended error messages
#include "nlohmann/json.hpp"

#include "dialogue.h"
#include "map.h"
#include "physics.h"
#include "Entities/area.h"
#include "Entities/dynamicobj.h"
#include "Entities/entity.h"
#include "Entities/living.h"
#include "Entities/monster.h"
#include "Entities/npc.h"
#include "Entities/samos.h"
#include "Entities/terrain.h"

#include <QString>
class Game
{
public:
    Game(std::string assetsPath);

    void addEntity(Entity *entity);
    void addEntities(std::vector<Entity*> es);
    void clearEntities(std::string excludeType = "", bool deleteEntities = true);
    void removeOtherRoomsEntities();
    void removeEntities(std::vector<Entity*> es);
    void updateAnimations();
    void updateMapViewer();
    void updateMenu();
    void updateDialogue();
    void updateCamera();
    nlohmann::json loadJson(std::string fileName); // Loads the given file name's json starting in the assets folder and returns it
    QString translate(std::string text, std::vector<std::string> subCategories);
    void loadGeneral();

    std::vector<Entity *> *getEntities();
    void setEntities(const std::vector<Entity *> &newRendering);
    int getSelectedOption() const;
    void setSelectedOption(int newSelectedOption);
    const std::string &getMenu() const;
    void setMenu(const std::string &newMenu);
    const std::vector<std::string> &getMenuOptions() const;
    void setMenuOptions(const std::vector<std::string> &newMenuOptions);
    QPoint getCamera();
    void setCamera(QPoint newCamera);
    std::vector<Terrain *> *getTerrains();
    void setTerrains(const std::vector<Terrain *> &newTerrains);
    std::vector<Monster *> *getMonsters();
    void setMonsters(const std::vector<Monster *> &newMonsters);
    std::vector<NPC *> *getNPCs();
    void setNPCs(const std::vector<NPC *> &newNPCs);
    std::vector<Projectile *> *getProjectiles();
    void setProjectiles(const std::vector<Projectile *> &newProjectiles);
    std::vector<Area *> *getAreas();
    void setAreas(const std::vector<Area *> &newAreas);
    std::vector<DynamicObj *> *getDynamicObjs();
    void setDynamicObjs(const std::vector<DynamicObj *> &newDynamicObjs);
    Samos *getS();
    void setS(Samos *newS);
    bool getMapViewer() const;
    void setMapViewer(bool newMapViewer);
    bool getRunning() const;
    void setRunning(bool newRunning);
    double getUpdateRate() const;
    void setUpdateRate(double newUpdateRate);
    double getGameSpeed() const;
    void setGameSpeed(double newGameSpeed);
    unsigned long long getFrameCount() const;
    void setFrameCount(unsigned long long newFrameCount);
    unsigned long long getUpdateCount() const;
    void setUpdateCount(unsigned long long newUpdateCount);
    nlohmann::json &getKeyCodes();
    void setKeyCodes(const nlohmann::json &newKeyCodes);
    Map &getCurrentMap();
    void setCurrentMap(const Map &newCurrentMap);
    const nlohmann::json &getStringsJson() const;
    void setStringsJson(const nlohmann::json &newStringsJson);
    std::map<std::string, bool> *getInputList();
    void setInputList(const std::map<std::string, bool> &newInputList);
    std::map<std::string, double> *getInputTime();
    void setInputTime(const std::map<std::string, double> &newInputTime);
    std::chrono::system_clock::time_point getLastFpsShown() const;
    void setLastFpsShown(std::chrono::system_clock::time_point newLastFpsShown);
    std::chrono::system_clock::time_point getLastFrameTime() const;
    void setLastFrameTime(std::chrono::system_clock::time_point newLastFrameTime);
    unsigned int getFps() const;
    void setFps(unsigned int newFps);
    unsigned int getShowFpsUpdateRate() const;
    void setShowFpsUpdateRate(unsigned int newShowFpsUpdateRate);
    bool getIsPaused() const;
    void setIsPaused(bool newIsPaused);
    const std::pair<int, int> &getResolution() const;
    void setResolution(const std::pair<int, int> &newResolution);
    double getMapViewerCameraSpeed() const;
    void setMapViewerCameraSpeed(double newMapViewerCameraSpeed);
    const std::string &getAssetsPath() const;
    const std::string &getDoorTransition() const;
    void setDoorTransition(const std::string &newDoorTransition);
    const nlohmann::json &getParams() const;
    void setParams(const nlohmann::json &newParams);
    const std::string &getLanguage() const;
    void setLanguage(const std::string &newLanguage);
    Dialogue &getCurrentDialogue();
    void setCurrentDialogue(const Dialogue &newCurrentDialogue);
    QPoint getCameraBeforeMapViewer() const;
    void setCameraBeforeMapViewer(QPoint newCameraBeforeMapViewer);
    double getMenuArrowsTime() const;
    void setMenuArrowsTime(double newMenuArrowsTime);
    bool getRenderHitboxes() const;
    void setRenderHitboxes(bool newRenderHitboxes);
    bool getShowFps() const;
    void setShowFps(bool newShowFps);
    bool getFullscreen() const;
    void setFullscreen(bool newFullscreen);

private:
    std::string assetsPath;

    std::vector<Entity*> entities;
    std::vector<Terrain*> terrains;
    std::vector<Monster*> monsters;
    std::vector<NPC*> NPCs;
    std::vector<Projectile*> projectiles;
    std::vector<Area*> areas;
    std::vector<DynamicObj*> dynamicObjs;
    Samos* s;
    int selectedOption = 0;
    std::string menu;
    std::vector<std::string> menuOptions;
    QPoint camera = QPoint(0,0);

    bool mapViewer = false; // Whether the game the game was launched as a map viewer
    bool running = true;
    double updateRate = 60.0; // How many game updates in one second
    double gameSpeed = 1.0;
    unsigned long long frameCount = 0;
    unsigned long long updateCount = 0;
    nlohmann::json keyCodes;
    Map currentMap;
    nlohmann::json stringsJson;
    std::map<std::string, bool> inputList;
    std::map<std::string, double> inputTime;
    std::chrono::system_clock::time_point lastFpsShown; // Time of the last frame in which the shown fps were updated
    std::chrono::system_clock::time_point lastFrameTime; // Time of the last frame in which the shown fps were updated
    unsigned int fps = 0; // Fps count when 'lastFpsShown' was updated
    unsigned int showFpsUpdateRate = true; // Time (in ns) between each fps update
    bool isPaused = false;
    std::pair<int,int> resolution = {1920, 1080};
    double mapViewerCameraSpeed = 100;
    std::string doorTransition = "";
    nlohmann::json params;
    std::string language;
    Dialogue currentDialogue;
    QPoint cameraBeforeMapViewer;
    double menuArrowsTime = 0.0;
    bool renderHitboxes = false; // Render hitboxes as rectangles
    bool showFps = true; // Whether to show fps in-game
    bool fullscreen = true;
};

#endif // GAME_H