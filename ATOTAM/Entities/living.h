#ifndef LIVING_H
#define LIVING_H

#include "entity.h"

class Living : public Entity
{
public:
    //enum State {Idle, Walking, Attacking, Crouching, Jumping, MorphBall};

    Living(double x, double y, CollisionBox* box, QImage* texture, std::string entityType, int health, int maxHealth, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name, bool isMovable);
    Living(double x, double y, std::string facing, std::string name);
    ~Living();

    bool hit(int damage, Entity *origin = nullptr, double kb = 0, bool forced = false);

    int getHealth();
    void setHealth(int newHealth);

    int getMaxHealth() const;
    void setMaxHealth(int newMaxHealth);

    CollisionBox *getGroundBox() const;
    void setGroundBox(CollisionBox *newGroundBox);
    void setGroundBoxNoDel(CollisionBox *newGroundBox);

    bool getOnGround() const;
    void setOnGround(bool newOnGround);

    bool getInvulnerable() const;
    void setInvulnerable(bool newInvulnerable);

    double getITime() const;
    void setITime(double newITime);

    Entity *getStandingOn() const;
    void setStandingOn(Entity *newStandingOn);

private:
    int health = 0;
    int maxHealth = 1;
    bool invulnerable = false;
    double iTime = 0.0;

    CollisionBox *groundBox = nullptr;
    bool onGround = false;

    Entity *standingOn = nullptr;
};

#endif // LIVING_H
