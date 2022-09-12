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
        nlohmann::json::json_pointer ptr("/rooms/" + std::to_string(entity->getRoomId())
                                         + "/content/" + entity->getEntType() + "/" + entity->getFullName());
        // Find Entity json node in the map
        nlohmann::json entityJson = map->find(entity);
        // Iterate over the entities with the same entType and name
        for (auto& ent : mapJson->at(ptr).items())
            // Find the correct one
            if (ent.value() == entityJson) {
                // Change the values
                entity->setJsonValues(property["unmake"]);

                // Update entityJson
                entityJson = entity->getJsonRepresentation(false);

                // Update the animation (and therefore, the texture)
                entity->setCurrentAnimation(entity->updateAnimation());

                // Update map
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
        nlohmann::json::json_pointer ptr("/rooms/" + std::to_string(entity->getRoomId())
                                         + "/content/" + entity->getEntType() + "/" + entity->getFullName());
        // Find Entity json node in the map
        nlohmann::json entityJson = map->find(entity);
        // Iterate over the entities with the same entType and name
        for (auto& ent : mapJson->at(ptr).items())
            // Find the correct one
            if (ent.value() == entityJson) {
                // Change the values
                entity->setJsonValues(property["make"]);

                // Update entityJson
                entityJson = entity->getJsonRepresentation(false);

                // Update the animation (and therefore, the texture)
                entity->setCurrentAnimation(entity->updateAnimation());

                // Update map
                mapJson->at(ptr)[std::stoi(ent.key())] = entityJson;
                break;
            }
    }
    made = true;
}
