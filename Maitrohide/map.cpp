#include "map.h"
#include <fstream>
#include <Entities/terrain.h>
#include <iostream>

std::vector<Entity*> Map::loadMap(Map map)
{
    // Result
    std::vector<Entity*> entities;
    // Json node of the selected room
    nlohmann::json roomJson = map.getJson()["rooms"][map.getStringRoom()];
    // Iterate over a map of entityTypes
    for (const std::pair<std::string, nlohmann::json> entity : roomJson["content"].get<nlohmann::json::object_t>())
        // Iterate over a map of entityNames
        for (const std::pair<std::string, nlohmann::json> name : entity.second.get<nlohmann::json::object_t>())
            // For the entity count
            for (unsigned int i = 0; i < name.second["count"]; i++)
                // Create and add the right entity
                if (entity.first == "Terrain") {
                    Terrain *t = new Terrain(name.second["x"][i], name.second["y"][i], name.first);
                    entities.push_back(t);
                } else if (entity.first == "DynamicObj") {
                    //DynamicObj t(name.second["x"][i], name.second["y"][i], name.first);
                    //entities.push_back(t);
                } else if (entity.first == "Monster") {
                    //Monster t(name.second["x"][i], name.second["y"][i], name.first);
                    //entities.push_back(t);
                } else if (entity.first == "NPC") {
                    //NPC t(name.second["x"][i], name.second["y"][i], name.first);
                    //entities.push_back(t);
                } else if (entity.first == "Area") {
                    //Area t(name.second["x"][i], name.second["y"][i], name.first);
                    //entities.push_back(t);
                }
    return entities;
}

Map::Map(std::string file)
    : filePath("../assets/maps/" + file + ".json"), room(0, 0)
{
    // Init Json object
    std::ifstream fileStream(this->filePath);
    fileStream >> json;

    name = json["name"];
}

std::string Map::getStringRoom()
{
    std::string result = "";
    result += std::to_string(room.x());
    result += "-";
    result += std::to_string(room.y());
    return result;
}

const std::string &Map::getName() const
{
    return name;
}

void Map::setName(const std::string &newName)
{
    name = newName;
}

const std::string &Map::getFilePath() const
{
    return filePath;
}

void Map::setFilePath(const std::string &newFilePath)
{
    filePath = newFilePath;
}

const nlohmann::json &Map::getJson() const
{
    return json;
}

void Map::setJson(const nlohmann::json &newJson)
{
    json = newJson;
}

QPoint Map::getRoom() const
{
    return room;
}

void Map::setRoom(QPoint newRoom)
{
    room = newRoom;
}
