#include "samos.h"

Samos::Samos(double x, double y, int maxHealth, int maxGrenadeCount, int maxMissileCount)
    : Living(x, y,Entity::Direction::Right, "Samos"),
      isInAltForm(false), maxGrenadeCount(maxGrenadeCount), maxMissileCount(maxMissileCount)
{
    this->setMaxHealth(maxHealth);
}

Samos::~Samos()
{

}

void Samos::shoot(Projectile::ProjectileType type)
{
    class Projectile* projectile = nullptr;

    if (type == Projectile::ProjectileType::Grenade) {
        if (grenadeCount > 0)
            grenadeCount--;
        else
            return;
    } else if (type == Projectile::ProjectileType::Missile) {
        if (missileCount > 0)
            missileCount--;
        else
            return;
    }

    switch (canonDirection) {
    case None:
        throw invalidDirection;
        break;
    case Up:
        projectile = new class Projectile(getX() + 50, getY() - 10, Direction::Up, type, Projectile::getStringProjType(type));
        break;
    case UpRight:
        projectile = new class Projectile(getX() + 100, getY(), Direction::UpRight, type, Projectile::getStringProjType(type));
        break;
    case Right:
        projectile = new class Projectile(getX() + 100, getY() + 75, Direction::Right, type, Projectile::getStringProjType(type));
        break;
    case DownRight:
        projectile = new class Projectile(getX() + 100, getY() + 150, Direction::DownRight, type, Projectile::getStringProjType(type));
        break;
    case Down:
        projectile = new class Projectile(getX() + 50, getY() + 210, Direction::Down, type, Projectile::getStringProjType(type));
        break;
    case DownLeft:
        projectile = new class Projectile(getX(), getY() + 150, Direction::DownLeft, type, Projectile::getStringProjType(type));
        break;
    case Left:
        projectile = new class Projectile(getX(), getY() + 75, Direction::Left, type, Projectile::getStringProjType(type));
        break;
    case UpLeft:
        projectile = new class Projectile(getX(), getY(), Direction::UpLeft, type, Projectile::getStringProjType(type));
        break;
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
