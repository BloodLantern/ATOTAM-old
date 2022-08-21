#include "projectile.h"

Projectile::Projectile(double x, double y, std::string facing, std::string type, std::string name, std::string ownerType)
    : Entity(x, y, nullptr, nullptr, "Projectile", false, facing, 0, name, true), projectileType(type), ownerType(ownerType)
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
    kb = proJson["knockback"];
    setLayer(proJson["layer"]);

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

bool Projectile::hitting(Entity* ent)
{
    if (getState() != "Hit") {
        if (getProjectileType() != "Bomb") {
            setLifeTime(0.0);
            setVX(0.0);
            setVY(0.0);
        }
    } else if (ent->getEntType() == "Samos" || ent->getEntType() == "Monster" || ent->getEntType() == "NPC" || ent->getEntType() == "DynamicObj") {
        Living* liv = static_cast<Living*>(ent);
        if (getProjectileType() != "Bomb" || ent->getEntType() != "Samos") {
            if ((liv->getITime() <= 0.0) && !liv->getInvulnerable()) {
                return liv->hit(damage, this, kb);
            }
        } else
            return liv->hit(0, this, 10*kb);
    }
    return false;
}

void Projectile::timeOut()
{
    setVX(0);
    setVY(0);
    setState("Hit");
    nlohmann::json pj = Entity::values["names"][getName()];
    setBox(new CollisionBox(pj["hit_offset_x"], pj["hit_offset_y"], pj["hit_width"], pj["hit_height"]));
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

double Projectile::getKb() const
{
    return kb;
}

void Projectile::setKb(double newKb)
{
    kb = newKb;
}

const std::string &Projectile::getOwnerType() const
{
    return ownerType;
}

void Projectile::setOwnerType(const std::string &newOwnerType)
{
    ownerType = newOwnerType;
}
