#include "projectile.h"

Projectile::Projectile(double x, double y, std::string facing, std::string type, std::string name)
    : Entity(x, y, nullptr, nullptr, "Projectile", false, facing, 0, name, true), projectileType(type)
{
    //Because animations are buggy
    setLastFrameState("None");
    setLastFrameFacing("None");

    nlohmann::json proJson = Entity::values["names"][name];
    setBox(new CollisionBox(proJson["width"], proJson["width"]));

    //Setting the speed depending on the direction
    if (facing == "None") {
        setVX(0);
        setVY(0);
    } else if (facing == "Up") {
        setVX(0);
        setVY(-static_cast<double>(proJson["speed"]));
    } else if (facing == "UpRight") {
        setVX(static_cast<double>(proJson["speed"]) * 0.707);
        setVY(-static_cast<double>(proJson["speed"]) * 0.707);
    } else if (facing == "Right") {
        setVX(static_cast<double>(proJson["speed"]));
        setVY(0);
    } else if (facing == "DownRight") {
        setVX(static_cast<double>(proJson["speed"]) * 0.707);
        setVY(static_cast<double>(proJson["speed"]) * 0.707);
    } else if (facing == "Down") {
        setVX(0);
        setVY(static_cast<double>(proJson["speed"]) );
    } else if (facing == "DownLeft") {
        setVX(-static_cast<double>(proJson["speed"]) * 0.707);
        setVY(static_cast<double>(proJson["speed"]) * 0.707);
    } else if (facing == "Left") {
        setVX(-static_cast<double>(proJson["speed"]) );
        setVY(0);
    } else if (facing == "UpLeft") {
        setVX(-static_cast<double>(proJson["speed"]) * 0.707);
        setVY(-static_cast<double>(proJson["speed"]) * 0.707);
    } else {
        throw Entity::invalidDirection;
    }

    //Setting the damage and adjusting the speed depending on the projectile type
    damage = proJson["damage"];
    lifeTime = proJson["lifeTime"];

    if (type == "Beam") {
        setState(facing);
    } else if (type == "Missile") {
        setState(facing);
    } else if (type == "Grenade") {
        setIsAffectedByGravity(proJson["gravity"]);
        setFrictionFactor(proJson["friction"]);
        setState(facing);
    } else if (type == "Bomb") {
        setState("Normal");
    } else {
        throw unknownProjectileType;
    }
}

void Projectile::hitting(Entity* ent)
{
    if (projectileType == "Beam") {
        setVX(0);
        setVY(0);
        setState("Hit");
        if (ent->getEntType() == "Samos" || ent->getEntType() == "Monster" || ent->getEntType() == "NPC" || ent->getEntType() == "DynamicObj") {
            Living* liv = static_cast<Living*>(ent);
            liv->hit();
        }
    } else if (projectileType == "Missile") {
        setVX(0);
        setVY(0);
        setState("Hit");
        if (ent->getEntType() == "Samos" || ent->getEntType() == "Monster" || ent->getEntType() == "NPC" || ent->getEntType() == "DynamicObj") {
            Living* liv = static_cast<Living*>(ent);
            liv->hit();
        }
    }
}

void Projectile::timeOut()
{
    if (projectileType == "Beam") {
        setVX(0);
        setVY(0);
        setState("Hit");
    } else if (projectileType == "Missile") {
        setVX(0);
        setVY(0);
        setState("Hit");
    } else if (projectileType == "Bomb") {
        setVX(0);
        setVY(0);
        setState("Hit");
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

double Projectile::getLifeTime() const
{
    return lifeTime;
}

void Projectile::setLifeTime(double newLifeTime)
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
