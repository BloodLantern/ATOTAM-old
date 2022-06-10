#include "Entities/monster.h"
#include "mainwindow.h"
#include "map.h"
#include <fstream>
#include <Entities/area.h>
#include <Entities/door.h>
#include <Entities/npc.h>
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

int Map::getLastRoomId() const
{
    return lastRoomId;
}

void Map::setLastRoomId(int newLastRoomId)
{
    lastRoomId = newLastRoomId;
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
                // Repeat 'obj["times"]' times
                for (unsigned int i = 0; i < (obj["times"].is_null() ? 1 : static_cast<unsigned int>(obj["times"])); i++) {
                    Entity* e = nullptr;

                    int x = static_cast<int>(roomJson["position"][0]) + static_cast<int>(obj["x"]);
                    int y = static_cast<int>(roomJson["position"][1]) + static_cast<int>(obj["y"]);
                    std::string n = name.first; // Name

                    if (!obj["times"].is_null())
                        if (!obj["vertical"].is_null()) { // Should always be true
                            if (obj["vertical"])
                                y += i * static_cast<int>(Entity::values["names"][n]["height"]);
                            else
                                x += i * static_cast<int>(Entity::values["names"][n]["width"]);
                        }

                    // Specific Entities fields and initialization
                    if (entity.first == "Terrain") {
                        Terrain *t = new Terrain(x, y, n);
                        e = t;
                    } else if (entity.first == "Door") {
                        Door *d = new Door(x, y, n);
                        e = d;
                        d->setEndingRoom(obj["to"]);
                    } else if (entity.first == "NPC") {
                        NPC *npc = new NPC(x, y, obj["facing"], n);
                        e = npc;
                    } else if (entity.first == "Monster") {
                        Monster *m = new Monster(x, y, obj["facing"], n);
                        e = m;
                    }

                    // Make sure not to use a null Entity pointer
                    if (e == nullptr)
                        continue;

                    // Shared entity fields
                    if (obj["state"].is_null())
                        e->setState("None");
                    else
                        e->setState(obj["state"]);

                    e->setRoomId(id);
                    e->setCurrentAnimation(e->updateAnimation(e->getState()));
                    e->setFrame(0);
                    e->updateTexture();
                    entities.push_back(e);
                }
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
    setLastRoomId(currentRoomId);
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
