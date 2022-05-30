#ifndef MAP_H
#define MAP_H

#include "nlohmann/json.hpp"
#include <string>
#include <Entities/entity.h>

class Map
{
public:
    static Map loadMap(std::string id); // Loads a map via its ID and returns it
    std::vector<Entity*> loadRoom(int id); // Loads the selected room id and returns the array of entities it contains
    std::vector<Entity*> loadRoom(); // Loads the current room id and returns the array of entities it contains

    const std::string &getName() const;
    void setName(const std::string &newName);

    const std::string &getFilePath() const;
    void setFilePath(const std::string &newFilePath);

    const nlohmann::json &getJson() const;
    void setJson(const nlohmann::json &newJson);

    int getCurrentRoomId() const;
    void setCurrentRoomId(int newCurrentRoomId);

private:
    Map(nlohmann::json json);
    std::string name;
    nlohmann::json json;
    int currentRoomId;
};

#endif // MAP_H
