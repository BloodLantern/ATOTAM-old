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
    enum EntityType {Null, Terrain, Samos, Monster, Area, DynamicObj, NPC, Projectile};
    static EntityType getEntTypeFromString(std::string type);
    enum Direction {None, Up, UpRight, Right, DownRight, Down, DownLeft, Left, UpLeft};
    static const int invalidDirection = -2;
    static nlohmann::json names;
    static void loadNames();
    void updateV(double framerate);

    Entity(double x, double y, CollisionBox* box, QImage* texture, EntityType entType, bool isAffectedByGravity, Direction facing, bool isAffectedByFriction, std::string name);
    Entity(double x, double y, Direction facing, std::string name);
    ~Entity();

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

    Entity::EntityType getEntType() const;

    bool getIsAffectedByGravity() const;
    void setIsAffectedByGravity(bool newIsAffectedByGravity);

    Direction getFacing() const;
    void setFacing(Direction newFacing);

    bool getIsAffectedByFriction() const;
    void setIsAffectedByFriction(bool newIsAffectedByFriction);

    const std::string &getName() const;
    void setName(const std::string &newName);

    void setEntType(EntityType newEntType);

private:
    CollisionBox* box;
    QImage* texture;
    double x; //in px
    double y; //in px
    double vX; //in px/s
    double vY; //in px/s
    EntityType entType;
    bool isAffectedByGravity;
    Direction facing;
    bool isAffectedByFriction;
    std::string name;
};

#endif // ENTITY_H
