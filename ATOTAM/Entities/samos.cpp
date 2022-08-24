#include "samos.h"
#include <iostream>

Samos::Samos(double x, double y, int health, int maxHealth, int grenadeCount, int maxGrenadeCount, int missileCount, int maxMissileCount)
    : Living(x, y, "Right", "Samos"),
      isInAltForm(false), grenadeCount(grenadeCount), maxGrenadeCount(maxGrenadeCount), missileCount(missileCount), maxMissileCount(maxMissileCount),
      wallBoxR(new CollisionBox(getBox()->getX() + getBox()->getWidth(), getBox()->getY(), 1, getBox()->getHeight())),
      wallBoxL(new CollisionBox(getBox()->getX() - 1, getBox()->getY(), 1, getBox()->getHeight()))
{
    setMaxHealth(maxHealth);
    setHealth(health);
    setState("Standing");
}

Samos::Samos(double x, double y, int maxHealth, int maxGrenadeCount, int maxMissileCount, CollisionBox *box, QImage *texture, std::string entityType, int health, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name, bool isMovable)
    : Living(x, y, box, texture, entityType, health, maxHealth, isAffectedByGravity, facing, frictionFactor, name, isMovable),
      maxGrenadeCount(maxGrenadeCount),
      maxMissileCount(maxMissileCount),
      wallBoxR(new CollisionBox(box->getX() + box->getWidth(), box->getY(), 1, box->getHeight())),
      wallBoxL(new CollisionBox(box->getX() - 1, box->getY(), 1, box->getHeight()))
{
    setState("Standing");
}

Samos::~Samos()
{
    if (wallBoxL) {
        delete wallBoxL;
        wallBoxL = nullptr;
    }
    if (wallBoxR) {
        delete wallBoxR;
        wallBoxR = nullptr;
    }
}

Projectile* Samos::shoot(std::string type)
{
    Projectile* projectile = nullptr;

    //Can't shoot a missile/grenade if you don't have any
    if (type == "Grenade") {
        if (grenadeCount > 0)
            grenadeCount--;
        else
            return projectile;
    } else if (type == "Missile") {
        if (missileCount > 0)
            missileCount--;
        else
            return projectile;
    }


    std::string shootState;
    if (getState() == "Falling" || getState() == "FallingAimUpDiag" || getState() == "FallingAimDownDiag" || getState() == "FallingAimUp" || getState() == "FallingAimDown"
            || getState() == "SpinJump" || getState() == "WallJump" || getState() == "Jumping" || getState() == "JumpEnd")
        shootState = "Falling";
    else if (getState() == "IdleCrouch" || getState() == "CrouchAimUp" || getState() == "CrouchAimUpDiag" || getState() == "CrouchAimDownDiag" || getState() == "Crouching")
        shootState = "Crouching";
    else if (getState() == "Walking" || getState() == "WalkingAimForward" || getState() == "WalkingAimDown" || getState() == "WalkingAimUp"
             || getState() == "Running" || getState() == "RunningAimForward" || getState() == "RunningAimDown" || getState() == "RunningAimUp")
        shootState = "Walking";
    else if (getState() == "MorphBall")
        shootState = "Morph";
    else
        shootState = "Standing";

    if (canonDirection == "None")
        canonDirection = "Right";

    if (canonDirection == "Right" || canonDirection == "UpRight" || canonDirection == "DownRight")
        setFacing("Right");
    if (canonDirection == "Left" || canonDirection == "UpLeft" || canonDirection == "DownLeft")
        setFacing("Left");

    nlohmann::json offsetJson = values["names"]["Samos"]["shootOffset"][getFacing()][shootState][canonDirection];
    nlohmann::json pOffsetJson = values["names"][type];
    int offset_x = offsetJson.is_null() ? 0 : static_cast<int>(offsetJson["x"]);
    int offset_y = offsetJson.is_null() ? 0 : static_cast<int>(offsetJson["y"]);

    if (isInAltForm) {
        offset_x -= static_cast<int>(pOffsetJson["width"]) / 2;
        offset_y -= static_cast<int>(pOffsetJson["height"]) / 2;
    } else if (canonDirection == "Up") {
        offset_x -= static_cast<int>(pOffsetJson["width"]) / 2;
        offset_y -= static_cast<int>(pOffsetJson["height"]);
    } else if (canonDirection == "UpRight") {
        offset_y -= static_cast<int>(pOffsetJson["height"]);
    } else if (canonDirection == "Right") {
        offset_y -= static_cast<int>(pOffsetJson["height"]) / 2;
    } else if (canonDirection == "Down") {
        offset_x -= static_cast<int>(pOffsetJson["width"]) / 2;
    } else if (canonDirection == "DownLeft") {
        offset_x -= static_cast<int>(pOffsetJson["width"]);
    } else if (canonDirection == "Left") {
        offset_x -= static_cast<int>(pOffsetJson["width"]);
        offset_y -= static_cast<int>(pOffsetJson["height"]) / 2;
    } else if (canonDirection == "UpLeft") {
        offset_x -= static_cast<int>(pOffsetJson["width"]);
        offset_y -= static_cast<int>(pOffsetJson["height"]);
    }

    //Spawn the projectile at certain coordinates to match the sprite
    projectile = new Projectile(getX() + offset_x, getY() + offset_y, canonDirection, type, type, getEntType());

    if (projectile->getVX() != 0.0 && (projectile->getVX() * getVX() > 0.0))
        projectile->setVX(projectile->getVX() + getVX());
    if (projectile->getVY() != 0.0 && (projectile->getVY() * getVY() > 0.0))
        projectile->setVY(projectile->getVY() + getVY());

    return projectile;
}

bool Samos::getIsInAltForm() const
{
    return isInAltForm;
}

void Samos::setIsInAltForm(bool newIsInAltForm)
{
    isInAltForm = newIsInAltForm;
}

int Samos::getGrenadeCount()
{
    return grenadeCount;
}

void Samos::setGrenadeCount(int newGrenadeCount)
{
    grenadeCount = newGrenadeCount;
}

int Samos::getMaxGrenadeCount() const
{
    return maxGrenadeCount;
}

void Samos::setMaxGrenadeCount(int newMaxGrenadeCount)
{
    maxGrenadeCount = newMaxGrenadeCount;
}

int Samos::getMissileCount()
{
    return missileCount;
}

void Samos::setMissileCount(int newMissileCount)
{
    missileCount = newMissileCount;
}

int Samos::getMaxMissileCount() const
{
    return maxMissileCount;
}

void Samos::setMaxMissileCount(int newMaxMissileCount)
{
    maxMissileCount = newMaxMissileCount;
}

double Samos::getJumpTime() const
{
    return jumpTime;
}

void Samos::setJumpTime(double newJumpTime)
{
    jumpTime = newJumpTime;
}

CollisionBox *Samos::getWallBoxR() const
{
    return wallBoxR;
}

void Samos::setWallBoxR(CollisionBox *newWallBoxR)
{
    delete wallBoxR;
    wallBoxR = newWallBoxR;
}

void Samos::setWallBoxRNoDel(CollisionBox *newWallBoxR)
{
    wallBoxR = newWallBoxR;
}

CollisionBox *Samos::getWallBoxL() const
{
    return wallBoxL;
}

void Samos::setWallBoxL(CollisionBox *newWallBoxL)
{
    delete wallBoxL;
    wallBoxL = newWallBoxL;
}

void Samos::setWallBoxLNoDel(CollisionBox *newWallBoxL)
{
    wallBoxL = newWallBoxL;
}

const std::string &Samos::getCanonDirection() const
{
    return canonDirection;
}

void Samos::setCanonDirection(const std::string &newCanonDirection)
{
    canonDirection = newCanonDirection;
}

double Samos::getShootTime() const
{
    return shootTime;
}

void Samos::setShootTime(double newShootTime)
{
    shootTime = newShootTime;
}

const std::string &Samos::getSelectedWeapon() const
{
    return selectedWeapon;
}

void Samos::setSelectedWeapon(const std::string &newSelectedWeapon)
{
    selectedWeapon = newSelectedWeapon;
}

void Samos::nextWeapon()
{
    if (selectedWeapon == "Beam")
        selectedWeapon = "Missile";
    else
        selectedWeapon = "Beam";
}

double Samos::getSwitchDelay() const
{
    return switchDelay;
}

void Samos::setSwitchDelay(double newSwitchDelay)
{
    switchDelay = newSwitchDelay;
}

double Samos::getLagTime() const
{
    return lagTime;
}

void Samos::setLagTime(double newLagTime)
{
    lagTime = newLagTime;
}

double Samos::getRetainTime() const
{
    return retainTime;
}

void Samos::setRetainTime(double newRetainTime)
{
    retainTime = newRetainTime;
}

double Samos::getSpeedRetained() const
{
    return speedRetained;
}

void Samos::setSpeedRetained(double newSpeedRetained)
{
    speedRetained = newSpeedRetained;
}

bool Samos::getFastFalling() const
{
    return fastFalling;
}

void Samos::setFastFalling(bool newFastFalling)
{
    fastFalling = newFastFalling;
}

double Samos::getDashTime() const
{
    return dashTime;
}

void Samos::setDashTime(double newDashTime)
{
    dashTime = newDashTime;
}

const std::string &Samos::getDashDirection() const
{
    return dashDirection;
}

void Samos::setDashDirection(const std::string &newDashDirection)
{
    dashDirection = newDashDirection;
}

const std::pair<double, double> &Samos::getSpeedPriorDash() const
{
    return speedPriorDash;
}

void Samos::setSpeedPriorDash(const std::pair<double, double> &newSpeedPriorDash)
{
    speedPriorDash = newSpeedPriorDash;
}

double Samos::getDashCoolDown() const
{
    return dashCoolDown;
}

void Samos::setDashCoolDown(double newDashCoolDown)
{
    dashCoolDown = newDashCoolDown;
}

const std::string &Samos::getDashCoolDownType() const
{
    return dashCoolDownType;
}

void Samos::setDashCoolDownType(const std::string &newDashCoolDownType)
{
    dashCoolDownType = newDashCoolDownType;
}
