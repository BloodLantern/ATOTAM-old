#ifndef DYNAMICOBJ_H
#define DYNAMICOBJ_H

#include "living.h"

class DynamicObj : public Living
{
public:
    DynamicObj(double x, double y, std::string facing, std::string name);
};

#endif // DYNAMICOBJ_H
