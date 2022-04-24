#ifndef ENTITY_H
#define ENTITY_H

#include "collisionbox.h"
#include <QImage>

class Entity
{
public:
    static bool checkCollision(Entity* obj1, Entity* obj2);
    enum EntityType {Terrain, Samos, Monster, Area, DynamicObj, NPC, Projectile};
    enum Direction {None, Up, UpRight, Right, DownRight, Down, DownLeft, Left, UpLeft};
    static const int invalidDirection = -2;
    void updateV(double framerate);

    Entity(double x, double y, CollisionBox* box, QImage* texture, EntityType entType, bool isAffectedByGravity, Direction facing, bool isAffectedByFriction);
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

private:
    CollisionBox* box;
    QImage* texture;
    double x; //in px
    double y; //in px
    double vX; //in px/s
    double vY; //in px/s
    const EntityType entType;
    bool isAffectedByGravity;
    Direction facing;
    bool isAffectedByFriction;
};

#endif // ENTITY_H
