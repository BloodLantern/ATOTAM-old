#ifndef ENTITY_H
#define ENTITY_H

#include "collisionbox.h"
#include <QImage>
#include <QString>
#include <string>
#include "nlohmann/json.hpp"
#include <fstream>
#include <cmath>

class Entity
{
public:
    static bool checkCollision(Entity* obj1, CollisionBox* box1, Entity* obj2, CollisionBox* box2);
    static void calcCollisionReplacement(Entity* obj1, Entity* obj2);
    //enum EntityType {Null, Terrain, Samos, Monster, Area, DynamicObj, NPC, Projectile};
    static const int unknownEntityType = -1;
    static const int invalidDirection = -2;
    static nlohmann::json values;
    static nlohmann::json loadValues(std::string assetsPath);

    Entity(double x, double y, CollisionBox* box, QImage* texture, std::string entType, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name, bool isMovable);
    Entity(double x, double y, std::string facing, std::string name);
    ~Entity();

    void updateTexture();
    std::vector<QImage> updateAnimation(std::string state);
    void updateV(double framerate);
    void applyKnockback(Entity *e, double kBForce);
    void forceKnockback(Entity *e, double kBForce);

    CollisionBox *getBox() const;
    void setBox(CollisionBox *newBox);

    double getX() const;
    void setX(double newX);

    double getY() const;
    void setY(double newY);

    QImage *getTexture() const;
    void setTexture(QImage *newTexture);

    double getVX() const;
    void setVX(double newVX);

    double getVY() const;
    void setVY(double newVY);

    bool getIsAffectedByGravity() const;
    void setIsAffectedByGravity(bool newIsAffectedByGravity);

    std::string getFacing() const;
    void setFacing(std::string newFacing);

    double getFrictionFactor() const;
    void setFrictionFactor(double newFrictionFactor);

    const std::string &getName() const;
    void setName(const std::string &newName);

    std::string getEntType() const;
    void setEntType(std::string newEntType);

    unsigned int getFrame() const;
    void setFrame(unsigned int newFrame);

    const std::string &getState() const;
    void setState(const std::string &newState);

    const std::string &getLastFrameState() const;
    void setLastFrameState(const std::string &newLastFrameState);

    const std::vector<QImage> &getCurrentAnimation() const;
    void setCurrentAnimation(const std::vector<QImage> &newCurrentAnimation);

    bool getIsMovable() const;
    void setIsMovable(bool newIsMovable);

    const std::string &getLastFrameFacing() const;
    void setLastFrameFacing(const std::string &newLastFrameFacing);

    double getMass() const;
    void setMass(double newMass);

    int getRoomId() const;
    void setRoomId(int newRoomId);

private:
    CollisionBox* box = nullptr;
    QImage* texture = nullptr; // Image to be rendered now
    double x; //in px
    double y; //in px
    double vX = 0; //in px/s
    double vY = 0; //in px/s
    std::string entType;
    bool isAffectedByGravity;
    std::string facing;
    std::string lastFrameFacing;
    double frictionFactor;
    bool isMovable;
    std::string name;
    double mass = 100;

    // Rendering
    unsigned int frame = 0;
    std::vector<QImage> currentAnimation; // Full current animation
    std::string state = "None"; // Which animation should be rendered
    std::string lastFrameState = "None"; // Which animation was rendered in the last frame
    int roomId; // ID of the room in which this Entity is
};

#endif // ENTITY_H
