#include "door.h"

Door::Door(double x, double y, std::string name)
    : Area(x, y, name)
{
    setAreaType("Door");
}

Door::~Door()
{

}

nlohmann::json Door::getJsonRepresentation()
{
    nlohmann::json result;
    result["x"] = getX();
    result["y"] = getY();
    if (getState() != "None")
        result["state"] = getState();
    if (getFacing() != "None")
        result["facing"] = getFacing();
    if (getHorizontalRepeat() != 1)
        result["horizontalRepeat"] = getHorizontalRepeat();
    if (getVerticalRepeat() != 1)
        result["verticalRepeat"] = getVerticalRepeat();
    result["areaType"] = getAreaType();
    result["to"] = endingRoom;
    return result;
}

void Door::setJsonValues(nlohmann::json json)
{
    setX(json["x"]);
    setY(json["y"]);
    if (!json["state"].is_null())
        setState(json["state"]);
    if (!json["facing"].is_null())
        setFacing(json["facing"]);
    if (!json["horizontalRepeat"].is_null())
        setHorizontalRepeat(json["horizontalRepeat"]);
    if (!json["verticalRepeat"].is_null())
        setVerticalRepeat(json["verticalRepeat"]);
    setAreaType(json["areaType"]);
    endingRoom = json["to"];
}

int Door::getEndingRoom() const
{
    return endingRoom;
}

void Door::setEndingRoom(int newEndingRoom)
{
    endingRoom = newEndingRoom;
}
