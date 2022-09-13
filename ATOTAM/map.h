#ifndef MAP_H
#define MAP_H
#include <fstream>
#include "Entities/entity.h"
#define JSON_DIAGNOSTICS 1 // Json extended error messages
#include "nlohmann/json.hpp"
#include <string>

class Map
{
public:
    Map(); // Creates an empty Map
    static Map loadMap(std::string id, std::string assetsPath); // Loads a map via its ID and returns it
    std::vector<Entity*> loadRoom(std::string id); // Loads the selected room id and returns the array of entities it contains
    std::vector<Entity*> loadRoom(); // Loads the current room id and returns the array of entities it contains
    std::vector<Entity*> loadRooms(); // Loads all rooms and returns the array of entity they contain
    nlohmann::json find(Entity* entity); // Finds the given entity in this map's Json and returns its path
    void changeRoom(Entity* entity, std::string newRoomId); // Changes the current room of this Entity to the new one. DOES NOT CHANGE ITS COORDINATES

    const std::string &getName() const;
    void setName(const std::string &newName);
    const std::string &getFilePath() const;
    void setFilePath(const std::string &newFilePath);
    nlohmann::json* getJson();
    void setJson(const nlohmann::json &newJson);
    std::string getCurrentRoomId() const;
    void setCurrentRoomId(std::string newCurrentRoomId);
    std::string getLastRoomId() const;
    void setLastRoomId(std::string newLastRoomId);

private:
    Map(nlohmann::json json);
    nlohmann::json json = nlohmann::json();
    std::string name = "";
    std::string currentRoomId = "0";
    std::string lastRoomId = "0";
};

#endif // MAP_H
