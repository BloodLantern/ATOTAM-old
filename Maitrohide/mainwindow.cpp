#include "mainwindow.h"
#include "ui_mainwindow.h"

bool MainWindow::running = true;
double MainWindow::frameRate;
double MainWindow::gameSpeed;
bool MainWindow::renderHitboxes;
int MainWindow::renderingMultiplier;
unsigned long long MainWindow::frameCount;
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
    frameRate = Entity::values["general"]["frameRate"];
    gameSpeed = Entity::values["general"]["gameSpeed"];
    gravity = Entity::values["general"]["gravity"];
    renderHitboxes = Entity::values["general"]["renderHitboxes"];
    renderingMultiplier = Entity::values["general"]["renderingMultiplier"];
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

void MainWindow::updateSamos(Samos *s)
{
    if (inputList["aim"]) {
        if (s->getOnGround() || !s->getIsAffectedByGravity()) {
            if ((s->getState() == "Jumping") || (s->getState() == "SpinJump") || (s->getState() == "Falling") || (s->getState() == "JumpEnd") || (s->getState() == "WallJump")
                    || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                if ((s->getState() == "Falling") || (s->getState() == "FallingAimUp")|| (s->getState() == "FallingAimUpDiag")|| (s->getState() == "FallingAimDownDiag")|| (s->getState() == "FallingAimDown")) {
                    s->setY(s->getY() - (s->getBox()->getHeight() * 0.2 - 3 * renderingMultiplier));
                } else if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
                    s->setY(s->getY() - (s->getBox()->getHeight() * 2 - 14 * renderingMultiplier));
                }
                s->setState("Landing");
            } else if (inputList["jump"] && s->getJumpTime() == -1) {
                s->setVY(-400);
                s->setJumpTime(0);
                s->setState("Jumping");
            } else if (s->getState() == "Crouching" || s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag") {
                if (inputList["left"] && !inputList["right"]) {
                    s->setFacing("Left");
                    if (inputList["up"] && !inputList["down"]) {
                        s->setState("CrouchAimUpDiag");
                    } else
                        s->setState("IdleCrouch");
                } else if (inputList["right"] && !inputList["left"]) {
                    s->setFacing("Right");
                    if (inputList["up"] && !inputList["down"]) {
                        s->setState("CrouchAimUpDiag");
                    } else
                        s->setState("IdleCrouch");
                } else {
                    if (inputList["up"] && !inputList["down"]) {
                        s->setState("CrouchAimUp");
                    } else
                        s->setState("IdleCrouch");
                }
            } else {
                    if (inputList["left"] && !inputList["right"]) {
                    s->setFacing("Left");
                    if (inputList["up"] && !inputList["down"]) {
                        s->setState("StandingAimUpDiag");
                    } else if (inputList["down"] && !inputList["up"]) {
                        s->setState("StandingAimDownDiag");
                    } else
                        s->setState("Standing");
                } else if (inputList["right"] && !inputList["left"]) {
                    s->setFacing("Right");
                    if (inputList["up"] && !inputList["down"]) {
                        s->setState("StandingAimUpDiag");
                    } else if (inputList["down"] && !inputList["up"]) {
                        s->setState("StandingAimDownDiag");
                    } else
                        s->setState("Standing");
                } else {
                    if (inputList["up"] && !inputList["down"]) {
                        s->setState("StandingAimUp");
                    } else
                        s->setState("Standing");
                }
            }
            if (!inputList["jump"])
                s->setJumpTime(-1);
        }
        if ((!s->getOnGround() && s->getIsAffectedByGravity()) || s->getState() == "Jumping") {
            if (inputList["jump"] && ((s->getJumpTime() == -1 && s->getState() == "WallJump") || ((s->getJumpTime() == -2 || s->getJumpTime() == -3) && s->getState() == "SpinJump"))) {
                if (s->getFacing() == "Left") {
                    s->setVX(-500);
                } else {
                    s->setVX(500);
                }
                s->setVY(-350);
                s->setJumpTime(20);
            } else if (s->getJumpTime() == -3) {
                s->setJumpTime(-2);
            } else if (s->getJumpTime() == -2) {
                s->setJumpTime(20);
            } else if (inputList["left"] && !inputList["right"]) {
                s->setFacing("Left");
                if (inputList["jump"] && s->getJumpTime() < 20 && s->getJumpTime() >= 0) {
                    s->setVY(s->getVY() - 10);
                    s->setJumpTime(s->getJumpTime() + 1);
                } else {
                    s->setJumpTime(20);
                }
                if (inputList["up"] && !inputList["down"]) {
                    s->setState("FallingAimUpDiag");
                } else if (inputList["down"] && !inputList["up"]) {
                    s->setState("FallingAimDownDiag");
                } else
                    s->setState("Falling");
            } else if (!inputList["left"] && inputList["right"]) {
                s->setFacing("Right");
                if (inputList["jump"] && s->getJumpTime() < 20 && s->getJumpTime() >= 0) {
                    s->setVY(s->getVY() - 10);
                    s->setJumpTime(s->getJumpTime() + 1);
                } else {
                    s->setJumpTime(20);
                }
                if (inputList["up"] && !inputList["down"]) {
                    s->setState("FallingAimUpDiag");
                } else if (inputList["down"] && !inputList["up"]) {
                    s->setState("FallingAimDownDiag");
                } else
                    s->setState("Falling");
            } else if ((!inputList["left"] && !inputList["right"]) || (inputList["left"] && inputList["right"])) {
                s->setFrictionFactor(1);
                if (inputList["jump"] && s->getJumpTime() < 20 && s->getJumpTime() >= 0) {
                    s->setVY(s->getVY() - 10);
                    s->setJumpTime(s->getJumpTime() + 1);
                } else {
                    s->setJumpTime(20);
                }
                if (inputList["up"] && !inputList["down"]) {
                    s->setState("FallingAimUp");
                } else if (inputList["down"] && !inputList["up"]) {
                    s->setState("FallingAimDown");
                } else
                    s->setState("Falling");

            }
        }

    if (std::abs(s->getVX()) < 25)
        s->setVX(0);
    s->setFrictionFactor(1);
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
                    s->setY(s->getY() - (s->getBox()->getHeight() * 0.2 - 3 * renderingMultiplier));
                } else if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
                    s->setY(s->getY() - (s->getBox()->getHeight() * 2 - 14 * renderingMultiplier));
                }
                s->setState("Landing");
            } else if (inputList["left"] && !inputList["right"]) {
                if (wall != "Left") {
                    if (s->getVX() > -400) {
                        s->setVX(s->getVX() - 30);
                    } else if (s->getVX() < -400 && s->getVX() > -420) {
                        s->setVX(-420);
                    }
                    s->setFrictionFactor(0.1);
                }
                s->setFacing("Left");
                if (inputList["jump"] && s->getJumpTime() == -1) {
                    s->setVY(-400);
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
                    if (s->getVX() < 400) {
                        s->setVX(s->getVX() + 30);
                    } else if (s->getVX() > 400 && s->getVX() < 420) {
                        s->setVX(420);
                    }
                    s->setFrictionFactor(0.1);
                }
                s->setFacing("Right");
                if (inputList["jump"] && s->getJumpTime() == -1) {
                    s->setVY(-400);
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
                s->setFrictionFactor(1);
                if (inputList["jump"] && s->getJumpTime() == -1) {
                    s->setVY(-400);
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
                if (std::abs(s->getVX()) < 25)
                    s->setVX(0);
            }
            if (!inputList["jump"]) {
                s->setJumpTime(-1);
            }
        } else {
            if (inputList["jump"] && ((s->getJumpTime() == -1 && s->getState() == "WallJump") || ((s->getJumpTime() == -2 || s->getJumpTime() == -3) && s->getState() == "SpinJump"))) {
                if (s->getFacing() == "Left") {
                    s->setVX(-500);
                } else {
                    s->setVX(500);
                }
                s->setVY(-350);
                s->setState("SpinJump");
                s->setJumpTime(20);
            } else if (s->getJumpTime() == -3) {
                s->setJumpTime(-2);
            } else if (s->getJumpTime() == -2) {
                s->setJumpTime(20);
            }  else if (inputList["left"] && !inputList["right"]) {
                if (s->getVX() > -180) {
                    s->setVX(s->getVX() - 20);
                } else if (s->getVX() < -180 && s->getVX() > -200) {
                    s->setVX(-200);
                }
                s->setFrictionFactor(0.1);
                s->setFacing("Left");
                if (inputList["jump"] && s->getJumpTime() < 20 && s->getJumpTime() >= 0) {
                    s->setVY(s->getVY() - 10);
                    s->setJumpTime(s->getJumpTime() + 1);
                } else if (s->getState() == "Jumping" && (!inputList["jump"] || s->getJumpTime() >= 20)) {
                    s->setState("JumpEnd");
                    s->setJumpTime(20);
                } else {
                    s->setJumpTime(20);
                }
                if (s->getState() != "SpinJump" && s->getState() != "WallJump" && s->getState() != "Jumping" && s->getState() != "JumpEnd") {
                    s->setState("Falling");
                }
            } else if (!inputList["left"] && inputList["right"]) {
                if (s->getVX() < 180) {
                    s->setVX(s->getVX() + 20);
                } else if (s->getVX() < -180 && s->getVX() > -200) {
                    s->setVX(-200);
                }
                s->setFrictionFactor(0.1);
                s->setFacing("Right");
                if (inputList["jump"] && s->getJumpTime() < 20 && s->getJumpTime() >= 0) {
                    s->setVY(s->getVY() - 10);
                    s->setJumpTime(s->getJumpTime() + 1);
                } else if (s->getState() == "Jumping" && (!inputList["jump"] || s->getJumpTime() >= 20)) {
                    s->setState("JumpEnd");
                    s->setJumpTime(20);
                } else {
                    s->setJumpTime(20);
                }
                if (s->getState() != "SpinJump" && s->getState() != "WallJump" && s->getState() != "Jumping" && s->getState() != "JumpEnd") {
                    s->setState("Falling");
                }
            } else if ((!inputList["left"] && !inputList["right"]) || (inputList["left"] && inputList["right"])) {
                s->setFrictionFactor(1);
                if (inputList["jump"] && s->getJumpTime() < 20 && s->getJumpTime() >= 0) {
                    s->setVY(s->getVY() - 10);
                    s->setJumpTime(s->getJumpTime() + 1);
                } else if (s->getState() == "Jumping" && (!inputList["jump"] || s->getJumpTime() >= 20)) {
                    s->setState("JumpEnd");
                    s->setJumpTime(20);
                } else {
                    s->setJumpTime(20);
                }
                if (s->getState() != "SpinJump" && s->getState() != "Jumping" && s->getState() != "JumpEnd" && s->getState() != "WallJump") {
                    s->setState("Falling");
                }

                if (std::abs(s->getVX()) < 25)
                    s->setVX(0);
            }
        }
    }
    nlohmann::json entJson = Entity::values["names"]["Samos"];

    if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
        s->setBox(new CollisionBox(static_cast<int>(entJson["offset_x"]) * renderingMultiplier,
                  (static_cast<int>(entJson["offset_y"]) + 14) * renderingMultiplier,
                  static_cast<int>(entJson["width"]) * renderingMultiplier,
                  static_cast<int>(entJson["height"]) * renderingMultiplier / 3));
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 2));
        s->setWallBoxR(new CollisionBox(s->getBox()->getX() + s->getBox()->getWidth(), s->getBox()->getY(), 2, s->getBox()->getHeight()));
        s->setWallBoxL(new CollisionBox(s->getBox()->getX() - 2, s->getBox()->getY(), 2, s->getBox()->getHeight()));
    } else if (s->getState() == "Falling" || s->getState() == "FallingAimUp" || s->getState() == "FallingAimUpDiag" || s->getState() == "FallingAimDownDiag" || s->getState() == "FallingAimDown") {
        s->setBox(new CollisionBox(static_cast<int>(entJson["offset_x"]) * renderingMultiplier,
                  (static_cast<int>(entJson["offset_y"]) + 3) * renderingMultiplier,
                  static_cast<int>(entJson["width"]) * renderingMultiplier,
                  static_cast<int>(entJson["height"]) * renderingMultiplier / 1.2));
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 2));
        s->setWallBoxR(new CollisionBox(s->getBox()->getX() + s->getBox()->getWidth(), s->getBox()->getY(), 2, s->getBox()->getHeight()));
        s->setWallBoxL(new CollisionBox(s->getBox()->getX() - 2, s->getBox()->getY(), 2, s->getBox()->getHeight()));
    } else if (s->getState() == "IdleCrouch" || s->getState() == "CrouchAimUp" || s->getState() == "CrouchAimUpDiag" || s->getState() == "UnCrouching" || s->getState() == "Crouching") {
        s->setBox(new CollisionBox(static_cast<int>(entJson["offset_x"]) * renderingMultiplier,
                  (static_cast<int>(entJson["offset_y"]) + 11) * renderingMultiplier,
                  static_cast<int>(entJson["width"]) * renderingMultiplier,
                  static_cast<int>(entJson["height"]) * renderingMultiplier / 1.35));
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 2));
        s->setWallBoxR(new CollisionBox(s->getBox()->getX() + s->getBox()->getWidth(), s->getBox()->getY(), 2, s->getBox()->getHeight()));
        s->setWallBoxL(new CollisionBox(s->getBox()->getX() - 2, s->getBox()->getY(), 2, s->getBox()->getHeight()));
    } else {
        s->setBox(new CollisionBox(static_cast<int>(entJson["offset_x"]) * renderingMultiplier,
                  static_cast<int>(entJson["offset_y"]) * renderingMultiplier,
                  static_cast<int>(entJson["width"]) * renderingMultiplier,
                  static_cast<int>(entJson["height"]) * renderingMultiplier));
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 2));
        s->setWallBoxR(new CollisionBox(s->getBox()->getX() + s->getBox()->getWidth(), s->getBox()->getY(), 2, s->getBox()->getHeight()));
        s->setWallBoxL(new CollisionBox(s->getBox()->getX() - 2, s->getBox()->getY(), 2, s->getBox()->getHeight()));
    }

    if (s->getState() == "SpinJump" || s->getState() == "WallJump") {
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

        if (wallJump == "Right") {
            s->setFacing("Left");
            s->setState("WallJump");
            if (!inputList["jump"])
                s->setJumpTime(-1);
            s->setVY(s->getVY() * (1.0 - 0.01 * std::abs(s->getVY()) / MainWindow::frameRate));
        } else if (wallJump == "Left") {
            s->setFacing("Right");
            s->setState("WallJump");
            if (!inputList["jump"])
                s->setJumpTime(-1);
            s->setVY(s->getVY() * (1.0 - 0.01 * std::abs(s->getVY()) / MainWindow::frameRate));
        } else if (s->getState() == "WallJump") {
            s->setState("SpinJump");
            if (s->getJumpTime() == -1)
                s->setJumpTime(-3);
        }
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
        if (entity->getTexture() == nullptr)
            entity->setTexture(&errorTexture);

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
    painter.end();
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
    //Ground objects list
    std::vector<Entity*> solidList;
    //Living objects list
    std::vector<Living*> livingList;
    for (Entity* ent : rendering) {

        if (ent->getEntType() == "Samos" || ent->getEntType() == "Monster" || ent->getEntType() == "NPC" || ent->getEntType() == "DynamicObj") {
            Living* liv = static_cast<Living*>(ent);
            livingList.push_back(liv);
            //Calc earth's attraction's acceleration if the entity is affected
            if (liv->getIsAffectedByGravity() && !liv->getOnGround() && liv->getIsMovable()) {
                liv->setVY(liv->getVY() + MainWindow::gravity / MainWindow::frameRate);
            }
            //Calc frictions
            if (liv->getOnGround() && liv->getIsMovable()) {
                //Grounded frictions
                if (std::abs(liv->getVX()) < 500)
                    liv->setVX(liv->getVX() * (1.0 - 0.1 * std::abs(liv->getVX()) * liv->getFrictionFactor() / MainWindow::frameRate));
                //To much speed might cause a problem
                else if (std::abs(liv->getVX()) < 1000)
                    liv->setVX(liv->getVX() * (1.0 - 0.05 * std::abs(liv->getVX()) * liv->getFrictionFactor() / MainWindow::frameRate));
                //Speedcap
                else {
                    if (liv->getVX() > 0)
                        liv->setVX(1000);
                    else
                        liv->setVX(-1000);
                }
            } else if (liv->getIsMovable()) {
                //Air friction
                if (std::abs(liv->getVX()) < 1000)
                    liv->setVX(liv->getVX() * (1.0 - 0.01 * std::abs(liv->getVX()) * liv->getFrictionFactor() / MainWindow::frameRate));
                //Speedcap
                else {
                    if (liv->getVX() > 0)
                        liv->setVX(1000);
                    else
                        liv->setVX(-1000);
                }
            }
        } else {
            //Non-living objects can't be grounded
            if (ent->getIsAffectedByGravity() && ent->getIsMovable())
                ent->setVY(ent->getVY() + MainWindow::gravity / MainWindow::frameRate);
            if (ent->getIsMovable()) {
                if (std::abs(ent->getVX()) < 1000)
                    ent->setVX(ent->getVX() * (1.0 - 0.01 * std::abs(ent->getVX()) * ent->getFrictionFactor() / MainWindow::frameRate));
                //Speedcap
                else {
                    if (ent->getVX() > 0)
                        ent->setVX(1000);
                    else
                        ent->setVX(-1000);
                }
            }
        }
        //Livings can only stand on terrain or dynamic objects
        if (ent->getEntType() == "Terrain" || ent->getEntType() == "DynamicObj") {
            solidList.push_back(ent);
        }
        //Move entities
        if (ent->getIsMovable())
            ent->updateV(MainWindow::frameRate);
    } //{Null, Terrain, Samos, Monster, Area, DynamicObj, NPC, Projectile};

    //Check for collisions and handle them
    for (std::vector<Entity*>::iterator i = rendering.begin(); i != rendering.end(); i++) {
        for (std::vector<Entity*>::iterator j = i+1; j!= rendering.end(); j++) {
            if (Entity::checkCollision(*i, (*i)->getBox(), *j, (*j)->getBox())) {
                Entity::handleCollision(*i,*j);
            }
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
}

void MainWindow::updateAnimations()
{
    for (Entity* entity : rendering) {
        // Every 'refreshRate' frames
        if (!Entity::values["textures"][entity->getName()][entity->getState()]["refreshRate"].is_null())
            if (frameCount % static_cast<int>(Entity::values["textures"][entity->getName()][entity->getState()]["refreshRate"]) == 0)
                // If the animation index still exists
                if (entity->getCurrentAnimation().size() > entity->getAnimation())
                    // Increment the animation index
                    entity->setAnimation(entity->getAnimation() + 1);

        // If the entity state is different from the last frame
        if (entity->getState() != entity->getLastFrameState()
                || entity->getFacing() != entity->getLastFrameFacing()) {
            // Update the QImage array representing the animation
            entity->setCurrentAnimation(entity->updateAnimation(entity->getState()));
            // If the animation should reset the next one
            if (!Entity::values["textures"][entity->getName()][entity->getLastFrameState()]["dontReset"])
                // Because the animation changed, reset its
                entity->setAnimation(0);
            else
                // Else, make sure not to end up with a too high index
                entity->setAnimation(entity->getAnimation() % entity->getCurrentAnimation().size());
        }

        // Every 'refreshRate' frames
        if (!Entity::values["textures"][entity->getName()][entity->getState()]["refreshRate"].is_null())
            if (frameCount % static_cast<int>(Entity::values["textures"][entity->getName()][entity->getState()]["refreshRate"]) == 0)
                // If the animation has to loop
                if (!Entity::values["textures"][entity->getName()][entity->getState()]["loop"].is_null()) {
                    if (Entity::values["textures"][entity->getName()][entity->getState()]["loop"]) {
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
        entity->setLastFrameState(entity->getState());
        entity->setLastFrameFacing(entity->getFacing());
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
