#include "entity.h"
#include <QPainter>
#include <iostream>

unsigned long long Entity::lastID = 0;

bool Entity::checkCollision(Entity *obj1, CollisionBox *box1, Entity *obj2, CollisionBox *box2)
{
    if (box1 == nullptr || box2 == nullptr)
        return false;
    //Big check to see if two boxes overlap
    return ((obj1->x + box1->getX() + box1->getWidth() > obj2->x + box2->getX())
            && (obj1->x + box1->getX() < obj2->x + box2->getX()  + box2->getWidth())
            && (obj1->y) + box1->getY() + box1->getHeight() > obj2->y + box2->getY())
            && (obj1->y + box1->getY() < obj2->y + box2->getY()  + box2->getHeight());
}

void Entity::calcCollisionReplacement(Entity *obj1, Entity *obj2)
{
    //Calc the minimal distance needed to move two entities so that they don't overlap anymore, along both axis and both directions
    double minX1 = obj1->x + obj1->box->getWidth() + obj1->getBox()->getX() - obj2->x - obj2->getBox()->getX();
    double minX2 = obj2->x + obj2->box->getWidth() + obj2->getBox()->getX() - obj1->x - obj1->getBox()->getX();
    double minY1 = obj1->y + obj1->box->getHeight() + obj1->getBox()->getY() - obj2->y - obj2->getBox()->getY();
    double minY2 = obj2->y + obj2->box->getHeight() + obj2->getBox()->getY() - obj1->y - obj1->getBox()->getY();
    double minX;
    double minY;

    //Choose which direction is the optimal one (smaller distance)
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

    //Adjust values depending on the side of the collision
    if (obj1->x + obj1->getBox()->getX() + obj1->getBox()->getWidth() / 2 > obj2->x + obj2->getBox()->getX() + obj2->getBox()->getWidth() / 2) minX *= -1;
    if (obj1->y + obj1->getBox()->getY() + obj1->getBox()->getHeight() / 2 > obj2->y + obj2->getBox()->getY() + obj2->getBox()->getHeight() / 2) minY *= -1;

    //Decide which entity to move (depending on if one is not movable)
    if (obj1->isMovable && obj2->isMovable) {
        //Decide along which axis two move the entities (still smaller distance)
        if (std::abs(minX) < std::abs(minY)) {
            if ((obj1->getEntType() == "Projectile")
                    || (obj1->getEntType() == "Samos" && obj2->getEntType() != "Projectile")
                    || (obj1->getEntType() == "Monster" && obj2->getEntType() != "Projectile" && obj2->getEntType() != "Samos" && obj2->getEntType() != "Monster"))
                obj1->x -= minX;
            else if (obj2->getEntType() == "Projectile" || obj2->getEntType() == "Samos" || obj2->getEntType() == "Monster")
                obj2->x += minX;
            else {
                obj1->x -= minX / 2;
                obj2->x += minX / 2;
            }
            if (std::signbit(obj1->getVX()) == std::signbit(minX))
                obj1->vX = 0;
            if (std::signbit(obj2->getVX()) == std::signbit(-minX))
                obj2->vX = 0;
        } else {
            if ((obj1->getEntType() == "Projectile")
                    || (obj1->getEntType() == "Samos" && obj2->getEntType() != "Projectile")
                    || (obj1->getEntType() == "Monster" && obj2->getEntType() != "Projectile" && obj2->getEntType() != "Samos" && obj2->getEntType() != "Monster"))
                obj1->y -= minY;
            else if (obj2->getEntType() == "Projectile" || obj2->getEntType() == "Samos" || obj2->getEntType() == "Monster")
                obj2->y += minY;
            else {
                obj1->y -= minY / 2;
                obj2->y += minY / 2;
            }
            if (std::signbit(obj1->getVY()) == std::signbit(minY))
                obj1->vY = 0;
            if (std::signbit(obj2->getVY()) == std::signbit(-minY))
                obj2->vY = 0;
        }
    } else if (!obj1->isMovable && obj2->isMovable) {
        //Same thing
        if (std::abs(minX) < std::abs(minY)) {
            obj2->x += minX;
            if (std::signbit(obj2->getVX()) == std::signbit(-minX))
                obj2->vX = 0;
        } else {
            obj2->y += minY;
            if (std::signbit(obj2->getVY()) == std::signbit(-minY))
                obj2->vY = 0;
        }
    } else if (obj1->isMovable && !obj2->isMovable) {
        //Read previous comments bro
        if (std::abs(minX) < std::abs(minY)) {
            obj1->x -= minX;
            if (std::signbit(obj1->getVX()) == std::signbit(minX))
                obj1->vX = 0;
        } else {
            obj1->y -= minY;
            if (std::signbit(obj1->getVY()) == std::signbit(minY))
                obj1->vY = 0;
        }
    }
}

void Entity::calcCollisionReplacementAxis(Entity *obj1, Entity *obj2, bool alongY)
{
    if (alongY) {
        double minY1 = obj1->y + obj1->box->getHeight() + obj1->getBox()->getY() - obj2->y - obj2->getBox()->getY();
        double minY2 = obj2->y + obj2->box->getHeight() + obj2->getBox()->getY() - obj1->y - obj1->getBox()->getY();
        double minY;

        if (std::abs(minY1) < std::abs(minY2)) {
            minY = minY1;
        } else {
            minY = minY2;
        }

        if (obj1->y + obj1->getBox()->getY() > obj2->y + obj2->getBox()->getY()) minY *= -1;

        if (obj1->isMovable && obj2->isMovable) {
            if ((obj1->getEntType() == "Projectile")
                    || (obj1->getEntType() == "Samos" && obj2->getEntType() != "Projectile")
                    || (obj1->getEntType() == "Monster" && obj2->getEntType() != "Projectile" && obj2->getEntType() != "Samos" && obj2->getEntType() != "Monster"))
                obj1->y -= minY;
            else if (obj2->getEntType() == "Projectile" || obj2->getEntType() == "Samos" || obj2->getEntType() == "Monster")
                obj2->y += minY;
            else {
                obj1->y -= minY / 2;
                obj2->y += minY / 2;
            }
            if (std::signbit(obj1->getVY()) == std::signbit(minY))
                obj1->vY = 0;
            if (std::signbit(obj2->getVY()) == std::signbit(-minY))
                obj2->vY = 0;
        } else if (!obj1->isMovable && obj2->isMovable) {
            obj2->y += minY;
            if (std::signbit(obj2->getVY()) == std::signbit(-minY))
                obj2->vY = 0;
        } else if (obj1->isMovable && !obj2->isMovable) {
            obj1->y -= minY;
            if (std::signbit(obj1->getVY()) == std::signbit(minY))
                obj1->vY = 0;
        }
    } else {
        double minX1 = obj1->x + obj1->box->getWidth() + obj1->getBox()->getX() - obj2->x - obj2->getBox()->getX();
        double minX2 = obj2->x + obj2->box->getWidth() + obj2->getBox()->getX() - obj1->x - obj1->getBox()->getX();
        double minX;

        //Choose which direction is the optimal one (smaller distance)
        if (std::abs(minX1) < std::abs(minX2)) {
            minX = minX1;
        } else {
            minX = minX2;
        }

        //Adjust values depending on the side of the collision
        if (obj1->x + obj1->getBox()->getX() > obj2->x + obj2->getBox()->getX()) minX *= -1;

        //Decide which entity to move (depending on if one is not movable)
        if (obj1->isMovable && obj2->isMovable) {
            if ((obj1->getEntType() == "Projectile")
                    || (obj1->getEntType() == "Samos" && obj2->getEntType() != "Projectile")
                    || (obj1->getEntType() == "Monster" && obj2->getEntType() != "Projectile" && obj2->getEntType() != "Samos" && obj2->getEntType() != "Monster"))
                obj1->x -= minX;
            else if (obj2->getEntType() == "Projectile" || obj2->getEntType() == "Samos" || obj2->getEntType() == "Monster")
                obj2->x += minX;
            else {
                obj1->x -= minX / 2;
                obj2->x += minX / 2;
            }
            if (std::signbit(obj1->getVX()) == std::signbit(minX))
                obj1->vX = 0;
            if (std::signbit(obj2->getVX()) == std::signbit(-minX))
                obj2->vX = 0;
        } else if (!obj1->isMovable && obj2->isMovable) {
            obj2->x += minX;
            if (std::signbit(obj2->getVX()) == std::signbit(-minX))
                obj2->vX = 0;
        } else if (obj1->isMovable && !obj2->isMovable) {
            obj1->x -= minX;
            if (std::signbit(obj1->getVX()) == std::signbit(minX))
                obj1->vX = 0;

        }
    }
}

std::string assetsPath;

nlohmann::json Entity::loadValues(std::string assetsPath_)
{
    assetsPath = assetsPath_;
    //loading the json with the info about mobs
    std::ifstream names_file(assetsPath + "/entities.json");
    nlohmann::json temp;
    names_file >> temp;
    return temp;
}

nlohmann::json Entity::values;

void Entity::updateV(double framerate)
{
    //moving the entity
    x += vX/framerate;
    y += vY/framerate;
}

void Entity::applyKnockback(Entity *e, double kBForce)
{
    if (mass == -1)
        return;
    if (std::abs((x + box->getX() + (box->getWidth() / 2)) - (e->x + e->box->getX() + (e->box->getWidth() / 2))) >= 5)
        vX += (((x + box->getX() + (box->getWidth() / 2)) < (e->x + e->box->getX() + (e->box->getWidth() / 2))) ? -1 : 1) * 1000 * kBForce / mass;
    vY += (((y + box->getY() + (box->getHeight() / 2)) < (e->y + e->box->getY() + (e->box->getHeight() / 2))) ? -1 : 1) * 1000 * kBForce / mass;
}

void Entity::forceKnockback(Entity *e, double kBForce)
{
    if (mass == -1)
        return;
    vX = (((x + box->getX() + (box->getWidth() / 2)) < (e->x + e->box->getX() + (e->box->getWidth() / 2))) ? -1 : 1) * 1000 * kBForce / mass;
    vY = (((y + box->getY() + (box->getHeight() / 2)) < (e->y + e->box->getY() + (e->box->getHeight() / 2))) ? -1 : 1) * 1000 * kBForce / mass;
}

nlohmann::json Entity::getJsonRepresentation(bool defaultValues)
{
    nlohmann::json result;
    result["x"] = x; // !!! ABSOLUTE POSITION, NOT RELATIVE TO THE ROOM AS IN THE JSON
    result["y"] = y; // !!! ABSOLUTE POSITION, NOT RELATIVE TO THE ROOM AS IN THE JSON
    if (!defaultValues) {
        if (state != "None")
            result["state"] = state;
    } else
        result["state"] = state;
    if (!defaultValues) {
        if (facing != "None")
            result["facing"] = facing;
    } else
        result["facing"] = facing;
    if (!defaultValues) {
        if (horizontalRepeat != 1)
            result["horizontalRepeat"] = horizontalRepeat;
    } else
        result["horizontalRepeat"] = horizontalRepeat;
    if (!defaultValues) {
        if (verticalRepeat != 1)
            result["verticalRepeat"] = verticalRepeat;
    } else
        result["verticalRepeat"] = verticalRepeat;
    return result;
}

void Entity::setJsonValues(nlohmann::json json)
{
    if (!json["x"].is_null())
        x = json["x"];
    if (!json["y"].is_null())
        y = json["y"];
    if (!json["state"].is_null())
        state = json["state"];
    if (!json["facing"].is_null())
        facing = json["facing"];
    if (!json["horizontalRepeat"].is_null())
        horizontalRepeat = json["horizontalRepeat"];
    if (!json["verticalRepeat"].is_null())
        verticalRepeat = json["verticalRepeat"];
    if (!json["roomId"].is_null())
        roomId = json["roomId"];
}

Entity::Entity(double x, double y, CollisionBox* box, QImage* texture, std::string entType, bool isAffectedByGravity, std::string facing, double frictionFactor, std::string name, bool isMovable)
    : box(box), texture(texture), x(x), y(y), entType(entType), isAffectedByGravity(isAffectedByGravity), facing(facing), frictionFactor(frictionFactor), isMovable(isMovable), name(name), entityID(lastID++)
{

}

Entity::Entity(double x, double y, std::string facing, std::string name)
    : x(x), y(y), facing(facing), name(name), entityID(lastID++)
{
    //fast constructor using the json file
    nlohmann::json entJson = values["names"][name];

    entType = entJson["type"];
    state = entJson["defaultState"];
    lastFrameState = "None";
    isAffectedByGravity = entJson["gravity"];
    frictionFactor = entJson["friction"];
    isMovable = entJson["movable"];
    mass = entJson["mass"];
    box = new CollisionBox(entJson["offset_x"], entJson["offset_y"], entJson["width"], entJson["height"]);
    layer = entJson["layer"];
}

Entity::Entity(const Entity &entity)
    : Entity(entity.x, entity.y, entity.facing, entity.name)
{
    // Texture
    setState(entity.getState());
    setHorizontalRepeat(entity.getHorizontalRepeat());
    setVerticalRepeat(entity.getVerticalRepeat());
    setCurrentAnimation(updateAnimation());
    setFrame(0);
    updateTexture();
    // Box
    box->setWidth(box->getWidth() * getHorizontalRepeat());
    box->setHeight(box->getHeight() * getVerticalRepeat());
    roomId = entity.getRoomId();
}

Entity::Entity()
{

}

Entity::~Entity()
{
    if (box != nullptr) {
        delete box;
        box = nullptr;
    }
}

void Entity::updateTexture()
{
    if (currentAnimation.size() > 0)
        texture = &currentAnimation[frame];
}

std::vector<QImage> Entity::updateAnimation(std::string state, std::pair<int, int> repeat)
{
    // First create the image list which will be returned
    // The first dimension is for the horizontal repetitions
    // The second one is for the vertical repetitions
    // The third one is for the animation
    std::vector<std::vector<std::vector<QImage>>>* anim = new std::vector<std::vector<std::vector<QImage>>>();

    unsigned int horizontalRepeat = repeat.first;
    unsigned int verticalRepeat = repeat.second;
    // Initialize vectors size
    for (unsigned int i = 0; i < horizontalRepeat; i++) {
        (*anim).push_back(std::vector<std::vector<QImage>>());
        for (unsigned int i2 = 0; i2 < verticalRepeat; i2++) {
            (*anim)[i].push_back(std::vector<QImage>());
        }
    }

    nlohmann::json animJson;
    for (unsigned int hRepeat = 0; hRepeat < horizontalRepeat; hRepeat++) {
        for (unsigned int vRepeat = 0; vRepeat < verticalRepeat; vRepeat++) {
            // Getting a json object representing the animation
            if (!values["names"][name]["randomTexture"].is_null() && state == "None") {
                std::string newState = values["names"][name]["randomTexture"]
                        [((double) rand() / (RAND_MAX+1)) * (values["names"][name]["randomTexture"].size() - 1 + 1) + 0]; // Generates a random number in range [0; values["names"][name]["randomTexture"].size()]
                animJson = values["textures"][values["names"][name]["texture"]][newState];
            } else
                animJson = values["textures"][values["names"][name]["texture"]][state];

            // Setting default Json values
            for (const std::pair<std::string, nlohmann::json> value
                 : values["general"]["defaultAnimationValues"].get<nlohmann::json::object_t>())
                if (animJson[value.first].is_null())
                    animJson[value.first] = value.second;

            // Getting the full animation image which will be cropped afterwards
            QImage fullAnim = QImage(QString::fromStdString(std::string(assetsPath + "/textures/") + std::string(animJson["file"])))
                    .copy(animJson["x"], animJson["y"], animJson["width"], animJson["height"]);
            // If the animation is multi-directional the program shouldn't keep the irrelevant part
            if (animJson["multi-directional"]) {
                if (facing == "Left" || facing == "UpLeft" || facing == "DownLeft") {
                    fullAnim = fullAnim.copy(0, 0, static_cast<int>(animJson["width"]) / 2, static_cast<int>(animJson["height"]));
                } else if (facing == "Right" || facing == "UpRight" || facing == "DownRight") {
                    fullAnim = fullAnim.copy(static_cast<int>(animJson["width"]) / 2, 0,
                            static_cast<int>(animJson["width"]), static_cast<int>(animJson["height"]));
                }
            }

            int imagesPerLine = (static_cast<int>(animJson["count"]) + static_cast<int>(animJson["emptyFrames"])) / static_cast<int>(animJson["lines"]);
            int width = static_cast<int>(animJson["width"]) / imagesPerLine;
            // Make sure not to get two images at a time
            if (animJson["multi-directional"])
                width /= 2;
            int height = static_cast<int>(animJson["height"]) / static_cast<int>(animJson["lines"]);
            // For each line
            for (int i = (animJson["reversed"] ? static_cast<int>(animJson["lines"]) - 1 : 0);
                 (animJson["reversed"] ? i > -1 : i < animJson["lines"]);
                 (animJson["reversed"] ? i-- : i++)) {
                // For each sprite
                if (facing == "Right" || facing == "UpRight" || facing == "DownRight" || facing == "None" || facing == "Down" || facing == "Up")
                    for (int ii = 0;
                         ii < (i + 1 == animJson["lines"] ? /*Remove empty frames from the last line*/ animJson["emptyFramesReversed"] ?
                               imagesPerLine : imagesPerLine - static_cast<int>(animJson["emptyFrames"]) : imagesPerLine); ii++) {

                        if (animJson["emptyFramesReversed"] && i + 1 == animJson["lines"] && ii == 0)
                            ii += static_cast<int>(animJson["emptyFrames"]);

                        (*anim)[hRepeat][vRepeat].push_back(fullAnim.copy(ii * width, i * height, width, height));
                    }
                else if (facing == "Left" || facing == "UpLeft" || facing == "DownLeft")
                    for (int ii = imagesPerLine - 1;
                         ii > (i + 1 == animJson["lines"] ? /*Remove empty frames from the last line*/ animJson["emptyFramesReversed"] ?
                               -1 : static_cast<int>(animJson["emptyFrames"]) - 1 : -1); ii--) {

                        if (animJson["emptyFramesReversed"] && i + 1 == animJson["lines"] && ii == imagesPerLine - 1)
                            ii -= static_cast<int>(animJson["emptyFrames"]);

                        (*anim)[hRepeat][vRepeat].push_back(fullAnim.copy(ii * width, i * height, width, height));
                    }
            }
        }
    }

    // Repeating the frame
    if (horizontalRepeat != 1 || verticalRepeat != 1) {
        for (unsigned int frameIndex = 0; frameIndex < (*anim)[0][0].size(); frameIndex++) {
            const QImage frame = (*anim)[0][0][frameIndex];
            // Creating the final frame
            QImage* temp = new QImage(frame.width() * horizontalRepeat,
                              frame.height() * verticalRepeat,
                              frame.format());
            QPainter* painter = new QPainter(temp);
            for (unsigned int hRepeat = 0; hRepeat < horizontalRepeat; hRepeat++) {
                for (unsigned int vRepeat = 0; vRepeat < verticalRepeat; vRepeat++) {
                    // Modifying the final frame using the current one
                    painter->drawImage(hRepeat * static_cast<int>(animJson["width"]), vRepeat * static_cast<int>(animJson["height"]), (*anim)[hRepeat][vRepeat][frameIndex]);
                }
            }
            // Deleting variables and setting the final frame value
            painter->end();
            delete painter;
            (*anim)[0][0][frameIndex] = *temp;
            delete temp;
        }
        for (unsigned int i = 1; i < anim->size(); i++)
            for (unsigned int i2 = 1; i2 < (*anim)[i].size(); i2++)
                (*anim)[i][i2].clear();
    }

    // At this point, anim[0][0] is the animation
    std::vector<QImage> animation = (*anim)[0][0];
    delete anim;

    // Place the overlay on top of the current animation
    for (nlohmann::json ovrly : animJson["overlay"]) {
        std::vector<QImage> overlay = updateAnimation(ovrly, std::pair<int, int>(horizontalRepeat, verticalRepeat));
        for (unsigned int i = 0; i < animation.size(); i++) {
            QImage result(std::max(overlay[i].width(), animation[i].width()), std::max(overlay[i].height(), animation[i].height()), animation[i].format());
            QPainter merger(&result);
            merger.drawImage(0, 0, animation[i]);
            merger.drawImage(0, 0, overlay[i]);
            animation[i] = result;
        }
    }

    // Remove the empty pixels on the top-left
    for (unsigned int i = 0; i < animation.size(); i++) {
        QImage img = animation[i];
        if (facing == "Left" || facing == "UpLeft" || facing == "DownLeft")
            img.setOffset(QPoint(static_cast<int>(animJson["xOffset"][0]), static_cast<int>(animJson["yOffset"][0])));
        else if (facing == "Right" || facing == "UpRight" || facing == "DownRight" || facing == "None" || facing == "Down" || facing == "Up")
            img.setOffset(QPoint(static_cast<int>(animJson["xOffset"][1]), static_cast<int>(animJson["yOffset"][1])));
        animation[i] = img;
    }

    return animation;
}

std::vector<QImage> Entity::updateAnimation()
{
    return updateAnimation(state, std::pair<int, int>(horizontalRepeat, verticalRepeat));
}

CollisionBox *Entity::getBox()
{
    return box;
}

void Entity::setBox(CollisionBox *newBox)
{
    if (box != nullptr)
        delete box;
    box = newBox;
}

void Entity::setBoxNoDel(CollisionBox *newBox)
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

unsigned int Entity::getFrame() const
{
    return frame;
}

void Entity::setFrame(unsigned int newFrame)
{
    frame = newFrame;
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

const std::string &Entity::getLastFrameFacing() const
{
    return lastFrameFacing;
}

void Entity::setLastFrameFacing(const std::string &newLastFrameFacing)
{
    lastFrameFacing = newLastFrameFacing;
}

double Entity::getMass() const
{
    return mass;
}

void Entity::setMass(double newMass)
{
    mass = newMass;
}

std::string Entity::getRoomId() const
{
    return roomId;
}

void Entity::setRoomId(std::string newRoomId)
{
    roomId = newRoomId;
}

const std::vector<std::string> &Entity::getNameParameters() const
{
    return nameParameters;
}

void Entity::setNameParameters(const std::vector<std::string> &newNameParameters)
{
    nameParameters = newNameParameters;
}

const std::string &Entity::getFullName() const
{
    return fullName == "" ? name : fullName;
}

void Entity::setFullName(const std::string &newFullName)
{
    fullName = newFullName;
}

unsigned int Entity::getHorizontalRepeat() const
{
    return horizontalRepeat;
}

void Entity::setHorizontalRepeat(unsigned int newHorizontalRepeat)
{
    horizontalRepeat = newHorizontalRepeat;
}

unsigned int Entity::getVerticalRepeat() const
{
    return verticalRepeat;
}

void Entity::setVerticalRepeat(unsigned int newVerticalRepeat)
{
    verticalRepeat = newVerticalRepeat;
}

unsigned long long Entity::getEntityID() const
{
    return entityID;
}

float Entity::getLayer() const
{
    return layer;
}

void Entity::setLayer(float newLayer)
{
    layer = newLayer;
}

bool operator==(Entity a, Entity b) {
    return a.getEntityID() == b.getEntityID();
}
