#ifndef SAMOS_H
#define SAMOS_H

#include "living.h"
#include "projectile.h"

class Samos : public Living
{
public:
    Samos(double x, double y, QImage* texture, int maxHealth, int maxGrenadeCount, int maxMissileCount);
    ~Samos();

    void shoot(Projectile::ProjectileType type);

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

private:
    bool isInAltForm;
    int grenadeCount;
    int maxGrenadeCount;
    int missileCount;
    int maxMissileCount;
    Direction canonDirection = Right;
};

#endif // SAMOS_H
