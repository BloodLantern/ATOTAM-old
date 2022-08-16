#include "editorpreview.h"
#include "mainwindow.h"

#include <Entities/area.h>
#include <Entities/door.h>
#include <Entities/npc.h>

MainWindow::MainWindow(QApplication *app, std::string assetsPath)
    : m_qApp(app)
    , game(new Game(assetsPath))
    , errorTexture(QString::fromStdString(game->getAssetsPath() + "/textures/error.png"))
    , emptyTexture(QString::fromStdString(game->getAssetsPath() + "/textures/empty.png"))
    , showHUD(true)
{
    setFixedSize(game->getResolution().first, game->getResolution().second);

    renderingMultiplier = Entity::values["general"]["renderingMultiplier"];

    if (Entity::values["general"]["mapEditor"])
        setupEditorWindow();
}

MainWindow::~MainWindow()
{
    delete game;
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

void MainWindow::setupEditorWindow()
{
    // Window
    QWidget editorWindow;
    editorWindow.setWindowTitle("ATOTAM Map Editor");

    // Widgets inside the window
    EditorPreview preview(&editorWindow, game->getEntities(), &errorTexture, &emptyTexture, &renderingMultiplier);

    editorWindow.show();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    game->setRunning(false);
    game->clearEntities();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    for (std::vector<NPC*>::iterator ent = game->getNPCs()->begin(); ent != game->getNPCs()->end(); ent++) {
        // Make sure not to use a null pointer in case there is one
        if (*ent == nullptr)
            continue;

        // If the texture is null, set it to the empty texture
        if ((*ent)->getTexture() == nullptr) {
            (*ent)->setTexture(&emptyTexture);
        }

        // Make sure not to draw the Entities that aren't visible
        if ((*ent)->getX() + (*ent)->getTexture()->offset().x() + (*ent)->getTexture()->width() * renderingMultiplier < game->getCamera().x() // If too much on the left
                || (*ent)->getX() + (*ent)->getTexture()->offset().x() > game->getCamera().x() + 1920 // If too much on the right
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() + (*ent)->getTexture()->height() * renderingMultiplier < game->getCamera().y() // If too high
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() > game->getCamera().y() + 1080) { // If too low
            continue;
        }

        //Try to draw the texture: if it fails, set it to the error texture and try again
        try {
            if ((*ent)->getITime() > 0.0 && (game->getUpdateCount() % 20 < 10)) {
                QColor base_color(200,200,200,200);

                QImage new_image = *(*ent)->getTexture();
                QPainter pa(&new_image);
                pa.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                pa.fillRect(new_image.rect(), base_color);
                pa.end();
                painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                    new_image);
            } else {
                painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                    *(*ent)->getTexture());
            }
        } catch (...) {
            (*ent)->setTexture(&errorTexture);
            painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                    *(*ent)->getTexture());
        }
    }

    if (game->getS() != nullptr) {
        //If the texture is null, set it to the empty texture
        if (game->getS()->getTexture() == nullptr) {
            game->getS()->setTexture(&emptyTexture);
        }

        //Try to draw the texture: if it fails, set it to the error texture and try again
        try {
            if (game->getS()->getITime() > 0.0 && (game->getUpdateCount() % 20 < 10)) {
                QColor base_color(200,200,200,200);

                QImage new_image = *game->getS()->getTexture();
                QPainter pa(&new_image);
                pa.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                pa.fillRect(new_image.rect(), base_color);
                pa.end();
                painter.drawImage(QRect(game->getS()->getX() + game->getS()->getTexture()->offset().x() - game->getCamera().x(), game->getS()->getY() + game->getS()->getTexture()->offset().y() - game->getCamera().y(),
                                    game->getS()->getTexture()->width() * renderingMultiplier, game->getS()->getTexture()->height() * renderingMultiplier),
                                    new_image);
            } else {
                painter.drawImage(QRect(game->getS()->getX() + game->getS()->getTexture()->offset().x() - game->getCamera().x(), game->getS()->getY() + game->getS()->getTexture()->offset().y() - game->getCamera().y(),
                                    game->getS()->getTexture()->width() * renderingMultiplier, game->getS()->getTexture()->height() * renderingMultiplier),
                                    *game->getS()->getTexture());
            }
        } catch (...) {
            game->getS()->setTexture(&errorTexture);
            painter.drawImage(QRect(game->getS()->getX() + game->getS()->getTexture()->offset().x() - game->getCamera().x(), game->getS()->getY() + game->getS()->getTexture()->offset().y() - game->getCamera().y(),
                                    game->getS()->getTexture()->width() * renderingMultiplier, game->getS()->getTexture()->height() * renderingMultiplier),
                                    *game->getS()->getTexture());
        }
    }

    for (std::vector<Monster*>::iterator ent = game->getMonsters()->begin(); ent != game->getMonsters()->end(); ent++) {
        //If the texture is null, set it to the empty texture
        if ((*ent)->getTexture() == nullptr) {
            (*ent)->setTexture(&emptyTexture);
        }

        // Make sure not to draw the Entities that aren't visible
        if ((*ent)->getX() + (*ent)->getTexture()->offset().x() + (*ent)->getTexture()->width() * renderingMultiplier < game->getCamera().x() // If too much on the left
                || (*ent)->getX() + (*ent)->getTexture()->offset().x() > game->getCamera().x() + 1920 // If too much on the right
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() + (*ent)->getTexture()->height() * renderingMultiplier < game->getCamera().y() // If too high
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() > game->getCamera().y() + 1080) { // If too low
            continue;
        }

        //Try to draw the texture: if it fails, set it to the error texture and try again
        try {
            if ((*ent)->getITime() > 0.0 && (game->getUpdateCount() % 20 < 10)) {
                QColor base_color(200,200,200,200);

                QImage new_image = *(*ent)->getTexture();
                QPainter pa(&new_image);
                pa.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                pa.fillRect(new_image.rect(), base_color);
                pa.end();
                painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                    new_image);
            } else {
                painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                    *(*ent)->getTexture());
            }
        } catch (...) {
            (*ent)->setTexture(&errorTexture);
            painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                    *(*ent)->getTexture());
        }
    }

    for (std::vector<DynamicObj*>::iterator ent = game->getDynamicObjs()->begin(); ent != game->getDynamicObjs()->end(); ent++) {
        //If the texture is null, set it to the empty texture
        if ((*ent)->getTexture() == nullptr) {
            (*ent)->setTexture(&emptyTexture);
        }

        // Make sure not to draw the Entities that aren't visible
        if ((*ent)->getX() + (*ent)->getTexture()->offset().x() + (*ent)->getTexture()->width() * renderingMultiplier < game->getCamera().x() // If too much on the left
                || (*ent)->getX() + (*ent)->getTexture()->offset().x() > game->getCamera().x() + 1920 // If too much on the right
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() + (*ent)->getTexture()->height() * renderingMultiplier < game->getCamera().y() // If too high
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() > game->getCamera().y() + 1080) { // If too low
            continue;
        }

        //Try to draw the texture: if it fails, set it to the error texture and try again
        try {
            if ((*ent)->getITime() > 0.0 && (game->getUpdateCount() % 20 < 10)) {
                QColor base_color(200,200,200,200);

                QImage new_image = *(*ent)->getTexture();
                QPainter pa(&new_image);
                pa.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                pa.fillRect(new_image.rect(), base_color);
                pa.end();
                painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                    new_image);
            } else {
                painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                    *(*ent)->getTexture());
            }
        } catch (...) {
            (*ent)->setTexture(&errorTexture);
            painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                    *(*ent)->getTexture());
        }
    }

    for (std::vector<Terrain*>::iterator ent = game->getTerrains()->begin(); ent != game->getTerrains()->end(); ent++) {
        //If the texture is null, set it to the empty texture
        if ((*ent)->getTexture() == nullptr) {
            (*ent)->setTexture(&emptyTexture);
        }

        // Make sure not to draw the Entities that aren't visible
        if ((*ent)->getX() + (*ent)->getTexture()->offset().x() + (*ent)->getTexture()->width() * renderingMultiplier < game->getCamera().x() // If too much on the left
                || (*ent)->getX() + (*ent)->getTexture()->offset().x() > game->getCamera().x() + 1920 // If too much on the right
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() + (*ent)->getTexture()->height() * renderingMultiplier < game->getCamera().y() // If too high
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() > game->getCamera().y() + 1080) { // If too low
            continue;
        }

        //Try to draw the texture: if it fails, set it to the error texture and try again
        try {
            painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                *(*ent)->getTexture());
        } catch (...) {
            (*ent)->setTexture(&errorTexture);
            painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                    *(*ent)->getTexture());
        }
    }

    for (std::vector<Projectile*>::iterator ent = game->getProjectiles()->begin(); ent != game->getProjectiles()->end(); ent++) {
        //If the texture is null, set it to the empty texture
        if ((*ent)->getTexture() == nullptr) {
            (*ent)->setTexture(&emptyTexture);
        }

        // Make sure not to draw the Entities that aren't visible
        if ((*ent)->getX() + (*ent)->getTexture()->offset().x() + (*ent)->getTexture()->width() * renderingMultiplier < game->getCamera().x() // If too much on the left
                || (*ent)->getX() + (*ent)->getTexture()->offset().x() > game->getCamera().x() + 1920 // If too much on the right
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() + (*ent)->getTexture()->height() * renderingMultiplier < game->getCamera().y() // If too high
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() > game->getCamera().y() + 1080) { // If too low
            continue;
        }

        //Try to draw the texture: if it fails, set it to the error texture and try again
        try {
            painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                *(*ent)->getTexture());
        } catch (...) {
            (*ent)->setTexture(&errorTexture);
            painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                    *(*ent)->getTexture());
        }
    }

    for (std::vector<Area*>::iterator ent = game->getAreas()->begin(); ent != game->getAreas()->end(); ent++) {
        //If the texture is null, set it to the empty texture
        if ((*ent)->getTexture() == nullptr) {
            (*ent)->setTexture(&emptyTexture);
        }

        // Make sure not to draw the Entities that aren't visible
        if ((*ent)->getX() + (*ent)->getTexture()->offset().x() + (*ent)->getTexture()->width() * renderingMultiplier < game->getCamera().x() // If too much on the left
                || (*ent)->getX() + (*ent)->getTexture()->offset().x() > game->getCamera().x() + 1920 // If too much on the right
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() + (*ent)->getTexture()->height() * renderingMultiplier < game->getCamera().y() // If too high
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() > game->getCamera().y() + 1080) { // If too low
            continue;
        }

        //Try to draw the texture: if it fails, set it to the error texture and try again
        try {
            painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                *(*ent)->getTexture());
        } catch (...) {
            (*ent)->setTexture(&errorTexture);
            painter.drawImage(QRect((*ent)->getX() + (*ent)->getTexture()->offset().x() - game->getCamera().x(), (*ent)->getY() + (*ent)->getTexture()->offset().y() - game->getCamera().y(),
                                    (*ent)->getTexture()->width() * renderingMultiplier, (*ent)->getTexture()->height() * renderingMultiplier),
                                    *(*ent)->getTexture());
        }
    }



    //Draw hitboxes if necessary
    if (game->getRenderHitboxes()) {

        for (std::vector<NPC*>::iterator ent = game->getNPCs()->begin(); ent != game->getNPCs()->end(); ent++) {
            painter.setPen(QColor("blue"));
            if ((*ent)->getBox() != nullptr)
                painter.drawRect((*ent)->getX() + (*ent)->getBox()->getX() - game->getCamera().x(), (*ent)->getY() + (*ent)->getBox()->getY() - game->getCamera().y(),
                                 (*ent)->getBox()->getWidth(), (*ent)->getBox()->getHeight());

            //Ground boxes);
            painter.setPen(QColor("green"));
            if ((*ent)->getGroundBox() != nullptr)
                painter.drawRect((*ent)->getX() + (*ent)->getGroundBox()->getX() - game->getCamera().x(), (*ent)->getY() + (*ent)->getGroundBox()->getY() - game->getCamera().y(),
                                (*ent)->getGroundBox()->getWidth(), (*ent)->getGroundBox()->getHeight());
        }

        if (game->getS() != nullptr) {
            painter.setPen(QColor("orange"));
            if (game->getS()->getBox() != nullptr)
                painter.drawRect(game->getS()->getX() + game->getS()->getBox()->getX() - game->getCamera().x(), game->getS()->getY() + game->getS()->getBox()->getY() - game->getCamera().y(),
                                 game->getS()->getBox()->getWidth(), game->getS()->getBox()->getHeight());

            //Ground boxes
            painter.setPen(QColor("green"));
            if (game->getS()->getGroundBox() != nullptr)
                painter.drawRect(game->getS()->getX() + game->getS()->getGroundBox()->getX() - game->getCamera().x(), game->getS()->getY() + game->getS()->getGroundBox()->getY() - game->getCamera().y(),
                                game->getS()->getGroundBox()->getWidth(), game->getS()->getGroundBox()->getHeight());

            //Wall boxes
            if (game->getS()->getWallBoxL() != nullptr)
                painter.drawRect(game->getS()->getX() + game->getS()->getWallBoxL()->getX() - game->getCamera().x(), game->getS()->getY() + game->getS()->getWallBoxL()->getY() - game->getCamera().y(),
                                game->getS()->getWallBoxL()->getWidth(), game->getS()->getWallBoxL()->getHeight());
            if (game->getS()->getWallBoxR() != nullptr)
                painter.drawRect(game->getS()->getX() + game->getS()->getWallBoxR()->getX() - game->getCamera().x(), game->getS()->getY() + game->getS()->getWallBoxR()->getY() - game->getCamera().y(),
                                game->getS()->getWallBoxR()->getWidth(), game->getS()->getWallBoxR()->getHeight());
        }

        for (std::vector<Monster*>::iterator ent = game->getMonsters()->begin(); ent != game->getMonsters()->end(); ent++) {
            painter.setPen(QColor("red"));
            if ((*ent)->getBox() != nullptr)
                painter.drawRect((*ent)->getX() + (*ent)->getBox()->getX() - game->getCamera().x(), (*ent)->getY() + (*ent)->getBox()->getY() - game->getCamera().y(),
                                 (*ent)->getBox()->getWidth(), (*ent)->getBox()->getHeight());

            //Ground boxes);
            painter.setPen(QColor("green"));
            if ((*ent)->getGroundBox() != nullptr)
                painter.drawRect((*ent)->getX() + (*ent)->getGroundBox()->getX() - game->getCamera().x(), (*ent)->getY() + (*ent)->getGroundBox()->getY() - game->getCamera().y(),
                                (*ent)->getGroundBox()->getWidth(), (*ent)->getGroundBox()->getHeight());
        }

        for (std::vector<DynamicObj*>::iterator ent = game->getDynamicObjs()->begin(); ent != game->getDynamicObjs()->end(); ent++) {
            painter.setPen(QColor("blue"));
            if ((*ent)->getBox() != nullptr)
                painter.drawRect((*ent)->getX() + (*ent)->getBox()->getX() - game->getCamera().x(), (*ent)->getY() + (*ent)->getBox()->getY() - game->getCamera().y(),
                                 (*ent)->getBox()->getWidth(), (*ent)->getBox()->getHeight());

            //Ground boxes);
            painter.setPen(QColor("green"));
            if ((*ent)->getGroundBox() != nullptr)
                painter.drawRect((*ent)->getX() + (*ent)->getGroundBox()->getX() - game->getCamera().x(), (*ent)->getY() + (*ent)->getGroundBox()->getY() - game->getCamera().y(),
                                (*ent)->getGroundBox()->getWidth(), (*ent)->getGroundBox()->getHeight());
        }

        for (std::vector<Terrain*>::iterator ent = game->getTerrains()->begin(); ent != game->getTerrains()->end(); ent++) {
            painter.setPen(QColor("blue"));
            if ((*ent)->getBox() != nullptr)
                painter.drawRect((*ent)->getX() + (*ent)->getBox()->getX() - game->getCamera().x(), (*ent)->getY() + (*ent)->getBox()->getY() - game->getCamera().y(),
                                 (*ent)->getBox()->getWidth(), (*ent)->getBox()->getHeight());
        }

        for (std::vector<Projectile*>::iterator ent = game->getProjectiles()->begin(); ent != game->getProjectiles()->end(); ent++) {
            painter.setPen(QColor("blue"));
            if ((*ent)->getBox() != nullptr)
                painter.drawRect((*ent)->getX() + (*ent)->getBox()->getX() - game->getCamera().x(), (*ent)->getY() + (*ent)->getBox()->getY() - game->getCamera().y(),
                                 (*ent)->getBox()->getWidth(), (*ent)->getBox()->getHeight());
        }

        for (std::vector<Area*>::iterator ent = game->getAreas()->begin(); ent != game->getAreas()->end(); ent++) {
            painter.setPen(QColor("gray"));
            if ((*ent)->getBox() != nullptr)
                painter.drawRect((*ent)->getX() + (*ent)->getBox()->getX() - game->getCamera().x(), (*ent)->getY() + (*ent)->getBox()->getY() - game->getCamera().y(),
                                 (*ent)->getBox()->getWidth(), (*ent)->getBox()->getHeight());
        }

        nlohmann::json mapJson = game->getCurrentMap().getJson()["rooms"][std::to_string(game->getCurrentMap().getCurrentRoomId())];

        painter.setPen(QColor("magenta"));
        painter.drawRect(static_cast<int>(mapJson["position"][0]) - game->getCamera().x(), static_cast<int>(mapJson["position"][1]) - game->getCamera().y(), mapJson["size"][0], mapJson["size"][1]);
    }

    // Draw fps if necessary
    if (game->getShowFps()) {
        if ((std::chrono::high_resolution_clock::now() - game->getLastFpsShown()).count() > game->getShowFpsUpdateRate()) {
            game->getLastFpsShown() = std::chrono::high_resolution_clock::now();
        }
        painter.setPen(QColor("black"));
        painter.drawText(QPoint(2, 12), QString::fromStdString(std::to_string(game->getFps()) + " FPS"));
    }

    QFont f = painter.font();
    f.setPointSize(f.pointSize() * 2);
    painter.setFont(f);

    //HUD
    if (game->getS() != nullptr && showHUD) {

        painter.setPen(QColor("black"));

        //Selected weapon
        painter.fillRect(QRect(70,15,100,30), QColor("white"));
        painter.drawRect(QRect(70,15,100,30));
        painter.drawText(QPoint(80, 40), /*game->translate(*/QString::fromStdString(game->getS()->getSelectedWeapon())/*, "ui", "selectedWeapon")*/);

        //Missile count
        painter.fillRect(QRect(200,15,70,30), QColor("white"));
        painter.drawRect(QRect(200,15,70,30));
        painter.drawText(QPoint(210, 40), QString::fromStdString("M : " + std::to_string(game->getS()->getMissileCount())));

        //Grenade count
        painter.fillRect(QRect(290,15,70,30), QColor("white"));
        painter.drawRect(QRect(290,15,70,30));
        painter.drawText(QPoint(300, 40), QString::fromStdString("G : " + std::to_string(game->getS()->getGrenadeCount())));

        //Health count
        painter.fillRect(QRect(380,15,80,30), QColor("white"));
        painter.drawRect(QRect(380,15,80,30));
        painter.drawText(QPoint(390, 40), QString::fromStdString("H : " + std::to_string(game->getS()->getHealth())));
    }

    // Dialogue
    if (!game->getCurrentDialogue().isNull()) {
        // In case we changed it before
        painter.setPen(QColor("black"));

        painter.drawText(QRectF(QRect(200, 200, 1000, 200)),
                         QString::fromStdString(game->getCurrentDialogue().getTalkingName() + ": " + game->getCurrentDialogue().getText()[game->getCurrentDialogue().getTextAdvancement()]),
                QTextOption(Qt::AlignHCenter));
    }

    //Menu
    if (game->getIsPaused()) {
        painter.fillRect(QRect(0,0,size().width(),size().height()), QBrush(QColor(0,0,0,200)));
        for (unsigned int i = 0; i < game->getMenuOptions().size(); i++) {
            painter.setPen(QColor("white"));
            if (game->getSelectedOption() == (int) i)
                painter.setPen(QColor("cyan"));
            painter.drawText(0, size().height() / 2 - 15 * game->getMenuOptions().size() + 30 * i, size().width(), 50, Qt::AlignHCenter, QString::fromStdString(game->getMenuOptions()[i]));
        }
    }

    f.setPointSize(f.pointSize() / 2);
    painter.setFont(f);
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
