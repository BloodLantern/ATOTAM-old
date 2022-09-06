#include "resizeedit.h"

ResizeEdit::ResizeEdit(Map *map, Entity *entity, Direction direction, std::pair<int, int> delta)
    : MultiTypeEdit(map, entity)
    , delta(delta)
    , move(new MoveEdit(map, entity, QPoint(delta.first, delta.second)))
    , direction(direction)
{

}

ResizeEdit::ResizeEdit(Map *map, int roomId, Direction direction, std::pair<int, int> delta)
    : MultiTypeEdit(map, roomId)
    , delta(delta)
    , move(new MoveEdit(map, roomId, QPoint(delta.first, delta.second)))
    , direction(direction)
{

}

ResizeEdit::~ResizeEdit()
{
    if (move) {
        delete move;
        move = nullptr;
    }
}

void ResizeEdit::unmake()
{
    if (roomEdit) {
        /*nlohmann::json json = map->getJson();
        int roomPos[] = {json["rooms"][roomId]["position"][0], json["rooms"][roomId]["size"][1]};
        roomPos[0] = roomPos[0] - delta->first;
        roomPos[1] = roomPos[1] - delta->second;
        json["rooms"][roomId]["size"] = roomPos;
        map->setJson(json);*/
    } else {
        // Get map Json pointer
        nlohmann::json* mapJson = map->getJson();
        // json_pointer to the entity list
        nlohmann::json::json_pointer ptr("/rooms/" + std::to_string(entity->getRoomId())
                                         + "/content/" + entity->getEntType() + "/" + entity->getFullName());
        // json_pointer to the room position
        nlohmann::json::json_pointer roomPtr("/rooms/" + std::to_string(entity->getRoomId()) + "/position");
        // Find Entity json node in the map
        nlohmann::json entityJson = map->find(entity);
        // Iterate over the entities with the same entType and name
        for (auto& ent : mapJson->at(ptr).items())
            // Find the correct one
            if (ent.value() == entityJson) {
                // Change the values
                // Reset box size
                entity->getBox()->setWidth(entity->getBox()->getWidth() / entity->getHorizontalRepeat());
                entity->getBox()->setHeight(entity->getBox()->getHeight() / entity->getVerticalRepeat());
                // Change repeat argument
                entity->setHorizontalRepeat(entity->getHorizontalRepeat() - delta.first);
                entity->setVerticalRepeat(entity->getVerticalRepeat() - delta.second);
                // Resize box
                entity->getBox()->setWidth(entity->getBox()->getWidth() * entity->getHorizontalRepeat());
                entity->getBox()->setHeight(entity->getBox()->getHeight() * entity->getVerticalRepeat());

                // Update entityJson
                entityJson = entity->getJsonRepresentation();

                // Update map
                mapJson->at(ptr)[std::stoi(ent.key())] = entityJson;
                mapJson->at(ptr)[std::stoi(ent.key())]["x"] = entityJson["x"].get<double>() - mapJson->at(roomPtr)[0].get<int>();
                mapJson->at(ptr)[std::stoi(ent.key())]["y"] = entityJson["y"].get<double>() - mapJson->at(roomPtr)[1].get<int>();

                // Eventually update the animation (and therefore, the texture)
                entity->setCurrentAnimation(entity->updateAnimation());
                break;
            }
    }
    move->unmake();
    made = false;
}

void ResizeEdit::make()
{
    if (roomEdit) {
        /*nlohmann::json json = map->getJson();
        int roomPos[] = {json["rooms"][roomId]["position"][0], json["rooms"][roomId]["size"][1]};
        roomPos[0] = roomPos[0] + delta->first;
        roomPos[1] = roomPos[1] + delta->second;
        json["rooms"][roomId]["size"] = roomPos;
        map->setJson(json);*/
    } else {
        // Get map Json pointer
        nlohmann::json* mapJson = map->getJson();
        // json_pointer to the entity list
        nlohmann::json::json_pointer ptr("/rooms/" + std::to_string(entity->getRoomId())
                                         + "/content/" + entity->getEntType() + "/" + entity->getFullName());
        // json_pointer to the room position
        nlohmann::json::json_pointer roomPtr("/rooms/" + std::to_string(entity->getRoomId()) + "/position");
        // Find Entity json node in the map
        nlohmann::json entityJson = map->find(entity);
        // Iterate over the entities with the same entType and name
        for (auto& ent : mapJson->at(ptr).items())
            // Find the correct one
            if (ent.value() == entityJson) {
                // Change the values
                // Reset box size
                entity->getBox()->setWidth(entity->getBox()->getWidth() / entity->getHorizontalRepeat());
                entity->getBox()->setHeight(entity->getBox()->getHeight() / entity->getVerticalRepeat());
                // Change repeat argument
                entity->setHorizontalRepeat(entity->getHorizontalRepeat() + delta.first);
                entity->setVerticalRepeat(entity->getVerticalRepeat() + delta.second);
                // Resize box
                entity->getBox()->setWidth(entity->getBox()->getWidth() * entity->getHorizontalRepeat());
                entity->getBox()->setHeight(entity->getBox()->getHeight() * entity->getVerticalRepeat());

                // Update entityJson
                entityJson = entity->getJsonRepresentation();

                // Update map
                mapJson->at(ptr)[std::stoi(ent.key())] = entityJson;
                mapJson->at(ptr)[std::stoi(ent.key())]["x"] = entityJson["x"].get<double>() - mapJson->at(roomPtr)[0].get<int>();
                mapJson->at(ptr)[std::stoi(ent.key())]["y"] = entityJson["y"].get<double>() - mapJson->at(roomPtr)[1].get<int>();

                // Eventually update the animation (and therefore, the texture)
                entity->setCurrentAnimation(entity->updateAnimation());
                break;
            }
    }
    move->make();
    made = true;
}

bool ResizeEdit::getDeltaChangedLastFrame() const
{
    return deltaChangedLastFrame;
}

void ResizeEdit::setDeltaChangedLastFrame(bool newDeltaChangedLastFrame)
{
    deltaChangedLastFrame = newDeltaChangedLastFrame;
}

ResizeEdit::Direction ResizeEdit::getDirection() const
{
    return direction;
}

void ResizeEdit::setDirection(ResizeEdit::Direction newDirection)
{
    direction = newDirection;
}

std::pair<int, int> ResizeEdit::getDelta() const
{
    return delta;
}

void ResizeEdit::setDelta(std::pair<int, int> newDelta)
{
    if (delta != newDelta)
        deltaChangedLastFrame = true;
    delta = newDelta;
}

MoveEdit *ResizeEdit::getMove() const
{
    return move;
}

void ResizeEdit::setMove(MoveEdit *newMove)
{
    move = newMove;
}
