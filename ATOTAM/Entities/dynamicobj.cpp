#include "dynamicobj.h"

DynamicObj::DynamicObj(double x, double y, std::string facing, std::string name)
    : Living(x, y, facing, name)
{

}

const std::string &DynamicObj::getMaterial() const
{
    return material;
}

void DynamicObj::setMaterial(const std::string &newMaterial)
{
    material = newMaterial;
}
