#include "mainwindow.h"
#include "ui_mainwindow.h"

bool MainWindow::running = true;
double MainWindow::frameRate = 60.0;
unsigned long long MainWindow::frameCount = 0;
double MainWindow::gravity = 300.0;
std::map<std::string, bool> MainWindow::inputList;

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

nlohmann::json MainWindow::keyCodes = MainWindow::loadKeyCodes();

void MainWindow::getInputs()
{
    for (nlohmann::json::iterator i = MainWindow::keyCodes.begin(); i != MainWindow::keyCodes.end(); i++) {
        MainWindow::inputList[i.key()] = GetKeyState(i.value()) & 0x8000;
    }
}

void MainWindow::updateSamos(Samos *s)
{
    if (inputList["left"] && !inputList["right"]) {
        if (s->getVX() > -400) {
            s->setVX(s->getVX() - 30);
        } else if (s->getVX() < -400 && s->getVX() > -420) {
            s->setVX(-420);
        }
        s->setFrictionFactor(0.1);
        s->setFacing("Left");
        if (s->getOnGround()) {
            s->setState("Walking");
        } else {
            s->setState("Falling");
        }
    } else if (!inputList["left"] && inputList["right"]) {
        if (s->getVX() < 400) {
            s->setVX(s->getVX() + 30);
        } else if (s->getVX() > 400 && s->getVX() < 420) {
            s->setVX(420);
        }
        s->setFrictionFactor(0.1);
        s->setFacing("Right");
        if (s->getOnGround()) {
            s->setState("Walking");
        } else {
            s->setState("Falling");
        }
    } else if ((!inputList["left"] && !inputList["right"]) || (inputList["left"] && inputList["right"])) {
        s->setFrictionFactor(1);
        if (s->getOnGround()) {
            s->setState("Idle");
        }  else {
            s->setState("Falling");
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
    for (Entity *entity : rendering) {
        painter.drawImage(QRect(entity->getX(), entity->getY(), entity->getTexture()->width() * renderingMultiplier, entity->getTexture()->height() * renderingMultiplier),
                          *entity->getTexture());
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
            if (liv->getIsAffectedByGravity() && !liv->getOnGround())
                liv->setVY(ent->getVY() + MainWindow::gravity / MainWindow::frameRate);
            if (liv->getOnGround())
                ent->setVX(ent->getVX() * (1.0 - 0.1 * std::abs(ent->getVX()) * ent->getFrictionFactor() / MainWindow::frameRate));
            else ent->setVX(ent->getVX() * (1.0 - 0.01 * std::abs(ent->getVX()) * ent->getFrictionFactor() / MainWindow::frameRate));
        } else {
            if (ent->getIsAffectedByGravity())
                ent->setVY(ent->getVY() + MainWindow::gravity / MainWindow::frameRate);
            ent->setVX(ent->getVX() * (1.0 - 0.01 * std::abs(ent->getVX()) * ent->getFrictionFactor() / MainWindow::frameRate));
        }
        if (ent->getEntType() == "Terrain" || ent->getEntType() == "DynamicObj") {
            solidList.push_back(ent);
        }
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
        if (entity->getState() != entity->getLastFrameState()) {
            entity->setCurrentAnimation(entity->updateAnimation(entity->getState()));
            entity->setLastFrameState(entity->getState());
        }
        entity->setAnimation(entity->getAnimation() + 1);
        if (entity->getCurrentAnimation().size() <= entity->getAnimation())
            entity->setAnimation(0);
        entity->updateTexture();
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
