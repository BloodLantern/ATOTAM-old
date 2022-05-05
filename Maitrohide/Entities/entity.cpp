#include "entity.h"
#include <iostream>

bool Entity::checkCollision(Entity *obj1, Entity *obj2)
{
    return ((obj1->getX() + obj1->getBox()->getX() + obj1->getBox()->getWidth() > obj2->getX() + obj2->getBox()->getX())
            && (obj1->getX() + obj1->getBox()->getX() < obj2->getX() + obj2->getBox()->getX()  + obj2->getBox()->getWidth())
            && (obj1->getY() + obj1->getBox()->getY() + obj1->getBox()->getHeight() > obj2->getY() + obj2->getBox()->getY())
            && (obj1->getY() + obj1->getBox()->getY() < obj2->getY() + obj2->getBox()->getY()  + obj2->getBox()->getHeight()));
}

nlohmann::json Entity::loadNames()
{
    std::ifstream names_file("../assets/entities.json");
    nlohmann::json temp;
    names_file >> temp;
    return temp;
}

nlohmann::json Entity::values = Entity::loadNames();

void Entity::updateV(double framerate)
{
    x += vX/framerate;
    y += vY/framerate;
}

Entity::Entity(double x, double y, CollisionBox* box, QImage* texture, std::string entType, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name)
    : box(box), texture(texture), x(x), y(y), entType(entType), isAffectedByGravity(isAffectedByGravity), facing(facing), frictionFactor(frictionFactor), name(name)
{

}

Entity::Entity(double x, double y, std::string facing, std::string name)
    : x(x), y(y), facing(facing)
{
    nlohmann::json entJson = values["names"][name];
    nlohmann::json textureJson = values["textures"][entJson["texture"]];
    nlohmann::json variantJson = textureJson["variants"]["standing"];

    QImage fullImage(QString::fromStdString(std::string("../assets/textures/") + std::string(textureJson["file"])));
    QImage image = fullImage.copy(variantJson["x"], variantJson["y"], variantJson["width"], variantJson["height"]);

    texture = &image;
    entType = entJson["type"];
    isAffectedByGravity = entJson["gravity"];
    frictionFactor = entJson["friction"];
}

Entity::~Entity()
{
    delete box;
}

void Entity::updateTexture()
{
    if (animation >= currentAnimation.size())
        animation = 0;

    texture = currentAnimation[animation];
}

void Entity::updateAnimation()
{
    // First clear the current image list
    currentAnimation.clear();

    // Getting a json object representing the animation
    nlohmann::json variantJson = values["textures"][values["names"][name]["texture"]][state];
    // Getting the full animation image which will be modified afterwards
    QImage fullAnim = QImage(QString::fromStdString(std::string("../assets/textures/") + std::string(variantJson["file"])))
            .copy(variantJson["x"], variantJson["y"], variantJson["width"], variantJson["height"]);
    // If the animation is multi-directional the program should keep the relevant part
    if (!variantJson["multi-directional"].is_null())
        if (variantJson["multi-directional"]) {
            if (facing == "Left") {
                fullAnim = fullAnim.copy(0, 0, static_cast<int>(variantJson["width"]) / 2, static_cast<int>(variantJson["height"]) / 2);
            } else if (facing == "Right") {
                fullAnim = fullAnim.copy(static_cast<int>(variantJson["width"]) / 2, static_cast<int>(variantJson["height"]) / 2, static_cast<int>(variantJson["width"]), static_cast<int>(variantJson["height"]));
            }
        }

    // Finish by starting the new animation at the beginning
    animation = 0;
    updateTexture();
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

std::string Entity::getEntType() const
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

std::string Entity::getFacing() const
{
    return facing;
}

void Entity::setFacing(std::string newFacing)
{
    facing = newFacing;
}

double Entity::getFrictionFactor() const
{
    return frictionFactor;
}

void Entity::setFrictionFactor(double newFrictionFactor)
{
    frictionFactor = newFrictionFactor;
}

const std::string &Entity::getName() const
{
    return name;
}

void Entity::setName(const std::string &newName)
{
    name = newName;
}

void Entity::setEntType(std::string newEntType)
{
    entType = newEntType;
}

unsigned int Entity::getAnimation() const
{
    return animation;
}

void Entity::setAnimation(unsigned int newAnimation)
{
    animation = newAnimation;
}

const std::string &Entity::getState() const
{
    return state;
}

void Entity::setState(const std::string &newState)
{
    state = newState;
}

const std::string &Entity::getLastFrameState() const
{
    return lastFrameState;
}

void Entity::setLastFrameState(const std::string &newLastFrameState)
{
    lastFrameState = newLastFrameState;
}

const std::vector<QImage *> &Entity::getCurrentAnimation() const
{
    return currentAnimation;
}

void Entity::setCurrentAnimation(const std::vector<QImage *> &newCurrentAnimation)
{
    currentAnimation = newCurrentAnimation;
}


