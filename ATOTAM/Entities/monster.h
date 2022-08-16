#ifndef MONSTER_H
#define MONSTER_H

#include "living.h"

class Monster : public Living
{
public:
    Monster(double x, double y, std::string facing, std::string name);
    ~Monster();

    const std::string &getBehavior() const;
    void setBehavior(const std::string &newBehavior);

    int getDamage() const;
    void setDamage(int newDamage);

    float getAttackSpeed() const;
    void setAttackSpeed(float newAttackSpeed);

    double getAttackCooldown() const;
    void setAttackCooldown(double newAttackCooldown);

    bool getRanged() const;
    void setRanged(bool newRanged);

    bool getDamageOnContact() const;
    void setDamageOnContact(bool newDamageOnContact);

private:
    std::string behavior; // Behavior of the Monster
    int damage; // Damage per attack
    bool damageOnContact;
    float attackSpeed; // Number of attack per seconds
    double attackCooldown = 0; // Current cooldown left
    bool ranged; // Whether this Monster uses ranged attacks
};

#endif // MONSTER_H
