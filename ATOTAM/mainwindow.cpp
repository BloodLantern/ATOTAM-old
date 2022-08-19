#include "editorpreview.h"
#include "mainwindow.h"
#include "qcoreevent.h"
#include "physics.h"
#include "qevent.h"

#include <QApplication>
#include <QScreen>
#include <Entities/area.h>
#include <Entities/door.h>
#include <Entities/npc.h>
#include <QSplitter>
#include <iostream>

MainWindow::MainWindow(QApplication *app, std::string assetsPath)
    : m_qApp(app)
    , game(new Game(assetsPath, "1"))
    , errorTexture(QString::fromStdString(game->getAssetsPath() + "/textures/error.png"))
    , emptyTexture(QString::fromStdString(game->getAssetsPath() + "/textures/empty.png"))
    , showHUD(true)
{
    setFixedSize(game->getResolution().first, game->getResolution().second);

    renderingMultiplier = Entity::values["general"]["renderingMultiplier"];

    if (game->getFullscreen())
        showFullScreen();
    else
        show();

    nlohmann::json editorJson = game->loadJson("map_editor");
    if (editorJson["enabled"])
        setupEditorWindow(editorJson);
    QCoreApplication::instance()->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete game;
    if (editorWindow != nullptr) {
        editorWindow->close();
        delete editorWindow;
    }
}

bool MainWindow::eventFilter(QObject* object, QEvent* event) {
     if (event->type() == QEvent::Paint && !render && object == this)
         return true;
     else if (event->type() == QEvent::KeyPress && object == this) {
         QKeyEvent * keyEvent = dynamic_cast<QKeyEvent*>(event);
         if (keyEvent->key() == Qt::Key_Tab)
             return true;
     }
     return false;
}

void MainWindow::getInputs()
{
    for (std::map<std::string, bool>::iterator i = game->getInputList()->begin(); i != game->getInputList()->end(); i ++) {
        if (i->second)
            (*game->getInputTime())[i->first] += 1 / Physics::frameRate;
        else
            (*game->getInputTime())[i->first] = 0;
    }

    //Only listen for inputs if the window is currently selected
    if (isActiveWindow()) {
        //Check every key
        for (nlohmann::json::iterator i = game->getKeyCodes().begin(); i != game->getKeyCodes().end(); i++) {
            (*game->getInputList())[i.key()] = GetKeyState(i.value()) & 0x8000;
        }
    } else
        //Reset the keys if the window is not selected
        for (nlohmann::json::iterator i = game->getKeyCodes().begin(); i != game->getKeyCodes().end(); i++) {
            (*game->getInputList())[i.key()] = 0;
        }
}

void MainWindow::setupEditorWindow(nlohmann::json editorJson)
{
    // Map preview
    Map editedMap = Map::loadMap(editorJson["map"]["id"], game->getAssetsPath());
    EditorPreview* preview = new EditorPreview(editedMap.loadRooms(), &errorTexture, &emptyTexture, &renderingMultiplier);

    // Window
    editorWindow = new EditorWindow(preview);
    editorWindow->setWindowTitle("ATOTAM Map Editor");
    editorWindow->setMinimumSize(QSize(800, 600));
    // Set window screen
    QList<QScreen*> screens = m_qApp->screens();
    if (screens.size() > 1) {
        editorWindow->setScreen(screens[1]);
        editorWindow->setGeometry(screens[1]->geometry().center().x() - 800/2, screens[1]->geometry().center().y() - 600/2, 800, 600);
    }

    // Widgets inside the window

    // Align widgets
    QSplitter* splitter = new QSplitter;
    splitter->addWidget(preview);

    editorWindow->setCentralWidget(splitter);
    editorWindow->show();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    game->setRunning(false);
    game->clearEntities();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    render = false;
    QPainter painter(this);
    Game tempG = *game;
    QPoint tempC = tempG.getCamera();

    if (!tempG.getInInventory() && !tempG.getInMap()) {
        for (std::vector<NPC*>::iterator ent = tempG.getNPCs()->begin(); ent != tempG.getNPCs()->end(); ent++) {
            // Make sure not to use a null pointer in case there is one
            if (*ent == nullptr)
                continue;

            NPC tEnt = **ent;

            // If the texture is null, set it to the empty texture
            if (tEnt.getTexture() == nullptr) {
                (*ent)->setTexture(&emptyTexture);
                tEnt.setTexture(&emptyTexture);
            }

            QImage tTex = *tEnt.getTexture();

            // Make sure not to draw the Entities that aren't visible
            if (tEnt.getX() + tTex.offset().x() + tTex.width() * renderingMultiplier < tempC.x() // If too much on the left
                    || tEnt.getX() + tTex.offset().x() > tempC.x() + game->getCameraSize().first // If too much on the right
                    || tEnt.getY() + tTex.offset().y() + tTex.height() * renderingMultiplier < tempC.y() // If too high
                    || tEnt.getY() + tTex.offset().y() > tempC.y() + game->getCameraSize().second) { // If too low
                tEnt.setBoxNoDel(nullptr);
                tEnt.setGroundBoxNoDel(nullptr);
                continue;
            }

            //Try to draw the texture: if it fails, set it to the error texture and try again
            try {
                if (tEnt.getITime() > 0.0 && (tempG.getUpdateCount() % 2 == 0)) {
                    QColor base_color(200,200,200,200);

                    QImage new_image = tTex;
                    QPainter pa(&new_image);
                    pa.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    pa.fillRect(new_image.rect(), base_color);
                    pa.end();
                    painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempG.getCamera().y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        new_image);
                } else {
                    painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        tTex);
                }
            } catch (...) {
                (*ent)->setTexture(&errorTexture);
                tEnt.setTexture(&errorTexture);
                painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        tTex);
            }
            tEnt.setBoxNoDel(nullptr);
            tEnt.setGroundBoxNoDel(nullptr);
        }

        if (tempG.getS() != nullptr) {

            Samos tS = *tempG.getS();

            //If the texture is null, set it to the empty texture
            if (tS.getTexture() == nullptr) {
                tempG.getS()->setTexture(&emptyTexture);
                tS.setTexture(&emptyTexture);
            }

            QImage tTex = *tS.getTexture();

            //Try to draw the texture: if it fails, set it to the error texture and try again
            try {
                if (tS.getITime() > 0.0 && (tempG.getUpdateCount() % 2 == 0)) {
                    QColor base_color(200,200,200,200);

                    QImage new_image = tTex;
                    QPainter pa(&new_image);
                    pa.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    pa.fillRect(new_image.rect(), base_color);
                    pa.end();
                    painter.drawImage(QRect(tS.getX() + tTex.offset().x() - tempC.x(), tS.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        new_image);
                } else {
                    painter.drawImage(QRect(tS.getX() + tTex.offset().x() - tempC.x(), tS.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        tTex);
                }
            } catch (...) {
                tempG.getS()->setTexture(&errorTexture);
                tS.setTexture(&errorTexture);
                painter.drawImage(QRect(tS.getX() + tTex.offset().x() - tempC.x(), tS.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        tTex);
            }
            tS.setBoxNoDel(nullptr);
            tS.setGroundBoxNoDel(nullptr);
            tS.setWallBoxLNoDel(nullptr);
            tS.setWallBoxRNoDel(nullptr);
        }

        for (std::vector<Monster*>::iterator ent = tempG.getMonsters()->begin(); ent != tempG.getMonsters()->end(); ent++) {
            if (*ent == nullptr)
                continue;

            Monster tEnt = **ent;

            // If the texture is null, set it to the empty texture
            if (tEnt.getTexture() == nullptr) {
                (*ent)->setTexture(&emptyTexture);
                tEnt.setTexture(&emptyTexture);
            }

            QImage tTex = *tEnt.getTexture();

            // Make sure not to draw the Entities that aren't visible
            if (tEnt.getX() + tTex.offset().x() + tTex.width() * renderingMultiplier < tempC.x() // If too much on the left
                    || tEnt.getX() + tTex.offset().x() > tempC.x() + game->getCameraSize().first // If too much on the right
                    || tEnt.getY() + tTex.offset().y() + tTex.height() * renderingMultiplier < tempC.y() // If too high
                    || tEnt.getY() + tTex.offset().y() > tempC.y() + game->getCameraSize().second) { // If too low
                tEnt.setBoxNoDel(nullptr);
                tEnt.setGroundBoxNoDel(nullptr);
                continue;
            }

            //Try to draw the texture: if it fails, set it to the error texture and try again
            try {
                if (tEnt.getITime() > 0.0 && (tempG.getUpdateCount() % 2 == 0)) {
                    QColor base_color(200,200,200,200);

                    QImage new_image = *tEnt.getTexture();
                    QPainter pa(&new_image);
                    pa.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    pa.fillRect(new_image.rect(), base_color);
                    pa.end();
                    painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        new_image);
                } else {
                    painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        tTex);
                }
            } catch (...) {
                (*ent)->setTexture(&errorTexture);
                tEnt.setTexture(&errorTexture);
                painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        tTex);
            }
            tEnt.setBoxNoDel(nullptr);
            tEnt.setGroundBoxNoDel(nullptr);
        }

        for (std::vector<DynamicObj*>::iterator ent = tempG.getDynamicObjs()->begin(); ent != tempG.getDynamicObjs()->end(); ent++) {
            if (*ent == nullptr)
                continue;

            DynamicObj tEnt = **ent;

            // If the texture is null, set it to the empty texture
            if (tEnt.getTexture() == nullptr) {
                (*ent)->setTexture(&emptyTexture);
                tEnt.setTexture(&emptyTexture);
            }

            QImage tTex = *tEnt.getTexture();

            // Make sure not to draw the Entities that aren't visible
            if (tEnt.getX() + tTex.offset().x() + tTex.width() * renderingMultiplier < tempC.x() // If too much on the left
                    || tEnt.getX() + tTex.offset().x() > tempC.x() + game->getCameraSize().first // If too much on the right
                    || tEnt.getY() + tTex.offset().y() + tTex.height() * renderingMultiplier < tempC.y() // If too high
                    || tEnt.getY() + tTex.offset().y() > tempC.y() + game->getCameraSize().second) { // If too low
                tEnt.setBoxNoDel(nullptr);
                tEnt.setGroundBoxNoDel(nullptr);
                continue;
            }

            //Try to draw the texture: if it fails, set it to the error texture and try again
            try {
                if (tEnt.getITime() > 0.0 && (tempG.getUpdateCount() % 2 == 0)) {
                    QColor base_color(200,200,200,200);

                    QImage new_image = *tEnt.getTexture();
                    QPainter pa(&new_image);
                    pa.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                    pa.fillRect(new_image.rect(), base_color);
                    pa.end();
                    painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        new_image);
                } else {
                    painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        tTex);
                }
            } catch (...) {
                (*ent)->setTexture(&errorTexture);
                tEnt.setTexture(&errorTexture);
                painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        tTex);
            }
            tEnt.setBoxNoDel(nullptr);
            tEnt.setGroundBoxNoDel(nullptr);
        }

        for (std::vector<Terrain*>::iterator ent = tempG.getTerrains()->begin(); ent != tempG.getTerrains()->end(); ent++) {
            if (*ent == nullptr)
                continue;

            Terrain tEnt = **ent;

            // If the texture is null, set it to the empty texture
            if (tEnt.getTexture() == nullptr) {
                (*ent)->setTexture(&emptyTexture);
                tEnt.setTexture(&emptyTexture);
            }

            QImage tTex = *tEnt.getTexture();
            // Make sure not to draw the Entities that aren't visible
            if (tEnt.getX() + tTex.offset().x() + tTex.width() * renderingMultiplier < tempC.x() // If too much on the left
                    || tEnt.getX() + tTex.offset().x() > tempC.x() + game->getCameraSize().first // If too much on the right
                    || tEnt.getY() + tTex.offset().y() + tTex.height() * renderingMultiplier < tempC.y() // If too high
                    || tEnt.getY() + tTex.offset().y() > tempC.y() + game->getCameraSize().second) { // If too low
                tEnt.setBoxNoDel(nullptr);
                continue;
            }

            //Try to draw the texture: if it fails, set it to the error texture and try again
            try {
                painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                    tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                    tTex);
            } catch (...) {
                (*ent)->setTexture(&errorTexture);
                tEnt.setTexture(&errorTexture);
                painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        tTex);
            }
            tEnt.setBoxNoDel(nullptr);
        }

        for (std::vector<Projectile*>::iterator ent = tempG.getProjectiles()->begin(); ent != tempG.getProjectiles()->end(); ent++) {
            if (*ent == nullptr)
                continue;

            Projectile tEnt = **ent;

            // If the texture is null, set it to the empty texture
            if (tEnt.getTexture() == nullptr) {
                (*ent)->setTexture(&emptyTexture);
                tEnt.setTexture(&emptyTexture);
            }

            QImage tTex = *tEnt.getTexture();

            // Make sure not to draw the Entities that aren't visible
            if (tEnt.getX() + tTex.offset().x() + tTex.width() * renderingMultiplier < tempC.x() // If too much on the left
                    || tEnt.getX() + tTex.offset().x() > tempC.x() + game->getCameraSize().first // If too much on the right
                    || tEnt.getY() + tTex.offset().y() + tTex.height() * renderingMultiplier < tempC.y() // If too high
                    || tEnt.getY() + tTex.offset().y() > tempC.y() + game->getCameraSize().second) { // If too low
                tEnt.setBoxNoDel(nullptr);
                continue;
            }

            //Try to draw the texture: if it fails, set it to the error texture and try again
            try {
                painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                    tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                    tTex);
            } catch (...) {
                (*ent)->setTexture(&errorTexture);
                tEnt.setTexture(&errorTexture);
                painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        tTex);
            }
            tEnt.setBoxNoDel(nullptr);
        }

        for (std::vector<Area*>::iterator ent = tempG.getAreas()->begin(); ent != tempG.getAreas()->end(); ent++) {
            if (*ent == nullptr)
                continue;

            Area tEnt = **ent;

            // If the texture is null, set it to the empty texture
            if (tEnt.getTexture() == nullptr) {
                (*ent)->setTexture(&emptyTexture);
                tEnt.setTexture(&emptyTexture);
            }

            QImage tTex = *tEnt.getTexture();

            // Make sure not to draw the Entities that aren't visible
            if (tEnt.getX() + tTex.offset().x() + tTex.width() * renderingMultiplier < tempC.x() // If too much on the left
                    || tEnt.getX() + tTex.offset().x() > tempC.x() + game->getCameraSize().first // If too much on the right
                    || tEnt.getY() + tTex.offset().y() + tTex.height() * renderingMultiplier < tempC.y() // If too high
                    || tEnt.getY() + tTex.offset().y() > tempC.y() + game->getCameraSize().second) { // If too low
                tEnt.setBoxNoDel(nullptr);
                continue;
            }

            //Try to draw the texture: if it fails, set it to the error texture and try again
            try {
                painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                    tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                    tTex);
            } catch (...) {
                (*ent)->setTexture(&errorTexture);
                tEnt.setTexture(&errorTexture);
                painter.drawImage(QRect(tEnt.getX() + tTex.offset().x() - tempC.x(), tEnt.getY() + tTex.offset().y() - tempC.y(),
                                        tTex.width() * renderingMultiplier, tTex.height() * renderingMultiplier),
                                        tTex);
            }
            tEnt.setBoxNoDel(nullptr);
        }



        //Draw hitboxes if necessary
        if (tempG.getRenderHitboxes()) {

            for (std::vector<NPC*>::iterator ent = tempG.getNPCs()->begin(); ent != tempG.getNPCs()->end(); ent++) {
                painter.setPen(QColor("blue"));
                NPC tEnt = **ent;
                if (tEnt.getBox() != nullptr) {
                    CollisionBox tBox = *tEnt.getBox();
                    painter.drawRect(tEnt.getX() + tBox.getX() - tempC.x(), tEnt.getY() + tBox.getY() - tempC.y(),
                                     tBox.getWidth(), tBox.getHeight());
                }

                //Ground boxes);
                painter.setPen(QColor("green"));
                if (tEnt.getGroundBox() != nullptr) {
                    CollisionBox tGBox = *tEnt.getGroundBox();
                    painter.drawRect(tEnt.getX() + tGBox.getX() - tempC.x(), tEnt.getY() + tGBox.getY() - tempC.y(),
                                    tGBox.getWidth(), tGBox.getHeight());
                }
                tEnt.setBoxNoDel(nullptr);
                tEnt.setGroundBoxNoDel(nullptr);
            }

            if (tempG.getS() != nullptr) {
                painter.setPen(QColor("orange"));
                Samos tS = *tempG.getS();
                if (tS.getBox() != nullptr) {
                    CollisionBox tBox = *tS.getBox();
                    painter.drawRect(tS.getX() + tBox.getX() - tempC.x(), tS.getY() + tBox.getY() - tempC.y(),
                                     tBox.getWidth(), tBox.getHeight());
                }

                //Ground boxes
                painter.setPen(QColor("green"));
                if (tS.getGroundBox() != nullptr) {
                    CollisionBox tGBox = *tS.getGroundBox();
                    painter.drawRect(tS.getX() + tGBox.getX() - tempC.x(), tS.getY() + tGBox.getY() - tempC.y(),
                                    tGBox.getWidth(), tGBox.getHeight());
                }

                //Wall boxes
                if (tS.getWallBoxL() != nullptr) {
                    CollisionBox tWBoxL = *tS.getWallBoxL();
                    painter.drawRect(tS.getX() + tWBoxL.getX() - tempC.x(), tS.getY() + tWBoxL.getY() - tempC.y(),
                                    tWBoxL.getWidth(), tWBoxL.getHeight());
                }
                if (tS.getWallBoxR() != nullptr) {
                    CollisionBox tWBoxR = *tS.getWallBoxR();
                    painter.drawRect(tS.getX() + tWBoxR.getX() - tempC.x(), tS.getY() + tWBoxR.getY() - tempC.y(),
                                    tWBoxR.getWidth(), tWBoxR.getHeight());
                }
                tS.setBoxNoDel(nullptr);
                tS.setGroundBoxNoDel(nullptr);
                tS.setWallBoxLNoDel(nullptr);
                tS.setWallBoxRNoDel(nullptr);
            }

            for (std::vector<Monster*>::iterator ent = tempG.getMonsters()->begin(); ent != tempG.getMonsters()->end(); ent++) {
                painter.setPen(QColor("red"));
                Monster tEnt = **ent;
                if (tEnt.getBox() != nullptr) {
                    CollisionBox tBox = *tEnt.getBox();
                    painter.drawRect(tEnt.getX() + tBox.getX() - tempC.x(), tEnt.getY() + tBox.getY() - tempC.y(),
                                     tBox.getWidth(), tBox.getHeight());
                }

                //Ground boxes);
                painter.setPen(QColor("green"));
                if (tEnt.getGroundBox() != nullptr) {
                    CollisionBox tGBox = *tEnt.getGroundBox();
                    painter.drawRect(tEnt.getX() + tGBox.getX() - tempC.x(), tEnt.getY() + tGBox.getY() - tempC.y(),
                                    tGBox.getWidth(), tGBox.getHeight());
                }
                tEnt.setBoxNoDel(nullptr);
                tEnt.setGroundBoxNoDel(nullptr);
            }

            for (std::vector<DynamicObj*>::iterator ent = tempG.getDynamicObjs()->begin(); ent != tempG.getDynamicObjs()->end(); ent++) {
                painter.setPen(QColor("blue"));
                DynamicObj tEnt = **ent;
                if (tEnt.getBox() != nullptr) {
                    CollisionBox tBox = *tEnt.getBox();
                    painter.drawRect(tEnt.getX() + tBox.getX() - tempC.x(), tEnt.getY() + tBox.getY() - tempC.y(),
                                     tBox.getWidth(), tBox.getHeight());
                }

                //Ground boxes);
                painter.setPen(QColor("green"));
                if (tEnt.getGroundBox() != nullptr) {
                    CollisionBox tGBox = *tEnt.getGroundBox();
                    painter.drawRect(tEnt.getX() + tGBox.getX() - tempC.x(), tEnt.getY() + tGBox.getY() - tempC.y(),
                                    tGBox.getWidth(), tGBox.getHeight());
                }
                tEnt.setBoxNoDel(nullptr);
                tEnt.setGroundBoxNoDel(nullptr);
            }

            for (std::vector<Terrain*>::iterator ent = tempG.getTerrains()->begin(); ent != tempG.getTerrains()->end(); ent++) {
                painter.setPen(QColor("blue"));
                Terrain tEnt = **ent;
                if (tEnt.getBox() != nullptr) {
                    CollisionBox tBox = *tEnt.getBox();
                    painter.drawRect(tEnt.getX() + tBox.getX() - tempC.x(), tEnt.getY() + tBox.getY() - tempC.y(),
                                     tBox.getWidth(), tBox.getHeight());
                }
                tEnt.setBoxNoDel(nullptr);
            }

            for (std::vector<Projectile*>::iterator ent = tempG.getProjectiles()->begin(); ent != tempG.getProjectiles()->end(); ent++) {
                painter.setPen(QColor("blue"));
                Projectile tEnt = **ent;
                if (tEnt.getBox() != nullptr) {
                    CollisionBox tBox = *tEnt.getBox();
                    painter.drawRect(tEnt.getX() + tBox.getX() - tempC.x(), tEnt.getY() + tBox.getY() - tempC.y(),
                                     tBox.getWidth(), tBox.getHeight());
                }
                tEnt.setBoxNoDel(nullptr);
            }

            for (std::vector<Area*>::iterator ent = tempG.getAreas()->begin(); ent != tempG.getAreas()->end(); ent++) {
                painter.setPen(QColor("gray"));
                Area tEnt = **ent;
                if (tEnt.getBox() != nullptr) {
                    CollisionBox tBox = *tEnt.getBox();
                    painter.drawRect(tEnt.getX() + tBox.getX() - tempC.x(), tEnt.getY() + tBox.getY() - tempC.y(),
                                     tBox.getWidth(), tBox.getHeight());
                }
                tEnt.setBoxNoDel(nullptr);
            }

            nlohmann::json mapJson = tempG.getCurrentMap().getJson()["rooms"][std::to_string(tempG.getCurrentMap().getCurrentRoomId())];

            painter.setPen(QColor("magenta"));
            painter.drawRect(static_cast<int>(mapJson["position"][0]) - tempC.x(), static_cast<int>(mapJson["position"][1]) - tempC.y(), mapJson["size"][0], mapJson["size"][1]);
        }

        // Draw fps if necessary
        if (tempG.getShowFps()) {
            if ((std::chrono::high_resolution_clock::now() - tempG.getLastFpsShown()).count() > tempG.getShowFpsUpdateRate()) {
                tempG.setLastFpsShown(std::chrono::high_resolution_clock::now());
            }
            painter.setPen(QColor("black"));
            painter.drawText(QPoint(2, 12), QString::fromStdString(std::to_string(tempG.getFps()) + " FPS"));
        }

        QFont f = painter.font();
        f.setPointSize(f.pointSize() * 2);
        painter.setFont(f);

        //HUD
        if (tempG.getS() != nullptr && showHUD) {

            painter.setPen(QColor("black"));

            Samos tS = * tempG.getS();

            //Selected weapon
            painter.fillRect(QRect(70,15,100,30), QColor("white"));
            painter.drawRect(QRect(70,15,100,30));
            painter.drawText(QPoint(80, 40), tempG.translate(tS.getSelectedWeapon(), std::vector<std::string>({"ui", "selectedWeapon"})));

            //Missile count
            painter.fillRect(QRect(200,15,70,30), QColor("white"));
            painter.drawRect(QRect(200,15,70,30));
            painter.drawText(QPoint(210, 40), QString::fromStdString("M : " + std::to_string(tS.getMissileCount())));

            //Grenade count
            painter.fillRect(QRect(290,15,70,30), QColor("white"));
            painter.drawRect(QRect(290,15,70,30));
            painter.drawText(QPoint(300, 40), QString::fromStdString("G : " + std::to_string(tS.getGrenadeCount())));

            //Health count
            painter.fillRect(QRect(380,15,80,30), QColor("white"));
            painter.drawRect(QRect(380,15,80,30));
            painter.drawText(QPoint(390, 40), QString::fromStdString("H : " + std::to_string(tS.getHealth())));

            tS.setBoxNoDel(nullptr);
            tS.setGroundBoxNoDel(nullptr);
            tS.setWallBoxLNoDel(nullptr);
            tS.setWallBoxRNoDel(nullptr);
        }

        // Dialogue
        if (!tempG.getCurrentDialogue().isNull()) {
            // In case we changed it before
            painter.setPen(QColor("black"));

            painter.drawText(QRectF(QRect(200, 200, 1000, 200)),
                             QString::fromStdString(tempG.getCurrentDialogue().getTalkingName() + ": " + tempG.getCurrentDialogue().getText()[tempG.getCurrentDialogue().getTextAdvancement()]),
                    QTextOption(Qt::AlignHCenter));
        }

        //Menu
        if (tempG.getIsPaused()) {
            painter.fillRect(QRect(0,0,size().width(),size().height()), QBrush(QColor(0,0,0,200)));
            for (unsigned int i = 0; i < tempG.getMenuOptions().size(); i++) {
                painter.setPen(QColor("white"));
                if (tempG.getSelectedOption() == (int) i)
                    painter.setPen(QColor("cyan"));
                painter.drawText(0, size().height() / 2 - 15 * tempG.getMenuOptions().size() + 30 * i, size().width(), 50, Qt::AlignHCenter, QString::fromStdString(tempG.getMenuOptions()[i]));
            }
        }
        f.setPointSize(f.pointSize() / 2);
        painter.setFont(f);

    } else {
        nlohmann::json tMap = tempG.getCurrentMap().getJson();
        std::vector<int> tRooms = tempG.getCurrentProgress().getRoomsDiscovered()[tempG.getCurrentMap().getName()];
        QPoint tempMC = tempG.getMapCameraPosition();
        int mapScaleDown = Entity::values["general"]["mapScaleDown"].get<int>();
        for (int i : tRooms) {
            nlohmann::json room = tMap["rooms"][std::to_string(i)];
            if (room["position"][0].get<int>() + room["size"][0].get<int>() < tempMC.x() // If too much on the left
                    || room["position"][0].get<int>() > tempMC.x() + game->getCameraSize().first * mapScaleDown // If too much on the right
                    || room["position"][1].get<int>() + room["size"][1].get<int>() < tempMC.y() // If too high
                    || room["position"][1].get<int>() > tempMC.y() + game->getCameraSize().second * mapScaleDown) { // If too low
                continue;
            }

            painter.fillRect(QRect((room["position"][0].get<int>() - tempMC.x()) / mapScaleDown, (room["position"][1].get<int>() - tempMC.y()) / mapScaleDown,
                                room["size"][0].get<int>() / mapScaleDown, room["size"][1].get<int>() / mapScaleDown),
                                QColor(32,50,100));

            painter.setPen(QPen(QColor(64,100,200), 5));
            painter.drawRect(QRect((room["position"][0].get<int>() - tempMC.x()) / mapScaleDown, (room["position"][1].get<int>() - tempMC.y()) / mapScaleDown,
                                room["size"][0].get<int>() / mapScaleDown, room["size"][1].get<int>() / mapScaleDown));
        }

        for (int i : tRooms) {
            nlohmann::json room = tMap["rooms"][std::to_string(i)];
            if (room["position"][0].get<int>() + room["size"][0].get<int>() < tempMC.x() // If too much on the left
                    || room["position"][0].get<int>() > tempMC.x() + game->getCameraSize().first * mapScaleDown // If too much on the right
                    || room["position"][1].get<int>() + room["size"][1].get<int>() < tempMC.y() // If too high
                    || room["position"][1].get<int>() > tempMC.y() + game->getCameraSize().second * mapScaleDown) { // If too low
                continue;
            }
            if (!room["content"]["Area"].is_null()) {
                if (!room["content"]["Area"]["HorizontalDoor"].is_null()) {
                    for (nlohmann::json hd : room["content"]["Area"]["HorizontalDoor"]) {
                        painter.fillRect(QRect((hd["x"].get<int>() + Entity::values["names"]["HorizontalDoor"]["offset_x"].get<int>() + room["position"][0].get<int>() - tempMC.x()) / mapScaleDown,
                                (hd["y"].get<int>() + Entity::values["names"]["HorizontalDoor"]["offset_y"].get<int>() + room["position"][1].get<int>() - tempMC.y()) / mapScaleDown,
                                Entity::values["names"]["HorizontalDoor"]["width"].get<int>() / mapScaleDown,
                                Entity::values["names"]["HorizontalDoor"]["height"].get<int>() / mapScaleDown),
                                QColor("darkGray"));
                    }
                }
                if (!room["content"]["Area"]["VerticalDoor"].is_null()) {
                    for (nlohmann::json vd : room["content"]["Area"]["VerticalDoor"]) {
                        painter.fillRect(QRect((vd["x"].get<int>() + Entity::values["names"]["VerticalDoor"]["offset_x"].get<int>() + room["position"][0].get<int>() - tempMC.x()) / mapScaleDown,
                                (vd["y"].get<int>() + Entity::values["names"]["VerticalDoor"]["offset_y"].get<int>() + room["position"][1].get<int>() - tempMC.y()) / mapScaleDown,
                                Entity::values["names"]["VerticalDoor"]["width"].get<int>() / mapScaleDown,
                                Entity::values["names"]["VerticalDoor"]["height"].get<int>() / mapScaleDown),
                                QColor("darkGray"));
                    }
                }
            }
        }
        painter.setPen(QPen(QColor(64,192,64,100), 5));
        painter.drawRect(QRect((tempC.x() - tempMC.x()) / mapScaleDown, (tempC.y() - tempMC.y()) / mapScaleDown,
                            tempG.getCameraSize().first / mapScaleDown, tempG.getCameraSize().second / mapScaleDown));
        if (tempG.getS() != nullptr) {
            QImage minimos(QString::fromStdString(tempG.getAssetsPath() + "/textures/" + Entity::values["textures"]["Map"]["Samos"]["file"].get<std::string>()));

            painter.drawImage(QRect((tempG.getS()->getX() + tempG.getS()->getBox()->getX() + tempG.getS()->getBox()->getWidth() / 2 - tempMC.x()) / mapScaleDown - minimos.width() / 2,
                                    (tempG.getS()->getY() + tempG.getS()->getBox()->getY() + tempG.getS()->getBox()->getHeight() / 2 - tempMC.y()) / mapScaleDown - minimos.height() / 2,
                                    minimos.width(),
                                    minimos.height()),
                              minimos);
        }
    }
    painter.end();
}

int MainWindow::getRenderingMultiplier() const
{
    return renderingMultiplier;
}

void MainWindow::setRenderingMultiplier(int newRenderingMultiplier)
{
    renderingMultiplier = newRenderingMultiplier;
}

const QImage &MainWindow::getErrorTexture() const
{
    return errorTexture;
}

void MainWindow::setErrorTexture(const QImage &newErrorTexture)
{
    errorTexture = newErrorTexture;
}

const QImage &MainWindow::getEmptyTexture() const
{
    return emptyTexture;
}

void MainWindow::setEmptyTexture(const QImage &newEmptyTexture)
{
    emptyTexture = newEmptyTexture;
}

bool MainWindow::getShowHUD() const
{
    return showHUD;
}

void MainWindow::setShowHUD(bool newShowHUD)
{
    showHUD = newShowHUD;
}

Game *MainWindow::getGame() const
{
    return game;
}

void MainWindow::setGame(Game *newGame)
{
    game = newGame;
}

EditorWindow *MainWindow::getEditorWindow() const
{
    return editorWindow;
}

void MainWindow::setEditorWindow(EditorWindow *newEditorWindow)
{
    editorWindow = newEditorWindow;
}

bool MainWindow::getRender() const
{
    return render;
}

void MainWindow::setRender(bool newRender)
{
    render = newRender;
}
