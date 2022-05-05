#ifndef LIVING_H
#define LIVING_H

#include "entity.h"

class Living : public Entity
{
public:
    Living(double x, double y, CollisionBox* box, QImage* texture, std::string entityType, int health, int maxHealth, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name);
    Living(double x, double y, std::string facing, std::string name);
    ~Living();

    int getHealth() const;
    void setHealth(int newHealth);

    int getMaxHealth() const;
    void setMaxHealth(int newMaxHealth);

    CollisionBox *getGroundBox() const;
    void setGroundBox(CollisionBox *newGroundBox);

    bool getOnGround() const;
    void setOnGround(bool newOnGround);

    bool getInvulnerable() const;
    void setInvulnerable(bool newInvulnerable);

    int getDamage() const;
    void setDamage(int newDamage);

    std::string getState() const;
    void setState(std::string newState);

    std::string getLastFrameState() const;
    void setLastFrameState(std::string newLastFrameState);

private:
    int health;
    int maxHealth;
    int damage;
    bool invulnerable;

    CollisionBox *groundBox;
    bool onGround;
};

#endif // LIVING_H
