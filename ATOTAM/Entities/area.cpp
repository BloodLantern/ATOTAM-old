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

Area::~Area()
{

}

const std::string &Area::getAreaType() const
{
    return areaType;
}

void Area::setAreaType(const std::string &newAreaType)
{
    areaType = newAreaType;
}
