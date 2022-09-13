#include "door.h"

Door::Door(double x, double y, std::string name)
    : Area(x, y, name)
{
    setAreaType("Door");
}

Door::~Door()
{

}

nlohmann::json Door::getJsonRepresentation(bool defaultValues)
{
    nlohmann::json result = Area::getJsonRepresentation(defaultValues);
    result["to"] = endingRoom;
    return result;
}

void Door::setJsonValues(nlohmann::json json)
{
    Area::setJsonValues(json);
    if (!json["to"].is_null())
        endingRoom = json["to"];
}

std::string Door::getEndingRoom() const
{
    return endingRoom;
}

void Door::setEndingRoom(std::string newEndingRoom)
{
    endingRoom = newEndingRoom;
}
