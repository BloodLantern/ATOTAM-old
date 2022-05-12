#include "projectile.h"

Projectile::Projectile(double x, double y, std::string facing, std::string type, std::string name)
    : Entity(x, y, new CollisionBox(7, 7), nullptr, "Projectile", false, facing, 0, name, true)
{
    //Bombs don't move
    if (type == "Bomb")
        facing = "None";

    //Setting the speed depending on the direction
    if (facing == "None") {
        setVX(0);
        setVY(0);
    } else if (facing == "Up") {
        setVX(0);
        setVY(-1000);
    } else if (facing == "UpRight") {
        setVX(707);
        setVY(-707);
    } else if (facing == "Right") {
        setVX(1000);
        setVY(0);
    } else if (facing == "DownRight") {
        setVX(707);
        setVY(707);
    } else if (facing == "Down") {
        setVX(0);
        setVY(1000);
    } else if (facing == "DownLeft") {
        setVX(-707);
        setVY(707);
    } else if (facing == "Left") {
        setVX(-1000);
        setVY(0);
    } else if (facing == "UpLeft") {
        setVX(-707);
        setVY(-707);
    } else {
        throw Entity::invalidDirection;
    }

    //Setting the damage and adjusting the speed depending on the projectile type
    if (type == "Beam") {
        damage = 20;
        lifeTime = 1500;
        setState(facing);
    } else if (type == "Missile") {
        damage = 50;
        lifeTime = 3000;
        setVX(getVX() / 2);
        setVY(getVY() / 2);
        setState(facing);
    } else if (type == "Grenade") {
        damage = 60;
        lifeTime = 3000;
        setVX(getVX() / 2);
        setVY(getVY() / 2);
        setIsAffectedByGravity(true);
        setFrictionFactor(0.2);
        setState(facing);
    } else if (type == "Bomb") {
        damage = 50;
        lifeTime = 2000;
    } else {
        throw unknownProjectileType;
    }
}

int Projectile::getDamage() const
{
    return damage;
}

void Projectile::setDamage(int newDamage)
{
    damage = newDamage;
}

int Projectile::getLifeTime() const
{
    return lifeTime;
}

void Projectile::setLifeTime(int newLifeTime)
{
    lifeTime = newLifeTime;
}

const std::string &Projectile::getProjectileType() const
{
    return projectileType;
}

void Projectile::setProjectileType(const std::string &newProjectileType)
{
    projectileType = newProjectileType;
}
