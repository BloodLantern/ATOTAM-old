#ifndef MAP_H
#define MAP_H
#include "Entities/monster.h"
#include "Entities/savepoint.h"
#include <fstream>
#include "Entities/area.h"
#include "Entities/door.h"
#include "Entities/npc.h"
#include "Entities/terrain.h"
#include "Entities/entity.h"
#define JSON_DIAGNOSTICS 1 // Json extended error messages
#include "nlohmann/json.hpp"
#include <string>

class Map
{
public:
    Map(); // Creates an empty Map
    static Map loadMap(std::string id, std::string assetsPath); // Loads a map via its ID and returns it
    std::vector<Entity*> loadRoom(int id); // Loads the selected room id and returns the array of entities it contains
    std::vector<Entity*> loadRoom(); // Loads the current room id and returns the array of entities it contains
    std::vector<Entity*> loadRooms(); // Loads all rooms and returns the array of entity they contain
    nlohmann::json find(Entity* entity); // Finds the given entity in this map's Json and returns its path

    const std::string &getName() const;
    void setName(const std::string &newName);
    const std::string &getFilePath() const;
    void setFilePath(const std::string &newFilePath);
    nlohmann::json* getJson();
    void setJson(const nlohmann::json &newJson);
    int getCurrentRoomId() const;
    void setCurrentRoomId(int newCurrentRoomId);
    int getLastRoomId() const;
    void setLastRoomId(int newLastRoomId);

private:
    Map(nlohmann::json json);
    nlohmann::json json = nlohmann::json();
    std::string name = "";
    int currentRoomId = 0;
    int lastRoomId = 0;
};

#endif // MAP_H
