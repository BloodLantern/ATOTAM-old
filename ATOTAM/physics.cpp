#include "physics.h"

double Physics::frameRate;
double Physics::gravity;

bool Physics::canChangeBox(Entity *e, CollisionBox *b, std::vector<Terrain*> *ts, std::vector<DynamicObj*> *ds, std::pair<int, int> roomS, std::pair<int, int> roomE)
{
    Entity ne = Entity(e->getX(), e->getY(), new CollisionBox(*b), nullptr, e->getEntType(), e->getIsAffectedByGravity(), e->getFacing(), e->getFrictionFactor(), e->getName(), e->getIsMovable());
    for (std::vector<Terrain*>::iterator i = ts->begin(); i != ts->end(); i++) {
        if (Entity::checkCollision(&ne, b, *i, (*i)->getBox())) {
            Entity::calcCollisionReplacement(&ne, *i);
        }
    }
    for (std::vector<DynamicObj*>::iterator i = ds->begin(); i != ds->end(); i++) {
        if (Entity::checkCollision(&ne, b, *i, (*i)->getBox())) {
            Entity::calcCollisionReplacement(&ne, *i);
        }
    }

    if (ne.getX() + ne.getBox()->getX() + ne.getBox()->getWidth() > roomE.first) {
        ne.setX(roomE.first - ne.getBox()->getX() - ne.getBox()->getWidth());
    } else if (ne.getX() + ne.getBox()->getX() < roomS.first) {
        ne.setX(roomS.first - ne.getBox()->getX());
    } if (ne.getY() + ne.getBox()->getY() + ne.getBox()->getHeight() > roomE.second) {
        ne.setY(roomE.second - ne.getBox()->getY() - ne.getBox()->getHeight());
    } else if (ne.getY() + ne.getBox()->getY() < roomS.second) {
        ne.setY(roomS.second - ne.getBox()->getY());
    }

    for (std::vector<Terrain*>::iterator i = ts->begin(); i != ts->end(); i++) {
        if (Entity::checkCollision(&ne, b, *i, (*i)->getBox())) {
            return false;
        }
    }
    for (std::vector<DynamicObj*>::iterator i = ds->begin(); i != ds->end(); i++) {
        if (Entity::checkCollision(&ne, b, *i, (*i)->getBox())) {
            return false;
        }
    }

    if (ne.getX() + ne.getBox()->getX() + ne.getBox()->getWidth() > roomE.first) {
        return false;
    } else if (ne.getX() + ne.getBox()->getX() < roomS.first) {
        return false;
    } if (ne.getY() + ne.getBox()->getY() + ne.getBox()->getHeight() > roomE.second) {
        return false;
    } else if (ne.getY() + ne.getBox()->getY() < roomS.second) {
        return false;
    }

    return true;
}

bool Physics::canChangeBoxAxis(Entity *e, CollisionBox *b, std::vector<Terrain *> *ts, std::vector<DynamicObj *> *ds, std::pair<int, int> roomS, std::pair<int, int> roomE, bool alongY)
{
    Entity ne = Entity(e->getX(), e->getY(), new CollisionBox(*b), nullptr, e->getEntType(), e->getIsAffectedByGravity(), e->getFacing(), e->getFrictionFactor(), e->getName(), e->getIsMovable());
    for (std::vector<Terrain*>::iterator i = ts->begin(); i != ts->end(); i++) {
        if (Entity::checkCollision(&ne, b, *i, (*i)->getBox())) {
            Entity::calcCollisionReplacementAxis(&ne, *i, alongY);
        }
    }
    for (std::vector<DynamicObj*>::iterator i = ds->begin(); i != ds->end(); i++) {
        if (Entity::checkCollision(&ne, b, *i, (*i)->getBox())) {
            Entity::calcCollisionReplacementAxis(&ne, *i, alongY);
        }
    }

    if (ne.getX() + ne.getBox()->getX() + ne.getBox()->getWidth() > roomE.first) {
        ne.setX(roomE.first - ne.getBox()->getX() - ne.getBox()->getWidth());
    } else if (ne.getX() + ne.getBox()->getX() < roomS.first) {
        ne.setX(roomS.first - ne.getBox()->getX());
    } if (ne.getY() + ne.getBox()->getY() + ne.getBox()->getHeight() > roomE.second) {
        ne.setY(roomE.second - ne.getBox()->getY() - ne.getBox()->getHeight());
    } else if (ne.getY() + ne.getBox()->getY() < roomS.second) {
        ne.setY(roomS.second - ne.getBox()->getY());
    }

    for (std::vector<Terrain*>::iterator i = ts->begin(); i != ts->end(); i++) {
        if (Entity::checkCollision(&ne, b, *i, (*i)->getBox())) {
            return false;
        }
    }
    for (std::vector<DynamicObj*>::iterator i = ds->begin(); i != ds->end(); i++) {
        if (Entity::checkCollision(&ne, b, *i, (*i)->getBox())) {
            return false;
        }
    }

    if (ne.getX() + ne.getBox()->getX() + ne.getBox()->getWidth() > roomE.first) {
        return false;
    } else if (ne.getX() + ne.getBox()->getX() < roomS.first) {
        return false;
    } if (ne.getY() + ne.getBox()->getY() + ne.getBox()->getHeight() > roomE.second) {
        return false;
    } else if (ne.getY() + ne.getBox()->getY() < roomS.second) {
        return false;
    }

    return true;
}

bool Physics::updateProjectile(Projectile *p)
{
    if (p->getState() == "Hit")
        p->setBox(nullptr);
    else {
        p->setLifeTime(p->getLifeTime() - (1 / frameRate));
        if (p->getLifeTime() <= 0.0) {
            p->timeOut();
        }
    }

    if (p->getState() == "Hit" && p->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"][p->getName()]["texture"]]["Hit"]["count"]) - 1))
        return true;
    else
        return false;
}

std::vector<Entity*> Physics::updateSamos(Samos* s, std::vector<Terrain*> *ts, std::vector<DynamicObj*> *ds, std::map<std::string, bool> inputList, std::map<std::string, double> inputTime, Map currentMap)
{
    std::vector<Entity*> toAdd;
    if (s->getState() == "MorphBallStop" || s->getState() == "MorphBallSlow" || s->getState() == "MorphBallSuperSlow")
        s->setState("MorphBall");
    else if (s->getState() == "Running")
        s->setState("Walking");
    else if (s->getState() == "RunningAimForward")
        s->setState("WalkingAimForward");
    else if (s->getState() == "RunningAimUp")
        s->setState("WalkingAimUp");
    else if (s->getState() == "RunningAimDown")
        s->setState("WalkingAimDown");

    if (s->getLagTime() > 0.0)
        s->setLagTime(s->getLagTime() - 1 / frameRate);

    nlohmann::json samosJson = Entity::values["names"]["Samos"];
    nlohmann::json roomJson = (*currentMap.getJson())["rooms"][currentMap.getCurrentRoomId()];

    if (s->getOnGround()) {
        if ((s->getState() == "Jumping") || (s->getState() == "SpinJump") || (s->getState() == "Falling") || (s->getState() == "JumpEnd") || (s->getState() == "WallJump")
                || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
            if ((s->getState() == "Falling") || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                s->setY(s->getY() - static_cast<double>(samosJson["height"]) - static_cast<int>(samosJson["offset_y"]) +
                        static_cast<double>(samosJson["fallingHitbox_height"]) + static_cast<int>(samosJson["fallingHitbox_offset_y"]));
            } else if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
                s->setY(s->getY() - static_cast<double>(samosJson["height"]) - static_cast<int>(samosJson["offset_y"]) +
                        static_cast<double>(samosJson["spinJumpHitbox_height"]) + static_cast<int>(samosJson["spinJumpHitbox_offset_y"]));
            }
        }
    }
    std::pair<int, int> roomS = {roomJson["position"][0], roomJson["position"][1]};
    std::pair<int, int> roomE = {roomJson["position"][0].get<int>() + roomJson["size"][0].get<int>(),
                                 roomJson["position"][1].get<int>() + roomJson["size"][1].get<int>()};

    CollisionBox* spinBox = new CollisionBox(samosJson["spinJumpHitbox_offset_x"], samosJson["spinJumpHitbox_offset_y"],
                  samosJson["spinJumpHitbox_width"], samosJson["spinJumpHitbox_height"]);
    bool canSpin = canChangeBoxAxis(s, spinBox, ts, ds, roomS, roomE, true);
    bool freeCanSpin = false;
    if (!canSpin) {
        canSpin = canChangeBox(s, spinBox, ts, ds, roomS, roomE);
        freeCanSpin = true;
    }
    CollisionBox* morphBox = new CollisionBox(samosJson["morphBallHitbox_offset_x"], samosJson["morphBallHitbox_offset_y"],
                  samosJson["morphBallHitbox_width"], samosJson["morphBallHitbox_height"]);
    bool canMorph = canChangeBoxAxis(s, morphBox, ts, ds, roomS, roomE, true);
    bool freeCanMorph = false;
    if (!canMorph) {
        canMorph = canChangeBox(s, morphBox, ts, ds, roomS, roomE);
        freeCanMorph = true;
    }
    CollisionBox* fallBox = new CollisionBox(samosJson["fallingHitbox_offset_x"], samosJson["fallingHitbox_offset_y"],
                samosJson["fallingHitbox_width"], samosJson["fallingHitbox_height"]);
    bool canFall = canChangeBoxAxis(s, fallBox, ts, ds, roomS, roomE, true);
    bool freeCanFall = false;
    if (!canFall) {
        canFall = canChangeBox(s, fallBox, ts, ds, roomS, roomE);
        freeCanFall = true;
    }
    CollisionBox* crouchBox = new CollisionBox(samosJson["crouchHitbox_offset_x"], samosJson["crouchHitbox_offset_y"],
                  samosJson["crouchHitbox_width"], samosJson["crouchHitbox_height"]);
    bool canCrouch = canChangeBoxAxis(s, crouchBox, ts, ds, roomS, roomE, true);
    bool freeCanCrouch = false;
    if (!canCrouch) {
        canCrouch = canChangeBox(s, crouchBox, ts, ds, roomS, roomE);
        freeCanCrouch = true;
    }
    CollisionBox* standBox = new CollisionBox(samosJson["offset_x"], samosJson["offset_y"], samosJson["width"], samosJson["height"]);
    bool canStand = canChangeBoxAxis(s, standBox, ts, ds, roomS, roomE, true);
    bool freeCanStand = false;
    if (!canStand) {
        canStand = canChangeBox(s, standBox, ts, ds, roomS, roomE);
        freeCanStand = true;
    }

    bool wallL = false;
    bool wallR = false;
    for (std::vector<Terrain*>::iterator i = ts->begin(); i != ts->end(); i++) {
        if (Entity::checkCollision(s ,s->getWallBoxL(), *i, (*i)->getBox())) {
            wallL = true;
        }
        if (Entity::checkCollision(s, s->getWallBoxR(), *i, (*i)->getBox())) {
            wallR = true;
        }
        if (wallL && wallR)
            break;
    }
    for (std::vector<DynamicObj*>::iterator i = ds->begin(); i != ds->end(); i++) {
        if (Entity::checkCollision(s ,s->getWallBoxL(), *i, (*i)->getBox())) {
            wallL = true;
        }
        if (Entity::checkCollision(s, s->getWallBoxR(), *i, (*i)->getBox())) {
            wallR = true;
        }
        if (wallL && wallR)
            break;
    }

    if (s->getState() == "MorphBalling" && s->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"]["Samos"]["texture"]]["MorphBalling"]["count"]) - 1)) {
        s->setIsInAltForm(true);
        s->setState("MorphBall");
    } else if (s->getState() == "UnMorphBalling" && s->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"]["Samos"]["texture"]]["UnMorphBalling"]["count"]) - 1)) {
        if (s->getOnGround()) {
            s->setState("IdleCrouch");
            s->setY(s->getY() - static_cast<double>(samosJson["crouchHitbox_height"]) - static_cast<int>(samosJson["crouchHitbox_offset_y"]) +
                    static_cast<double>(samosJson["morphBallHitbox_height"]) + static_cast<int>(samosJson["morphBallHitbox_offset_y"]));
        } else
            s->setState("Falling");
    }

    if (inputList["dash"] && inputTime["dash"] == 0.0 && s->getDashCoolDown() <= 0.0 && s->getDashDirection() == "") {
        s->setDashTime(samosJson["dashTime"].get<double>());
        s->setIsAffectedByGravity(false);
        s->setFrictionFactor(0);

        if (inputList["right"] && !inputList["left"]) {
            if (inputList["up"] && !inputList["down"]) {

                s->setDashDirection("UpRight");
                s->setFacing("Right");
                if (!s->getIsInAltForm())
                    s->setState("DashUpDiag");
                s->setSpeedPriorDash({s->getVX(), s->getVY()});
                s->setVX(std::max(s->getVX() + samosJson["dashBonus"].get<double>(), samosJson["dashBaseSpeed"].get<double>() * 0.707));
                s->setVY(std::min(s->getVY() - samosJson["dashBonus"].get<double>(), -samosJson["dashBaseSpeed"].get<double>() * 0.707));

            } else if (!inputList["up"] && inputList["down"]) {

                s->setDashDirection("DownRight");
                s->setFacing("Right");
                if (!s->getIsInAltForm())
                    s->setState("DashDownDiag");
                s->setSpeedPriorDash({s->getVX(), s->getVY()});
                s->setVX(std::max(s->getVX() + samosJson["dashBonus"].get<double>(), samosJson["dashBaseSpeed"].get<double>() * 0.707));
                s->setVY(std::max(s->getVY() + samosJson["dashBonus"].get<double>(), samosJson["dashBaseSpeed"].get<double>() * 0.707));

            } else {

                s->setDashDirection("Right");
                s->setFacing("Right");
                if (!s->getIsInAltForm())
                    s->setState("Dash");
                s->setSpeedPriorDash({s->getVX(), 0.0});
                s->setVX(std::max(s->getVX() + samosJson["dashBonus"].get<double>(), samosJson["dashBaseSpeed"].get<double>()));
                s->setVY(0.0);

            }
        } else if (!inputList["right"] && inputList["left"]) {
            if (inputList["up"] && !inputList["down"]) {

                s->setDashDirection("UpLeft");
                s->setFacing("Left");
                if (!s->getIsInAltForm())
                    s->setState("DashUpDiag");
                s->setSpeedPriorDash({s->getVX(), s->getVY()});
                s->setVX(std::min(s->getVX() - samosJson["dashBonus"].get<double>(), -samosJson["dashBaseSpeed"].get<double>() * 0.707));
                s->setVY(std::min(s->getVY() - samosJson["dashBonus"].get<double>(), -samosJson["dashBaseSpeed"].get<double>() * 0.707));

            } else if (!inputList["up"] && inputList["down"]) {

                s->setDashDirection("DownLeft");
                s->setFacing("Left");
                if (!s->getIsInAltForm())
                    s->setState("DashDownDiag");
                s->setSpeedPriorDash({s->getVX(), s->getVY()});
                s->setVX(std::min(s->getVX() - samosJson["dashBonus"].get<double>(), -samosJson["dashBaseSpeed"].get<double>() * 0.707));
                s->setVY(std::max(s->getVY() + samosJson["dashBonus"].get<double>(), samosJson["dashBaseSpeed"].get<double>() * 0.707));

            } else {

                s->setDashDirection("Left");
                s->setFacing("Left");
                if (!s->getIsInAltForm())
                    s->setState("Dash");
                s->setSpeedPriorDash({s->getVX(), 0.0});
                s->setVX(std::min(s->getVX() - samosJson["dashBonus"].get<double>(), -samosJson["dashBaseSpeed"].get<double>()));
                s->setVY(0.0);

            }
        } else {
            if (inputList["up"] && !inputList["down"]) {

                s->setDashDirection("Up");
                if (!s->getIsInAltForm())
                    s->setState("DashUp");
                s->setSpeedPriorDash({0.0, s->getVY()});
                s->setVX(0.0);
                s->setVY(std::min(s->getVY() - samosJson["dashBonus"].get<double>(), -samosJson["dashBaseSpeed"].get<double>()));

            } else if (!inputList["up"] && inputList["down"]) {

                s->setDashDirection("Down");
                if (!s->getIsInAltForm())
                    s->setState("DashDown");
                s->setSpeedPriorDash({0.0, s->getVY()});
                s->setVX(0.0);
                s->setVY(std::max(s->getVY() + samosJson["dashBonus"].get<double>(), samosJson["dashBaseSpeed"].get<double>()));

            } else {
                if (s->getFacing() == "Right") {

                    s->setDashDirection("Right");
                    if (!s->getIsInAltForm())
                        s->setState("Dash");
                    s->setSpeedPriorDash({s->getVX(), 0.0});
                    s->setVX(std::max(s->getVX() + samosJson["dashBonus"].get<double>(), samosJson["dashBaseSpeed"].get<double>()));
                    s->setVY(0.0);

                } else {

                    s->setDashDirection("Left");
                    if (!s->getIsInAltForm())
                        s->setState("Dash");
                    s->setSpeedPriorDash({s->getVX(), 0.0});
                    s->setVX(std::min(s->getVX() - samosJson["dashBonus"].get<double>(), -samosJson["dashBaseSpeed"].get<double>()));
                    s->setVY(0.0);
                }
            }
        }
        if (s->getIsInAltForm())
            s->setState("MorphBallDash");
    } else if (s->getDashDirection() != "") {
        if (s->getOnGround()) {
            if (inputList["jump"] && inputTime["jump"] == 0.0 && canSpin) {
                s->setDashTime(0.0);
                s->setDashDirection("");
                s->setDashCoolDown(samosJson["dashGroundCooldown"].get<double>());
                s->setDashCoolDownType("Ground");
                s->setJumpTime(0);
                s->setIsAffectedByGravity(true);
                s->setFrictionFactor(samosJson["movingFriction"].get<double>());
                if (s->getIsInAltForm()) {
                    s->setVY(-static_cast<double>(samosJson["morphJumpPower"]));
                    s->setState("MorphBall");
                } else {
                    s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                    s->setState("SpinJump");
                }
            } else if (s->getDashTime() <= 0.0) {
                s->setDashDirection("");
                s->setIsAffectedByGravity(true);
                s->setFrictionFactor(samosJson["movingFriction"].get<double>());
                s->setDashCoolDown(samosJson["dashGroundCooldown"].get<double>());
                s->setDashCoolDownType("Ground");

                if (s->getVX() > 0) {
                    if (!wallR) {
                        if (s->getSpeedPriorDash().first > 0.0) {
                            s->setVX(std::min(s->getVX(), std::max(s->getSpeedPriorDash().first, samosJson["dashBaseEndingSpeed"].get<double>())));
                        } else {
                            s->setVX(std::min(s->getVX(), samosJson["dashBaseEndingSpeed"].get<double>()));
                        }
                    } else {
                        s->setVX(0.0);
                    }
                } else {
                    if (!wallL) {
                        if (s->getSpeedPriorDash().first < 0.0) {
                            s->setVX(std::max(s->getVX(), std::min(s->getSpeedPriorDash().first, -samosJson["dashBaseEndingSpeed"].get<double>())));
                        } else {
                            s->setVX(std::max(s->getVX(), -samosJson["dashBaseEndingSpeed"].get<double>()));
                        }
                    } else {
                        s->setVX(0.0);
                    }
                }

                if (s->getVY() > 0) {
                    s->setVY(0.0);
                } else {
                    if (s->getSpeedPriorDash().second < 0.0) {
                        s->setVY(std::max(s->getVY(), std::min(s->getSpeedPriorDash().second, -samosJson["dashBaseEndingUpSpeed"].get<double>())));
                    } else {
                        s->setVY(std::max(s->getVY(), -samosJson["dashBaseEndingUpSpeed"].get<double>()));
                    }
                }

                if (s->getIsInAltForm())
                    s->setState("MorphBall");
                else {
                    if (canStand)
                        s->setState("Standing");
                    else if (canMorph && !canCrouch)
                        s->setState("morphBalling");
                    else
                        s->setState("IdleCrouch");
                }
            }
        } else {
            if (s->getDashTime() <= 0.0) {
                s->setDashDirection("");
                s->setIsAffectedByGravity(true);
                s->setFrictionFactor(samosJson["movingFriction"].get<double>());
                s->setDashCoolDown(samosJson["dashAirCooldown"].get<double>());
                s->setDashCoolDownType("Air");

                if (s->getVX() > 0) {
                    if (!wallR) {
                        if (s->getSpeedPriorDash().first > 0.0) {
                            s->setVX(std::min(s->getVX(), std::max(s->getSpeedPriorDash().first, samosJson["dashBaseEndingSpeed"].get<double>())));
                        } else {
                            s->setVX(std::min(s->getVX(), samosJson["dashBaseEndingSpeed"].get<double>()));
                        }
                    } else {
                        s->setVX(0.0);
                    }
                } else {
                    if (!wallL) {
                        if (s->getSpeedPriorDash().first < 0.0) {
                            s->setVX(std::max(s->getVX(), std::min(s->getSpeedPriorDash().first, -samosJson["dashBaseEndingSpeed"].get<double>())));
                        } else {
                            s->setVX(std::max(s->getVX(), -samosJson["dashBaseEndingSpeed"].get<double>()));
                        }
                    } else {
                        s->setVX(0.0);
                    }
                }

                if (s->getVY() < 0) {
                    if (s->getSpeedPriorDash().second < 0.0) {
                        s->setVY(std::max(s->getVY(), std::min(s->getSpeedPriorDash().second, -samosJson["dashBaseEndingUpSpeed"].get<double>())));
                    } else {
                        s->setVY(std::max(s->getVY(), -samosJson["dashBaseEndingUpSpeed"].get<double>()));
                    }
                }

                if (s->getIsInAltForm())
                    s->setState("MorphBall");
                else {
                    if (canFall)
                        s->setState("Falling");
                    else if (canMorph && !canSpin)
                        s->setState("morphBalling");
                    else
                        s->setState("SpinJump");
                }
            }
        }
        s->setDashTime(s->getDashTime() - 1 / frameRate);
    } else {
        if (s->getIsInAltForm()) {
            if (((inputList["morph"] && inputTime["morph"] == 0.0) || (inputList["up"] && inputTime["up"] == 0.0)) && canCrouch) {
                s->setState("UnMorphBalling");
                s->setIsInAltForm(false);
            } else {
                if (s->getOnGround()) {
                    if (inputList["left"] && !inputList["right"]) {
                        if (!wallL && s->getLagTime() <= 0.0) {
                            if (s->getVX() > (static_cast<double>(samosJson["morphGroundAcceleration"]) / frameRate - static_cast<double>(samosJson["morphGroundMaxSpeed"]))) {
                                s->setVX(s->getVX() - static_cast<double>(samosJson["morphGroundAcceleration"]) / frameRate);
                            } else if (s->getVX() < (static_cast<double>(samosJson["morphGroundAcceleration"]) / frameRate - static_cast<double>(samosJson["morphGroundMaxSpeed"]))
                                       && s->getVX() > -static_cast<double>(samosJson["morphGroundMaxSpeed"])) {
                                s->setVX(-static_cast<double>(samosJson["morphGroundMaxSpeed"]));
                            }
                            s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                        } else
                            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                        s->setFacing("Left");
                        if (inputList["jump"] && inputTime["jump"] < static_cast<double>(samosJson["preJumpWindow"])) {
                            s->setVY(-static_cast<double>(samosJson["morphJumpPower"]));
                            s->setJumpTime(0);
                        }
                    } else if (!inputList["left"] && inputList["right"]) {
                        if (!wallR && s->getLagTime() <= 0.0) {
                            if (s->getVX() < (static_cast<double>(samosJson["morphGroundMaxSpeed"]) - static_cast<double>(samosJson["morphGroundAcceleration"]) / frameRate)) {
                                s->setVX(s->getVX() + static_cast<double>(samosJson["morphGroundAcceleration"]) / frameRate);
                            } else if (s->getVX() > (static_cast<double>(samosJson["morphGroundMaxSpeed"]) - static_cast<double>(samosJson["morphGroundAcceleration"]) / frameRate)
                                       && s->getVX() < static_cast<double>(samosJson["morphGroundMaxSpeed"])) {
                                s->setVX(static_cast<double>(samosJson["morphGroundMaxSpeed"]));
                            }
                            s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                        } else
                            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                        s->setFacing("Right");
                        if (inputList["jump"] && inputTime["jump"] < static_cast<double>(samosJson["preJumpWindow"])) {
                            s->setVY(-static_cast<double>(samosJson["morphJumpPower"]));
                            s->setJumpTime(0);
                        }
                    } else {
                        s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                        if (inputList["jump"]  && inputTime["jump"] < static_cast<double>(samosJson["preJumpWindow"])) {
                            s->setVY(-static_cast<double>(samosJson["morphJumpPower"]));
                            s->setJumpTime(0);
                        }
                        if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
                            s->setVX(0);
                    }
                    if (!inputList["jump"]) {
                        s->setJumpTime(-1);
                    }
                } else {
                    if (inputList["left"] && !inputList["right"]) {
                        if (!wallL && s->getLagTime() <= 0.0) {
                            if (s->getVX() > (static_cast<double>(samosJson["morphAirAcceleration"]) / frameRate - static_cast<double>(samosJson["morphAirMaxSpeed"]))) {
                                s->setVX(s->getVX() - static_cast<double>(samosJson["morphAirAcceleration"]) / frameRate);
                            } else if (s->getVX() < (static_cast<double>(samosJson["morphAirAcceleration"]) / frameRate - static_cast<double>(samosJson["morphAirMaxSpeed"]))
                                       && s->getVX() > -static_cast<double>(samosJson["morphAirMaxSpeed"])) {
                                s->setVX(-static_cast<double>(samosJson["morphAirMaxSpeed"]));
                            }
                            s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                        } else
                            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                        s->setFacing("Left");
                        if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["morphJumpTimeMax"]) && s->getJumpTime() >= 0) {
                            s->setVY(s->getVY() - (static_cast<double>(samosJson["morphJumpTimeMax"]) - s->getJumpTime()) * static_cast<double>(samosJson["morphPostJumpBoost"]) / (frameRate * static_cast<double>(samosJson["morphJumpTimeMax"])));
                            s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                        } else if (s->getJumpTime() < static_cast<double>(samosJson["morphJumpTimeMax"]) && s->getJumpTime() >= 0){
                            if (s->getVY() < -static_cast<double>(samosJson["morphJumpFall"]))
                                s->setVY(-static_cast<double>(samosJson["morphJumpFall"]));
                            s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                        } else {
                            s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                        }
                    } else if (!inputList["left"] && inputList["right"]) {
                        if (!wallR && s->getLagTime() <= 0.0) {
                            if (s->getVX() < (static_cast<double>(samosJson["morphAirMaxSpeed"]) - static_cast<double>(samosJson["morphAirAcceleration"]) / frameRate)) {
                                s->setVX(s->getVX() + static_cast<double>(samosJson["morphAirAcceleration"]) / frameRate);
                            } else if (s->getVX() > (static_cast<double>(samosJson["morphAirMaxSpeed"]) - static_cast<double>(samosJson["morphAirAcceleration"]) / frameRate)
                                       && s->getVX() < static_cast<double>(samosJson["morphAirMaxSpeed"])) {
                                s->setVX(static_cast<double>(samosJson["morphAirMaxSpeed"]));
                            }
                            s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                        } else
                            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                        s->setFacing("Right");
                        if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["morphJumpTimeMax"]) && s->getJumpTime() >= 0) {
                            s->setVY(s->getVY() - (static_cast<double>(samosJson["morphJumpTimeMax"]) - s->getJumpTime()) * static_cast<double>(samosJson["morphPostJumpBoost"]) / (frameRate * static_cast<double>(samosJson["morphJumpTimeMax"])));
                            s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                        } else if (s->getJumpTime() < static_cast<double>(samosJson["morphJumpTimeMax"]) && s->getJumpTime() >= 0){
                            if (s->getVY() < -static_cast<double>(samosJson["morphJumpFall"]))
                                s->setVY(-static_cast<double>(samosJson["morphJumpFall"]));
                            s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                        } else {
                            s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                        }
                    } else {
                        s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                        if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["morphJumpTimeMax"]) && s->getJumpTime() >= 0) {
                            s->setVY(s->getVY() - (static_cast<double>(samosJson["morphJumpTimeMax"]) - s->getJumpTime()) * static_cast<double>(samosJson["morphPostJumpBoost"]) / (frameRate * static_cast<double>(samosJson["morphJumpTimeMax"])));
                            s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                        } else if (s->getJumpTime() < static_cast<double>(samosJson["morphJumpTimeMax"]) && s->getJumpTime() >= 0){
                            if (s->getVY() < -static_cast<double>(samosJson["morphJumpFall"]))
                                s->setVY(-static_cast<double>(samosJson["morphJumpFall"]));
                            s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                        } else {
                            s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                        }
                        if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
                            s->setVX(0);
                    }
                }
            }
        } else {
            if (inputList["morph"] && inputTime["morph"] == 0.0 && canMorph) {
                s->setState("MorphBalling");
                if (s->getOnGround()) {
                    if ((s->getState() == "Falling") || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag") || (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                        s->setY(s->getY() - static_cast<int>(samosJson["morphBallHitbox_height"]) - static_cast<int>(samosJson["morphBallHitbox_offset_y"]) +
                                static_cast<int>(samosJson["fallingHitbox_height"]) + static_cast<int>(samosJson["fallingHitbox_offset_y"]));
                    } else if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
                        s->setY(s->getY() - static_cast<int>(samosJson["morphBallHitbox_height"]) - static_cast<int>(samosJson["morphBallHitbox_offset_y"]) +
                                static_cast<int>(samosJson["spinJumpHitbox_height"]) + static_cast<int>(samosJson["spinJumpHitbox_offset_y"]));
                    } else if (s->getState() == "IdleCrouch" || s->getState() == "Crouching" || s->getState() == "UnCrouching") {
                        s->setY(s->getY() - static_cast<int>(samosJson["morphBallHitbox_height"]) - static_cast<int>(samosJson["morphBallHitbox_offset_y"]) +
                                static_cast<int>(samosJson["crouchHitbox_height"]) + static_cast<int>(samosJson["crouchHitbox_offset_y"]));
                    } else
                        s->setY(s->getY() - static_cast<int>(samosJson["morphBallHitbox_height"]) - static_cast<int>(samosJson["morphBallHitbox_offset_y"]) +
                                static_cast<int>(samosJson["height"]) + static_cast<int>(samosJson["offset_y"]));
               }
            } else if (inputList["aim"] && s->getState() != "MorphBalling") {
                if (s->getOnGround()) {
                    if ((s->getState() == "Jumping") || (s->getState() == "SpinJump") || (s->getState() == "Falling") || (s->getState() == "JumpEnd") || (s->getState() == "WallJump")
                            || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                        if (canStand)
                            s->setState("Landing");
                        else if (canCrouch)
                            s->setState("IdleCrouch");
                        else if (canMorph)
                            s->setState("MorphBalling");
                    } else if (inputList["jump"]  && inputTime["jump"] < static_cast<double>(samosJson["preJumpWindow"]) && canStand) {
                        s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                        s->setJumpTime(0);
                        s->setState("Jumping");
                    } else if ((s->getState() == "Crouching" || s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag"
                               || s->getState() == "CrouchAimDownDiag" || s->getState() == "UnMorphBalling") && canCrouch) {
                        s->setState("IdleCrouch");
                    } else {
                        if (canStand)
                            s->setState("Standing");
                        else if (canCrouch)
                            s->setState("IdleCrouch");
                        else if (canMorph)
                            s->setState("MorphBalling");
                    }
                    if (!inputList["jump"])
                        s->setJumpTime(-1);
                }
                if (!s->getOnGround() || s->getState() == "Jumping") {
                    if (inputList["jump"] && inputTime["jump"] < static_cast<double>(samosJson["preJumpWindow"]) && s->getState() == "WallJump" && (s->getJumpTime() < 0.0 || s->getJumpTime() >= static_cast<double>(samosJson["jumpTimeMax"]))) {
                        if (s->getFacing() == "Left") {
                            if (!wallL) {
                                if (s->getVX() > static_cast<double>(samosJson["wallJumpPower_x"]))
                                    s->setVX(-s->getVX());
                                else if (s->getRetainTime() > -samosJson["speedRetainWallJumpBonusWindow"].get<double>() && s->getSpeedRetained() > static_cast<double>(samosJson["wallJumpPower_x"])) {
                                    s->setVX(-s->getSpeedRetained());
                                    s->setRetainTime(0.0);
                                } else if (s->getVX() > -static_cast<double>(samosJson["wallJumpPower_x"]))
                                    s->setVX(-static_cast<double>(samosJson["wallJumpPower_x"]));
                            }
                        } else {
                            if (!wallR) {
                                if (s->getVX() < -static_cast<double>(samosJson["wallJumpPower_x"]))
                                    s->setVX(-s->getVX());
                                else if (s->getRetainTime() > -samosJson["speedRetainWallJumpBonusWindow"].get<double>() && s->getSpeedRetained() < -static_cast<double>(samosJson["wallJumpPower_x"])) {
                                    s->setVX(-s->getSpeedRetained());
                                    s->setRetainTime(0.0);
                                } else if (s->getVX() < static_cast<double>(samosJson["wallJumpPower_x"]))
                                    s->setVX(static_cast<double>(samosJson["wallJumpPower_x"]));
                            }
                        }
                        s->setVY(-static_cast<double>(samosJson["wallJumpPower_y"]));
                        s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                    }

                    if (canFall)
                        s->setState("Falling");
                    else if (canSpin)
                        s->setState("SpinJump");
                    else if (canMorph)
                        s->setState("MorphBalling");

                    if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0) {
                        s->setVY(s->getVY() - (static_cast<double>(samosJson["jumpTimeMax"]) - s->getJumpTime()) * static_cast<double>(samosJson["postJumpBoost"]) / (frameRate * static_cast<double>(samosJson["jumpTimeMax"])));
                        s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                    } else if (s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0){
                        if (s->getVY() < -static_cast<double>(samosJson["jumpFall"]))
                            s->setVY(-static_cast<double>(samosJson["jumpFall"]));
                        s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                    } else {
                        s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                    }
                }

                if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
                    s->setVX(0);
                s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
            } else {
                if (s->getOnGround()) {
                    if ((s->getState() == "Jumping") || (s->getState() == "SpinJump") || (s->getState() == "Falling") || (s->getState() == "JumpEnd") || (s->getState() == "WallJump")
                            || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                        if (canStand)
                            s->setState("Landing");
                        else if (canCrouch)
                            s->setState("IdleCrouch");
                        else if (canMorph)
                            s->setState("MorphBalling");
                    } else if (inputList["left"] && !inputList["right"]) {
                        if (!wallL && canStand && s->getLagTime() <= 0.0) {
                            if (!inputList["run"]) {
                                if (s->getVX() > (static_cast<double>(samosJson["groundAcceleration"]) / frameRate - static_cast<double>(samosJson["groundMaxSpeed"]))) {
                                    s->setVX(s->getVX() - static_cast<double>(samosJson["groundAcceleration"]) / frameRate);
                                } else if (s->getVX() < (static_cast<double>(samosJson["groundAcceleration"]) / frameRate - static_cast<double>(samosJson["groundMaxSpeed"]))
                                           && s->getVX() > -static_cast<double>(samosJson["groundMaxSpeed"])) {
                                    s->setVX(-static_cast<double>(samosJson["groundMaxSpeed"]));
                                }
                            } else {
                                if (s->getVX() > (static_cast<double>(samosJson["groundRunAcceleration"]) / frameRate - static_cast<double>(samosJson["groundRunMaxSpeedGap"]))) {
                                    if (s->getVX() > -static_cast<double>(samosJson["groundMaxSpeed"]))
                                        s->setVX(s->getVX() - static_cast<double>(samosJson["groundAcceleration"]) / frameRate);
                                    else
                                        s->setVX(s->getVX() - static_cast<double>(samosJson["groundRunAcceleration"]) / frameRate);
                                } else if (s->getVX() < (static_cast<double>(samosJson["groundRunAcceleration"]) / frameRate - static_cast<double>(samosJson["groundRunMaxSpeedGap"]))
                                           && s->getVX() > -static_cast<double>(samosJson["groundRunMaxSpeed"])) {
                                    s->setVX(-static_cast<double>(samosJson["groundRunMaxSpeed"]));
                                }
                            }
                            s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                        } else
                            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                        s->setFacing("Left");
                        if (inputList["jump"] && inputTime["jump"] < static_cast<double>(samosJson["preJumpWindow"]) && canSpin) {
                            s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                            s->setJumpTime(0);
                            s->setState("SpinJump");
                        } else if (s->getState() != "MorphBalling" && s->getState() != "UnMorphBalling" && canStand) {
                            if (!wallL && s->getLagTime() <= 0.0) {
                                if (inputList["down"] && !inputList["up"])
                                    s->setState("WalkingAimDown");
                                else if (!inputList["down"] && inputList["up"])
                                    s->setState("WalkingAimUp");
                                else
                                    s->setState("Walking");
                            } else {
                                s->setState("Standing");
                            }
                        }
                    } else if (!inputList["left"] && inputList["right"]) {
                        if (!wallR && canStand && s->getLagTime() <= 0.0) {
                            if (!inputList["run"]) {
                                if (s->getVX() < (static_cast<double>(samosJson["groundMaxSpeed"]) - static_cast<double>(samosJson["groundAcceleration"]) / frameRate)) {
                                    s->setVX(s->getVX() + static_cast<double>(samosJson["groundAcceleration"]) / frameRate);
                                } else if (s->getVX() > (static_cast<double>(samosJson["groundMaxSpeed"]) - static_cast<double>(samosJson["groundAcceleration"]) / frameRate)
                                           && s->getVX() < static_cast<double>(samosJson["groundMaxSpeed"])) {
                                    s->setVX(static_cast<double>(samosJson["groundMaxSpeed"]));
                                }
                            } else {
                                if (s->getVX() < (static_cast<double>(samosJson["groundRunMaxSpeedGap"]) - static_cast<double>(samosJson["groundRunAcceleration"]) / frameRate)) {
                                    if (s->getVX() < static_cast<double>(samosJson["groundMaxSpeed"]))
                                        s->setVX(s->getVX() + static_cast<double>(samosJson["groundAcceleration"]) / frameRate);
                                    else
                                        s->setVX(s->getVX() + static_cast<double>(samosJson["groundRunAcceleration"]) / frameRate);
                                } else if (s->getVX() > (static_cast<double>(samosJson["groundRunMaxSpeedGap"]) - static_cast<double>(samosJson["groundRunAcceleration"]) / frameRate)
                                           && s->getVX() < static_cast<double>(samosJson["groundRunMaxSpeed"])) {
                                    s->setVX(static_cast<double>(samosJson["groundRunMaxSpeed"]));
                                }
                            }
                            s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                        } else
                            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                        s->setFacing("Right");
                        if (inputList["jump"] && inputTime["jump"] < static_cast<double>(samosJson["preJumpWindow"]) && canSpin) {
                            s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                            s->setJumpTime(0);
                            s->setState("SpinJump");
                        } else if (s->getState() != "MorphBalling" && s->getState() != "UnMorphBalling") {
                            if (!wallR && s->getLagTime() <= 0.0) {
                                if (inputList["down"] && !inputList["up"])
                                    s->setState("WalkingAimDown");
                                else if (!inputList["down"] && inputList["up"])
                                    s->setState("WalkingAimUp");
                                else
                                    s->setState("Walking");
                            } else {
                                s->setState("Standing");
                            }
                        }
                    } else {
                        s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                        if (inputList["jump"] && inputTime["jump"] < static_cast<double>(samosJson["preJumpWindow"]) && canStand) {
                            s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                            s->setJumpTime(0);
                            s->setState("Jumping");
                        } else if (s->getState() != "MorphBalling" && s->getState() != "UnMorphBalling") {
                            if (inputList["down"] && !inputList["up"] && inputTime["down"] == 0.0 && canMorph && s->getState() == "IdleCrouch")
                                s->setState("MorphBalling");
                            else {
                                if (canCrouch) {
                                    if (!inputList["up"] && inputList["down"] && inputTime["down"] == 0.0 && (s->getState() == "Standing" || s->getState() == "UnCrouching"))
                                        s->setState("Crouching");
                                    else if (s->getState() == "Crouching" && s->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"]["Samos"]["texture"]]["Crouching"]["count"]) - 1))
                                        s->setState("IdleCrouch");
                                }
                                if (canStand) {
                                    if (inputList["up"] && !inputList["down"] && inputTime["up"] == 0.0 && (s->getState() == "IdleCrouch" || s->getState() == "Crouching") && s->getState() != "UnMorphBalling")
                                        s->setState("UnCrouching");
                                    else if (s->getState() == "UnCrouching" && s->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"]["Samos"]["texture"]]["UnCrouching"]["count"]) - 1))
                                        s->setState("Standing");

                                    if (s->getState() != "IdleCrouch" && s->getState() != "Crouching" && s->getState() != "UnCrouching" && s->getState() != "CrouchAimUp" && s->getState() != "CrouchAimUpDiag" && s->getState() != "CrouchAimDownDiag"&& s->getState() != "UnMorphBalling")
                                        s->setState("Standing");
                                }
                            }
                        }
                        if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
                            s->setVX(0);
                    }
                    if (!inputList["jump"]) {
                        s->setJumpTime(-1);
                    }
                } else {
                    if (inputList["jump"] && inputTime["jump"] < static_cast<double>(samosJson["preJumpWindow"]) && s->getState() == "WallJump" && (s->getJumpTime() < 0.0 || s->getJumpTime() >= static_cast<double>(samosJson["jumpTimeMax"]))) {
                        if (s->getFacing() == "Left") {
                            if (!wallL) {
                                if (s->getVX() > static_cast<double>(samosJson["wallJumpPower_x"]))
                                    s->setVX(-s->getVX());
                                else if (s->getRetainTime() > -samosJson["speedRetainWallJumpBonusWindow"].get<double>() && s->getSpeedRetained() > static_cast<double>(samosJson["wallJumpPower_x"])) {
                                    s->setVX(-s->getSpeedRetained());
                                    s->setRetainTime(0.0);
                                } else if (s->getVX() > -static_cast<double>(samosJson["wallJumpPower_x"]))
                                    s->setVX(-static_cast<double>(samosJson["wallJumpPower_x"]));
                            }
                        } else {
                            if (!wallR) {
                                if (s->getVX() < -static_cast<double>(samosJson["wallJumpPower_x"]))
                                    s->setVX(-s->getVX());
                                else if (s->getRetainTime() > -samosJson["speedRetainWallJumpBonusWindow"].get<double>() && s->getSpeedRetained() < -static_cast<double>(samosJson["wallJumpPower_x"])) {
                                    s->setVX(-s->getSpeedRetained());
                                    s->setRetainTime(0.0);
                                } else if (s->getVX() < static_cast<double>(samosJson["wallJumpPower_x"]))
                                    s->setVX(static_cast<double>(samosJson["wallJumpPower_x"]));
                            }
                        }
                        s->setVY(-static_cast<double>(samosJson["wallJumpPower_y"]));
                        s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                    } else if (inputList["left"] && !inputList["right"]) {
                        if (!wallL && s->getLagTime() <= 0.0) {
                            if (s->getVX() > (static_cast<double>(samosJson["airAcceleration"]) / frameRate - static_cast<double>(samosJson["airMaxSpeed"]))) {
                                s->setVX(s->getVX() - static_cast<double>(samosJson["airAcceleration"]) / frameRate);
                            } else if (s->getVX() < (static_cast<double>(samosJson["airAcceleration"]) / frameRate - static_cast<double>(samosJson["airMaxSpeed"]))
                                       && s->getVX() > -static_cast<double>(samosJson["airMaxSpeed"])) {
                                s->setVX(-static_cast<double>(samosJson["airMaxSpeed"]));
                            }
                            s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                        } else
                            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                        s->setFacing("Left");
                        if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0) {
                            s->setVY(s->getVY() - (static_cast<double>(samosJson["jumpTimeMax"]) - s->getJumpTime()) * static_cast<double>(samosJson["postJumpBoost"]) / (frameRate * static_cast<double>(samosJson["jumpTimeMax"])));
                            s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                        } else if (s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0){
                            if (s->getVY() < -static_cast<double>(samosJson["jumpFall"]))
                                s->setVY(-static_cast<double>(samosJson["jumpFall"]));
                            s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                        } else {
                            s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                        }
                        if (s->getState() == "Jumping" && (!inputList["jump"] || s->getJumpTime() >= static_cast<double>(samosJson["jumpTimeMax"])))
                                                s->setState("JumpEnd");

                        if (s->getState() != "SpinJump" && s->getState() != "WallJump" && s->getState() != "Jumping" && s->getState() != "JumpEnd"
                                && s->getState() != "MorphBalling" && s->getState() != "UnMorphBalling" && canFall) {
                            s->setState("Falling");
                        }
                    } else if (!inputList["left"] && inputList["right"]) {
                        if (!wallR && s->getLagTime() <= 0.0) {
                            if (s->getVX() < (static_cast<double>(samosJson["airMaxSpeed"]) - static_cast<double>(samosJson["airAcceleration"]) / frameRate)) {
                                s->setVX(s->getVX() + static_cast<double>(samosJson["airAcceleration"]) / frameRate);
                            } else if (s->getVX() > (static_cast<double>(samosJson["airMaxSpeed"]) - static_cast<double>(samosJson["airAcceleration"]) / frameRate)
                                       && s->getVX() < static_cast<double>(samosJson["airMaxSpeed"])) {
                                s->setVX(static_cast<double>(samosJson["airMaxSpeed"]));
                            }
                            s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                        } else
                            s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                        s->setFacing("Right");
                        if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0) {
                            s->setVY(s->getVY() - (static_cast<double>(samosJson["jumpTimeMax"]) - s->getJumpTime()) * static_cast<double>(samosJson["postJumpBoost"]) / (frameRate * static_cast<double>(samosJson["jumpTimeMax"])));
                            s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                        } else if (s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0){
                            if (s->getVY() < -static_cast<double>(samosJson["jumpFall"]))
                                s->setVY(-static_cast<double>(samosJson["jumpFall"]));
                            s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                        } else {
                            s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                        }
                        if (s->getState() == "Jumping" && (!inputList["jump"] || s->getJumpTime() >= static_cast<double>(samosJson["jumpTimeMax"])))
                                                s->setState("JumpEnd");

                        if (s->getState() != "SpinJump" && s->getState() != "WallJump" && s->getState() != "Jumping" && s->getState() != "JumpEnd"
                                && s->getState() != "MorphBalling" && s->getState() != "UnMorphBalling" && canFall) {
                            s->setState("Falling");
                        }
                    } else {
                        s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                        if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0) {
                            s->setVY(s->getVY() - (static_cast<double>(samosJson["jumpTimeMax"]) - s->getJumpTime()) * static_cast<double>(samosJson["postJumpBoost"]) / (frameRate * static_cast<double>(samosJson["jumpTimeMax"])));
                            s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                        } else if (s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0){
                            if (s->getVY() < -static_cast<double>(samosJson["jumpFall"]))
                                s->setVY(-static_cast<double>(samosJson["jumpFall"]));
                            s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                        } else {
                            s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                        }
                        if (s->getState() == "Jumping" && (!inputList["jump"] || s->getJumpTime() >= static_cast<double>(samosJson["jumpTimeMax"])))
                                                s->setState("JumpEnd");
                        if (s->getState() != "SpinJump" && s->getState() != "WallJump" && s->getState() != "Jumping" && s->getState() != "JumpEnd"
                                && s->getState() != "MorphBalling" && s->getState() != "UnMorphBalling" && canFall) {
                            s->setState("Falling");
                        }

                        if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
                            s->setVX(0);
                    }
                }
            }
        }
    }

    if (s->getShootTime() > 0.0) {
        s->setShootTime(s->getShootTime() - 1 / frameRate);
    }

    if (s->getDashCoolDown() > 0.0) {
        s->setDashCoolDown(s->getDashCoolDown() - 1 / frameRate);
    }

    if (s->getShootTime() <= 0.0 && inputList["shoot"]) {
        if (s->getState() == "Crouching" || s->getState() == "UnMorphBalling")
            s->setState("IdleCrouch");
        else if (s->getState() == "Uncrouching" || s->getState() == "Landing")
            s->setState("Standing");
        else if (s->getState() == "JumpEnd" || s->getState() == "Jumping" || s->getState() == "SpinJump" || s->getState() == "WallJump")
            s->setState("Falling");
        else if (s->getState() == "Walking")
            s->setState("WalkingAimForward");
    }

    if (s->getShootTime() > 0 && (s->getState() == "Walking" || s->getState() == "WalkingAimForward"))
        s->setState("WalkingAimForward");


    if (canMorph && !canCrouch && !canStand && s->getOnGround() && s->getState() != "MorphBalling") {
        s->setState("MorphBall");
        s->setIsInAltForm(true);
    }

    if (canMorph && !canCrouch && !canStand && !canSpin && !canFall && s->getState() != "MorphBalling") {
        s->setState("MorphBall");
        s->setIsInAltForm(true);
    }

    std::string changedBox = "";
    if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
        if ((*s->getBox()) != *spinBox) {
            s->setBox(spinBox);
            changedBox = "spin";
        } else
            delete spinBox;
        delete morphBox;
        delete fallBox;
        delete crouchBox;
        delete standBox;
    } else if (s->getState() == "MorphBall" || s->getState() == "MorphBalling" || s->getState() == "MorphBallDash") {
        if ((*s->getBox()) != *morphBox) {
            s->setBox(morphBox);
            changedBox = "morph";
        } else
            delete morphBox;
        delete spinBox;
        delete fallBox;
        delete crouchBox;
        delete standBox;
    } else if (s->getState() == "Falling" || s->getState() == "FallingAimUp" || s->getState() == "FallingAimUpDiag" || s->getState() == "FallingAimDownDiag" || s->getState() == "FallingAimDown") {
        if ((*s->getBox()) != *fallBox) {
            s->setBox(fallBox);
            changedBox = "fall";
        } else
            delete fallBox;
        delete spinBox;
        delete morphBox;
        delete crouchBox;
        delete standBox;
    } else if (s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag" || s->getState() == "CrouchAimDownDiag" || s->getState() == "UnCrouching" || s->getState() == "Crouching" || s->getState() == "UnMorphBalling") {
        if ((*s->getBox()) != *crouchBox) {
            s->setBox(crouchBox);
            changedBox = "crouch";
        } else
            delete crouchBox;
        delete spinBox;
        delete morphBox;
        delete fallBox;
        delete standBox;
    } else {
        if ((*s->getBox()) != *standBox) {
            s->setBox(standBox);
            changedBox = "stand";
        } else
            delete standBox;
        delete spinBox;
        delete morphBox;
        delete fallBox;
        delete crouchBox;
    }
    if (changedBox != "") {
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 1));
        s->setWallBoxR(new CollisionBox(s->getBox()->getX() + s->getBox()->getWidth(), s->getBox()->getY(), 1, s->getBox()->getHeight()));
        s->setWallBoxL(new CollisionBox(s->getBox()->getX() - 1, s->getBox()->getY(), 1, s->getBox()->getHeight()));
        if ((freeCanSpin && changedBox == "spin") || (freeCanMorph && changedBox == "morph") || (freeCanFall && changedBox == "fall") || (freeCanCrouch && changedBox == "crouch") || (freeCanStand && changedBox == "stand")) {
            for (std::vector<Terrain*>::iterator i = ts->begin(); i != ts->end(); i++) {
                if (Entity::checkCollision(s, s->getBox(), *i, (*i)->getBox())) {
                    Entity::calcCollisionReplacement(s, *i);
                }
            }
            for (std::vector<DynamicObj*>::iterator i = ds->begin(); i != ds->end(); i++) {
                if (Entity::checkCollision(s, s->getBox(), *i, (*i)->getBox())) {
                    Entity::calcCollisionReplacement(s, *i);
                }
            }
        } else {
            for (std::vector<Terrain*>::iterator i = ts->begin(); i != ts->end(); i++) {
                if (Entity::checkCollision(s, s->getBox(), *i, (*i)->getBox())) {
                    Entity::calcCollisionReplacementAxis(s, *i, true);
                }
            }
            for (std::vector<DynamicObj*>::iterator i = ds->begin(); i != ds->end(); i++) {
                if (Entity::checkCollision(s, s->getBox(), *i, (*i)->getBox())) {
                    Entity::calcCollisionReplacementAxis(s, *i, true);
                }
            }
        }
    }

    bool wallJumpL = false;
    bool wallJumpR = false;
    for (std::vector<Terrain*>::iterator i = ts->begin(); i != ts->end(); i++) {
        if (Entity::checkCollision(s ,s->getWallBoxL(), *i, (*i)->getBox())) {
            wallJumpL = true;
        } else if (Entity::checkCollision(s, s->getWallBoxR(), *i, (*i)->getBox())) {
            wallJumpR= true;
        }
        if (wallJumpL && wallJumpR)
            break;
    }
    for (std::vector<DynamicObj*>::iterator i = ds->begin(); i != ds->end(); i++) {
        if (Entity::checkCollision(s ,s->getWallBoxL(), *i, (*i)->getBox())) {
            wallJumpL = true;
        } else if (Entity::checkCollision(s, s->getWallBoxR(), *i, (*i)->getBox())) {
            wallJumpR= true;
        }
        if (wallJumpL && wallJumpR)
            break;
    }

    if (!s->getOnGround() && !inputList["aim"] && !inputList["shoot"] && s->getShootTime() <= 0 && !s->getIsInAltForm() && s->getState() != "MorphBalling" && canSpin && s->getDashDirection() == "") {
        std::string wallJump = "";

        if (wallJumpL && !wallJumpR)
            wallJump = "Left";
        else if (!wallJumpL && wallJumpR)
            wallJump = "Right";
        else if (wallJumpL && wallJumpR) {
            if (inputList["right"])
                wallJump = "Right";
            else
                wallJump = "Left";

        }


        if (wallJump == "Right" && (inputList["right"] || s->getState() == "WallJump" || s->getState() == "SpinJump")) {
            s->setFacing("Left");
            s->setState("WallJump");
            if (!inputList["jump"])
                s->setJumpTime(-1);
            if (s->getVY() > 0)
                s->setVY(1 / ((1 / s->getVY()) + (static_cast<double>(samosJson["wallFriction"]) / frameRate)));
            else if (s->getVY() < 0)
                s->setVY(1 / ((1 / s->getVY()) - (static_cast<double>(samosJson["wallFriction"]) / frameRate)));
        } else if (wallJump == "Left" && (inputList["left"] || s->getState() == "WallJump" || s->getState() == "SpinJump")) {
            s->setFacing("Right");
            s->setState("WallJump");
            if (!inputList["jump"])
                s->setJumpTime(-1);
            if (s->getVY() > 0)
                s->setVY(1 / ((1 / s->getVY()) + (static_cast<double>(samosJson["wallFriction"]) / frameRate)));
            else if (s->getVY() < 0)
                s->setVY(1 / ((1 / s->getVY()) - (static_cast<double>(samosJson["wallFriction"]) / frameRate)));
        } else if (s->getState() == "WallJump") {
            s->setState("SpinJump");
            if (s->getJumpTime() == -1)
                s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
        }
    }

    if (s->getRetainTime() > 0.0) {
        if (s->getSpeedRetained() < 0 && s->getVX() <= 0 && !wallJumpL) {
            s->setVX(std::min(s->getSpeedRetained(), s->getVX()));
            s->setRetainTime(0.0);
        } else if (s->getSpeedRetained() > 0 && s->getVX() >= 0 && !wallJumpR) {
            s->setVX(std::max(s->getSpeedRetained(), s->getVX()));
            s->setRetainTime(0.0);
        } else {
            s->setRetainTime(s->getRetainTime() - 1 / frameRate);
        }
    }
    if (s->getRetainTime() > -samosJson["speedRetainWallJumpBonusWindow"].get<double>())
        s->setRetainTime(s->getRetainTime() - 1 / frameRate);

    if (s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag" || s->getState() == "CrouchAimDownDiag") {
        if (inputList["left"] && !inputList["right"]) {
            s->setFacing("Left");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("CrouchAimUpDiag");
                s->setCanonDirection("UpLeft");
            } else if (!inputList["up"] && inputList["down"]) {
                s->setState("CrouchAimDownDiag");
                s->setCanonDirection("DownLeft");
            } else{
                s->setState("IdleCrouch");
                s->setCanonDirection("Left");
            }
        } else if (inputList["right"] && !inputList["left"]) {
            s->setFacing("Right");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("CrouchAimUpDiag");
                s->setCanonDirection("UpRight");
            } else if (!inputList["up"] && inputList["down"]) {
                s->setState("CrouchAimDownDiag");
                s->setCanonDirection("DownRight");
            } else {
                s->setState("IdleCrouch");
                s->setCanonDirection("Right");
            }
        } else {
            if (inputList["up"] && !inputList["down"]) {
                s->setState("CrouchAimUp");
                s->setCanonDirection("Up");
            } else {
                s->setState("IdleCrouch");
                if (s->getFacing() == "Left")
                    s->setCanonDirection("Left");
                else
                    s->setCanonDirection("Right");
            }
        }
    } else if (s->getState() == "Standing" || s->getState() == "StandingAimUpDiag" || s->getState() == "StandingAimDownDiag" || s->getState() == "StandingAimUp") {
        if (inputList["left"] && !inputList["right"]) {
            s->setFacing("Left");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("StandingAimUpDiag");
                s->setCanonDirection("UpLeft");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setState("StandingAimDownDiag");
                s->setCanonDirection("DownLeft");
            } else {
                s->setState("Standing");
                s->setCanonDirection("Left");
            }
        } else if (inputList["right"] && !inputList["left"]) {
            s->setFacing("Right");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("StandingAimUpDiag");
                s->setCanonDirection("UpRight");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setState("StandingAimDownDiag");
                s->setCanonDirection("DownRight");
            } else {
                s->setState("Standing");
                s->setCanonDirection("Right");
            }
        } else {
            if (inputList["up"] && !inputList["down"]) {
                s->setState("StandingAimUp");
                s->setCanonDirection("Up");
            } else {
                s->setState("Standing");
                if (s->getFacing() == "Left")
                    s->setCanonDirection("Left");
                else
                    s->setCanonDirection("Right");
            }
        }
    } else if (s->getState() == "Falling" || s->getState() == "FallingAimUpDiag" || s->getState() == "FallingAimDownDiag" || s->getState() == "FallingAimUp" || s->getState() == "FallingAimDown") {
        if (inputList["left"] && !inputList["right"]) {
            s->setFacing("Left");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("FallingAimUpDiag");
                s->setCanonDirection("UpLeft");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setState("FallingAimDownDiag");
                s->setCanonDirection("DownLeft");
            } else {
                s->setState("Falling");
                s->setCanonDirection("Left");
            }
        } else if (inputList["right"] && !inputList["left"]) {
            s->setFacing("Right");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("FallingAimUpDiag");
                s->setCanonDirection("UpRight");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setState("FallingAimDownDiag");
                s->setCanonDirection("DownRight");
            } else {
                s->setState("Falling");
                s->setCanonDirection("Right");
            }
        } else {
            if (inputList["up"] && !inputList["down"]) {
                s->setState("FallingAimUp");
                s->setCanonDirection("Up");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setState("FallingAimDown");
                s->setCanonDirection("Down");
            } else {
                s->setState("Falling");
                if (s->getFacing() == "Left")
                    s->setCanonDirection("Left");
                else
                    s->setCanonDirection("Right");
            }
        }
    } else if (s->getState() == "Walking" || s->getState() == "WalkingAimForward") {
        if (s->getFacing() == "Left")
            s->setCanonDirection("Left");
        else
            s->setCanonDirection("Right");
    } else if (s->getState() == "WalkingAimUp") {
        if (s->getFacing() == "Left")
            s->setCanonDirection("UpLeft");
        else
            s->setCanonDirection("UpRight");
    } else if (s->getState() == "WalkingAimDown") {
        if (s->getFacing() == "Left")
            s->setCanonDirection("DownLeft");
        else
            s->setCanonDirection("DownRight");
    } else if (s->getState() == "MorphBall" || s->getState() == "MorphBalling" || s->getState() == "UnMorphBalling") {
        if (s->getFacing() == "Left")
            s->setCanonDirection("Left");
        else
            s->setCanonDirection("Right");
    } else if (s->getState() == "SpinJump" || s->getState() == "WallJump" || s->getState() == "Jumping" || s->getState() == "JumpEnd") {
        if (inputList["left"] && !inputList["right"]) {
            if (inputList["up"] && !inputList["down"]) {
                s->setCanonDirection("UpLeft");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setCanonDirection("DownLeft");
            } else {
                s->setCanonDirection("Left");
            }
        } else if (inputList["right"] && !inputList["left"]) {
            if (inputList["up"] && !inputList["down"]) {
                s->setCanonDirection("UpRight");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setCanonDirection("DownRight");
            } else {
                s->setCanonDirection("Right");
            }
        } else {
            if (inputList["up"] && !inputList["down"]) {
                s->setCanonDirection("Up");
            } else if (inputList["down"] && !inputList["up"]) {
                s->setCanonDirection("Down");
            } else {
                if (s->getFacing() == "Left")
                    s->setCanonDirection("Left");
                else
                    s->setCanonDirection("Right");
            }
        }
    }

    if (inputList["weapon"]) {
        if (s->getSwitchDelay() == 0.0) {
            s->nextWeapon();
        } else if (s->getSwitchDelay() >= 3 * static_cast<double>(samosJson["switchDelay"]) || (s->getSwitchDelay() >= -1 && s->getSwitchDelay() < 0.0)) {
            s->nextWeapon();
            s->setSwitchDelay(-1 - static_cast<double>(samosJson["switchDelay"]));
        }

        s->setSwitchDelay(s->getSwitchDelay() + 1 / frameRate);

    } else
        s->setSwitchDelay(0.0);

    if (s->getShootTime() <= 0.0 && inputList["shoot"] && inputTime["shoot"] == 0.0 && s->getDashDirection() == "") {
        Projectile* p = nullptr;
        if (s->getIsInAltForm())
            p = s->shoot("Bomb");
        else if (s->getState() != "MorphBalling")
            p = s->shoot(s->getSelectedWeapon());
        if (p != nullptr) {
            p->setX(p->getX() + s->getVX() / frameRate);
            p->setY(p->getY() + s->getVY() / frameRate);

            toAdd.push_back(p);
            s->setShootTime(static_cast<double>(samosJson["shootTime"]));
        }
    }

    if (inputList["down"] && !s->getOnGround())
        s->setFastFalling(true);
    else
        s->setFastFalling(false);

    if (s->getState() == "MorphBall") {
        if (std::abs(s->getVX()) < 1)
            s->setState("MorphBallStop");
        else if (std::abs(s->getVX()) < 100)
            s->setState("MorphBallSuperSlow");
        else if (std::abs(s->getVX()) < 200)
            s->setState("MorphBallSlow");
    }
    if (std::abs(s->getVX()) > 500) {

        if (s->getFrictionFactor() == samosJson["friction"].get<double>())
            s->setFrictionFactor(samosJson["fastFriction"].get<double>());

        if (s->getFrictionFactor() == samosJson["movingFriction"].get<double>())
            s->setFrictionFactor(samosJson["movingFastFriction"].get<double>());

        if (std::abs(s->getVX()) > 1000) {
            if (s->getState() == "Walking")
                s->setState("Running");
            else if (s->getState() == "WalkingAimForward")
                s->setState("RunningAimForward");
            else if (s->getState() == "WalkingAimUp")
                s->setState("RunningAimUp");
            else if (s->getState() == "WalkingAimDown")
                s->setState("RunningAimDown");

            if (s->getFrictionFactor() == samosJson["fastFriction"].get<double>())
                s->setFrictionFactor(samosJson["superFastFriction"].get<double>());

            if (s->getFrictionFactor() == samosJson["movingFastFriction"].get<double>())
                s->setFrictionFactor(samosJson["movingSuperFastFriction"].get<double>());
        }
    }

    if (inputList["run"] && s->getFrictionFactor() == samosJson["movingFriction"].get<double>())
        s->setFrictionFactor(samosJson["movingFastFriction"].get<double>());

    return toAdd;
}

std::tuple<std::string, std::vector<Entity*>, std::vector<Entity*>, Map, Save> Physics::updatePhysics(Samos *s, std::vector<Terrain*> *ts, std::vector<DynamicObj*> *ds, std::vector<Monster*> *ms, std::vector<Area*> *as, std::vector<NPC*> *ns, std::vector<Projectile*> *ps, Map currentMap, Save currentProgress)
{
    std::string doorTransition = "";

    //Physics settings
    nlohmann::json paramJson = Entity::values["general"];
    double speedcap = static_cast<double>(paramJson["speedcap"]);
    double fallcap = static_cast<double>(paramJson["fallcap"]);
    double slowcap = static_cast<double>(paramJson["slowcap"]);
    //Deletion list
    std::vector<Entity*> toDel;

    nlohmann::json mapJson = (*currentMap.getJson())["rooms"][currentMap.getCurrentRoomId()];

    int roomS_x = mapJson["position"][0];
    int roomS_y = mapJson["position"][1];
    int roomE_x = mapJson["size"][0];
    int roomE_y = mapJson["size"][1];
    roomE_x += roomS_x;
    roomE_y += roomS_y;

    double groundFriction = paramJson["groundFriction"];
    double airFriction = paramJson["airFriction"];


    // SAMOS

    if (s != nullptr) {
        //I-frames
        if (s->getITime() > 0.0)
            s->setITime(s->getITime() - 1 / frameRate);

        if (s->getIsMovable()) {
            //Calc earth's attraction's acceleration if the (*ent)ity is affected
            if (s->getIsAffectedByGravity() && !s->getOnGround()) {
                if (s->getFastFalling())
                    s->setVY(s->getVY() + 2 * gravity / frameRate);
                else if (s->getVY() < fallcap)
                    s->setVY(s->getVY() + gravity / frameRate);
                else if (s->getVY() > fallcap + gravity / frameRate)
                    s->setVY(s->getVY() - gravity / (2 * frameRate));
            }

            //Calc frictions
            if (s->getOnGround()) {
                //Grounded frictions
                if (s->getVX() > 0)
                    s->setVX(1 / ((1 / s->getVX()) + (groundFriction * s->getFrictionFactor() / frameRate)));
                else if (s->getVX() < 0)
                    s->setVX(1 / ((1 / s->getVX()) - (groundFriction * s->getFrictionFactor() / frameRate)));

            } else {
                //Air friction
                if (s->getVX() > 0)
                    s->setVX(1 / ((1 / s->getVX()) + (airFriction * s->getFrictionFactor() / frameRate)));
                else if (s->getVX() < 0)
                    s->setVX(1 / ((1 / s->getVX()) - (airFriction * s->getFrictionFactor() / frameRate)));
            }
            //Speedcap
            if (std::abs(s->getVY()) > speedcap) {
                if (s->getVY() > 0)
                    s->setVY(speedcap);
                else
                    s->setVY(-speedcap);
            }
            //Move entities
            s->updateV(frameRate);
        }
    }

    // MONSTER

    for (std::vector<Monster*>::iterator m = ms->begin(); m != ms->end(); m++) {

        if ((*m)->getState() == "Death" && (*m)->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"][(*m)->getName()]["texture"]]["Death"]["count"]) - 1)) {
            toDel.push_back(*m);
            continue;
        }

        //I-frames
        if ((*m)->getITime() > 0.0)
            (*m)->setITime((*m)->getITime() - 1 / frameRate);

        if ((*m)->getIsMovable()) {
            //Calc earth's attraction's acceleration if the (*ent)ity is affected
            if ((*m)->getIsAffectedByGravity() && !(*m)->getOnGround() && (*m)->getVY() < fallcap) {
                (*m)->setVY((*m)->getVY() + gravity / frameRate);
            }
            //Calc frictions
            if ((*m)->getOnGround()) {
                //Grounded frictions
                if (std::abs((*m)->getVX()) < speedcap) {
                    if (std::abs((*m)->getVX()) > slowcap) {
                        if ((*m)->getVX() > 0)
                            (*m)->setVX(1 / ((1 / (*m)->getVX()) + (groundFriction * (*m)->getFrictionFactor() / frameRate)));
                        else if ((*m)->getVX() < 0)
                            (*m)->setVX(1 / ((1 / (*m)->getVX()) - (groundFriction * (*m)->getFrictionFactor() / frameRate)));
                    //Slowcap
                    } else
                        (*m)->setVX(0);
                }
            } else {
                //Air friction
                if (std::abs((*m)->getVX()) < speedcap) {
                    if (std::abs((*m)->getVX()) > slowcap) {
                        if ((*m)->getVX() > 0)
                            (*m)->setVX(1 / ((1 / (*m)->getVX()) + (airFriction * (*m)->getFrictionFactor() / frameRate)));
                        else if ((*m)->getVX() < 0)
                            (*m)->setVX(1 / ((1 / (*m)->getVX()) - (airFriction * (*m)->getFrictionFactor() / frameRate)));
                    //Slowcap
                    } else
                        (*m)->setVX(0);
                }
            }

            //Speedcap
            if (std::abs((*m)->getVY()) > speedcap) {
                if ((*m)->getVY() > 0)
                    (*m)->setVY(speedcap);
                else
                    (*m)->setVY(-speedcap);
            }
            if (std::abs((*m)->getVX()) > speedcap) {
                if ((*m)->getVX() > 0)
                    (*m)->setVX(speedcap);
                else
                    (*m)->setVX(-speedcap);
            }
            //Move entities
            (*m)->updateV(frameRate);
        }
    }

    // NPC

    for (std::vector<NPC*>::iterator n = ns->begin(); n != ns->end(); n++) {

        if ((*n)->getState() == "Death" && (*n)->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"][(*n)->getName()]["texture"]]["Death"]["count"]) - 1)) {
            toDel.push_back(*n);
            continue;
        }
        //I-frames
        if ((*n)->getITime() > 0.0)
            (*n)->setITime((*n)->getITime() - 1 / frameRate);

        if ((*n)->getIsMovable()) {
            //Calc earth's attraction's acceleration if the (*ent)ity is affected
            if ((*n)->getIsAffectedByGravity() && !(*n)->getOnGround() && (*n)->getVY() < fallcap) {
                (*n)->setVY((*n)->getVY() + gravity / frameRate);
            }
            //Calc frictions
            if ((*n)->getOnGround()) {
                //Grounded frictions
                if (std::abs((*n)->getVX()) < speedcap) {
                    if (std::abs((*n)->getVX()) > slowcap) {
                        if ((*n)->getVX() > 0)
                            (*n)->setVX(1 / ((1 / (*n)->getVX()) + (groundFriction * (*n)->getFrictionFactor() / frameRate)));
                        else if ((*n)->getVX() < 0)
                            (*n)->setVX(1 / ((1 / (*n)->getVX()) - (groundFriction * (*n)->getFrictionFactor() / frameRate)));
                    //Slowcap
                    } else
                        (*n)->setVX(0);
                }
            } else {
                //Air friction
                if (std::abs((*n)->getVX()) < speedcap) {
                    if (std::abs((*n)->getVX()) > slowcap) {
                        if ((*n)->getVX() > 0)
                            (*n)->setVX(1 / ((1 / (*n)->getVX()) + (airFriction * (*n)->getFrictionFactor() / frameRate)));
                        else if ((*n)->getVX() < 0)
                            (*n)->setVX(1 / ((1 / (*n)->getVX()) - (airFriction * (*n)->getFrictionFactor() / frameRate)));
                    //Slowcap
                    } else
                        (*n)->setVX(0);
                }
            }

            //Speedcap
            if (std::abs((*n)->getVY()) > speedcap) {
                if ((*n)->getVY() > 0)
                    (*n)->setVY(speedcap);
                else
                    (*n)->setVY(-speedcap);
            }
            if (std::abs((*n)->getVX()) > speedcap) {
                if ((*n)->getVX() > 0)
                    (*n)->setVX(speedcap);
                else
                    (*n)->setVX(-speedcap);
            }
            //Move entities
            (*n)->updateV(frameRate);
        }
    }

    // DYNAMICOBJ

    for (std::vector<DynamicObj*>::iterator d = ds->begin(); d != ds->end(); d++) {

        if ((*d)->getState() == "Death" && (*d)->getFrame() == (static_cast<unsigned int>(Entity::values["textures"][Entity::values["names"][(*d)->getName()]["texture"]]["Death"]["count"]) - 1)) {
            toDel.push_back(*d);
            continue;
        }

        //I-frames
        if ((*d)->getITime() > 0.0)
            (*d)->setITime((*d)->getITime() - 1 / frameRate);

        if ((*d)->getIsMovable()) {
            //Calc earth's attraction's acceleration if the (*ent)ity is affected
            if ((*d)->getIsAffectedByGravity() && !(*d)->getOnGround() && (*d)->getVY() < fallcap) {
                (*d)->setVY((*d)->getVY() + gravity / frameRate);
            }
            //Calc frictions
            if ((*d)->getOnGround()) {
                //Grounded frictions
                if (std::abs((*d)->getVX()) < speedcap) {
                    if (std::abs((*d)->getVX()) > slowcap) {
                        if ((*d)->getVX() > 0)
                            (*d)->setVX(1 / ((1 / (*d)->getVX()) + (groundFriction * (*d)->getFrictionFactor() / frameRate)));
                        else if ((*d)->getVX() < 0)
                            (*d)->setVX(1 / ((1 / (*d)->getVX()) - (groundFriction * (*d)->getFrictionFactor() / frameRate)));
                    //Slowcap
                    } else
                    (*d)->setVX(0);
                }
            } else {
                //Air friction
                if (std::abs((*d)->getVX()) < speedcap) {
                    if (std::abs((*d)->getVX()) > slowcap) {
                        if ((*d)->getVX() > 0)
                            (*d)->setVX(1 / ((1 / (*d)->getVX()) + (airFriction * (*d)->getFrictionFactor() / frameRate)));
                        else if ((*d)->getVX() < 0)
                            (*d)->setVX(1 / ((1 / (*d)->getVX()) - (airFriction * (*d)->getFrictionFactor() / frameRate)));
                    //Slowcap
                    } else
                        (*d)->setVX(0);
                }
            }

            //Speedcap
            if (std::abs((*d)->getVY()) > speedcap) {
                if ((*d)->getVY() > 0)
                    (*d)->setVY(speedcap);
                else
                    (*d)->setVY(-speedcap);
            }
            if (std::abs((*d)->getVX()) > speedcap) {
                if ((*d)->getVX() > 0)
                    (*d)->setVX(speedcap);
                else
                    (*d)->setVX(-speedcap);
            }
            //Move entities
            (*d)->updateV(frameRate);
        }
    }

    // AREA

    for (std::vector<Area*>::iterator a = as->begin(); a != as->end(); a++) {

        if ((*a)->getIsMovable()) {
            //Non-living objects can't be grounded
            if ((*a)->getIsAffectedByGravity() && (*a)->getVY() < fallcap)
                (*a)->setVY((*a)->getVY() + gravity / frameRate);

            if (std::abs((*a)->getVX()) < speedcap) {
                if (std::abs((*a)->getVX()) > slowcap) {
                    if ((*a)->getVX() > 0)
                        (*a)->setVX(1 / ((1 / (*a)->getVX()) + (airFriction * (*a)->getFrictionFactor() / frameRate)));
                    else if ((*a)->getVX() < 0)
                        (*a)->setVX(1 / ((1 / (*a)->getVX()) - (airFriction * (*a)->getFrictionFactor() / frameRate)));
                //Slowcap
                } else
                    (*a)->setVX(0);
            }

            //Speedcap
            if (std::abs((*a)->getVY()) > speedcap) {
                if ((*a)->getVY() > 0)
                    (*a)->setVY(speedcap);
                else
                    (*a)->setVY(-speedcap);
            }
            if (std::abs((*a)->getVX()) > speedcap) {
                if ((*a)->getVX() > 0)
                    (*a)->setVX(speedcap);
                else
                    (*a)->setVX(-speedcap);
            }
            //Move entities
            (*a)->updateV(frameRate);
        }
    }

    // PROJECTILES

    for (std::vector<Projectile*>::iterator p = ps->begin(); p != ps->end(); p++) {

        if ((*p)->getIsMovable()) {
            //Non-living objects can't be grounded
            if ((*p)->getIsAffectedByGravity() && (*p)->getVY() < fallcap)
                (*p)->setVY((*p)->getVY() + gravity / frameRate);

            if (std::abs((*p)->getVX()) < speedcap) {
                if (std::abs((*p)->getVX()) > slowcap) {
                //Slowcap
                } else
                    (*p)->setVX(0);
            }

            //Speedcap
            if (std::abs((*p)->getVY()) > speedcap) {
                if ((*p)->getVY() > 0)
                    (*p)->setVY(speedcap);
                else
                    (*p)->setVY(-speedcap);
            }
            if (std::abs((*p)->getVX()) > speedcap) {
                if ((*p)->getVX() > 0)
                    (*p)->setVX(speedcap);
                else
                    (*p)->setVX(-speedcap);
            }
            //Move entities
            if ((*p)->getLifeTime() != Entity::values["names"][(*p)->getName()]["lifeTime"])
                (*p)->updateV(frameRate);

            if (updateProjectile(*p))
                toDel.push_back(*p);
        }
    }

    std::vector<Entity*> toAdd;

     // SAMOS

    if (s != nullptr) {
        for (std::vector<Terrain*>::iterator j = ts->begin(); j != ts->end(); j++) {
            if (Entity::checkCollision(s, s->getBox(), *j, (*j)->getBox())) {
                double prevVX = s->getVX();
                Entity::calcCollisionReplacement(s, *j);
                if (prevVX != s->getVX()) {
                    s->setSpeedRetained(prevVX);
                    s->setRetainTime(Entity::values["names"]["Samos"]["speedRetainWindow"].get<double>());
                }
            }
        }
        for (std::vector<Monster*>::iterator j = ms->begin(); j != ms->end(); j++) {
            if (Entity::checkCollision(s, s->getBox(), *j, (*j)->getBox())) {
                Entity::calcCollisionReplacement(s, *j);
                if (s->getITime() <= 0.0) {
                    int prevHp = s->getHealth();
                    s->hit((*j)->getDamage(), *j, Entity::values["names"][s->getName()]["contactKB"], true);
                    s->setLagTime(Entity::values["names"][s->getName()]["lagTime"]);
                    if (s->getHealth() != prevHp)
                        currentProgress.addDamageReceived(prevHp - s->getHealth());
                }
                if (s->getLagTime() <= 0.0) {
                    s->hit(0, *j, Entity::values["names"][s->getName()]["contactKB"], true);
                    s->setLagTime(Entity::values["names"][s->getName()]["lagTime"]);
                }
                s->setDashTime(0.0);
            }
        }
        for (std::vector<DynamicObj*>::iterator j = ds->begin(); j != ds->end(); j++) {
            if (Entity::checkCollision(s, s->getBox(), *j, (*j)->getBox())) {
                double prevVX = s->getVX();
                Entity::calcCollisionReplacement(s, *j);
                if (prevVX != s->getVX()) {
                    s->setSpeedRetained(prevVX);
                    s->setRetainTime(Entity::values["names"]["Samos"]["speedRetainWindow"].get<double>());
                }
            }
        }
        for (std::vector<Area*>::iterator j = as->begin(); j != as->end(); j++) {
            if (Entity::checkCollision(s, s->getBox(), *j, (*j)->getBox())) {
                if ((*j)->getAreaType() == "Door") {
                    Door* d = static_cast<Door*>(*j);
                    currentMap.setCurrentRoomId(d->getEndingRoom());
                    if (d->getState().find("Right") != std::string::npos)
                        doorTransition = "Right";
                    else if (d->getState().find("Left") != std::string::npos)
                        doorTransition = "Left";
                    else if (d->getState().find("Up") != std::string::npos)
                        doorTransition = "Up";
                    else if (d->getState().find("Down") != std::string::npos)
                        doorTransition = "Down";
                    for (Entity* ent : currentMap.loadRoom())
                        toAdd.push_back(ent);
                }
            }
        }
        for (std::vector<Projectile*>::iterator j = ps->begin(); j != ps->end(); j++) {
            if (Entity::checkCollision(s, s->getBox(), *j, (*j)->getBox())) {
                int prevHp = s->getHealth();
                (*j)->hitting(s);
                if (s->getHealth() != prevHp) {
                    s->setDashTime(0.0);
                    currentProgress.addDamageReceived(prevHp - s->getHealth());
                    if ((*j)->getOwnerType() == "Samos")
                        currentProgress.addDamageDone(prevHp - s->getHealth());
                }
            }
        }

        if (s->getIsMovable() && s->getBox() != nullptr) {
            if (s->getX() + s->getBox()->getX() + s->getBox()->getWidth() > roomE_x) {
                s->setX(roomE_x - s->getBox()->getX() - s->getBox()->getWidth());
            } else if (s->getX() + s->getBox()->getX() < roomS_x) {
                s->setX(roomS_x - s->getBox()->getX());
            } if (s->getY() + s->getBox()->getY() + s->getBox()->getHeight() > roomE_y) {
                s->setY(roomE_y - s->getBox()->getY() - s->getBox()->getHeight());
            } else if (s->getY() + s->getBox()->getY() < roomS_y) {
                s->setY(roomS_y - s->getBox()->getY());
            }
        }
    }

    // MONSTER

    for (std::vector<Monster*>::iterator i = ms->begin(); i != ms->end(); i++) {
        for (std::vector<Terrain*>::iterator j = ts->begin(); j != ts->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getBox(), *j, (*j)->getBox())) {
                Entity::calcCollisionReplacement(*i, *j);
            }
        }
        for (std::vector<DynamicObj*>::iterator j = ds->begin(); j != ds->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getBox(), *j, (*j)->getBox())) {
                Entity::calcCollisionReplacement(*i, *j);
            }
        }
        for (std::vector<Projectile*>::iterator j = ps->begin(); j != ps->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getBox(), *j, (*j)->getBox())) {
                int prevHp = (*i)->getHealth();
                if ((*j)->hitting(*i)) {
                    (*i)->setBox(nullptr);
                    (*i)->setState("Death");
                    (*i)->setIsMovable(false);
                    (*i)->setHealth(0);
                    if ((*i)->getHealth() != prevHp) {
                        if ((*j)->getOwnerType() == "Samos")
                            currentProgress.addDamageDone(prevHp - (*i)->getHealth());
                    }
                    break;
                }
                if ((*i)->getHealth() != prevHp) {
                    if ((*j)->getOwnerType() == "Samos")
                        currentProgress.addDamageDone(prevHp - (*i)->getHealth());
                }
            }
        }

        if ((*i)->getIsMovable() && (*i)->getBox() != nullptr) {
            if ((*i)->getX() + (*i)->getBox()->getX() + (*i)->getBox()->getWidth() > roomE_x) {
                (*i)->setX(roomE_x - (*i)->getBox()->getX() - (*i)->getBox()->getWidth());
                (*i)->setVX(0.0);
            } else if ((*i)->getX() + (*i)->getBox()->getX() < roomS_x) {
                (*i)->setX(roomS_x - (*i)->getBox()->getX());
                (*i)->setVX(0.0);
            } if ((*i)->getY() + (*i)->getBox()->getY() + (*i)->getBox()->getHeight() > roomE_y) {
                (*i)->setY(roomE_y - (*i)->getBox()->getY() - (*i)->getBox()->getHeight());
                (*i)->setVY(0.0);
            } else if ((*i)->getY() + (*i)->getBox()->getY() < roomS_y) {
                (*i)->setY(roomS_y - (*i)->getBox()->getY());
                (*i)->setVY(0.0);
            }
        }
    }

    // DYNAMICOBJ

    for (std::vector<DynamicObj*>::iterator i = ds->begin(); i != ds->end(); i++) {
        for (std::vector<Terrain*>::iterator j = ts->begin(); j != ts->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getBox(), *j, (*j)->getBox())) {
                Entity::calcCollisionReplacement(*i, *j);
            }
        }
        for (std::vector<DynamicObj*>::iterator j = i + 1; j != ds->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getBox(), *j, (*j)->getBox())) {
                Entity::calcCollisionReplacement(*i, *j);
            }
        }
        for (std::vector<Projectile*>::iterator j = ps->begin(); j != ps->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getBox(), *j, (*j)->getBox())) {
                if ((*j)->hitting(*i)) {
                    (*i)->setBox(nullptr);
                    (*i)->setState("Death");
                    (*i)->setIsMovable(false);
                    (*i)->setHealth(0);
                    break;
                }
            }
        }

        if ((*i)->getIsMovable() && (*i)->getBox() != nullptr) {
            if ((*i)->getX() + (*i)->getBox()->getX() + (*i)->getBox()->getWidth() > roomE_x) {
                (*i)->setX(roomE_x - (*i)->getBox()->getX() - (*i)->getBox()->getWidth());
                (*i)->setVX(0.0);
            } else if ((*i)->getX() + (*i)->getBox()->getX() < roomS_x) {
                (*i)->setX(roomS_x - (*i)->getBox()->getX());
                (*i)->setVX(0.0);
            } if ((*i)->getY() + (*i)->getBox()->getY() + (*i)->getBox()->getHeight() > roomE_y) {
                (*i)->setY(roomE_y - (*i)->getBox()->getY() - (*i)->getBox()->getHeight());
                (*i)->setVY(0.0);
            } else if ((*i)->getY() + (*i)->getBox()->getY() < roomS_y) {
                (*i)->setY(roomS_y - (*i)->getBox()->getY());
                (*i)->setVY(0.0);
            }
        }
    }

    // NPC

    for (std::vector<NPC*>::iterator i = ns->begin(); i != ns->end(); i++) {
        for (std::vector<Terrain*>::iterator j = ts->begin(); j != ts->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getBox(), *j, (*j)->getBox())) {
                Entity::calcCollisionReplacement(*i, *j);
            }
        }

        if ((*i)->getIsMovable() && (*i)->getBox() != nullptr) {
            if ((*i)->getX() + (*i)->getBox()->getX() + (*i)->getBox()->getWidth() > roomE_x) {
                (*i)->setX(roomE_x - (*i)->getBox()->getX() - (*i)->getBox()->getWidth());
                (*i)->setVX(0.0);
            } else if ((*i)->getX() + (*i)->getBox()->getX() < roomS_x) {
                (*i)->setX(roomS_x - (*i)->getBox()->getX());
                (*i)->setVX(0.0);
            } if ((*i)->getY() + (*i)->getBox()->getY() + (*i)->getBox()->getHeight() > roomE_y) {
                (*i)->setY(roomE_y - (*i)->getBox()->getY() - (*i)->getBox()->getHeight());
                (*i)->setVY(0.0);
            } else if ((*i)->getY() + (*i)->getBox()->getY() < roomS_y) {
                (*i)->setY(roomS_y - (*i)->getBox()->getY());
                (*i)->setVY(0.0);
            }
        }
    }

    //PROJECTILES

    for (std::vector<Projectile*>::iterator i = ps->begin(); i != ps->end(); i++) {
        for (std::vector<Terrain*>::iterator j = ts->begin(); j != ts->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getBox(), *j, (*j)->getBox())) {
                (*i)->hitting(*j);
            }
        }

        if ((*i)->getIsMovable() && (*i)->getBox() != nullptr) {
            if ((*i)->getX() + (*i)->getBox()->getX() + (*i)->getBox()->getWidth() > roomE_x) {
                (*i)->setX(roomE_x - (*i)->getBox()->getX() - (*i)->getBox()->getWidth());
                (*i)->setVX(0.0);
                (*i)->timeOut();
            } else if ((*i)->getX() + (*i)->getBox()->getX() < roomS_x) {
                (*i)->setX(roomS_x - (*i)->getBox()->getX());
                (*i)->setVX(0.0);
                (*i)->timeOut();
            } if ((*i)->getY() + (*i)->getBox()->getY() + (*i)->getBox()->getHeight() > roomE_y) {
                (*i)->setY(roomE_y - (*i)->getBox()->getY() - (*i)->getBox()->getHeight());
                (*i)->setVY(0.0);
                (*i)->timeOut();
            } else if ((*i)->getY() + (*i)->getBox()->getY() < roomS_y) {
                (*i)->setY(roomS_y - (*i)->getBox()->getY());
                (*i)->setVY(0.0);
                (*i)->timeOut();
            }
        }
    }

    //Update the grounded state of livings
    if (s != nullptr) {
        bool prevOnGround = s->getOnGround();
        s->setOnGround(false);
        for (std::vector<Terrain*>::iterator j = ts->begin(); j!= ts->end(); j++) {
            if (Entity::checkCollision(s, s->getGroundBox(), *j, (*j)->getBox())) {
                s->setStandingOn(*j);
                s->setOnGround(true);
                break;
            }
        }
        for (std::vector<DynamicObj*>::iterator j = ds->begin(); j!= ds->end(); j++) {
            if (Entity::checkCollision(s, s->getGroundBox(), *j, (*j)->getBox())) {
                s->setStandingOn(*j);
                s->setOnGround(true);
                break;
            }
        }
        if (!s->getOnGround())
            s->setStandingOn(nullptr);
        if (s->getY() + s->getGroundBox()->getY() + s->getGroundBox()->getHeight() > roomE_y)
            s->setOnGround(true);

        if (s->getOnGround() && !prevOnGround)
            s->setDashCoolDown(0.0);
    }

    for (std::vector<Monster*>::iterator i = ms->begin(); i != ms->end(); i++) {
        (*i)->setOnGround(false);
        for (std::vector<Terrain*>::iterator j = ts->begin(); j!= ts->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getGroundBox(), *j, (*j)->getBox())) {
                (*i)->setStandingOn(*j);
                (*i)->setOnGround(true);
                break;
            }
        }
        for (std::vector<DynamicObj*>::iterator j = ds->begin(); j!= ds->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getGroundBox(), *j, (*j)->getBox())) {
                (*i)->setStandingOn(*j);
                (*i)->setOnGround(true);
                break;
            }
        }
        if (!(*i)->getOnGround())
            (*i)->setStandingOn(nullptr);
        if ((*i)->getY() + (*i)->getGroundBox()->getY() + (*i)->getGroundBox()->getHeight() > roomE_y)
            (*i)->setOnGround(true);
    }

    for (std::vector<NPC*>::iterator i = ns->begin(); i != ns->end(); i++) {
        (*i)->setOnGround(false);
        for (std::vector<Terrain*>::iterator j = ts->begin(); j!= ts->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getGroundBox(), *j, (*j)->getBox())) {
                (*i)->setStandingOn(*j);
                (*i)->setOnGround(true);
                break;
            }
        }
        for (std::vector<DynamicObj*>::iterator j = ds->begin(); j!= ds->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getGroundBox(), *j, (*j)->getBox())) {
                (*i)->setStandingOn(*j);
                (*i)->setOnGround(true);
                break;
            }
        }
        if (!(*i)->getOnGround())
            (*i)->setStandingOn(nullptr);
        if ((*i)->getY() + (*i)->getGroundBox()->getY() + (*i)->getGroundBox()->getHeight() > roomE_y)
            (*i)->setOnGround(true);
    }

    for (std::vector<DynamicObj*>::iterator i = ds->begin(); i != ds->end(); i++) {
        (*i)->setOnGround(false);
        for (std::vector<Terrain*>::iterator j = ts->begin(); j!= ts->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getGroundBox(), *j, (*j)->getBox())) {
                (*i)->setStandingOn(*j);
                (*i)->setOnGround(true);
                break;
            }
        }
        for (std::vector<DynamicObj*>::iterator j = ds->begin(); j!= ds->end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getGroundBox(), *j, (*j)->getBox())) {
                (*i)->setStandingOn(*j);
                (*i)->setOnGround(true);
                break;
            }
        }
        if (!(*i)->getOnGround())
            (*i)->setStandingOn(nullptr);
        if ((*i)->getY() + (*i)->getGroundBox()->getY() + (*i)->getGroundBox()->getHeight() > roomE_y)
            (*i)->setOnGround(true);
    }

    return std::tuple<std::string, std::vector<Entity*>, std::vector<Entity*>, Map, Save>(doorTransition, toAdd, toDel, currentMap, currentProgress);
}
