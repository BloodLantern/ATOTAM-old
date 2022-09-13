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
    areaType = area.getAreaType();
    setCurrentAnimation(updateAnimation());
    setFrame(0);
    updateTexture();
}

Area::~Area()
{

}

nlohmann::json Area::getJsonRepresentation(bool defaultValues)
{
    nlohmann::json result = Entity::getJsonRepresentation(defaultValues);
    result["areaType"] = areaType;
    return result;
}

void Area::setJsonValues(nlohmann::json json)
{
    Entity::setJsonValues(json);
    if (!json["areaType"].is_null())
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
