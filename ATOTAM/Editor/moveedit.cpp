#include "moveedit.h"

MoveEdit::MoveEdit(Map *map, Entity *entity, QPoint startingPos, QPoint endingPos)
    : Edit(map)
    , entity(entity)
    , roomMove(false)
    , startingPos(startingPos)
    , endingPos(endingPos)
{

}

MoveEdit::MoveEdit(Map *map, int roomId, QPoint startingPos, QPoint endingPos)
    : Edit(map)
    , roomId(roomId)
    , roomMove(true)
    , startingPos(startingPos)
    , endingPos(endingPos)
{

}


void MoveEdit::unmake()
{
}

void MoveEdit::make()
{
    if (roomMove) {
        nlohmann::json json = map->getJson();
        json["rooms"][roomId]["position"] = {endingPos.x(), endingPos.y()};
        map->setJson(json);
    } else {
        // Get map Json
        nlohmann::json mapJson = map->getJson();
        // Find Entity json node in the map
        nlohmann::json entityJson = map->find(entity, startingPos);
        // For easier reading and performance imrpovements
        nlohmann::json entities = mapJson["rooms"][std::to_string(entity->getRoomId())]["content"][entity->getEntType()][entity->getFullName()];
        // Iterate over the entities with the same entType and name
        for (auto& ent : entities.items())
            // Find the correct one
            if (ent.value() == entityJson) {
                // Change the values
                entities[std::stoi(ent.key())]["x"] = endingPos.x();
                entities[std::stoi(ent.key())]["y"] = endingPos.y();
                break;
            }
        // Set back the modified json to the right one
        mapJson["rooms"][std::to_string(entity->getRoomId())]["content"][entity->getEntType()][entity->getFullName()] = entities;
        // And eventually set the map Json with the new one
        map->setJson(mapJson);
    }
    made = true;
}
