#include "entity.h"
#include <iostream>

bool Entity::checkCollision(Entity *obj1, Entity *obj2)
{
    return ((obj1->x + obj1->box->getX() + obj1->box->getWidth() > obj2->x + obj2->box->getX())
            && (obj1->x + obj1->box->getX() < obj2->x + obj2->box->getX()  + obj2->box->getWidth())
            && (obj1->y) + obj1->box->getY() + obj1->box->getHeight() > obj2->y + obj2->box->getY())
            && (obj1->y + obj1->box->getY() < obj2->y + obj2->box->getY()  + obj2->box->getHeight());
}

void Entity::handleCollision(Entity *obj1, Entity *obj2)
{//{Null, Terrain, Samos, Monster, Area, DynamicObj, NPC, Projectile};
    if (obj1->entType == "Terrain") {
        if (obj2->entType == "Samos") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "Monster") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "Area") {
            // TODO
        } else if (obj2->entType == "DynamicObj") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "NPC") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "Projectile") {
            // TODO
        }
    } else if (obj1->entType == "Samos") {
        if (obj2->entType == "Terrain") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "Monster") {
            calcCollisionReplacement(obj1, obj2);
            // TODO hit
        } else if (obj2->entType == "Area") {
            // TODO
        } else if (obj2->entType == "DynamicObj") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "NPC") {
            // TODO
        } else if (obj2->entType == "Projectile") {
            // TODO
        }
    } else if (obj1->entType == "Monster") {
        if (obj2->entType == "Terrain") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "Samos") {
            calcCollisionReplacement(obj1, obj2);
            // TODO hit
        } else if (obj2->entType == "Monster") {
            // TODO
        } else if (obj2->entType == "Area") {
            // TODO
        } else if (obj2->entType == "DynamicObj") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "NPC") {
            // TODO
        } else if (obj2->entType == "Projectile") {
            // TODO
        }
    } else if (obj1->entType == "Area") {
        if (obj2->entType == "Terrain") {
            // TODO
        } else if (obj2->entType == "Samos") {
            // TODO
        } else if (obj2->entType == "Monster") {
            // TODO
        } else if (obj2->entType == "Area") {
            // TODO
        } else if (obj2->entType == "DynamicObj") {
            // TODO
        } else if (obj2->entType == "NPC") {
            // TODO
        } else if (obj2->entType == "Projectile") {
            // TODO
        }
    } else if (obj1->entType == "DynamicObj") {
        if (obj2->entType == "Terrain") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "Samos") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "Monster") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "Area") {
            // TODO
        } else if (obj2->entType == "DynamicObj") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "NPC") {
            // TODO
        } else if (obj2->entType == "Projectile") {
            // TODO
        }
    } else if (obj1->entType == "NPC") {
        if (obj2->entType == "Terrain") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "Samos") {
            // TODO
        } else if (obj2->entType == "Monster") {
            // TODO
        } else if (obj2->entType == "Area") {
            // TODO
        } else if (obj2->entType == "DynamicObj") {
            // TODO
        } else if (obj2->entType == "NPC") {
            // TODO
        } else if (obj2->entType == "Projectile") {
            // TODO
        }
    } else if (obj1->entType == "Projectile") {
        if (obj2->entType == "Terrain") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "Samos") {
            calcCollisionReplacement(obj1, obj2);
            // TODO hit
        } else if (obj2->entType == "Monster") {
            calcCollisionReplacement(obj1, obj2);
            // TODO hit
        } else if (obj2->entType == "Area") {
            // TODO
        } else if (obj2->entType == "DynamicObj") {
            calcCollisionReplacement(obj1, obj2);
        } else if (obj2->entType == "NPC") {
            // TODO
        } else if (obj2->entType == "Projectile") {
            // TODO
        }
    } else
        throw unknownEntityType;
}

void Entity::calcCollisionReplacement(Entity *obj1, Entity *obj2)
{
    double minX1 = obj1->x + obj1->box->getWidth() - obj2->x;
    double minX2 = obj2->x + obj2->box->getWidth() - obj1->x;
    double minY1 = obj1->y + obj1->box->getHeight() - obj2->y;
    double minY2 = obj2->y + obj2->box->getHeight() - obj1->y;
    double minX;
    double minY;
    if (std::abs(minX1) < std::abs(minX2)) {
        minX = minX1;
    } else {
        minX = minX2;
    }
    if (std::abs(minY1) < std::abs(minY2)) {
        minY = minY1;
    } else {
        minY = minY2;
    }

    if (obj1->x > obj2->x) minX *= -1;
    if (obj1->y > obj2->y) minY *= -1;

    if (obj1->isMovable && obj2->isMovable) {
        if (std::abs(minX) < std::abs(minY)) {
            obj1->x -= minX / 2;
            obj2->x += minX / 2;
            obj1->vX = 0;
            obj2->vX = 0;
        } else {
            obj1->y -= minY / 2;
            obj2->y += minY / 2;
            obj1->vY = 0;
            obj2->vY = 0;
        }
    } else if (!obj1->isMovable && obj2->isMovable) {
        if (std::abs(minX) < std::abs(minY)) {
            obj2->x += minX;
            obj2->vX = 0;
        } else {
            obj2->y += minY;
            obj2->vY = 0;
        }
    } else if (obj1->isMovable && !obj2->isMovable) {
        if (std::abs(minX) < std::abs(minY)) {
            obj1->x -= minX;
            obj1->vX = 0;
        } else {
            obj1->y -= minY;
            obj1->vY = 0;
        }
    }
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
    if (std::abs(vX) < 25) vX = 0;
}

Entity::Entity(double x, double y, CollisionBox* box, QImage* texture, std::string entType, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name, bool isMovable)
    : box(box), texture(texture), x(x), y(y), entType(entType), isAffectedByGravity(isAffectedByGravity), facing(facing), frictionFactor(frictionFactor), isMovable(isMovable), name(name)
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
    isMovable = entJson["movable"];
}

Entity::~Entity()
{
    delete box;
}

void Entity::updateTexture()
{
    if (animation >= currentAnimation.size())
        animation = 0;

    texture = &currentAnimation[animation];
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
    // If the animation is multi-directional the program should crop the irrelevant part
    if (!variantJson["multi-directional"].is_null())
        if (variantJson["multi-directional"]) {
            if (facing == "Left") {
                fullAnim = fullAnim.copy(0, 0, static_cast<int>(variantJson["width"]) / 2, static_cast<int>(variantJson["height"]));
            } else if (facing == "Right") {
                fullAnim = fullAnim.copy(static_cast<int>(variantJson["width"]) / 2, 0,
                        static_cast<int>(variantJson["width"]), static_cast<int>(variantJson["height"]));
            }
        }

    int imagesPerLine = (static_cast<int>(variantJson["count"]) + static_cast<int>(variantJson["emptyFrames"])) / static_cast<int>(variantJson["lines"]);
    int width = static_cast<int>(variantJson["width"]) / imagesPerLine;
    // Make sure not to get two images at a time
    if (!variantJson["multi-directional"].is_null())
        if (variantJson["multi-directional"])
            width /= 2;
    int height = static_cast<int>(variantJson["height"]) / static_cast<int>(variantJson["lines"]);
    // For each line
    for (int i = (variantJson["reversed"] ? static_cast<int>(variantJson["lines"]) : 0);
         (variantJson["reversed"] ? i > -1 : i < variantJson["lines"]);
         i++) {
        // For each image
        if (facing == "Right")
            for (int ii = 0;
                 ii < (i + 1 == variantJson["lines"] ? /*Remove empty frames from the last line*/ imagesPerLine - static_cast<int>(variantJson["emptyFrames"]) : imagesPerLine); ii++) {
                currentAnimation.push_back(fullAnim.copy(ii * width, i * height, width, height));
            }
        else if (facing == "Left")
            for (int ii = imagesPerLine - 1;
                 ii > (i + 1 == variantJson["lines"] ? /*Remove empty frames from the last line*/ static_cast<int>(variantJson["emptyFrames"]) - 1 : -1);ii--) {
                currentAnimation.push_back(fullAnim.copy(ii * width, i * height, width, height));
            }
    }
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

const std::vector<QImage> &Entity::getCurrentAnimation() const
{
    return currentAnimation;
}

void Entity::setCurrentAnimation(const std::vector<QImage> &newCurrentAnimation)
{
    currentAnimation = newCurrentAnimation;
}

bool Entity::getIsMovable() const
{
    return isMovable;
}

void Entity::setIsMovable(bool newIsMovable)
{
    isMovable = newIsMovable;
}


