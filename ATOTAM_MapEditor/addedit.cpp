#include "addedit.h"
#include "removeedit.h"

AddEdit::AddEdit(Map *map, Entity *entity)
    : MultiTypeEdit(map, entity)
{

}

AddEdit::AddEdit(Map *map, int roomId)
    : MultiTypeEdit(map, roomId)
{

}

void AddEdit::unmake()
{
    if (roomEdit) {
    } else {
        RemoveEdit edit(map, entity);
        edit.make();
    }
}

void AddEdit::make()
{
    if (roomEdit) {
    } else {
        // Get map Json
        nlohmann::json mapJson = map->getJson();
        // Find Entity json node in the map
        nlohmann::json entityJson = map->find(entity);
        // For easier reading and performance improvements
        nlohmann::json entities = mapJson["rooms"][std::to_string(entity->getRoomId())]["content"][entity->getEntType()][entity->getFullName()];
        // Iterate over the entities with the same entType and name
        for (auto& ent : entities.items())
            // Find the correct one
            if (ent.value() == entityJson) {
                // Remove it
                entities.erase(ent.key());
                break;
            }
        // Set back the modified json to the right one
        mapJson["rooms"][std::to_string(entity->getRoomId())]["content"][entity->getEntType()][entity->getFullName()] = entities;
        // And eventually set the map Json with the new one
        map->setJson(mapJson);
    }
    made = true;
}
