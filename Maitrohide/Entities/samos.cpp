#include "samos.h"
#include <iostream>

Samos::Samos(double x, double y, int maxHealth, int maxGrenadeCount, int maxMissileCount)
    : Living(x, y, "Right", "Samos"),
      isInAltForm(false), maxGrenadeCount(maxGrenadeCount), maxMissileCount(maxMissileCount),
      wallBoxR(new CollisionBox(getBox()->getX() + getBox()->getWidth(), getBox()->getY(), 2, getBox()->getHeight())),
      wallBoxL(new CollisionBox(getBox()->getX() - 2, getBox()->getY(), 2, getBox()->getHeight()))
{
    setMaxHealth(maxHealth);
}

Samos::Samos(double x, double y, int maxHealth, int maxGrenadeCount, int maxMissileCount, CollisionBox *box, QImage *texture, std::string entityType, int health, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name, bool isMovable)
    : Living(x, y, box, texture, entityType, health, maxHealth, isAffectedByGravity, facing, frictionFactor, name, isMovable),
      maxGrenadeCount(maxGrenadeCount),
      maxMissileCount(maxMissileCount),
      wallBoxR(new CollisionBox(box->getX() + box->getWidth(), box->getY(), 2, box->getHeight())),
      wallBoxL(new CollisionBox(box->getX() - 2, box->getY(), 2, box->getHeight()))
{

}

Samos::~Samos()
{
    delete wallBoxL;
    delete wallBoxR;
}

void Samos::shoot(std::string type)
{
    Projectile* projectile = nullptr;
    double renderingM = values["general"]["renderingMultiplier"];

    //Can't shoot a missile/grenade if you don't have any
    if (type == "Grenade") {
        if (grenadeCount > 0)
            grenadeCount--;
        else
            return;
    } else if (type == "Missile") {
        if (missileCount > 0)
            missileCount--;
        else
            return;
    }

    //Spawn the projectile at certain coordinates to match the sprite
    if (canonDirection == "None") {
        throw Entity::invalidDirection;
    } else if (canonDirection == "Up") {
        projectile = new Projectile(getX() + 11 * renderingM, getY() - 0 * renderingM, "Up", type, type);
    } else if (canonDirection == "UpRight") {
        projectile = new Projectile(getX() + 27 * renderingM, getY() - 0 * renderingM, "UpRight", type, type);
    } else if (canonDirection == "Right") {
        projectile = new Projectile(getX() + 30 * renderingM, getY() + 15 * renderingM, "Right", type, type);
    } else if (canonDirection == "DownRight") {
       projectile = new Projectile(getX() + 27 * renderingM, getY() + 30 * renderingM, "DownRight", type, type);
    } else if (canonDirection == "Down") {
        projectile = new Projectile(getX() + 11 * renderingM, getY() + 45 * renderingM, "Down", type, type);
    } else if (canonDirection == "DownLeft") {
        projectile = new Projectile(getX() + 0 * renderingM, getY() + 30 * renderingM, "DownLeft", type, type);
    } else if (canonDirection == "Left") {
        projectile = new Projectile(getX() + 0 * renderingM, getY() + 15 * renderingM, "Left", type, type);
    } else if (canonDirection == "UpLeft") {
        projectile = new Projectile(getX() + 0 * renderingM, getY() + 0 * renderingM, "UpLeft", type, type);
    } else {
        throw Entity::invalidDirection;
    }

    delete projectile; // TEMP
}

bool Samos::getIsInAltForm() const
{
    return isInAltForm;
}

void Samos::setIsInAltForm(bool newIsInAltForm)
{
    isInAltForm = newIsInAltForm;
}

int Samos::getGrenadeCount() const
{
    return grenadeCount;
}

void Samos::setGrenadeCount(int newGrenadeCount)
{
    grenadeCount = newGrenadeCount;
}

int Samos::getMaxGrenadeCount() const
{
    return maxGrenadeCount;
}

void Samos::setMaxGrenadeCount(int newMaxGrenadeCount)
{
    maxGrenadeCount = newMaxGrenadeCount;
}

int Samos::getMissileCount() const
{
    return missileCount;
}

void Samos::setMissileCount(int newMissileCount)
{
    missileCount = newMissileCount;
}

int Samos::getMaxMissileCount() const
{
    return maxMissileCount;
}

void Samos::setMaxMissileCount(int newMaxMissileCount)
{
    maxMissileCount = newMaxMissileCount;
}

double Samos::getJumpTime() const
{
    return jumpTime;
}

void Samos::setJumpTime(double newJumpTime)
{
    jumpTime = newJumpTime;
}

CollisionBox *Samos::getWallBoxR() const
{
    return wallBoxR;
}

void Samos::setWallBoxR(CollisionBox *newWallBoxR)
{
    delete wallBoxR;
    wallBoxR = newWallBoxR;
}

CollisionBox *Samos::getWallBoxL() const
{
    return wallBoxL;
}

void Samos::setWallBoxL(CollisionBox *newWallBoxL)
{
    delete wallBoxL;
    wallBoxL = newWallBoxL;
}
