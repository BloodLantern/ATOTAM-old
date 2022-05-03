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

Entity::Entity(double x, double y, CollisionBox* box, QImage* texture, EntityType entType, bool isAffectedByGravity, Direction facing, bool isAffectedByFriction, std::string name)
    : box(box), texture(texture), x(x), y(y), entType(entType), isAffectedByGravity(isAffectedByGravity), facing(facing), isAffectedByFriction(isAffectedByFriction), name(name)
{

}

Entity::Entity(double x, double y, Direction facing, std::string name)
{

}

Entity::~Entity()
{
    delete box;
}

Json::Value Entity::loadNames()
{
    std::ifstream names_file("assets/entityNames.json", std::ifstream::binary);
    Json::Value reader;
    names_file >> reader;
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

bool Entity::getIsAffectedByGravity() const
{
    return isAffectedByGravity;
}

void Entity::setIsAffectedByGravity(bool newIsAffectedByGravity)
{
    isAffectedByGravity = newIsAffectedByGravity;
}

Direction Entity::getFacing() const
{
    return facing;
}

void Entity::setFacing(Direction newFacing)
{
    facing = newFacing;
}

bool Entity::getIsAffectedByFriction() const
{
    return isAffectedByFriction;
}

void Entity::setIsAffectedByFriction(bool newIsAffectedByFriction)
{
    isAffectedByFriction = newIsAffectedByFriction;
}

const std::string &Entity::getName() const
{
    return name;
}

void Entity::setName(const std::string &newName)
{
    name = newName;
}


