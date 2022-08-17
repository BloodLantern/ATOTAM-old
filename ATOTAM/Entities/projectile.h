#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "entity.h"
#include "living.h"

class Projectile : public Entity
{
public:
    static const int unknownProjectileType = -3;
    //enum ProjectileType {Beam, Missile, Grenade, Bomb};
    Projectile(double x, double y, std::string facing, std::string type, std::string name);

    void hitting(Entity* ent);
    void timeOut();

    int getDamage() const;
    void setDamage(int newDamage);

    double getLifeTime() const;
    void setLifeTime(double newLifeTime);

    const std::string &getProjectileType() const;
    void setProjectileType(const std::string &newProjectileType);

    double getKb() const;
    void setKb(double newKb);

private:
    int damage = 0;
    double kb = 0;
    double lifeTime = 1; // in s, starts with a positive value, destroys the object when null or negative
    std::string projectileType = "";
};

#endif // PROJECTILE_H
