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
    std::ifstream keys_file("../assets/inputs.json");
    nlohmann::json temp;
    keys_file >> temp;
    return temp;
}

void MainWindow::loadGeneral()
{
    frameRate = Entity::values["general"]["frameRate"];
    gameSpeed = Entity::values["general"]["gameSpeed"];
    gravity = Entity::values["general"]["gravity"];
    renderHitboxes = Entity::values["general"]["renderHitboxes"];
    renderingMultiplier = Entity::values["general"]["renderingMultiplier"];
}

nlohmann::json MainWindow::keyCodes = MainWindow::loadKeyCodes();

void MainWindow::getInputs()
{
    for (nlohmann::json::iterator i = MainWindow::keyCodes.begin(); i != MainWindow::keyCodes.end(); i++) {
        MainWindow::inputList[i.key()] = GetKeyState(i.value()) & 0x8000;
    }
}

void MainWindow::updateSamos(Samos *s)
{
    if (s->getOnGround() || !s->getIsAffectedByGravity()) {
        if ((s->getState() == "Jumping") || (s->getState() == "SpinJump") || (s->getState() == "Falling") || (s->getState() == "JumpEnd")) {
            if (s->getState() == "Falling") {
                s->setY(s->getY() - (s->getBox()->getHeight() * 0.2 - 3 * renderingMultiplier));
            } else if (s->getState() == "SpinJump") {
                s->setY(s->getY() - (s->getBox()->getHeight() * 2 - 14 * renderingMultiplier));
            }
            s->setState("Landing");
        } else if (inputList["left"] && !inputList["right"]) {
            if (s->getVX() > -400) {
                s->setVX(s->getVX() - 30);
            } else if (s->getVX() < -400 && s->getVX() > -420) {
                s->setVX(-420);
            }
            s->setFrictionFactor(0.1);
            s->setFacing("Left");
            if (inputList["jump"] && s->getJumpTime() == -1) {
                s->setVY(-350);
                s->setJumpTime(0);
                s->setState("SpinJump");
            } else {
                s->setState("Walking");
            }
        } else if (!inputList["left"] && inputList["right"]) {
            if (s->getVX() < 400) {
                s->setVX(s->getVX() + 30);
            } else if (s->getVX() > 400 && s->getVX() < 420) {
                s->setVX(420);
            }
            s->setFrictionFactor(0.1);
            s->setFacing("Right");
            if (inputList["jump"] && s->getJumpTime() == -1) {
                s->setVY(-350);
                s->setJumpTime(0);
                s->setState("SpinJump");
            } else {
                s->setState("Walking");
            }
        } else if ((!inputList["left"] && !inputList["right"]) || (inputList["left"] && inputList["right"])) {
            s->setFrictionFactor(1);
            if (inputList["jump"] && s->getJumpTime() == -1) {
                s->setVY(-350);
                s->setJumpTime(0);
                s->setState("Jumping");
            } else {
                s->setState("Standing");
            }
        }
        if (!inputList["jump"]) {
            s->setJumpTime(-1);
        }
    } else {
        if (inputList["left"] && !inputList["right"]) {
            if (s->getVX() > -200) {
                s->setVX(s->getVX() - 30);
            } else if (s->getVX() < -200 && s->getVX() > -220) {
                s->setVX(-220);
            }
            s->setFrictionFactor(0.1);
            s->setFacing("Left");
            if (inputList["jump"] && s->getJumpTime() < 20 && s->getJumpTime() >= 0) {
                s->setVY(s->getVY() - 10);
                s->setJumpTime(s->getJumpTime() + 1);
            } else {
                s->setJumpTime(20);
            }
            if (!(s->getState() == "SpinJump")) {
                s->setState("Falling");
            }
        } else if (!inputList["left"] && inputList["right"]) {
            if (s->getVX() < 200) {
                s->setVX(s->getVX() + 30);
            } else if (s->getVX() > 200 && s->getVX() < 220) {
                s->setVX(220);
            }
            s->setFrictionFactor(0.1);
            s->setFacing("Right");
            if (inputList["jump"] && s->getJumpTime() < 20 && s->getJumpTime() >= 0) {
                s->setVY(s->getVY() - 10);
                s->setJumpTime(s->getJumpTime() + 1);
            } else {
                s->setJumpTime(20);
            }
            if (!(s->getState() == "SpinJump")) {
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
            if (!(s->getState() == "SpinJump") && !(s->getState() == "Jumping") && !(s->getState() == "JumpEnd")) {
                s->setState("Falling");
            }
        }

    }

    nlohmann::json entJson = Entity::values["names"]["Samos"];

    if (s->getState() == "SpinJump") {
        s->setBox(new CollisionBox(static_cast<int>(entJson["offset_x"]) * renderingMultiplier,
                  (static_cast<int>(entJson["offset_y"]) + 14) * renderingMultiplier,
                  static_cast<int>(entJson["width"]) * renderingMultiplier,
                  static_cast<int>(entJson["height"]) * renderingMultiplier / 3));
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 2));
    } else if (s->getState() == "Falling") {
        s->setBox(new CollisionBox(static_cast<int>(entJson["offset_x"]) * renderingMultiplier,
                  (static_cast<int>(entJson["offset_y"]) + 3) * renderingMultiplier,
                  static_cast<int>(entJson["width"]) * renderingMultiplier,
                  static_cast<int>(entJson["height"]) * renderingMultiplier / 1.2));
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 2));
    } else {
        s->setBox(new CollisionBox(static_cast<int>(entJson["offset_x"]) * renderingMultiplier,
                  static_cast<int>(entJson["offset_y"]) * renderingMultiplier,
                  static_cast<int>(entJson["width"]) * renderingMultiplier,
                  static_cast<int>(entJson["height"]) * renderingMultiplier));
        s->setGroundBox(new CollisionBox(s->getBox()->getX(), s->getBox()->getY() + s->getBox()->getHeight(), s->getBox()->getWidth(), 2));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    running = false;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    for (Entity *entity : rendering) {
        if (entity->getTexture() == nullptr)
            entity->setTexture(&errorTexture);
        painter.drawImage(QRect(entity->getX() + entity->getTexture()->offset().x(), entity->getY() + entity->getTexture()->offset().y(),
                                entity->getTexture()->width() * renderingMultiplier, entity->getTexture()->height() * renderingMultiplier),
                          *entity->getTexture());
        if (renderHitboxes) {
            painter.setPen(QColor("blue"));
            painter.drawRect(entity->getX() + entity->getBox()->getX(), entity->getY() + entity->getBox()->getY(),
                             entity->getBox()->getWidth(), entity->getBox()->getHeight());
            if (entity->getEntType() == "Samos" || entity->getEntType() == "Monster" || entity->getEntType() == "NPC" || entity->getEntType() == "DynamicObj") {
                Living* liv = static_cast<Living*>(entity);
                painter.setPen(QColor("green"));
                painter.drawRect(liv->getX() + liv->getGroundBox()->getX(), liv->getY() + liv->getGroundBox()->getY(),
                                 liv->getGroundBox()->getWidth(), liv->getGroundBox()->getHeight());
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
    std::vector<Entity*> solidList;
    std::vector<Living*> livingList;
    for (Entity* ent : rendering) {
        if (ent->getEntType() == "Samos" || ent->getEntType() == "Monster" || ent->getEntType() == "NPC" || ent->getEntType() == "DynamicObj") {
            Living* liv = static_cast<Living*>(ent);
            livingList.push_back(liv);
            if (liv->getIsAffectedByGravity() && !liv->getOnGround() && liv->getIsMovable()) {
                liv->setVY(liv->getVY() + MainWindow::gravity / MainWindow::frameRate);
            }
            if (liv->getOnGround() && liv->getIsMovable()) {
                liv->setVX(liv->getVX() * (1.0 - 0.1 * std::abs(liv->getVX()) * liv->getFrictionFactor() / MainWindow::frameRate));
            } else if (liv->getIsMovable())
                liv->setVX(liv->getVX() * (1.0 - 0.01 * std::abs(liv->getVX()) * liv->getFrictionFactor() / MainWindow::frameRate));
        } else {
            if (ent->getIsAffectedByGravity() && ent->getIsMovable())
                ent->setVY(ent->getVY() + MainWindow::gravity / MainWindow::frameRate);
            if (ent->getIsMovable())
                ent->setVX(ent->getVX() * (1.0 - 0.01 * std::abs(ent->getVX()) * ent->getFrictionFactor() / MainWindow::frameRate));
        }
        if (ent->getEntType() == "Terrain" || ent->getEntType() == "DynamicObj") {
            solidList.push_back(ent);
        }
        if (ent->getIsMovable())
            ent->updateV(MainWindow::frameRate);
    } //{Null, Terrain, Samos, Monster, Area, DynamicObj, NPC, Projectile};

    for (std::vector<Entity*>::iterator i = rendering.begin(); i != rendering.end(); i++) {
        for (std::vector<Entity*>::iterator j = i+1; j!= rendering.end(); j++) {
            if (Entity::checkCollision(*i,*j)) {
                Entity::handleCollision(*i,*j);
            }
        }
    }

    for (std::vector<Living*>::iterator i = livingList.begin(); i != livingList.end(); i++) {
        (*i)->setOnGround(false);
        for (std::vector<Entity*>::iterator j = solidList.begin(); j!= solidList.end(); j++) {
            if (Living::checkOn(*i,*j)) {
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
            // Because the animation changed, reset its index
            entity->setAnimation(0);
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
