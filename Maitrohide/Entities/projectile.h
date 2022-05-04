#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "entity.h"

class Projectile : public Entity
{
public:
    static const int unknownProjectileType = -1;
    enum ProjectileType {Beam, Missile, Grenade, Bomb};
    Projectile(double x, double y, Direction facing, ProjectileType type, std::string name);
    int getDamage() const;
    void setDamage(int newDamage);

    int getLifeTime() const;
    void setLifeTime(int newLifeTime);

private:
    int damage;
    int lifeTime; // in ms, starts with a positive value, destroys the object when null or negative
};

#endif // PROJECTILE_H
