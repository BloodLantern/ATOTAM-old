#include "entity.h"

bool Entity::checkCollision(Entity *obj1, Entity *obj2)
{
    return ((obj1->getX() + obj1->getBox()->getX() + obj1->getBox()->getWidth() > obj2->getX() + obj2->getBox()->getX())
            && (obj1->getX() + obj1->getBox()->getX() < obj2->getX() + obj2->getBox()->getX()  + obj2->getBox()->getWidth())
            && (obj1->getY() + obj1->getBox()->getY() + obj1->getBox()->getHeight() > obj2->getY() + obj2->getBox()->getY())
            && (obj1->getY() + obj1->getBox()->getY() < obj2->getY() + obj2->getBox()->getY()  + obj2->getBox()->getHeight()));
}

void Entity::updateV(double framerate)
{
    x += vX/framerate;
    y += vY/framerate;
}

Entity::Entity(double x, double y, CollisionBox* box, QImage* texture, int entType) : box(box), texture(texture), x(x), y(y), entType(entType)
{

}

Entity::~Entity()
{
    delete box;
    delete texture;
}

CollisionBox *Entity::getBox() const
{
    return box;
}

void Entity::setBox(CollisionBox *newBox)
{
    box = newBox;
}

double Entity::getX() const
{
    return x;
}

void Entity::setX(double newX)
{
    x = newX;
}

double Entity::getY() const
{
    return y;
}

void Entity::setY(double newY)
{
    y = newY;
}

QImage *Entity::getTexture() const
{
    return texture;
}

void Entity::setTexture(QImage *newTexture)
{
    texture = newTexture;
}

double Entity::getVX() const
{
    return vX;
}

void Entity::setVX(double newVX)
{
    vX = newVX;
}

double Entity::getVY() const
{
    return vY;
}

void Entity::setVY(double newVY)
{
    vY = newVY;
}

Entity::EntityType Entity::getEntType() const
{
    return entType;
}


