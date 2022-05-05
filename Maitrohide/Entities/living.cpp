#include "living.h"

bool Living::checkOn(Living *obj, Entity *ground)
{
    return ((obj->getX() + obj->getGroundBox()->getX() + obj->getGroundBox()->getWidth() > ground->getX() + ground->getBox()->getX())
            && (obj->getX() + obj->getGroundBox()->getX() < ground->getX() + ground->getBox()->getX()  + ground->getBox()->getWidth())
            && (obj->getY()) + obj->getGroundBox()->getY() + obj->getGroundBox()->getHeight() > ground->getY() + ground->getBox()->getY())
            && (obj->getY() + obj->getGroundBox()->getY() < ground->getY() + ground->getBox()->getY()  + ground->getBox()->getHeight());
}

Living::Living(double x, double y, CollisionBox* box, QImage* texture, std::string entityType, int health, int maxHealth, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name, bool isMovable)
    : Entity(x, y, box, texture, entityType, isAffectedByGravity, facing, frictionFactor, name, isMovable),
      health(health), maxHealth(maxHealth), groundBox(new CollisionBox(box->getX(), box->getY() + box->getHeight(), box->getWidth(), 2)), onGround(false)
{

}

Living::Living(double x, double y, std::string facing, std::string name)
    : Entity(x, y, facing, name)
{
    nlohmann::json livJson = values["livings"][name];
    health = livJson["maxHealth"];
    maxHealth = livJson["maxHealth"];
    damage = livJson["damage"];
    invulnerable = livJson["invulnerable"];
}

Living::~Living()
{
    delete groundBox;
}

int Living::getHealth() const
{
    return health;
}

void Living::setHealth(int newHealth)
{
    health = newHealth;
}

int Living::getMaxHealth() const
{
    return maxHealth;
}

void Living::setMaxHealth(int newMaxHealth)
{
    maxHealth = newMaxHealth;
}

CollisionBox *Living::getGroundBox() const
{
    return groundBox;
}

void Living::setGroundBox(CollisionBox *newGroundBox)
{
    groundBox = newGroundBox;
}

bool Living::getOnGround() const
{
    return onGround;
}

void Living::setOnGround(bool newOnGround)
{
    onGround = newOnGround;
}

bool Living::getInvulnerable() const
{
    return invulnerable;
}

void Living::setInvulnerable(bool newInvulnerable)
{
    invulnerable = newInvulnerable;
}

int Living::getDamage() const
{
    return damage;
}

void Living::setDamage(int newDamage)
{
    damage = newDamage;
}
