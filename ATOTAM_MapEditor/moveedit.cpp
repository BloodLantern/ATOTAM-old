#include "moveedit.h"

MoveEdit::MoveEdit(Map *map, Entity *entity, QPoint delta)
    : MultiTypeEdit(map, entity)
    , delta(delta)
{

}

MoveEdit::MoveEdit(Map *map, int roomId, QPoint delta)
    : MultiTypeEdit(map, roomId)
    , delta(delta)
{

}

MoveEdit::~MoveEdit()
{
}


void MoveEdit::unmake()
{
    if (roomEdit) {
        /*nlohmann::json json = map->getJson();
        int roomPos[] = {json["rooms"][roomId]["position"][0], json["rooms"][roomId]["position"][1]};
        roomPos[0] = roomPos[0] - delta->x();
        roomPos[1] = roomPos[1] - delta->y();
        json["rooms"][roomId]["position"] = roomPos;
        map->setJson(json);*/
    } else {
        // Get map Json pointer
        nlohmann::json* mapJson = map->getJson();
        // json_pointer to the entity list
        nlohmann::json::json_pointer ptr("/rooms/" + entity->getRoomId() + "/content/" + entity->getEntType() + "/" + entity->getFullName());
        // json_pointer to the room position
        nlohmann::json::json_pointer roomPtr("/rooms/" + entity->getRoomId() + "/position");
        // Find Entity json node in the map
        nlohmann::json entityJson = map->find(entity);
        // Iterate over the entities with the same entType and name
        for (auto& ent : mapJson->at(ptr).items())
            // Find the correct one
            if (ent.value() == entityJson) {
                // Change the values
                entity->setX(entity->getX() - delta.x());
                entity->setY(entity->getY() - delta.y());

                // Update entityJson
                entityJson = entity->getJsonRepresentation(false);

                // Update map
                mapJson->at(ptr)[std::stoi(ent.key())] = entityJson;
                mapJson->at(ptr)[std::stoi(ent.key())]["x"] = entityJson["x"].get<double>() - mapJson->at(roomPtr)[0].get<int>();
                mapJson->at(ptr)[std::stoi(ent.key())]["y"] = entityJson["y"].get<double>() - mapJson->at(roomPtr)[1].get<int>();
                break;
            }
    }
    made = false;
}

void MoveEdit::make()
{
    if (roomEdit) {
        /*nlohmann::json json = map->getJson();
        int roomPos[] = {json["rooms"][roomId]["position"][0], json["rooms"][roomId]["position"][1]};
        roomPos[0] = roomPos[0] + delta->x();
        roomPos[1] = roomPos[1] + delta->y();
        json["rooms"][roomId]["position"] = roomPos;
        map->setJson(json);*/
    } else {
        // Get map Json pointer
        nlohmann::json* mapJson = map->getJson();
        // json_pointer to the entity list
        nlohmann::json::json_pointer ptr("/rooms/" + entity->getRoomId()
                                         + "/content/" + entity->getEntType() + "/" + entity->getFullName());
        // json_pointer to the room position
        nlohmann::json::json_pointer roomPtr("/rooms/" + entity->getRoomId() + "/position");
        // Find Entity json node in the map
        nlohmann::json entityJson = map->find(entity);
        // Iterate over the entities with the same entType and name
        for (auto& ent : mapJson->at(ptr).items())
            // Find the correct one
            if (ent.value() == entityJson) {
                // Change the values
                entity->setX(entity->getX() + delta.x());
                entity->setY(entity->getY() + delta.y());

                // Update entityJson
                entityJson = entity->getJsonRepresentation(false);

                // Update map
                mapJson->at(ptr)[std::stoi(ent.key())] = entityJson;
                mapJson->at(ptr)[std::stoi(ent.key())]["x"] = entityJson["x"].get<double>() - mapJson->at(roomPtr)[0].get<int>();
                mapJson->at(ptr)[std::stoi(ent.key())]["y"] = entityJson["y"].get<double>() - mapJson->at(roomPtr)[1].get<int>();
                break;
            }
    }
    made = true;
}

QPoint MoveEdit::getDelta() const
{
    return delta;
}

void MoveEdit::setDelta(QPoint newDelta)
{
    delta = newDelta;
}
