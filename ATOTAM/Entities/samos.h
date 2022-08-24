#ifndef SAMOS_H
#define SAMOS_H

#include "living.h"
#include "projectile.h"

class Samos : public Living
{
public:
    Samos(double x, double y, int health, int maxHealth, int grenadeCount, int maxGrenadeCount, int missileCount, int maxMissileCount);
    Samos(double x, double y, int maxHealth, int maxGrenadeCount, int maxMissileCount, CollisionBox* box, QImage* texture, std::string entityType, int health, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name, bool isMovable);
    ~Samos();

    Projectile* shoot(std::string type);

    bool getIsInAltForm() const;
    void setIsInAltForm(bool newIsInAltForm);

    int getGrenadeCount();
    void setGrenadeCount(int newGrenadeCount);

    int getMaxGrenadeCount() const;
    void setMaxGrenadeCount(int newMaxGrenadeCount);

    int getMissileCount();
    void setMissileCount(int newMissileCount);

    int getMaxMissileCount() const;
    void setMaxMissileCount(int newMaxMissileCount);

    double getJumpTime() const;
    void setJumpTime(double newJumpTime);

    CollisionBox *getWallBoxR() const;
    void setWallBoxR(CollisionBox *newWallBoxR);
    void setWallBoxRNoDel(CollisionBox *newWallBoxR);

    CollisionBox *getWallBoxL() const;
    void setWallBoxL(CollisionBox *newWallBoxL);
    void setWallBoxLNoDel(CollisionBox *newWallBoxL);

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

    double getRetainTime() const;
    void setRetainTime(double newRetainTime);

    double getSpeedRetained() const;
    void setSpeedRetained(double newSpeedRetained);

    bool getFastFalling() const;
    void setFastFalling(bool newFastFalling);

    double getDashTime() const;
    void setDashTime(double newDashTime);

    const std::string &getDashDirection() const;
    void setDashDirection(const std::string &newDashDirection);

    const std::pair<double, double> &getSpeedPriorDash() const;
    void setSpeedPriorDash(const std::pair<double, double> &newSpeedPriorDash);

    double getDashCoolDown() const;
    void setDashCoolDown(double newDashCoolDown);

    const std::string &getDashCoolDownType() const;
    void setDashCoolDownType(const std::string &newDashCoolDownType);

private:
    bool isInAltForm = false;
    int grenadeCount = 0;
    int maxGrenadeCount = 1;
    int missileCount = 0;
    int maxMissileCount = 1;
    double jumpTime = 0.3;
    double shootTime = 0.0;
    double switchDelay = 0.0;
    double lagTime = 0.0;
    std::string canonDirection = "Right";
    std::string selectedWeapon = "Beam";
    CollisionBox* wallBoxR = nullptr;
    CollisionBox* wallBoxL = nullptr;
    double speedRetained = 0.0;
    double retainTime = 0.0;
    bool fastFalling = false;
    std::pair<double, double> speedPriorDash = {0.0, 0.0};
    double dashTime = 0.0;
    double dashCoolDown = 0.0;
    std::string dashCoolDownType = "Grounded";
    std::string dashDirection = "";
};

#endif // SAMOS_H
