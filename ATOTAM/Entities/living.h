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

    void hit(int damage, Entity *origin = nullptr, double kb = 0, bool forced = false);
    virtual void kill();

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

    double getITime() const;
    void setITime(double newITime);

private:
    int health;
    int maxHealth;
    bool invulnerable;
    double iTime = 0.0;

    CollisionBox *groundBox;
    bool onGround;
};

#endif // LIVING_H
