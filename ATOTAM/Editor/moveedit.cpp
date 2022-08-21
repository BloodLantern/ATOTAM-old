#include "moveedit.h"

MoveEdit::MoveEdit(Map *map, Entity *entity, QPoint* delta)
    : MultiTypeEdit(map, entity)
    , delta(delta)
{

}

MoveEdit::MoveEdit(Map *map, int roomId, QPoint* delta)
    : MultiTypeEdit(map, roomId)
    , delta(delta)
{

}

MoveEdit::~MoveEdit()
{
    delete delta;
}


void MoveEdit::unmake()
{
    if (roomEdit) {
        nlohmann::json json = map->getJson();
        int roomPos[] = {json["rooms"][roomId]["position"][0], json["rooms"][roomId]["position"][1]};
        roomPos[0] = roomPos[0] - delta->x();
        roomPos[1] = roomPos[1] - delta->y();
        json["rooms"][roomId]["position"] = roomPos;
        map->setJson(json);
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
                // Change the values
                entities[std::stoi(ent.key())]["x"] = entity->getX() - delta->x();
                entities[std::stoi(ent.key())]["y"] = entity->getY() - delta->y();
                entity->setX(entity->getX() - delta->x());
                entity->setY(entity->getY() - delta->y());
                break;
            }
        // Set back the modified json to the right one
        mapJson["rooms"][std::to_string(entity->getRoomId())]["content"][entity->getEntType()][entity->getFullName()] = entities;
        // And eventually set the map Json with the new one
        map->setJson(mapJson);
    }
    made = false;
}

void MoveEdit::make()
{
    if (roomEdit) {
        nlohmann::json json = map->getJson();
        int roomPos[] = {json["rooms"][roomId]["position"][0], json["rooms"][roomId]["position"][1]};
        roomPos[0] = roomPos[0] + delta->x();
        roomPos[1] = roomPos[1] + delta->y();
        json["rooms"][roomId]["position"] = roomPos;
        map->setJson(json);
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
                // Change the values
                entities[std::stoi(ent.key())]["x"] = entity->getX() + delta->x();
                entities[std::stoi(ent.key())]["y"] = entity->getY() + delta->y();
                entity->setX(entity->getX() + delta->x());
                entity->setY(entity->getY() + delta->y());
                break;
            }
        // Set back the modified json to the right one
        mapJson["rooms"][std::to_string(entity->getRoomId())]["content"][entity->getEntType()][entity->getFullName()] = entities;
        // And eventually set the map Json with the new one
        map->setJson(mapJson);
    }
    made = true;
}

QPoint *MoveEdit::getDelta() const
{
    return delta;
}

void MoveEdit::setDelta(QPoint *newDelta)
{
    delta = newDelta;
}
