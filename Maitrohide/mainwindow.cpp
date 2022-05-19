#include "mainwindow.h"
#include "ui_mainwindow.h"

bool MainWindow::running = true;
double MainWindow::frameRate;
double MainWindow::updateRate;
double MainWindow::gameSpeed;
bool MainWindow::renderHitboxes;
int MainWindow::renderingMultiplier;
unsigned long long MainWindow::frameCount;
unsigned long long MainWindow::updateCount;
std::chrono::system_clock::time_point MainWindow::lastFpsShown;
std::chrono::system_clock::time_point MainWindow::lastFrameTime;
unsigned int MainWindow::fps;
bool MainWindow::showFps;
unsigned int MainWindow::showFpsUpdateRate;
double MainWindow::gravity;
std::map<std::string, bool> MainWindow::inputList;
QImage MainWindow::errorTexture("../assets/textures/error.png");

MainWindow::MainWindow(QApplication *app)
    : ui(new Ui::MainWindow)
    , m_qApp(app)
    //, rendering()
{
    ui->setupUi(this);
    setFixedSize(1000, 500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

nlohmann::json MainWindow::loadKeyCodes()
{
    //Loading keybinds
    std::ifstream keys_file("../assets/inputs.json");
    nlohmann::json temp;
    keys_file >> temp;
    return temp;
}

void MainWindow::loadGeneral()
{
    //Loading the general settings of the game
    gameSpeed = Entity::values["general"]["gameSpeed"];
    frameRate = Entity::values["general"]["frameRate"];
    updateRate = Entity::values["general"]["updateRate"];
    showFps = Entity::values["general"]["showFps"];
    showFpsUpdateRate = Entity::values["general"]["showFpsUpdateRate"];
    gravity = Entity::values["general"]["gravity"];
    renderingMultiplier = Entity::values["general"]["renderingMultiplier"];
    renderHitboxes = Entity::values["general"]["renderHitboxes"];
}

nlohmann::json MainWindow::keyCodes = MainWindow::loadKeyCodes();

void MainWindow::getInputs()
{
    //Only listen for inputs if the window is currently selected
    if (isActiveWindow())
        //Check every key
        for (nlohmann::json::iterator i = MainWindow::keyCodes.begin(); i != MainWindow::keyCodes.end(); i++) {
            MainWindow::inputList[i.key()] = GetKeyState(i.value()) & 0x8000;
        }
    else
        //Reset the keys if the window is not selected
        for (nlohmann::json::iterator i = MainWindow::keyCodes.begin(); i != MainWindow::keyCodes.end(); i++) {
            MainWindow::inputList[i.key()] = 0;
        }
}

bool MainWindow::updateProjectile(Projectile *p)
{
    p->setLifeTime(p->getLifeTime() - (1 / frameRate));
    if (p->getLifeTime() <= 0.0) {
        p->timeOut();
    }
    if (p->getState() == "Hit" && p->getAnimation() == (static_cast<unsigned int>(Entity::values["textures"][p->getName()]["Hit"]["count"]) - 1))
        return true;
    else
        return false;
}

void MainWindow::handleCollision(Entity *obj1, Entity *obj2)
{//{Null, Terrain, Samos, Monster, Area, DynamicObj, NPC, Projectile};

    //Long if else to decide what to do between two entities after a collision
    if (obj1->getEntType() == "Terrain") {
        if (obj2->getEntType() == "Samos") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Monster") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "NPC") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Projectile") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Projectile* p = static_cast<Projectile*>(obj2);
                p->hitting(obj1);
            }
        }

    } else if (obj1->getEntType() == "Samos") {
        if (obj2->getEntType() == "Terrain") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Monster") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

            // TODO hit
        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "NPC") {
            // TODO
        } else if (obj2->getEntType() == "Projectile") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Projectile* p = static_cast<Projectile*>(obj2);
                p->hitting(obj1);
            }
        }

    } else if (obj1->getEntType() == "Monster") {
        if (obj2->getEntType() == "Terrain") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Samos") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

            // TODO hit
        } else if (obj2->getEntType() == "Monster") {
            // TODO

        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "NPC") {
            // TODO

        } else if (obj2->getEntType() == "Projectile") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Projectile* p = static_cast<Projectile*>(obj2);
                p->hitting(obj1);
            }
        }

    } else if (obj1->getEntType() == "Area") {
        if (obj2->getEntType() == "Terrain") {
            // TODO

        } else if (obj2->getEntType() == "Samos") {
            // TODO

        } else if (obj2->getEntType() == "Monster") {
            // TODO

        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            // TODO

        } else if (obj2->getEntType() == "NPC") {
            // TODO

        } else if (obj2->getEntType() == "Projectile") {
            // TODO
        }

    } else if (obj1->getEntType() == "DynamicObj") {
        if (obj2->getEntType() == "Terrain") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Samos") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Monster") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "NPC") {
            // TODO

        } else if (obj2->getEntType() == "Projectile") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Projectile* p = static_cast<Projectile*>(obj2);
                p->hitting(obj1);
            }
        }

    } else if (obj1->getEntType() == "NPC") {
        if (obj2->getEntType() == "Terrain") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox()))
                Entity::calcCollisionReplacement(obj1, obj2);

        } else if (obj2->getEntType() == "Samos") {
            // TODO

        } else if (obj2->getEntType() == "Monster") {
            // TODO

        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            // TODO

        } else if (obj2->getEntType() == "NPC") {
            // TODO

        } else if (obj2->getEntType() == "Projectile") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Projectile* p = static_cast<Projectile*>(obj2);
                p->hitting(obj1);
            }
        }

    } else if (obj1->getEntType() == "Projectile") {
        if (obj2->getEntType() == "Terrain") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Entity::calcCollisionReplacement(obj1, obj2);
                Projectile* p = static_cast<Projectile*>(obj1);
                p->hitting(obj2);
            }
        } else if (obj2->getEntType() == "Samos") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Entity::calcCollisionReplacement(obj1, obj2);
                Projectile* p = static_cast<Projectile*>(obj1);
                p->hitting(obj2);
            }
        } else if (obj2->getEntType() == "Monster") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Entity::calcCollisionReplacement(obj1, obj2);
                Projectile* p = static_cast<Projectile*>(obj1);
                p->hitting(obj2);
            }
        } else if (obj2->getEntType() == "Area") {
            // TODO

        } else if (obj2->getEntType() == "DynamicObj") {
            if (Entity::checkCollision(obj1, obj1->getBox(), obj2, obj2->getBox())) {
                Entity::calcCollisionReplacement(obj1, obj2);
                Projectile* p = static_cast<Projectile*>(obj1);
                p->hitting(obj2);
            }
        } else if (obj2->getEntType() == "NPC") {
            // TODO

        } else if (obj2->getEntType() == "Projectile") {
            // TODO
        }

    } else
        //If getEntType() has been given a wrong value
        throw Entity::unknownEntityType;
}

void MainWindow::updateSamos(Samos *s)
{
    nlohmann::json samosJson = Entity::values["names"]["Samos"];

    if (inputList["aim"]) {
        if (s->getOnGround() || !s->getIsAffectedByGravity()) {
            if ((s->getState() == "Jumping") || (s->getState() == "SpinJump") || (s->getState() == "Falling") || (s->getState() == "JumpEnd") || (s->getState() == "WallJump")
                    || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                if ((s->getState() == "Falling") || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                    s->setY(s->getY() - (s->getBox()->getHeight() * (static_cast<double>(samosJson["fallingHitbox_ratio_y"]) - 1) - static_cast<int>(samosJson["fallingHitbox_offset_y"]) * renderingMultiplier));
                } else if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
                    s->setY(s->getY() - (s->getBox()->getHeight() * (static_cast<double>(samosJson["spinJumpHitbox_ratio_y"]) - 1) - static_cast<int>(samosJson["spinJumpHitbox_offset_y"]) * renderingMultiplier));
                }
                s->setState("Landing");
            } else if (inputList["jump"] && s->getJumpTime() == -1) {
                s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                s->setJumpTime(0);
                s->setState("Jumping");
            } else if (s->getState() == "Crouching" || s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag") {
                s->setState("IdleCrouch");
            } else
                s->setState("Standing");
            if (!inputList["jump"])
                s->setJumpTime(-1);
        }
        if ((!s->getOnGround() && s->getIsAffectedByGravity()) || s->getState() == "Jumping") {
            if (inputList["jump"] && ((s->getJumpTime() == -1 && s->getState() == "WallJump") || ((s->getJumpTime() == -2 || s->getJumpTime() == -3) && s->getState() == "SpinJump"))) {
                if (s->getFacing() == "Left") {
                    s->setVX(-static_cast<double>(samosJson["wallJumpPower_x"]));
                } else {
                    s->setVX(static_cast<double>(samosJson["wallJumpPower_x"]));
                }
                s->setVY(-static_cast<double>(samosJson["wallJumpPower_y"]));
                s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
            } else if (s->getJumpTime() == -3) {
                s->setJumpTime(-2);
            } else if (s->getJumpTime() == -2) {
                s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
            }

            s->setState("Falling");

            if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0) {
                s->setVY(s->getVY() - static_cast<double>(samosJson["postJumpBoost"]) / frameRate);
                s->setJumpTime(s->getJumpTime() + 1 / frameRate);
            } else {
                s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
            }
        }

    if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
        s->setVX(0);
    s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
    } else {
        std::string wall;
        for (std::vector<Entity*>::iterator j = rendering.begin(); j!= rendering.end(); j++) {
            if ((*j)->getEntType() == "Terrain" || (*j)->getEntType() == "DynamicObj") {
                if (Entity::checkCollision(s ,s->getWallBoxL(), *j, (*j)->getBox())) {
                    wall = "Left";
                } else if (Entity::checkCollision(s, s->getWallBoxR(), *j, (*j)->getBox())) {
                    wall = "Right";
                }
            }
        }
        if (s->getOnGround() || !s->getIsAffectedByGravity()) {
            if ((s->getState() == "Jumping") || (s->getState() == "SpinJump") || (s->getState() == "Falling") || (s->getState() == "JumpEnd") || (s->getState() == "WallJump")
                    || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                if ((s->getState() == "Falling") || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                    s->setY(s->getY() - (s->getBox()->getHeight() * (static_cast<double>(samosJson["fallingHitbox_ratio_y"]) - 1) - static_cast<int>(samosJson["fallingHitbox_offset_y"]) * renderingMultiplier));
                } else if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
                    s->setY(s->getY() - (s->getBox()->getHeight() * (static_cast<double>(samosJson["spinJumpHitbox_ratio_y"]) - 1) - static_cast<int>(samosJson["spinJumpHitbox_offset_y"]) * renderingMultiplier));
                }
                s->setState("Landing");
            } else if (inputList["left"] && !inputList["right"]) {
                if (wall != "Left") {
                    if (s->getVX() > (static_cast<double>(samosJson["groundAcceleration"]) / frameRate - static_cast<double>(samosJson["groundMaxSpeed"]))) {
                        s->setVX(s->getVX() - static_cast<double>(samosJson["groundAcceleration"]) / frameRate);
                    } else if (s->getVX() < (static_cast<double>(samosJson["groundAcceleration"]) / frameRate - static_cast<double>(samosJson["groundMaxSpeed"]))
                               && s->getVX() > -static_cast<double>(samosJson["groundMaxSpeed"])) {
                        s->setVX(-static_cast<double>(samosJson["groundMaxSpeed"]));
                    }
                    if (s->getVX() < 0)
                        s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                    else
                        s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                }
                s->setFacing("Left");
                if (inputList["jump"] && s->getJumpTime() == -1) {
                    s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                    s->setJumpTime(0);
                    s->setState("SpinJump");
                } else if (wall != "Left") {
                    if (inputList["down"] && !inputList["up"])
                        s->setState("WalkingAimDown");
                    else if (!inputList["down"] && inputList["up"])
                        s->setState("WalkingAimUp");
                    else
                        s->setState("Walking");
                } else {
                    s->setState("Standing");
                }
            } else if (!inputList["left"] && inputList["right"]) {
                if (wall != "Right") {
                    if (s->getVX() < (static_cast<double>(samosJson["groundMaxSpeed"]) - static_cast<double>(samosJson["groundAcceleration"]) / frameRate)) {
                        s->setVX(s->getVX() + static_cast<double>(samosJson["groundAcceleration"]) / frameRate);
                    } else if (s->getVX() > (static_cast<double>(samosJson["groundMaxSpeed"]) - static_cast<double>(samosJson["groundAcceleration"]) / frameRate)
                               && s->getVX() < static_cast<double>(samosJson["groundMaxSpeed"])) {
                        s->setVX(static_cast<double>(samosJson["groundMaxSpeed"]));
                    }
                    if (s->getVX() > 0)
                        s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                    else
                        s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                }
                s->setFacing("Right");
                if (inputList["jump"] && s->getJumpTime() == -1) {
                    s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                    s->setJumpTime(0);
                    s->setState("SpinJump");
                } else if (wall != "Right") {
                    if (inputList["down"] && !inputList["up"])
                        s->setState("WalkingAimDown");
                    else if (!inputList["down"] && inputList["up"])
                        s->setState("WalkingAimUp");
                    else
                        s->setState("Walking");
                } else {
                    s->setState("Standing");
                }
            } else if ((!inputList["left"] && !inputList["right"]) || (inputList["left"] && inputList["right"])) {
                s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                if (inputList["jump"] && s->getJumpTime() == -1) {
                    s->setVY(-static_cast<double>(samosJson["jumpPower"]));
                    s->setJumpTime(0);
                    s->setState("Jumping");
                } else {
                    if (((!inputList["down"] && !inputList["up"]) || (inputList["down"] && inputList["up"])) && s->getState() != "IdleCrouch" && s->getState() != "CrouchAimUp" && s->getState() != "CrouchAimUpDiag" && s->getState() != "Uncrouching" && s->getState() != "Crouching")
                        s->setState("Standing");
                    else if (((s->getState() == "Crouching" && s->getAnimation() == 2) || s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag") && !inputList["up"])
                        s->setState("IdleCrouch");
                    else if (!inputList["up"] && !(s->getState() == "Uncrouching" && !inputList["down"]))
                        s->setState("Crouching");
                    else if (((s->getState() == "Uncrouching" && s->getAnimation() == 2)))
                        s->setState("Standing");
                    else if (s->getState() == "Uncrouching" || s->getState() == "Crouching" || s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag")
                        s->setState("Uncrouching");
                    else
                        s->setState("Standing");
                }
                if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
                    s->setVX(0);
            }
            if (!inputList["jump"]) {
                s->setJumpTime(-1);
            }
        } else {
            if (inputList["jump"] && ((s->getJumpTime() == -1 && s->getState() == "WallJump") || ((s->getJumpTime() == -2 || s->getJumpTime() == -3) && s->getState() == "SpinJump"))) {
                if (s->getFacing() == "Left") {
                    s->setVX(-static_cast<double>(samosJson["wallJumpPower_x"]));
                } else {
                    s->setVX(static_cast<double>(samosJson["wallJumpPower_x"]));
                }
                s->setVY(-static_cast<double>(samosJson["wallJumpPower_y"]));
                s->setState("SpinJump");
                s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
            } else if (s->getJumpTime() == -3) {
                s->setJumpTime(-2);
            } else if (s->getJumpTime() == -2) {
                s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
            }  else if (inputList["left"] && !inputList["right"]) {
                if (s->getVX() > (static_cast<double>(samosJson["airAcceleration"]) / frameRate - static_cast<double>(samosJson["airMaxSpeed"]))) {
                    s->setVX(s->getVX() - static_cast<double>(samosJson["airAcceleration"]) / frameRate);
                } else if (s->getVX() < (static_cast<double>(samosJson["airAcceleration"]) / frameRate - static_cast<double>(samosJson["airMaxSpeed"]))
                           && s->getVX() > -static_cast<double>(samosJson["airMaxSpeed"])) {
                    s->setVX(-static_cast<double>(samosJson["airMaxSpeed"]));
                }
                if (s->getVX() < 0)
                    s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                else
                    s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                s->setFacing("Left");
                if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0) {
                    s->setVY(s->getVY() - static_cast<double>(samosJson["postJumpBoost"]) / frameRate);
                    s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                } else if (s->getState() == "Jumping" && (!inputList["jump"] || s->getJumpTime() >= static_cast<double>(samosJson["jumpTimeMax"]))) {
                    s->setState("JumpEnd");
                    s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                } else {
                    s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                }
                if (s->getState() != "SpinJump" && s->getState() != "WallJump" && s->getState() != "Jumping" && s->getState() != "JumpEnd") {
                    s->setState("Falling");
                }
            } else if (!inputList["left"] && inputList["right"]) {
                if (s->getVX() < (static_cast<double>(samosJson["airMaxSpeed"]) - static_cast<double>(samosJson["airAcceleration"]) / frameRate)) {
                    s->setVX(s->getVX() + static_cast<double>(samosJson["airAcceleration"]) / frameRate);
                } else if (s->getVX() > (static_cast<double>(samosJson["airMaxSpeed"]) - static_cast<double>(samosJson["airAcceleration"]) / frameRate)
                           && s->getVX() < static_cast<double>(samosJson["airMaxSpeed"])) {
                    s->setVX(static_cast<double>(samosJson["airMaxSpeed"]));
                }
                if (s->getVX() > 0)
                    s->setFrictionFactor(static_cast<double>(samosJson["movingFriction"]));
                else
                    s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                s->setFacing("Right");
                if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0) {
                    s->setVY(s->getVY() - static_cast<double>(samosJson["postJumpBoost"]) / frameRate);
                    s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                } else if (s->getState() == "Jumping" && (!inputList["jump"] || s->getJumpTime() >= static_cast<double>(samosJson["jumpTimeMax"]))) {
                    s->setState("JumpEnd");
                    s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                } else {
                    s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                }
                if (s->getState() != "SpinJump" && s->getState() != "WallJump" && s->getState() != "Jumping" && s->getState() != "JumpEnd") {
                    s->setState("Falling");
                }
            } else if ((!inputList["left"] && !inputList["right"]) || (inputList["left"] && inputList["right"])) {
                s->setFrictionFactor(static_cast<double>(samosJson["friction"]));
                if (inputList["jump"] && s->getJumpTime() < static_cast<double>(samosJson["jumpTimeMax"]) && s->getJumpTime() >= 0) {
                    s->setVY(s->getVY() - static_cast<double>(samosJson["postJumpBoost"]) / frameRate);
                    s->setJumpTime(s->getJumpTime() + 1 / frameRate);
                } else if (s->getState() == "Jumping" && (!inputList["jump"] || s->getJumpTime() >= static_cast<double>(samosJson["jumpTimeMax"]))) {
                    s->setState("JumpEnd");
                    s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                } else {
                    s->setJumpTime(static_cast<double>(samosJson["jumpTimeMax"]));
                }
                if (s->getState() != "SpinJump" && s->getState() != "Jumping" && s->getState() != "JumpEnd" && s->getState() != "WallJump") {
                    s->setState("Falling");
                }

                if (std::abs(s->getVX()) < static_cast<double>(samosJson["slowcap"]))
                    s->setVX(0);
            }
        }
    }


    if (s->getShootTime() <= 0.0 && inputList["shoot"]) {
        if (s->getState() == "Crouching")
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

    if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
        s->setBox(new CollisionBox((static_cast<int>(samosJson["offset_x"]) + static_cast<int>(samosJson["spinJumpHitbox_offset_x"])) * renderingMultiplier,
                  (static_cast<int>(samosJson["offset_y"]) + static_cast<int>(samosJson["spinJumpHitbox_offset_y"])) * renderingMultiplier,
                  static_cast<int>(samosJson["width"]) * renderingMultiplier / static_cast<double>(samosJson["spinJumpHitbox_ratio_x"]),
                  static_cast<int>(samosJson["height"]) * renderingMultiplier / static_cast<double>(samosJson["spinJumpHitbox_ratio_y"])));
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 2));
        s->setWallBoxR(new CollisionBox(s->getBox()->getX() + s->getBox()->getWidth(), s->getBox()->getY(), 2, s->getBox()->getHeight()));
        s->setWallBoxL(new CollisionBox(s->getBox()->getX() - 2, s->getBox()->getY(), 2, s->getBox()->getHeight()));
    } else if (s->getState() == "Falling" || s->getState() == "FallingAimUp" || s->getState() == "FallingAimUpDiag" || s->getState() == "FallingAimDownDiag" || s->getState() == "FallingAimDown") {
        s->setBox(new CollisionBox((static_cast<int>(samosJson["offset_x"]) + static_cast<int>(samosJson["fallingHitbox_offset_x"])) * renderingMultiplier,
                  (static_cast<int>(samosJson["offset_y"]) + static_cast<int>(samosJson["fallingHitbox_offset_y"])) * renderingMultiplier,
                  static_cast<int>(samosJson["width"]) * renderingMultiplier / static_cast<double>(samosJson["fallingHitbox_ratio_x"]),
                  static_cast<int>(samosJson["height"]) * renderingMultiplier / static_cast<double>(samosJson["fallingHitbox_ratio_y"])));
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 2));
        s->setWallBoxR(new CollisionBox(s->getBox()->getX() + s->getBox()->getWidth(), s->getBox()->getY(), 2, s->getBox()->getHeight()));
        s->setWallBoxL(new CollisionBox(s->getBox()->getX() - 2, s->getBox()->getY(), 2, s->getBox()->getHeight()));
    } else if (s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag" || s->getState() == "UnCrouching" || s->getState() == "Crouching") {
        s->setBox(new CollisionBox((static_cast<int>(samosJson["offset_x"]) + static_cast<int>(samosJson["crouchHitbox_offset_x"])) * renderingMultiplier,
                  (static_cast<int>(samosJson["offset_y"]) + static_cast<int>(samosJson["crouchHitbox_offset_y"])) * renderingMultiplier,
                  static_cast<int>(samosJson["width"]) * renderingMultiplier / static_cast<double>(samosJson["crouchHitbox_ratio_x"]),
                  static_cast<int>(samosJson["height"]) * renderingMultiplier / static_cast<double>(samosJson["crouchHitbox_ratio_y"])));
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 2));
        s->setWallBoxR(new CollisionBox(s->getBox()->getX() + s->getBox()->getWidth(), s->getBox()->getY(), 2, s->getBox()->getHeight()));
        s->setWallBoxL(new CollisionBox(s->getBox()->getX() - 2, s->getBox()->getY(), 2, s->getBox()->getHeight()));
    } else {
        s->setBox(new CollisionBox(static_cast<int>(samosJson["offset_x"]) * renderingMultiplier,
                  static_cast<int>(samosJson["offset_y"]) * renderingMultiplier,
                  static_cast<int>(samosJson["width"]) * renderingMultiplier,
                  static_cast<int>(samosJson["height"]) * renderingMultiplier));
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 2));
        s->setWallBoxR(new CollisionBox(s->getBox()->getX() + s->getBox()->getWidth(), s->getBox()->getY(), 2, s->getBox()->getHeight()));
        s->setWallBoxL(new CollisionBox(s->getBox()->getX() - 2, s->getBox()->getY(), 2, s->getBox()->getHeight()));
    }

    if (!s->getOnGround() && !inputList["aim"]) {
        std::string wallJump;
        for (std::vector<Entity*>::iterator j = rendering.begin(); j!= rendering.end(); j++) {
            if ((*j)->getEntType() == "Terrain" || (*j)->getEntType() == "DynamicObj") {
                if (Entity::checkCollision(s, s->getWallBoxL(), *j, (*j)->getBox())) {
                    wallJump = "Left";
                } else if (Entity::checkCollision(s, s->getWallBoxR(), *j, (*j)->getBox())) {
                    wallJump = "Right";
                }
            }
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
                s->setJumpTime(-3);
        }
    }

    if (s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag") {
        if (inputList["left"] && !inputList["right"]) {
            s->setFacing("Left");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("CrouchAimUpDiag");
                s->setCanonDirection("UpLeft");
            } else {
                s->setState("IdleCrouch");
                s->setCanonDirection("Left");
            }
        } else if (inputList["right"] && !inputList["left"]) {
            s->setFacing("Right");
            if (inputList["up"] && !inputList["down"]) {
                s->setState("CrouchAimUpDiag");
                s->setCanonDirection("UpRight");
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

    if (s->getShootTime() > 0.0) {
        s->setShootTime(s->getShootTime() - 1 / frameRate);
    }

    if (s->getShootTime() <= 0.0 && inputList["shoot"]) {
        addRenderable(s->shoot("Beam"));
        s->setShootTime(samosJson["shootTime"]);
    }

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    running = false;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    //Draw every entity in the rendering list
    for (Entity *entity : rendering) {

        //If the texture is null, set it to the error texture
        if (entity->getTexture() == nullptr) {
            entity->setTexture(&errorTexture);
        }

        //Try to draw the textrue, if it fails, set it to the error texture and try again
        try {
            painter.drawImage(QRect(entity->getX() + entity->getTexture()->offset().x(), entity->getY() + entity->getTexture()->offset().y(),
                                    entity->getTexture()->width() * renderingMultiplier, entity->getTexture()->height() * renderingMultiplier),
                              *entity->getTexture());
        } catch (...) {
            entity->setTexture(&errorTexture);
            painter.drawImage(QRect(entity->getX() + entity->getTexture()->offset().x(), entity->getY() + entity->getTexture()->offset().y(),
                                    entity->getTexture()->width() * renderingMultiplier, entity->getTexture()->height() * renderingMultiplier),
                              *entity->getTexture());
        }
    }

    //Draw hitboxes if necessary
    if (renderHitboxes) {
        for (Entity *entity : rendering) {
            painter.setPen(QColor("blue"));
            painter.drawRect(entity->getX() + entity->getBox()->getX(), entity->getY() + entity->getBox()->getY(),
                             entity->getBox()->getWidth(), entity->getBox()->getHeight());

            //Ground boxes
            if (entity->getEntType() == "Samos" || entity->getEntType() == "Monster" || entity->getEntType() == "NPC" || entity->getEntType() == "DynamicObj") {
                Living* liv = static_cast<Living*>(entity);
                painter.setPen(QColor("green"));
                painter.drawRect(liv->getX() + liv->getGroundBox()->getX(), liv->getY() + liv->getGroundBox()->getY(),
                                 liv->getGroundBox()->getWidth(), liv->getGroundBox()->getHeight());

                //Wall boxes
                if (entity->getEntType() == "Samos") {
                    Samos* s = static_cast<Samos*>(liv);
                    painter.drawRect(s->getX() + s->getWallBoxL()->getX(), s->getY() + s->getWallBoxL()->getY(), s->getWallBoxL()->getWidth(), s->getWallBoxL()->getHeight());
                    painter.drawRect(s->getX() + s->getWallBoxR()->getX(), s->getY() + s->getWallBoxR()->getY(), s->getWallBoxR()->getWidth(), s->getWallBoxR()->getHeight());
                }
            }
        }
    }

    // Draw fps if necessary
    if (showFps) {
        if ((std::chrono::high_resolution_clock::now() - lastFpsShown).count() > showFpsUpdateRate) {
            lastFpsShown = std::chrono::high_resolution_clock::now();
        }
        painter.setPen(QColor("black"));
        painter.drawText(QPoint(2, 12), QString::fromStdString(std::to_string(fps) + " FPS"));
    }
}

void MainWindow::addRenderable(Entity *entity)
{
    rendering.push_back(entity);
}

void MainWindow::clearRendering()
{
    rendering = {};
}

void MainWindow::updatePhysics()
{
    //Physics settings
    nlohmann::json paramJson = Entity::values["general"];
    double speedcap = static_cast<double>(paramJson["speedcap"]);
    //Ground objects list
    std::vector<Entity*> solidList;
    //Living objects list
    std::vector<Living*> livingList;
    //Deletion list
    std::vector<Entity*> toDel;
    for (Entity* ent : rendering) {

        if (ent->getEntType() == "Samos" || ent->getEntType() == "Monster" || ent->getEntType() == "NPC" || ent->getEntType() == "DynamicObj") {
            Living* liv = static_cast<Living*>(ent);
            livingList.push_back(liv);
            //Calc earth's attraction's acceleration if the entity is affected
            if (liv->getIsAffectedByGravity() && !liv->getOnGround() && liv->getIsMovable()) {
                liv->setVY(liv->getVY() + gravity / frameRate);
            }
            //Calc frictions
            if (liv->getOnGround() && liv->getIsMovable()) {
                //Grounded frictions
                if (std::abs(liv->getVX()) < speedcap) {
                    if (liv->getVX() > 0)
                        liv->setVX(1 / ((1 / liv->getVX()) + (static_cast<double>(paramJson["groundFriction"]) * liv->getFrictionFactor() / frameRate)));
                    else if (liv->getVX() < 0)
                        liv->setVX(1 / ((1 / liv->getVX()) - (static_cast<double>(paramJson["groundFriction"]) * liv->getFrictionFactor() / frameRate)));
                //Speedcap
                } else {
                    if (liv->getVX() > 0)
                        liv->setVX(speedcap);
                    else
                        liv->setVX(-speedcap);
                }
            } else if (liv->getIsMovable()) {
                //Air friction
                if (std::abs(liv->getVX()) < speedcap) {
                    if (liv->getVX() > 0)
                        liv->setVX(1 / ((1 / liv->getVX()) + (static_cast<double>(paramJson["airFriction"]) * liv->getFrictionFactor() / frameRate)));
                    else if (liv->getVX() < 0)
                        liv->setVX(1 / ((1 / liv->getVX()) - (static_cast<double>(paramJson["airFriction"]) * liv->getFrictionFactor() / frameRate)));
                //Speedcap
                } else {
                    if (liv->getVX() > 0)
                        liv->setVX(speedcap);
                    else
                        liv->setVX(-speedcap);
                }
            }
        } else {
            if (ent->getEntType() == "Projectile") {
                Projectile* p = static_cast<Projectile*>(ent);
                if (updateProjectile(p))
                    toDel.push_back(ent);
            }
            //Non-living objects can't be grounded
            if (ent->getIsAffectedByGravity() && ent->getIsMovable())
                ent->setVY(ent->getVY() + gravity / frameRate);
            if (ent->getIsMovable()) {
                if (std::abs(ent->getVX()) < speedcap) {
                    if (ent->getVX() > 0)
                        ent->setVX(1 / ((1 / ent->getVX()) + (static_cast<double>(paramJson["airFriction"]) * ent->getFrictionFactor() / frameRate)));
                    else if (ent->getVX() < 0)
                        ent->setVX(1 / ((1 / ent->getVX()) - (static_cast<double>(paramJson["airFriction"]) * ent->getFrictionFactor() / frameRate)));
                //Speedcap
                } else {
                    if (ent->getVX() > 0)
                        ent->setVX(speedcap);
                    else
                        ent->setVX(-speedcap);
                }
            }
        }
        //Livings can only stand on terrain or dynamic objects
        if (ent->getEntType() == "Terrain" || ent->getEntType() == "DynamicObj") {
            solidList.push_back(ent);
        }
        //Speedcap
        if (std::abs(ent->getVY()) > speedcap) {
            if (ent->getVY() > 0)
                ent->setVY(speedcap);
            else
                ent->setVY(-speedcap);
        }
        //Move entities
        if (ent->getIsMovable())
            ent->updateV(frameRate);
    } //{Null, Terrain, Samos, Monster, Area, DynamicObj, NPC, Projectile};

    //Check for collisions and handle them
    for (std::vector<Entity*>::iterator i = rendering.begin(); i != rendering.end(); i++) {
        for (std::vector<Entity*>::iterator j = i+1; j!= rendering.end(); j++) {
            handleCollision(*i,*j);
        }
    }

    //Update the grounded state of livings
    for (std::vector<Living*>::iterator i = livingList.begin(); i != livingList.end(); i++) {
        (*i)->setOnGround(false);
        for (std::vector<Entity*>::iterator j = solidList.begin(); j!= solidList.end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getGroundBox(), *j, (*j)->getBox())) {
                (*i)->setOnGround(true);
                break;
            }
        }
    }

    for (std::vector<Entity*>::iterator i = toDel.begin(); i != toDel.end(); i++) {
        std::vector<Entity*> newRen;
        for (std::vector<Entity*>::iterator j = rendering.begin(); j != rendering.end(); j++) {
            if (*j != *i)
                newRen.push_back(*j);
        }
        rendering = newRen;
        delete *i;
    }
}

void MainWindow::updateAnimations()
{
    unsigned long long uC = updateCount;
    for (Entity* entity : rendering) {
        std::string state = entity->getState();
        std::string facing = entity->getFacing();
        // Every 'refreshRate' frames
        if (!Entity::values["textures"][entity->getName()][state]["refreshRate"].is_null())
            if (uC % static_cast<int>(Entity::values["textures"][entity->getName()][state]["refreshRate"]) == 0)
                // If the animation index still exists
                if (entity->getCurrentAnimation().size() > entity->getAnimation())
                    // Increment the animation index
                    entity->setAnimation(entity->getAnimation() + 1);

        // If the entity state is different from the last frame
        if (state != entity->getLastFrameState()
                || facing != entity->getLastFrameFacing()) {
            // Update the QImage array representing the animation
            entity->setCurrentAnimation(entity->updateAnimation(state));
            // If the animation should reset the next one
            if (!Entity::values["textures"][entity->getName()][entity->getLastFrameState()]["dontReset"])
                // Because the animation changed, reset it
                entity->setAnimation(0);
            else
                // Else, make sure not to end up with a too high index
                if (entity->getAnimation() >= entity->getCurrentAnimation().size())
                    entity->setAnimation(0);
        }
        // Every 'refreshRate' frames
        if (!Entity::values["textures"][entity->getName()][state]["refreshRate"].is_null())
            if (uC % static_cast<int>(Entity::values["textures"][entity->getName()][state]["refreshRate"]) == 0)
                // If the animation has to loop
                if (!Entity::values["textures"][entity->getName()][state]["loop"].is_null()) {
                    if (Entity::values["textures"][entity->getName()][state]["loop"]) {
                        // If the animation index still exists
                        if (entity->getCurrentAnimation().size() - 1 < entity->getAnimation())
                            // Reset animation
                            entity->setAnimation(0);
                    } else
                        // If the animation index still exists
                        if (entity->getCurrentAnimation().size() - 1 < entity->getAnimation())
                            // If the animation doesn't loop, make sure it stays on its last frame
                            entity->setAnimation(entity->getAnimation() - 1);
                }

        // Update the texture with the animation index
        entity->updateTexture();

        // Make sure to update these values for the next frame
        entity->setLastFrameState(state);
        entity->setLastFrameFacing(facing);
    }
}

const std::vector<Entity *> &MainWindow::getRendering() const
{
    return rendering;
}

void MainWindow::setRendering(const std::vector<Entity *> &newRendering)
{
    rendering = newRendering;
}
