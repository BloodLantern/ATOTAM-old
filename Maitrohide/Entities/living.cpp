#include "living.h"

Living::Living(double x, double y, CollisionBox* box, QImage* texture, std::string entityType, int health, int maxHealth, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name)
    : Entity(x, y, box, texture, entityType, isAffectedByGravity, facing, frictionFactor, name),
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

Living::Living(Entity entity)
    : Entity(entity.getX(), entity.getY(), entity.getBox(), entity.getTexture(), entity.getEntType(), entity.getIsAffectedByGravity(), entity.getFacing(), entity.getFrictionFactor(), entity.getName())
{
    nlohmann::json livJson = values["livings"][entity.getName()];
    if (livJson.is_null())
        maxHealth = -1; // Check for this value to know if this Living is null
    else {
        health = livJson["maxHealth"];
        maxHealth = livJson["maxHealth"];
        damage = livJson["damage"];
        invulnerable = livJson["invulnerable"];
    }
}

Living::~Living()
{
    delete groundBox;
}

void Living::updateAnimation()
{

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

std::string Living::getState() const
{
    return state;
}

void Living::setState(std::string newState)
{
    state = newState;
}

std::string Living::getLastFrameState() const
{
    return lastFrameState;
}

void Living::setLastFrameState(std::string newLastFrameState)
{
    lastFrameState = newLastFrameState;
}
