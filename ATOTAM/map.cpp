#include "mainwindow.h"
#include "map.h"
#include <fstream>
#include <Entities/area.h>
#include <Entities/door.h>
#include <Entities/terrain.h>

Map Map::loadMap(std::string id)
{
    std::ifstream file(MainWindow::assetsPath + "/maps/" + id + ".json");
    nlohmann::json json;
    file >> json;
    return Map(json);
}

Map::Map(nlohmann::json json)
    : name(json["name"]), json(json), currentRoomId(json["startingRoom"])
{

}

int Map::getCurrentRoomId() const
{
    return currentRoomId;
}

void Map::setCurrentRoomId(int newCurrentRoomId)
{
    currentRoomId = newCurrentRoomId;
}

std::vector<Entity *> Map::loadRoom(int id)
{
    // Result
    std::vector<Entity*> entities;
    // Json node of the selected room
    nlohmann::json roomJson = json["rooms"][std::to_string(id)];
    // Iterate over a map of entityTypes
    for (const std::pair<std::string, nlohmann::json> entity : roomJson["content"].get<nlohmann::json::object_t>())
        // Iterate over a map of entityNames
        for (const std::pair<std::string, nlohmann::json> name : entity.second.get<nlohmann::json::object_t>())
            // For the entity count
            for (unsigned int i = 0; i < name.second["x"].size(); i++)
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
                    Area *t = new Area(name.second["x"][i], name.second["y"][i], name.first);
                    entities.push_back(t);
                } else if (entity.first == "Door") {
                    Door *t = new Door(name.second["x"][i], name.second["y"][i], name.first);
                    entities.push_back(t);
                }
    return entities;
}

std::vector<Entity *> Map::loadRoom()
{
    return loadRoom(currentRoomId);
}

const std::string &Map::getName() const
{
    return name;
}

void Map::setName(const std::string &newName)
{
    name = newName;
}

const nlohmann::json &Map::getJson() const
{
    return json;
}

void Map::setJson(const nlohmann::json &newJson)
{
    json = newJson;
}
