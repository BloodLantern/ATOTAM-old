
#include "map.h"
#include "Entities/door.h"
#include "Entities/npc.h"
#include "Entities/terrain.h"
#include "Entities/monster.h"
#include "Entities/savepoint.h"

Map::Map()
{
    // Create an empty map with an empty room. That room is necessary because of the 'startingRoom' node
    json = "{"
           "    \"name\": \"\","
           "    \"rooms\": {"
           "        \"0\": {"
           "            \"content\": {},"
           "            \"position\": ["
           "                0,"
           "                0"
           "            ],"
           "            \"size\": ["
           "                1920,"
           "                1080"
           "            ]"
           "        }"
           "    },"
           "    \"startingRoom\": \"0\""
           "}"_json;
}

Map Map::loadMap(std::string id, std::string assetsPath)
{
    std::ifstream file(assetsPath + "/maps/" + id + ".json");
    nlohmann::json json;
    file >> json;
    return Map(json);
}

Map::Map(nlohmann::json json)
    : json(json), name(json["name"]), currentRoomId(json["startingRoom"])
{

}

std::vector<Entity *> Map::loadRoom(std::string id)
{
    // Result
    std::vector<Entity*> entities;
    // Json node of the selected room
    nlohmann::json roomJson = json["rooms"][id];
    // Iterate over a map of entityTypes
    for (auto entity : roomJson["content"].items())
        // Iterate over a map of entityNames
        for (auto name : entity.value().items())
            // For each entity json node
            for (nlohmann::json obj : name.value()) {
                // Repeat 'obj["times"]' times
                for (unsigned int i = 0; i < (obj["times"].is_null() ? 1 : static_cast<unsigned int>(obj["times"])); i++) {
                    Entity* e = nullptr;

                    int x = static_cast<int>(roomJson["position"][0]) + static_cast<int>(obj["x"]);
                    int y = static_cast<int>(roomJson["position"][1]) + static_cast<int>(obj["y"]);
                    std::string n; // Name
                    std::vector<std::string> np; // Name parameters
                    std::string fullName = name.key(); // Temporary

                    // Only get name parameters if they exist
                    for (std::string::iterator c = fullName.begin(); c != fullName.end(); c++)
                        if ((*c) == '_') {
                            // Name initialization
                            for (std::string::iterator character = fullName.begin(); character < c; character++)
                                n += *character;

                            // Name parameters initialization
                            std::string* params = new std::string;
                            for (std::string::iterator character = c + 1; character != fullName.end(); character++)
                                (*params) += *character;
                            np.push_back("");
                            unsigned int* currentParam = new unsigned int(0);
                            for (std::string::iterator character = params->begin(); character != params->end(); character++)
                                if ((*character) == '-') {
                                    np.push_back("");
                                    (*currentParam)++;
                                } else
                                    np[*currentParam] += *character;
                            delete currentParam;
                            delete params;

                            break;
                        }
                    // If the full name doesn't contain name paramters
                    if (n == "")
                        n = fullName;

                    if (!obj["times"].is_null())
                        if (!obj["vertical"].is_null()) { // Should always be true at this point
                            if (obj["vertical"])
                                y += i * static_cast<int>(Entity::values["names"][n]["height"]);
                            else
                                x += i * static_cast<int>(Entity::values["names"][n]["width"]);
                        }

                    // Specific Entities fields and initialization
                    if (entity.key() == "Terrain") {
                        Terrain *t = new Terrain(x, y, n);
                        e = t;
                    } else if (entity.key() == "Area") {
                        if (n.substr(n.size() - 4, 4) == "Door") {
                            Door *d = new Door(x, y, n);
                            e = d;
                            d->setEndingRoom(obj["to"]);
                        }
                    } else if (entity.key() == "NPC") {
                        if (n == "Savepoint") {
                            Savepoint *s = new Savepoint(x, y, obj["spID"], this->name);
                            e = s;
                        } else {
                            NPC *npc = new NPC(x, y, obj["facing"], n);
                            e = npc;
                        }
                    } else if (entity.key() == "Monster") {
                        Monster *m = new Monster(x, y, obj["facing"], n);
                        e = m;
                    }

                    // Make sure not to use a null Entity pointer
                    if (e == nullptr)
                        continue;

                    // Shared entity fields
                    e->setFullName(fullName);

                    if (obj["state"].is_null())
                        e->setState("None");
                    else
                        e->setState(obj["state"]);

                    // If the entity has a repetition, extend its collision box
                    unsigned int horizontalRepeat = obj["horizontalRepeat"].is_null() ? 1 : obj["horizontalRepeat"].get<int>();
                    unsigned int verticalRepeat = obj["verticalRepeat"].is_null() ? 1 : obj["verticalRepeat"].get<int>();
                    if (horizontalRepeat != 1 || verticalRepeat != 1) {
                        CollisionBox* box = e->getBox();
                        box->setHeight(box->getHeight() * (verticalRepeat));
                        box->setWidth(box->getWidth() * (horizontalRepeat));
                    }
                    e->setHorizontalRepeat(horizontalRepeat);
                    e->setVerticalRepeat(verticalRepeat);

                    e->setNameParameters(np);
                    e->setRoomId(id);

                    // Rendering (should be the last function calls)
                    e->setCurrentAnimation(e->updateAnimation());
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

std::vector<Entity *> Map::loadRooms()
{
    std::vector<Entity*> result;
    for (const std::pair<std::string, nlohmann::json> room : json["rooms"].get<nlohmann::json::object_t>()) {
        // Load room content
        std::vector<Entity*> roomContent = loadRoom(room.first);
        // Preallocate memory
        result.reserve(roomContent.size());
        // Merge vectors
        result.insert(result.end(), roomContent.begin(), roomContent.end());
    }
    return result;
}

// Checks if this Entity is present in this room.
// If true, returns its JSON representation with coordinates relative to the room ones; otherwise, throws an InvalidArgument exception.
nlohmann::json Map::find(Entity *entity)
{
    nlohmann::json j = json["rooms"][entity->getRoomId()];
    // Entity isn't in any room
    if (j.is_null())
        throw std::invalid_argument("Map::find(Entity*) received an Entity which is in a non-existent room.");

    j = j["content"][entity->getEntType()];
    // Entity type isn't present in this room
    if (j.is_null())
        throw std::invalid_argument("Map::find(Entity*) received an Entity type which isn't present in this room.");

    j = j[entity->getFullName()];
    // Entity name isn't present in this room
    if (j.is_null())
        throw std::invalid_argument("Map::find(Entity*) received an Entity name which isn't present in this room.");

    nlohmann::json entJson = entity->getJsonRepresentation(false);
    // X pos relative to the room
    entJson["x"] = entJson["x"].get<double>()
                    - json["rooms"][entity->getRoomId()]["position"][0].get<int>();
    // Y pos relative to the room
    entJson["y"] = entJson["y"].get<double>()
                    - json["rooms"][entity->getRoomId()]["position"][1].get<int>();
    // Eventually compare the entity and the json values
    for (const nlohmann::json &ent : j) {
        if (entJson == ent) {
            return entJson;
        }
    }

    // Nothing was found
    std::cerr << "Map entity type JSON node:" << std::endl;
    std::cerr << j.dump(4) << std::endl << std::endl;
    std::cerr << "Entity JSON node (the one to find):" << std::endl;
    std::cerr << entJson.dump(4) << std::endl << std::endl;
    throw std::invalid_argument("Map::find(Entity*) received an Entity that couldn't be found.");
}

void Map::changeRoom(Entity *entity, std::string newRoomId)
{
    std::string oldRoomId = entity->getRoomId();
    // Do nothing if the new room is the same one
    if (oldRoomId == newRoomId)
        return;

    // Make sure this Entity is in this Map and get its JSON node
    nlohmann::json entJson = find(entity);

    // Create the JSON pointer
    nlohmann::json::json_pointer ptr("/rooms/" + oldRoomId + "/content/" + entity->getEntType() + "/" + entity->getName());

    // Then erase it from the old room
    nlohmann::json nameJson = json.at(ptr);
    for (unsigned int i = 0; i < nameJson.size(); i++)
        if (nameJson[i] == entJson) {
            json.at(ptr).erase(i);
            break;
        }

    // Remove the subnodes in the old room if they are empty
    ptr = nlohmann::json::json_pointer("/rooms/" + oldRoomId + "/content/" + entity->getEntType());
    if (json.at(ptr)[entity->getName()].empty())
        json.at(ptr).erase(entity->getName());
    ptr = nlohmann::json::json_pointer("/rooms/" + oldRoomId + "/content");
    if (json.at(ptr)[entity->getEntType()].empty())
        json.at(ptr).erase(entity->getEntType());

    // Create the subnodes in the new room if they don't already exist
    ptr = nlohmann::json::json_pointer("/rooms/" + newRoomId + "/content/" + entity->getEntType());
    if (json.at(ptr).is_null())
        json.at(ptr) = "{}"_json;
    ptr = nlohmann::json::json_pointer("/rooms/" + newRoomId + "/content/" + entity->getEntType() + "/" + entity->getName());
    if (json.at(ptr).is_null())
        json.at(ptr) = "[]"_json;

    // Eventually add it to the new room
    json.at(ptr).push_back(entJson);
}

std::string Map::getCurrentRoomId() const
{
    return currentRoomId;
}

void Map::setCurrentRoomId(std::string newCurrentRoomId)
{
    setLastRoomId(currentRoomId);
    currentRoomId = newCurrentRoomId;
}

std::string Map::getLastRoomId() const
{
    return lastRoomId;
}

void Map::setLastRoomId(std::string newLastRoomId)
{
    lastRoomId = newLastRoomId;
}

const std::string &Map::getName() const
{
    return name;
}

void Map::setName(const std::string &newName)
{
    name = newName;
}

nlohmann::json* Map::getJson()
{
    return &json;
}

void Map::setJson(const nlohmann::json &newJson)
{
    json = newJson;
}
