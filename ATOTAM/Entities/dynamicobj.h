#ifndef DYNAMICOBJ_H
#define DYNAMICOBJ_H

#include "living.h"

class DynamicObj : public Living
{
public:
    DynamicObj(double x, double y, std::string facing, std::string name);
    const std::string &getMaterial() const;
    void setMaterial(const std::string &newMaterial);

private:
    std::string material = "";
};

#endif // DYNAMICOBJ_H
