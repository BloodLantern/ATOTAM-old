#include "projectile.h"

Projectile::Projectile(double x, double y, Direction facing, ProjectileType type)
    : Entity(x, y, new CollisionBox(5, 5), nullptr, EntityType::Projectile, false, facing, false)
{
    if (type == Bomb)
        facing = None;

    switch (facing) {
    case None:
        setVX(0);
        setVY(0);
        break;
    case Up:
        setVX(0);
        setVY(-1000);
        break;
    case UpRight:
        setVX(707);
        setVY(-707);
        break;
    case Right:
        setVX(1000);
        setVY(0);
        break;
    case DownRight:
        setVX(707);
        setVY(707);
        break;
    case Down:
        setVX(0);
        setVY(1000);
        break;
    case DownLeft:
        setVX(-707);
        setVY(707);
        break;
    case Left:
        setVX(-1000);
        setVY(0);
        break;
    case UpLeft:
        setVX(-707);
        setVY(-707);
        break;
    }

    switch (type) {
    case Beam:
        damage = 20;
        lifeTime = 1500;
        break;
    case Missile:
        damage = 50;
        lifeTime = 3000;
        setVX(getVX() / 2);
        setVY(getVY() / 2);
        setIsAffectedByGravity(true);
        break;
    case Grenade:
        damage = 60;
        lifeTime = 3000;
        setVX(getVX() / 2);
        setVY(getVY() / 2);
        break;
    case Bomb:
        damage = 50;
        lifeTime = 2000;
        break;
    default:
        throw unknownProjectileType;
        break;
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