#include "samos.h"

Samos::Samos(double x, double y, int maxHealth, int maxGrenadeCount, int maxMissileCount)
    : Living(x, y, "Right", "Samos"),
      isInAltForm(false), maxGrenadeCount(maxGrenadeCount), maxMissileCount(maxMissileCount)
{
    this->setMaxHealth(maxHealth);
}

Samos::Samos(double x, double y, int maxHealth, int maxGrenadeCount, int maxMissileCount, CollisionBox *box, QImage *texture, std::string entityType, int health, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name, bool isMovable)
    : Living(x, y, box, texture, entityType, health, maxHealth, isAffectedByGravity, facing, frictionFactor, name, isMovable), maxGrenadeCount(maxGrenadeCount), maxMissileCount(maxMissileCount)
{

}

Samos::~Samos()
{

}

void Samos::shoot(std::string type)
{
    Projectile* projectile = nullptr;

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

    if (canonDirection == "None") {
        throw Entity::invalidDirection;
    } else if (canonDirection == "Up") {
        projectile = new Projectile(getX() + 50, getY() - 10, "Up", type, type);
    } else if (canonDirection == "UpRight") {
        projectile = new Projectile(getX() + 100, getY(), "UpRight", type, type);
    } else if (canonDirection == "Right") {
        projectile = new Projectile(getX() + 100, getY() + 75, "Right", type, type);
    } else if (canonDirection == "DownRight") {
       projectile = new Projectile(getX() + 100, getY() + 150, "DownRight", type, type);
    } else if (canonDirection == "Down") {
        projectile = new Projectile(getX() + 50, getY() + 210, "Down", type, type);
    } else if (canonDirection == "DownLeft") {
        projectile = new Projectile(getX(), getY() + 150, "DownLeft", type, type);
    } else if (canonDirection == "Left") {
        projectile = new Projectile(getX(), getY() + 75, "Left", type, type);
    } else if (canonDirection == "UpLeft") {
        projectile = new Projectile(getX(), getY(), "UpLeft", type, type);
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
