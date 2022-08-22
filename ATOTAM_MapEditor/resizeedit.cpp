#include "resizeedit.h"

ResizeEdit::ResizeEdit(Map *map, Entity *entity, Direction direction, std::pair<int, int>* delta)
    : MultiTypeEdit(map, entity)
    , delta(delta)
    , move(new MoveEdit(map, entity, new QPoint(delta->first, delta->second)))
    , direction(direction)
{

}

ResizeEdit::ResizeEdit(Map *map, int roomId, Direction direction, std::pair<int, int>* delta)
    : MultiTypeEdit(map, roomId)
    , delta(delta)
    , move(new MoveEdit(map, roomId, new QPoint(delta->first, delta->second)))
    , direction(direction)
{

}

ResizeEdit::~ResizeEdit()
{
    delete delta;
    delete move;
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
        nlohmann::json::json_pointer ptr("/rooms/" + std::to_string(entity->getRoomId()) + "/content/" + entity->getEntType() + "/" + entity->getFullName());
        // Find Entity json node in the map
        nlohmann::json entityJson = map->find(entity);
        // Iterate over the entities with the same entType and name
        for (auto& ent : mapJson->at(ptr).items())
            // Find the correct one
            if (ent.value() == entityJson) {
                // Change the values
                mapJson->at(ptr)[std::stoi(ent.key())]["horizontalRepeat"] = entity->getHorizontalRepeat() - delta->first;
                mapJson->at(ptr)[std::stoi(ent.key())]["verticalRepeat"] = entity->getVerticalRepeat() - delta->second;
                entity->getBox()->setWidth(entity->getBox()->getWidth() / entity->getHorizontalRepeat());
                entity->getBox()->setHeight(entity->getBox()->getHeight() / entity->getVerticalRepeat());
                entity->setHorizontalRepeat(entity->getHorizontalRepeat() - delta->first);
                entity->setVerticalRepeat(entity->getVerticalRepeat() - delta->second);
                entity->getBox()->setWidth(entity->getBox()->getWidth() * entity->getHorizontalRepeat());
                entity->getBox()->setHeight(entity->getBox()->getHeight() * entity->getVerticalRepeat());
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
        nlohmann::json::json_pointer ptr("/rooms/" + std::to_string(entity->getRoomId()) + "/content/" + entity->getEntType() + "/" + entity->getFullName());
        // Find Entity json node in the map
        nlohmann::json entityJson = map->find(entity);
        // Iterate over the entities with the same entType and name
        for (auto& ent : mapJson->at(ptr).items())
            // Find the correct one
            if (ent.value() == entityJson) {
                // Change the values
                mapJson->at(ptr)[std::stoi(ent.key())]["horizontalRepeat"] = entity->getHorizontalRepeat() + delta->first;
                mapJson->at(ptr)[std::stoi(ent.key())]["verticalRepeat"] = entity->getVerticalRepeat() + delta->second;
                entity->getBox()->setWidth(entity->getBox()->getWidth() / entity->getHorizontalRepeat());
                entity->getBox()->setHeight(entity->getBox()->getHeight() / entity->getVerticalRepeat());
                entity->setHorizontalRepeat(entity->getHorizontalRepeat() + delta->first);
                entity->setVerticalRepeat(entity->getVerticalRepeat() + delta->second);
                entity->getBox()->setWidth(entity->getBox()->getWidth() * entity->getHorizontalRepeat());
                entity->getBox()->setHeight(entity->getBox()->getHeight() * entity->getVerticalRepeat());
                entity->setCurrentAnimation(entity->updateAnimation());
                break;
            }
    }
    move->make();
    made = true;
}

ResizeEdit::Direction ResizeEdit::getDirection() const
{
    return direction;
}

void ResizeEdit::setDirection(ResizeEdit::Direction newDirection)
{
    direction = newDirection;
}

std::pair<int, int> *ResizeEdit::getDelta() const
{
    return delta;
}

void ResizeEdit::setDelta(std::pair<int, int> *newDelta)
{
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
