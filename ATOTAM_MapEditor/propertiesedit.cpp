#include "propertiesedit.h"

PropertiesEdit::PropertiesEdit(Map *map, Entity *entity, nlohmann::json propertyParam)
    : MultiTypeEdit(map, entity)
    , property(propertyParam)
{
    nlohmann::json completeProperty;
    completeProperty["make"] = propertyParam;
    completeProperty["unmake"][propertyParam.begin().key()] = entity->getJsonRepresentation(true)[propertyParam.begin().key()];
    property = completeProperty;
}

PropertiesEdit::PropertiesEdit(Map *map, int roomId, nlohmann::json propertyParam)
    : MultiTypeEdit(map, roomId)
    , property(propertyParam)
{
}

PropertiesEdit::~PropertiesEdit()
{
}


void PropertiesEdit::unmake()
{
    if (roomEdit) {
    } else {
        // Get map Json pointer
        nlohmann::json* mapJson = map->getJson();
        // json_pointer to the entity list
        nlohmann::json::json_pointer ptr("/rooms/" + entity->getRoomId()
                                         + "/content/" + entity->getEntType() + "/" + entity->getFullName());
        // Find Entity json node in the map
        nlohmann::json entityJson = map->find(entity);
        // Iterate over the entities with the same entType and name
        for (auto& ent : mapJson->at(ptr).items())
            // Find the correct one
            if (ent.value() == entityJson) {
                // Update entity room
                if (property["unmake"].begin().key() == "roomId") {
                    map->changeRoom(entity, property["unmake"]["roomId"]);
                }

                // Change the values
                entity->setJsonValues(property["unmake"]);

                // Update entityJson
                entityJson = entity->getJsonRepresentation(false);

                // Change the entity position relative to the room
                if (property["unmake"].begin().key() == "roomId") {
                    property["unmake"]["x"] = entity->getX()
                            - mapJson->at(
                                nlohmann::json::json_pointer("/rooms/" + property["make"]["roomId"].get<std::string>() + "/position/0")).get<int>()
                            + mapJson->at(
                                nlohmann::json::json_pointer("/rooms/" + property["unmake"]["roomId"].get<std::string>() + "/position/0")).get<int>();
                    property["unmake"]["y"] = entity->getY()
                            - mapJson->at(
                                nlohmann::json::json_pointer("/rooms/" + property["make"]["roomId"].get<std::string>() + "/position/1")).get<int>()
                            + mapJson->at(
                                nlohmann::json::json_pointer("/rooms/" + property["unmake"]["roomId"].get<std::string>() + "/position/1")).get<int>();
                }

                // Update the animation (and therefore, the texture)
                entity->setCurrentAnimation(entity->updateAnimation());

                // Update map
                ptr = nlohmann::json::json_pointer("/rooms/" + entity->getRoomId()
                                                   + "/content/" + entity->getEntType() + "/" + entity->getFullName());
                mapJson->at(ptr)[std::stoi(ent.key())] = entityJson;
                break;
            }
    }
    made = false;
}

void PropertiesEdit::make()
{
    if (roomEdit) {
    } else {
        // Get map Json pointer
        nlohmann::json* mapJson = map->getJson();
        // json_pointer to the entity list
        nlohmann::json::json_pointer ptr("/rooms/" + entity->getRoomId()
                                         + "/content/" + entity->getEntType() + "/" + entity->getFullName());
        // Find Entity json node in the map
        nlohmann::json entityJson = map->find(entity);
        // Iterate over the entities with the same entType and name
        for (auto& ent : mapJson->at(ptr).items())
            // Find the correct one
            if (ent.value() == entityJson) {
                // Update entity room
                if (property["make"].begin().key() == "roomId") {
                    map->changeRoom(entity, property["make"]["roomId"]);
                }

                // Change the values
                entity->setJsonValues(property["make"]);

                // Update entityJson
                entityJson = entity->getJsonRepresentation(false);

                // Change the entity position relative to the room
                if (property["make"].begin().key() == "roomId") {
                    property["make"]["x"] = entity->getX()
                            - mapJson->at(
                                nlohmann::json::json_pointer("/rooms/" + property["unmake"]["roomId"].get<std::string>() + "/position/0")).get<int>()
                            + mapJson->at(
                                nlohmann::json::json_pointer("/rooms/" + property["make"]["roomId"].get<std::string>() + "/position/0")).get<int>();
                    property["make"]["y"] = entity->getY()
                            - mapJson->at(
                                nlohmann::json::json_pointer("/rooms/" + property["unmake"]["roomId"].get<std::string>() + "/position/1")).get<int>()
                            + mapJson->at(
                                nlohmann::json::json_pointer("/rooms/" + property["make"]["roomId"].get<std::string>() + "/position/1")).get<int>();
                }

                // Update the animation (and therefore, the texture)
                entity->setCurrentAnimation(entity->updateAnimation());

                // Update map
                ptr = nlohmann::json::json_pointer("/rooms/" + entity->getRoomId()
                                                   + "/content/" + entity->getEntType() + "/" + entity->getFullName());
                mapJson->at(ptr)[std::stoi(ent.key())] = entityJson;
                break;
            }
    }
    made = true;
}
