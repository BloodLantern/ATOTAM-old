#ifndef ENTITY_H
#define ENTITY_H

#include "collisionbox.h"
#include <QImage>
#include <QString>
#include <string>
#include "nlohmann/json.hpp"
#include <fstream>

class Entity
{
public:
    static bool checkCollision(Entity* obj1, Entity* obj2);
    //enum EntityType {Null, Terrain, Samos, Monster, Area, DynamicObj, NPC, Projectile};
    //static EntityType getEntTypeFromString(std::string type);
    //enum Direction {None, Up, UpRight, Right, DownRight, Down, DownLeft, Left, UpLeft};
    static const int invalidDirection = -2;
    static nlohmann::json values;
    static nlohmann::json loadNames();

    Entity(double x, double y, CollisionBox* box, QImage* texture, std::string entType, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name);
    Entity(double x, double y, std::string facing, std::string name);
    ~Entity();

    void updateTexture();
    void updateV(double framerate);

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

    int getAnimation() const;
    void setAnimation(int newAnimation);

    int getMaxAnimation() const;
    void setMaxAnimation(int newMaxAnimation);

private:
    CollisionBox* box;
    QImage* texture;
    double x; //in px
    double y; //in px
    double vX; //in px/s
    double vY; //in px/s
    std::string entType;
    bool isAffectedByGravity;
    std::string facing;
    double frictionFactor;
    std::string name;
    int animation = 0;
    int maxAnimation;
    std::vector<QImage*> currentAnimation;
};

#endif // ENTITY_H
