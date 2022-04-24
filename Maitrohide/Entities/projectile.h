#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "entity.h"

class Projectile : public Entity
{
public:
    enum ProjectileType {Beam, Rocket, Grenade, Bomb};
    Projectile(double x, double y, Direction facing, ProjectileType type);
};

#endif // PROJECTILE_H
