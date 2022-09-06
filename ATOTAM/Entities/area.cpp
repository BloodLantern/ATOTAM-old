#include "area.h"

Area::Area(double x, double y, CollisionBox* box, QImage* texture, std::string name)
    : Entity(x, y, box, texture, "Area", false, "None", 0.0, name, false)
{
    setLastFrameFacing("None");
}

Area::Area(double x, double y, std::string name) :
    Entity(x, y, "None", name)
{
    setLastFrameFacing("None");
    setLastFrameState("None");
    setState("Normal");
}

Area::Area(const Area &area)
    : Area(area.getX(), area.getY(), area.getName())
{

}

Area::~Area()
{

}

nlohmann::json Area::getJsonRepresentation()
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
    result["areaType"] = areaType;
    return result;
}

void Area::setJsonValues(nlohmann::json json)
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
    areaType = json["areaType"];
}

const std::string &Area::getAreaType() const
{
    return areaType;
}

void Area::setAreaType(const std::string &newAreaType)
{
    areaType = newAreaType;
}
