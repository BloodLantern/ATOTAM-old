#include "mainwindow.h"
#include "ui_mainwindow.h"

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

void MainWindow::updateGame()
{
    for (std::vector<Entity*>::iterator i = rendering.begin(); i != rendering.end(); i++) {
        for (std::vector<Entity*>::iterator j = i+1; j!= rendering.end(); j++) {
            bool toCheck = true;
            Entity::EntityType jType = (*j)->getEntType();
            // Checking if collision check is necessary
            switch ((*i)->getEntType()) {
            case Entity::EntityType::Terrain:
                toCheck = false;
                break;
            case Entity::EntityType::Monster:
                if (jType == Entity::EntityType::Samos)
                    toCheck = false;
                break;
            case Entity::EntityType::DynamicObj:
                if (jType == Entity::EntityType::Samos || jType == Entity::EntityType::Monster)
                    toCheck = false;
                break;
            case Entity::EntityType::NPC:
                if (jType == Entity::EntityType::Samos || jType == Entity::EntityType::Monster || jType == Entity::EntityType::DynamicObj)
                    toCheck = false;
                break;
            case Entity::EntityType::Area:
                if (jType != Entity::EntityType::Area)
                    toCheck = false;
                break;
            default:
                // Samos is always true
                break;
            }
            if (toCheck)
                if (Entity::checkCollision(*i,*j)) {

                }
        }
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
