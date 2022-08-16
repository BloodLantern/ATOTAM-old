#include "monster.h"

Monster::Monster(double x, double y, std::string facing, std::string name)
    : Living(x, y, facing, name)
{
    // Json initialization
    nlohmann::json monsterJson = values["names"][name];
    behavior = monsterJson["behavior"];
    damage = monsterJson["damage"];
    damageOnContact = monsterJson["damageOnContact"];
    attackSpeed = monsterJson["attackSpeed"];
    ranged = monsterJson["ranged"];
}

Monster::~Monster()
{

}

const std::string &Monster::getBehavior() const
{
    return behavior;
}

void Monster::setBehavior(const std::string &newBehavior)
{
    behavior = newBehavior;
}

int Monster::getDamage() const
{
    return damage;
}

void Monster::setDamage(int newDamage)
{
    damage = newDamage;
}

float Monster::getAttackSpeed() const
{
    return attackSpeed;
}

void Monster::setAttackSpeed(float newAttackSpeed)
{
    attackSpeed = newAttackSpeed;
}

double Monster::getAttackCooldown() const
{
    return attackCooldown;
}

void Monster::setAttackCooldown(double newAttackCooldown)
{
    attackCooldown = newAttackCooldown;
}

bool Monster::getRanged() const
{
    return ranged;
}

void Monster::setRanged(bool newRanged)
{
    ranged = newRanged;
}

bool Monster::getDamageOnContact() const
{
    return damageOnContact;
}

void Monster::setDamageOnContact(bool newDamageOnContact)
{
    damageOnContact = newDamageOnContact;
}
