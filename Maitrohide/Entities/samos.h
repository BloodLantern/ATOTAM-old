#ifndef SAMOS_H
#define SAMOS_H

#include "living.h"
#include "projectile.h"

class Samos : public Living
{
public:
    Samos(double x, double y, int maxHealth, int maxGrenadeCount, int maxMissileCount);
    Samos(double x, double y, int maxHealth, int maxGrenadeCount, int maxMissileCount, CollisionBox* box, QImage* texture, std::string entityType, int health, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name, bool isMovable);
    ~Samos();

    void shoot(std::string type);

    bool getIsInAltForm() const;
    void setIsInAltForm(bool newIsInAltForm);

    int getGrenadeCount() const;
    void setGrenadeCount(int newGrenadeCount);

    int getMaxGrenadeCount() const;
    void setMaxGrenadeCount(int newMaxGrenadeCount);

    int getMissileCount() const;
    void setMissileCount(int newMissileCount);

    int getMaxMissileCount() const;
    void setMaxMissileCount(int newMaxMissileCount);

    double getJumpTime() const;
    void setJumpTime(double newJumpTime);

    CollisionBox *getWallBoxR() const;
    void setWallBoxR(CollisionBox *newWallBoxR);

    CollisionBox *getWallBoxL() const;
    void setWallBoxL(CollisionBox *newWallBoxL);

    const std::string &getCanonDirection() const;
    void setCanonDirection(const std::string &newCanonDirection);

private:
    bool isInAltForm;
    int grenadeCount;
    int maxGrenadeCount;
    int missileCount;
    int maxMissileCount;
    double jumpTime = 0.3;
    std::string canonDirection = "Right";
    CollisionBox* wallBoxR;
    CollisionBox* wallBoxL;
};

#endif // SAMOS_H
