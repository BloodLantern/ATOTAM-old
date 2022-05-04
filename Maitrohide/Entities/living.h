#ifndef LIVING_H
#define LIVING_H

#include "entity.h"

class Living : public Entity
{
public:
    Living(double x, double y, CollisionBox* box, QImage* texture, Entity::EntityType entityType, int health, int maxHealth, bool isAffectedByGravity, Entity::Direction facing, std::string name);
    Living(double x, double y, Direction facing, std::string name);
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

private:
    int health;
    int maxHealth;
    int damage;
    bool invulnerable;

    CollisionBox *groundBox;
    bool onGround;
};

#endif // LIVING_H
