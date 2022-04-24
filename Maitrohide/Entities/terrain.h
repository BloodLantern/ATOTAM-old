#ifndef TERRAIN_H
#define TERRAIN_H

#include "entity.h"

class Terrain : public Entity
{
public:
    Terrain(double x, double y, CollisionBox* box, QImage* texture);
    ~Terrain();
};

#endif // TERRAIN_H
