#include "living.h"

std::string Living::getStringState(State state)
{
    switch (state) {
    case Idle:
        return "Idle";
    case Walking:
        return "Walking";
    case Attacking:
        return "Attacking";
    case Crouching:
        return "Crouching";
    case Jumping:
        return "Jumping";
    case MorphBall:
        return "MorphBall";
    default:
        return getStringState(State::Idle);
    }
}

Living::Living(double x, double y, CollisionBox* box, QImage* texture, Entity::EntityType entityType, int health, int maxHealth, bool isAffectedByGravity, Direction facing, std::string name)
    : Entity(x, y, box, texture, entityType, isAffectedByGravity, facing, true, name),
      health(health), maxHealth(maxHealth), groundBox(new CollisionBox(box->getX(), box->getY() + box->getHeight(), box->getWidth(), 2)), onGround(false)
{

}

Living::Living(double x, double y, Direction facing, std::string name)
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

State Living::getState() const
{
    return state;
}

void Living::setState(State newState)
{
    state = newState;
}
