#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

bool MainWindow::running = true;
double MainWindow::frameRate = 60.0;
double MainWindow::gravity = 60.0;

MainWindow::MainWindow(QApplication *app)
    : ui(new Ui::MainWindow)
    , m_qApp(app)
    //, rendering()
{
    ui->setupUi(this);
    // Set size of the window
    setFixedSize(1000, 500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    for (Entity *entity : rendering) {
        painter.drawImage(QPoint(entity->getX(), entity->getY()), *entity->getTexture());
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
    for (Entity* ent : rendering) {
        if (ent->getIsAffectedByGravity())
            ent->setVY(ent->getVY() + MainWindow::gravity/MainWindow::frameRate);
        ent->updateV(MainWindow::frameRate);
    }
    for (std::vector<Entity*>::iterator i = rendering.begin(); i != rendering.end(); i++) {
        for (std::vector<Entity*>::iterator j = i+1; j!= rendering.end(); j++) {
            if (Entity::checkCollision(*i,*j)) {

            }
        }
    }
}

void MainWindow::updateAnimations()
{
    for (Entity* entity : rendering) {
        entity->setAnimation(entity->getAnimation() + 1);
        if (entity->getMaxAnimation() <= entity->getAnimation())
            entity->setAnimation(0);
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
