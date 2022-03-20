#ifndef ENTITY_H
#define ENTITY_H

#include "collisionbox.h"
#include <QImage>

class Entity
{
public:
    static bool checkCollision(Entity* obj1, Entity* obj2);
    enum EntityType {Terrain, Samos, Monster, Area, DynamicObj, NPC};
    void updateV(double framerate);

    Entity(double x, double y, CollisionBox* box, QImage* texture, int entType);
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

private:
    CollisionBox* box;
    QImage* texture;
    double x; //in px
    double y; //in px
    double vX; //in px/s
    double vY; //in px/s
    const EntityType entType;
};

#endif // ENTITY_H
