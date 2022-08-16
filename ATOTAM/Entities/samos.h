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

    Projectile* shoot(std::string type);

    void kill();

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

    double getShootTime() const;
    void setShootTime(double newShootTime);

    const std::string &getSelectedWeapon() const;
    void setSelectedWeapon(const std::string &newSelectedWeapon);
    void nextWeapon();

    double getSwitchDelay() const;
    void setSwitchDelay(double newSwitchDelay);

    double getLagTime() const;
    void setLagTime(double newLagTime);

private:
    bool isInAltForm;
    int grenadeCount;
    int maxGrenadeCount;
    int missileCount;
    int maxMissileCount;
    double jumpTime = 0.3;
    double shootTime = 0.0;
    double switchDelay = 0.0;
    double lagTime = 0.0;
    std::string canonDirection = "Right";
    std::string selectedWeapon = "Beam";
    CollisionBox* wallBoxR;
    CollisionBox* wallBoxL;
};

#endif // SAMOS_H
