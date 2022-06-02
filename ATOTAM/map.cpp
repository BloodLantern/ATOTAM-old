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
            // For each entity json node
            for (nlohmann::json obj : name.second) {
                Entity* e = nullptr;

                // Specific Entities fields and initialization
                if (entity.first == "Terrain") {
                    Terrain *t = new Terrain(static_cast<int>(roomJson["position"][0]) + static_cast<int>(obj["x"]),
                            static_cast<int>(roomJson["position"][1]) + static_cast<int>(obj["y"]), name.first);
                    e = t;
                } else if (entity.first == "Door") {
                    Door *d = new Door(static_cast<int>(roomJson["position"][0]) + static_cast<int>(obj["x"]),
                            static_cast<int>(roomJson["position"][1]) + static_cast<int>(obj["y"]), name.first);
                    e = d;
                    d->setEndingRoom(obj["to"]);
                }

                // Make sure not to use a null Entity pointer
                if (e == nullptr)
                    continue;

                // Shared entity fields
                if (obj["state"].is_null())
                    e->setState("None");
                else
                    e->setState(obj["state"]);

                e->setCurrentAnimation(e->updateAnimation(e->getState()));
                e->setFrame(0);
                e->updateTexture();
                entities.push_back(e);
            }
    return entities;
}

std::vector<Entity *> Map::loadRoom()
{
    return loadRoom(currentRoomId);
}

int Map::getCurrentRoomId() const
{
    return currentRoomId;
}

void Map::setCurrentRoomId(int newCurrentRoomId)
{
    currentRoomId = newCurrentRoomId;
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
