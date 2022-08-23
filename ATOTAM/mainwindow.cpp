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
{
    setFixedSize(game->getResolution().first, game->getResolution().second);

    renderingMultiplier = Entity::values["general"]["renderingMultiplier"];

    if (game->getFullscreen())
        showFullScreen();
    else
        show();

    QCoreApplication::instance()->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete game;
}

bool MainWindow::eventFilter(QObject* object, QEvent* event) {
     if (event->type() == QEvent::Paint && !render && object == this) {
         render = false;
         return true;
     }
     else if (event->type() == QEvent::KeyPress && object == this) {
         QKeyEvent * keyEvent = dynamic_cast<QKeyEvent*>(event);
         if (keyEvent->key() == Qt::Key_Tab)
             return true;
     }
     return false;
}

bool MainWindow::getCopyingToDraw() const
{
    return copyingToDraw;
}

void MainWindow::setCopyingToDraw(bool newCopyingToDraw)
{
    copyingToDraw = newCopyingToDraw;
}

const std::map<int, QImage> &MainWindow::getToDrawTextures() const
{
    return toDrawTextures;
}

void MainWindow::setToDrawTextures(const std::map<int, QImage> &newToDrawTextures)
{
    toDrawTextures = newToDrawTextures;
}

const nlohmann::json &MainWindow::getToDraw() const
{
    return toDraw;
}

void MainWindow::setToDraw(const nlohmann::json &newToDraw)
{
    toDraw = newToDraw;
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
            for (nlohmann::json::iterator j = i.value().begin(); j != i.value().end(); j++)
                if (GetKeyState(game->getWindowsKeyCodes()[j.value()].get<int>()) & 0x8000) {
                    (*game->getInputList())[i.key()] = true;
                    break;
                } else
                    (*game->getInputList())[i.key()] = false;
        }
    } else
        //Reset the keys if the window is not selected
        for (nlohmann::json::iterator i = game->getKeyCodes().begin(); i != game->getKeyCodes().end(); i++) {
            (*game->getInputList())[i.key()] = 0;
        }
}

void MainWindow::getSpecialInputs()
{
    for (std::map<std::string, bool>::iterator i = game->getInputList()->begin(); i != game->getInputList()->end(); i ++) {
        if (i->first.size() > 8)
            if (i->first.substr(0, 8) == "SPECIAL_") {
                if (i->second)
                    (*game->getInputTime())[i->first] += 1 / Physics::frameRate;
                else
                    (*game->getInputTime())[i->first] = 0;
            }
    }

    //Only listen for inputs if the window is currently selected
    if (isActiveWindow()) {
        //Check every key
        for (nlohmann::json::iterator i = game->getKeyCodes().begin(); i != game->getKeyCodes().end(); i++) {
            for (nlohmann::json::iterator j = i.value().begin(); j != i.value().end(); j++)
                if (i.key().size() > 8)
                    if (i.key().substr(0, 8) == "SPECIAL_") {
                        if (GetKeyState(game->getWindowsKeyCodes()[j.value()].get<int>()) & 0x8000) {
                            (*game->getInputList())[i.key()] = true;
                            break;
                        } else
                            (*game->getInputList())[i.key()] = false;
                    }
        }
    }
}

void MainWindow::setupToDraw()
{
    toDrawTextures.clear();
    toDraw.clear();

    toDraw["camera_position_x"] = game->getCamera().x();
    toDraw["camera_position_y"] = game->getCamera().y();
    toDraw["camera_size_x"] = game->getCameraSize().first;
    toDraw["camera_size_y"] = game->getCameraSize().second;
    toDraw["renderingMultiplier"] = renderingMultiplier;

    toDraw["inInventory"] = game->getInInventory();
    toDraw["inMap"] = game->getInMap();
    toDraw["renderHitboxes"] = game->getRenderHitboxes();
    toDraw["showFps"] = game->getShowFps();
    toDraw["showHUD"] = game->getShowHUD();
    toDraw["inDialogue"] = !game->getCurrentDialogue().isNull();
    toDraw["isPaused"] = game->getIsPaused();
    toDraw["updateCount"] = game->getUpdateCount();
    toDraw["showDebugInfo"] = game->getShowDebugInfo();


    if (!game->getInInventory() && !game->getInMap()) {
        std::map<float, std::vector<nlohmann::json>> entitiesInfo;
        for (std::vector<Entity*>::iterator ent = game->getEntities()->begin(); ent != game->getEntities()->end(); ent++) {
            // Make sure not to use a null pointer in case there is one
            if (*ent == nullptr)
                continue;

            // If the texture is null, set it to the empty texture
            if ((*ent)->getTexture() == nullptr) {
                (*ent)->setTexture(&emptyTexture);
            }

            // Make sure not to draw the Entities that aren't visible
            if ((*ent)->getX() + (*ent)->getTexture()->offset().x() + (*ent)->getTexture()->width() * renderingMultiplier < game->getCamera().x() // If too much on the left
                    || (*ent)->getX() + (*ent)->getTexture()->offset().x() > game->getCamera().x() + game->getCameraSize().first // If too much on the right
                    || (*ent)->getY() + (*ent)->getTexture()->offset().y() + (*ent)->getTexture()->height() * renderingMultiplier < game->getCamera().y() // If too high
                    || (*ent)->getY() + (*ent)->getTexture()->offset().y() > game->getCamera().y() + game->getCameraSize().second) { // If too low
                continue;
            }

            nlohmann::json entityInfo;

            entityInfo["x"] = (*ent)->getX();
            entityInfo["y"] = (*ent)->getY();
            entityInfo["entityType"] = (*ent)->getEntType();
            entityInfo["entityID"] = (*ent)->getEntityID();

            toDrawTextures[(*ent)->getEntityID()] = *(*ent)->getTexture();

            if ((*ent)->getEntType() == "Samos" || (*ent)->getEntType() == "NPC" || (*ent)->getEntType() == "Monster" || (*ent)->getEntType() == "DynamicObj") {
                    Living* liv = static_cast<Living*>(*ent);
                    entityInfo["iTime"] = liv->getITime();
            }

            entitiesInfo[(*ent)->getLayer()].push_back(entityInfo);
        }
        std::vector<std::string> layers;
        for (std::map<float, std::vector<nlohmann::json>>::iterator i = entitiesInfo.begin(); i != entitiesInfo.end(); i++) {
            layers.push_back(std::to_string(i->first));
            toDraw["entitiesInfo"][std::to_string(i->first)] = i->second;
        }
        std::sort(layers.begin(), layers.end());
        toDraw["layers"] = layers;


        //Draw hitboxes if necessary
        if (game->getRenderHitboxes()) {

            nlohmann::json roomJson = (*game->getCurrentMap().getJson())["rooms"][std::to_string(game->getCurrentMap().getCurrentRoomId())];
            toDraw["room_position_x"] = roomJson["position"][0];
            toDraw["room_position_y"] = roomJson["position"][1];
            toDraw["room_size_x"] = roomJson["size"][0];
            toDraw["room_size_y"] = roomJson["size"][1];

            std::vector<nlohmann::json> hitboxesInfo;

            for (std::vector<NPC*>::iterator ent = game->getNPCs()->begin(); ent != game->getNPCs()->end(); ent++) {
                nlohmann::json hitboxInfo;
                hitboxInfo["entityType"] = (*ent)->getEntType();

                if ((*ent)->getBox() != nullptr) {
                    hitboxInfo["box_x"] = (*ent)->getBox()->getX() + (*ent)->getX();
                    hitboxInfo["box_y"] = (*ent)->getBox()->getY() + (*ent)->getY();
                    hitboxInfo["box_w"] = (*ent)->getBox()->getWidth();
                    hitboxInfo["box_h"] = (*ent)->getBox()->getHeight();
                }

                if ((*ent)->getGroundBox() != nullptr) {
                    hitboxInfo["groundBox_x"] = (*ent)->getGroundBox()->getX() + (*ent)->getX();
                    hitboxInfo["groundBox_y"] = (*ent)->getGroundBox()->getY() + (*ent)->getY();
                    hitboxInfo["groundBox_w"] = (*ent)->getGroundBox()->getWidth();
                    hitboxInfo["groundBox_h"] = (*ent)->getGroundBox()->getHeight();
                }

                hitboxesInfo.push_back(hitboxInfo);

            }

            if (game->getS() != nullptr) {
                Samos* s = game->getS();

                nlohmann::json hitboxInfo;
                hitboxInfo["entityType"] = s->getEntType();

                if (s->getBox() != nullptr) {
                    hitboxInfo["box_x"] = s->getBox()->getX() + s->getX();
                    hitboxInfo["box_y"] = s->getBox()->getY() + s->getY();
                    hitboxInfo["box_w"] = s->getBox()->getWidth();
                    hitboxInfo["box_h"] = s->getBox()->getHeight();
                }

                if (s->getGroundBox() != nullptr) {
                    hitboxInfo["groundBox_x"] = s->getGroundBox()->getX() + s->getX();
                    hitboxInfo["groundBox_y"] = s->getGroundBox()->getY() + s->getY();
                    hitboxInfo["groundBox_w"] = s->getGroundBox()->getWidth();
                    hitboxInfo["groundBox_h"] = s->getGroundBox()->getHeight();
                }

                if (s->getWallBoxL() != nullptr) {
                    hitboxInfo["wallBoxL_x"] = s->getWallBoxL()->getX() + s->getX();
                    hitboxInfo["wallBoxL_y"] = s->getWallBoxL()->getY() + s->getY();
                    hitboxInfo["wallBoxL_w"] = s->getWallBoxL()->getWidth();
                    hitboxInfo["wallBoxL_h"] = s->getWallBoxL()->getHeight();
                }

                if (s->getWallBoxR() != nullptr) {
                    hitboxInfo["wallBoxR_x"] = s->getWallBoxR()->getX() + s->getX();
                    hitboxInfo["wallBoxR_y"] = s->getWallBoxR()->getY() + s->getY();
                    hitboxInfo["wallBoxR_w"] = s->getWallBoxR()->getWidth();
                    hitboxInfo["wallBoxR_h"] = s->getWallBoxR()->getHeight();
                }

                hitboxesInfo.push_back(hitboxInfo);
            }

            for (std::vector<Monster*>::iterator ent = game->getMonsters()->begin(); ent != game->getMonsters()->end(); ent++) {
                nlohmann::json hitboxInfo;
                hitboxInfo["entityType"] = (*ent)->getEntType();

                if ((*ent)->getBox() != nullptr) {
                    hitboxInfo["box_x"] = (*ent)->getBox()->getX() + (*ent)->getX();
                    hitboxInfo["box_y"] = (*ent)->getBox()->getY() + (*ent)->getY();
                    hitboxInfo["box_w"] = (*ent)->getBox()->getWidth();
                    hitboxInfo["box_h"] = (*ent)->getBox()->getHeight();
                }

                if ((*ent)->getGroundBox() != nullptr) {
                    hitboxInfo["groundBox_x"] = (*ent)->getGroundBox()->getX() + (*ent)->getX();
                    hitboxInfo["groundBox_y"] = (*ent)->getGroundBox()->getY() + (*ent)->getY();
                    hitboxInfo["groundBox_w"] = (*ent)->getGroundBox()->getWidth();
                    hitboxInfo["groundBox_h"] = (*ent)->getGroundBox()->getHeight();
                }

                hitboxesInfo.push_back(hitboxInfo);
            }

            for (std::vector<DynamicObj*>::iterator ent = game->getDynamicObjs()->begin(); ent != game->getDynamicObjs()->end(); ent++) {
                nlohmann::json hitboxInfo;
                hitboxInfo["entityType"] = (*ent)->getEntType();

                if ((*ent)->getBox() != nullptr) {
                    hitboxInfo["box_x"] = (*ent)->getBox()->getX() + (*ent)->getX();
                    hitboxInfo["box_y"] = (*ent)->getBox()->getY() + (*ent)->getY();
                    hitboxInfo["box_w"] = (*ent)->getBox()->getWidth();
                    hitboxInfo["box_h"] = (*ent)->getBox()->getHeight();
                }

                if ((*ent)->getGroundBox() != nullptr) {
                    hitboxInfo["groundBox_x"] = (*ent)->getGroundBox()->getX() + (*ent)->getX();
                    hitboxInfo["groundBox_y"] = (*ent)->getGroundBox()->getY() + (*ent)->getY();
                    hitboxInfo["groundBox_w"] = (*ent)->getGroundBox()->getWidth();
                    hitboxInfo["groundBox_h"] = (*ent)->getGroundBox()->getHeight();
                }

                hitboxesInfo.push_back(hitboxInfo);
            }

            for (std::vector<Terrain*>::iterator ent = game->getTerrains()->begin(); ent != game->getTerrains()->end(); ent++) {
                nlohmann::json hitboxInfo;
                hitboxInfo["entityType"] = (*ent)->getEntType();

                if ((*ent)->getBox() != nullptr) {
                    hitboxInfo["box_x"] = (*ent)->getBox()->getX() + (*ent)->getX();
                    hitboxInfo["box_y"] = (*ent)->getBox()->getY() + (*ent)->getY();
                    hitboxInfo["box_w"] = (*ent)->getBox()->getWidth();
                    hitboxInfo["box_h"] = (*ent)->getBox()->getHeight();
                }

                hitboxesInfo.push_back(hitboxInfo);
            }

            for (std::vector<Projectile*>::iterator ent = game->getProjectiles()->begin(); ent != game->getProjectiles()->end(); ent++) {
                nlohmann::json hitboxInfo;
                hitboxInfo["entityType"] = (*ent)->getEntType();

                if ((*ent)->getBox() != nullptr) {
                    hitboxInfo["box_x"] = (*ent)->getBox()->getX() + (*ent)->getX();
                    hitboxInfo["box_y"] = (*ent)->getBox()->getY() + (*ent)->getY();
                    hitboxInfo["box_w"] = (*ent)->getBox()->getWidth();
                    hitboxInfo["box_h"] = (*ent)->getBox()->getHeight();
                }

                hitboxesInfo.push_back(hitboxInfo);
            }

            for (std::vector<Area*>::iterator ent = game->getAreas()->begin(); ent != game->getAreas()->end(); ent++) {
                nlohmann::json hitboxInfo;
                hitboxInfo["entityType"] = (*ent)->getEntType();

                if ((*ent)->getBox() != nullptr) {
                    hitboxInfo["box_x"] = (*ent)->getBox()->getX() + (*ent)->getX();
                    hitboxInfo["box_y"] = (*ent)->getBox()->getY() + (*ent)->getY();
                    hitboxInfo["box_w"] = (*ent)->getBox()->getWidth();
                    hitboxInfo["box_h"] = (*ent)->getBox()->getHeight();
                }

                hitboxesInfo.push_back(hitboxInfo);
            }

            toDraw["hitboxesInfo"] = hitboxesInfo;
        }

        // Draw fps if necessary
        if (game->getShowFps()) {
            if ((std::chrono::high_resolution_clock::now() - game->getLastFpsShown()).count() > game->getShowFpsUpdateRate()) {
                game->setLastFpsShown(std::chrono::high_resolution_clock::now());
            }
            toDraw["fps"] = game->getFps();
        }

        //HUD
        if (game->getS() != nullptr && game->getShowHUD()) {

            toDraw["hud_selectedWeapon"] = game->translate(game->getS()->getSelectedWeapon(), std::vector<std::string>({"ui", "selectedWeapon"})).toStdString();
            toDraw["hud_health"] = game->getS()->getHealth();
            toDraw["hud_missileCount"] = game->getS()->getMissileCount();
            toDraw["hud_grenadeCount"] = game->getS()->getGrenadeCount();
        }

        // Dialogue
        if (!game->getCurrentDialogue().isNull()) {
            toDraw["talkingName"] = game->getCurrentDialogue().getTalkingName();
            toDraw["dialogueText"] = game->getCurrentDialogue().getText()[game->getCurrentDialogue().getTextAdvancement()];
        }

        //Menu
        if (game->getIsPaused()) {
            toDraw["menuOptions"] = game->getMenuOptions();
            toDraw["selectedOption"] = game->getSelectedOption();
        }

    } else {
        nlohmann::json tMap = *game->getCurrentMap().getJson();
        std::vector<int> tRooms = game->getCurrentProgress().getRoomsDiscovered()[game->getCurrentMap().getName()];
        QPoint tempMC = game->getMapCameraPosition();
        int mapScaleDown = Entity::values["general"]["mapScaleDown"].get<int>();

        toDraw["mapScaleDown"]  = mapScaleDown;
        toDraw["mapCamera_position_x"] = game->getMapCameraPosition().x();
        toDraw["mapCamera_position_y"] = game->getMapCameraPosition().y();
        toDraw["mapCamera_size_x"] = game->getCameraSize().first / mapScaleDown;
        toDraw["mapCamera_size_y"] = game->getCameraSize().second / mapScaleDown;

        std::vector<nlohmann::json> roomsInfo;

        for (int i : tRooms) {
            nlohmann::json room = tMap["rooms"][std::to_string(i)];
            if (room["position"][0].get<int>() + room["size"][0].get<int>() < tempMC.x() // If too much on the left
                    || room["position"][0].get<int>() > tempMC.x() + game->getCameraSize().first * mapScaleDown // If too much on the right
                    || room["position"][1].get<int>() + room["size"][1].get<int>() < tempMC.y() // If too high
                    || room["position"][1].get<int>() > tempMC.y() + game->getCameraSize().second * mapScaleDown) { // If too low
                continue;
            }

            nlohmann::json roomInfo;

            roomInfo["position_x"] = room["position"][0];
            roomInfo["position_y"] = room["position"][1];
            roomInfo["size_x"] = room["size"][0];
            roomInfo["size_y"] = room["size"][1];

            if (!room["content"]["Area"].is_null()) {
                if (!room["content"]["Area"]["HorizontalDoor"].is_null()) {

                    std::vector<nlohmann::json> hDoorsInfo;

                    for (nlohmann::json hd : room["content"]["Area"]["HorizontalDoor"]) {
                        nlohmann::json hDoorInfo;

                        hDoorInfo["x"] = hd["x"].get<int>() + Entity::values["names"]["HorizontalDoor"]["offset_x"].get<int>();
                        hDoorInfo["y"] = hd["y"].get<int>() + Entity::values["names"]["HorizontalDoor"]["offset_y"].get<int>();
                        hDoorInfo["w"] = Entity::values["names"]["HorizontalDoor"]["width"].get<int>();
                        hDoorInfo["h"] = Entity::values["names"]["HorizontalDoor"]["height"].get<int>();

                        hDoorsInfo.push_back(hDoorInfo);
                    }

                    roomInfo["hDoorsInfo"] = hDoorsInfo;
                }

                if (!room["content"]["Area"]["VerticalDoor"].is_null()) {

                    std::vector<nlohmann::json> vDoorsInfo;

                    for (nlohmann::json vd : room["content"]["Area"]["VerticalDoor"]) {
                        nlohmann::json vDoorInfo;

                        vDoorInfo["x"] = vd["x"].get<int>() + Entity::values["names"]["VerticalDoor"]["offset_x"].get<int>();
                        vDoorInfo["y"] = vd["y"].get<int>() + Entity::values["names"]["VerticalDoor"]["offset_y"].get<int>();
                        vDoorInfo["w"] = Entity::values["names"]["VerticalDoor"]["width"].get<int>();
                        vDoorInfo["h"] = Entity::values["names"]["VerticalDoor"]["height"].get<int>();

                        vDoorsInfo.push_back(vDoorInfo);
                    }

                    roomInfo["vDoorsInfo"] = vDoorsInfo;
                }
            }

            roomsInfo.push_back(roomInfo);
        }

        toDraw["mapRoomsInfo"] = roomsInfo;

        if (game->getS() != nullptr) {
            toDraw["minimosPath"] = game->getAssetsPath() + "/textures/" + Entity::values["textures"]["Map"]["Samos"]["file"].get<std::string>();

            Samos* s =game->getS();

            toDraw["samos_x"] = s->getX();
            toDraw["samos_y"] = s->getY();
            toDraw["samos_box_x"] = s->getBox()->getX();
            toDraw["samos_box_y"] = s->getBox()->getY();
            toDraw["samos_box_w"] = s->getBox()->getWidth();
            toDraw["samos_box_h"] = s->getBox()->getHeight();

        }
    }

    if (game->getShowDebugInfo()) {
        Samos* s =game->getS();

        toDraw["samos_x"] = s->getX();
        toDraw["samos_y"] = s->getY();
        toDraw["samos_vx"] = s->getVX();
        toDraw["samos_vy"] = s->getVY();
        toDraw["samos_box_x"] = s->getBox()->getX();
        toDraw["samos_box_y"] = s->getBox()->getY();
        toDraw["samos_box_w"] = s->getBox()->getWidth();
        toDraw["samos_box_h"] = s->getBox()->getHeight();
        toDraw["samos_state"] = s->getState();
        toDraw["samos_facing"] = s->getFacing();
        toDraw["samos_speedRetained"] = s->getSpeedRetained();
        toDraw["samos_retainTime"] = s->getRetainTime();
        toDraw["samos_onGround"] = s->getOnGround();
        toDraw["samos_room"] = game->getCurrentMap().getCurrentRoomId();
        toDraw["samos_iTime"] = s->getITime();
        toDraw["samos_lagTime"] = s->getLagTime();
        toDraw["samos_shootTime"] = s->getShootTime();
        toDraw["samos_switchDelay"] = s->getSwitchDelay();
        toDraw["samos_fastFalling"] = s->getFastFalling();
        toDraw["samos_jumpTime"] = s->getJumpTime();
        toDraw["samos_priorDash_vx"] = s->getSpeedPriorDash().first;
        toDraw["samos_priorDash_vy"] = s->getSpeedPriorDash().second;
        toDraw["samos_dashTime"] = s->getDashTime();
        toDraw["samos_dashCoolDown"] = s->getDashCoolDown();
        toDraw["samos_dashDirection"] = s->getDashDirection();
        toDraw["samos_frameCount"] = game->getFrameCount();
        toDraw["tas_lineFrameCount"] = game->getCurrentInstructionFrames();
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    game->setRunning(false);
}

void MainWindow::paintEvent(QPaintEvent *)
{
    copyingToDraw = true;
    nlohmann::json tempToDraw = nlohmann::json(toDraw);
    std::map<int, QImage> tempToDrawTextures = std::map<int, QImage>(toDrawTextures);
    copyingToDraw = false;

    int c_x =  tempToDraw["camera_position_x"].get<int>();
    int c_y =  tempToDraw["camera_position_y"].get<int>();
    QPainter painter(this);

    if (!tempToDraw["inInventory"].get<bool>() && !tempToDraw["inMap"].get<bool>()) {

        for (std::string layer : tempToDraw["layers"]) {
            for (nlohmann::json entityInfo : tempToDraw["entitiesInfo"][layer]) {
                try {
                    QImage tTex = tempToDrawTextures[entityInfo["entityID"].get<int>()];
                    if (entityInfo["iTime"].is_null() ? false : (entityInfo["iTime"].get<double>() > 0.0 && (tempToDraw["updateCount"].get<int>() % 2 == 0))) {
                        QColor base_color(200,200,200,200);

                        QImage new_image = tTex;
                        QPainter pa(&new_image);
                        pa.setCompositionMode(QPainter::CompositionMode_SourceAtop);
                        pa.fillRect(new_image.rect(), base_color);
                        pa.end();
                        painter.drawImage(QRect(entityInfo["x"].get<int>() + tTex.offset().x() -  c_x,
                                            entityInfo["y"].get<int>() + tTex.offset().y() -  c_y,
                                            tTex.width() * tempToDraw["renderingMultiplier"].get<int>(),
                                            tTex.height() * tempToDraw["renderingMultiplier"].get<int>()),
                                            new_image);
                    } else {
                        painter.drawImage(QRect(entityInfo["x"].get<int>() + tTex.offset().x() -  c_x,
                                            entityInfo["y"].get<int>() + tTex.offset().y() -  c_y,
                                            tTex.width() * tempToDraw["renderingMultiplier"].get<int>(),
                                            tTex.height() * tempToDraw["renderingMultiplier"].get<int>()),
                                            tTex);
                    }
                } catch (...) {
                    painter.drawImage(QRect(entityInfo["x"].get<int>() + errorTexture.offset().x() -  c_x,
                                        entityInfo["y"].get<int>() + errorTexture.offset().y() -  c_y,
                                        errorTexture.width() * tempToDraw["renderingMultiplier"].get<int>(),
                                        errorTexture.height() * tempToDraw["renderingMultiplier"].get<int>()),
                                        errorTexture);
                }
            }
        }


        //Draw hitboxes if necessary
        if (tempToDraw["renderHitboxes"].get<bool>()) {

            for (nlohmann::json hitboxInfo : tempToDraw["hitboxesInfo"]) {

                if (hitboxInfo["entityType"].get<std::string>() == "Terrain") {

                    painter.setPen(QColor("blue"));
                    if (!hitboxInfo["box_x"].is_null()) {
                        painter.drawRect(hitboxInfo["box_x"].get<int>() -  c_x,
                                            hitboxInfo["box_y"].get<int>() -  c_y,
                                            hitboxInfo["box_w"].get<int>(),
                                            hitboxInfo["box_h"].get<int>());
                    }

                } else if (hitboxInfo["entityType"].get<std::string>() == "Projectile") {

                    painter.setPen(QColor("blue"));
                    if (!hitboxInfo["box_x"].is_null()) {
                        painter.drawRect(hitboxInfo["box_x"].get<int>() -  c_x,
                                            hitboxInfo["box_y"].get<int>() -  c_y,
                                            hitboxInfo["box_w"].get<int>(),
                                            hitboxInfo["box_h"].get<int>());
                    }

                } else if (hitboxInfo["entityType"].get<std::string>() == "Area") {

                    painter.setPen(QColor("gray"));
                    if (!hitboxInfo["box_x"].is_null()) {
                        painter.drawRect(hitboxInfo["box_x"].get<int>() -  c_x,
                                            hitboxInfo["box_y"].get<int>() -  c_y,
                                            hitboxInfo["box_w"].get<int>(),
                                            hitboxInfo["box_h"].get<int>());
                    }

                } else if (hitboxInfo["entityType"].get<std::string>() == "NPC") {

                    painter.setPen(QColor("blue"));
                    if (!hitboxInfo["box_x"].is_null()) {
                        painter.drawRect(hitboxInfo["box_x"].get<int>() -  c_x,
                                            hitboxInfo["box_y"].get<int>() -  c_y,
                                            hitboxInfo["box_w"].get<int>(),
                                            hitboxInfo["box_h"].get<int>());
                    }

                    //Ground boxes);
                    painter.setPen(QColor("green"));
                    if (!hitboxInfo["groundBox_x"].is_null()) {
                        painter.drawRect(hitboxInfo["groundBox_x"].get<int>() -  c_x,
                                            hitboxInfo["groundBox_y"].get<int>() -  c_y,
                                            hitboxInfo["groundBox_w"].get<int>(),
                                            hitboxInfo["groundBox_h"].get<int>());
                    }

                } else if (hitboxInfo["entityType"].get<std::string>() == "DynamicObj") {

                    painter.setPen(QColor("blue"));
                    if (!hitboxInfo["box_x"].is_null()) {
                        painter.drawRect(hitboxInfo["box_x"].get<int>() -  c_x,
                                            hitboxInfo["box_y"].get<int>() -  c_y,
                                            hitboxInfo["box_w"].get<int>(),
                                            hitboxInfo["box_h"].get<int>());
                    }

                    //Ground boxes);
                    painter.setPen(QColor("green"));
                    if (!hitboxInfo["groundBox_x"].is_null()) {
                        painter.drawRect(hitboxInfo["groundBox_x"].get<int>() -  c_x,
                                            hitboxInfo["groundBox_y"].get<int>() -  c_y,
                                            hitboxInfo["groundBox_w"].get<int>(),
                                            hitboxInfo["groundBox_h"].get<int>());
                    }

                } else if (hitboxInfo["entityType"].get<std::string>() == "Monster") {

                    painter.setPen(QColor("red"));
                    if (!hitboxInfo["box_x"].is_null()) {
                        painter.drawRect(hitboxInfo["box_x"].get<int>() -  c_x,
                                            hitboxInfo["box_y"].get<int>() -  c_y,
                                            hitboxInfo["box_w"].get<int>(),
                                            hitboxInfo["box_h"].get<int>());
                    }

                    //Ground boxes);
                    painter.setPen(QColor("green"));
                    if (!hitboxInfo["groundBox_x"].is_null()) {
                        painter.drawRect(hitboxInfo["groundBox_x"].get<int>() -  c_x,
                                            hitboxInfo["groundBox_y"].get<int>() -  c_y,
                                            hitboxInfo["groundBox_w"].get<int>(),
                                            hitboxInfo["groundBox_h"].get<int>());
                    }

                } else if (hitboxInfo["entityType"].get<std::string>() == "Samos") {

                    painter.setPen(QColor("orange"));
                    if (!hitboxInfo["box_x"].is_null()) {
                        painter.drawRect(hitboxInfo["box_x"].get<int>() -  c_x,
                                            hitboxInfo["box_y"].get<int>() -  c_y,
                                            hitboxInfo["box_w"].get<int>(),
                                            hitboxInfo["box_h"].get<int>());
                    }

                    //Ground boxes);
                    painter.setPen(QColor("green"));
                    if (!hitboxInfo["groundBox_x"].is_null()) {
                        painter.drawRect(hitboxInfo["groundBox_x"].get<int>() -  c_x,
                                            hitboxInfo["groundBox_y"].get<int>() -  c_y,
                                            hitboxInfo["groundBox_w"].get<int>(),
                                            hitboxInfo["groundBox_h"].get<int>());
                    }

                    //Wall boxes);
                    if (!hitboxInfo["wallBoxL_x"].is_null()) {
                        painter.drawRect(hitboxInfo["wallBoxL_x"].get<int>() -  c_x,
                                            hitboxInfo["wallBoxL_y"].get<int>() -  c_y,
                                            hitboxInfo["wallBoxL_w"].get<int>(),
                                            hitboxInfo["wallBoxL_h"].get<int>());
                    }
                    if (!hitboxInfo["wallBoxR_x"].is_null()) {
                        painter.drawRect(hitboxInfo["wallBoxR_x"].get<int>() -  c_x,
                                            hitboxInfo["wallBoxR_y"].get<int>() -  c_y,
                                            hitboxInfo["wallBoxR_w"].get<int>(),
                                            hitboxInfo["wallBoxR_h"].get<int>());
                    }
                }
            }



            painter.setPen(QColor("magenta"));
            painter.drawRect(tempToDraw["room_position_x"].get<int>() - c_x,
                                tempToDraw["room_position_y"].get<int>() - c_y,
                                tempToDraw["room_size_x"].get<int>(),
                                tempToDraw["room_size_y"].get<int>());
        }

        // Draw fps if necessary
        if (tempToDraw["showFps"].get<bool>()) {
            painter.setPen(QColor("black"));
            painter.drawText(QPoint(2, 12), QString::fromStdString(std::to_string(tempToDraw["fps"].get<int>()) + " FPS"));
        }

        QFont f = painter.font();
        f.setPointSize(f.pointSize() * 2);
        painter.setFont(f);

        //HUD
        if ((!tempToDraw["hud_selectedWeapon"].is_null()) && tempToDraw["showHUD"].get<bool>()) {

            painter.setPen(QColor("black"));

            //Selected weapon
            painter.fillRect(QRect(70,15,100,30), QColor("white"));
            painter.drawRect(QRect(70,15,100,30));
            painter.drawText(QPoint(80, 40), QString::fromStdString(tempToDraw["hud_selectedWeapon"].get<std::string>()));

            //Missile count
            painter.fillRect(QRect(200,15,70,30), QColor("white"));
            painter.drawRect(QRect(200,15,70,30));
            painter.drawText(QPoint(210, 40), QString::fromStdString("M : " + std::to_string(tempToDraw["hud_missileCount"].get<int>())));

            //Grenade count
            painter.fillRect(QRect(290,15,70,30), QColor("white"));
            painter.drawRect(QRect(290,15,70,30));
            painter.drawText(QPoint(300, 40), QString::fromStdString("G : " + std::to_string(tempToDraw["hud_grenadeCount"].get<int>())));

            //Health count
            painter.fillRect(QRect(380,15,80,30), QColor("white"));
            painter.drawRect(QRect(380,15,80,30));
            painter.drawText(QPoint(390, 40), QString::fromStdString("H : " + std::to_string(tempToDraw["hud_health"].get<int>())));
        }

        // Dialogue
        if (tempToDraw["inDialogue"].get<bool>()) {
            // In case we changed it before
            painter.setPen(QColor("black"));

            painter.drawText(QRectF(QRect(200, 200, 1000, 200)),
                             QString::fromStdString(tempToDraw["talkingName"].get<std::string>() + " : " + tempToDraw["dialogueText"].get<std::string>()),
                    QTextOption(Qt::AlignHCenter));
        }

        //Menu
        if (tempToDraw["isPaused"].get<bool>()) {
            painter.fillRect(QRect(0,0,size().width(),size().height()), QBrush(QColor(0,0,0,200)));
            for (unsigned int i = 0; i < tempToDraw["menuOptions"].size(); i++) {
                painter.setPen(QColor("white"));
                if (tempToDraw["selectedOption"].get<int>() == (int) i)
                    painter.setPen(QColor("cyan"));
                painter.drawText(0, size().height() / 2 - 15 * tempToDraw["menuOptions"].size() + 30 * i, size().width(), 50, Qt::AlignHCenter, QString::fromStdString(tempToDraw["menuOptions"][i]));
            }
        }
        f.setPointSize(f.pointSize() / 2);
        painter.setFont(f);

    } else {
        int mc_x =  tempToDraw["mapCamera_position_x"].get<int>();
        int mc_y =  tempToDraw["mapCamera_position_y"].get<int>();
        int mapScaleDown = tempToDraw["mapScaleDown"].get<int>();
        for (nlohmann::json roomInfo : tempToDraw["mapRoomsInfo"]) {
            painter.fillRect(QRect((roomInfo["position_x"].get<int>() - mc_x) / mapScaleDown,
                                    (roomInfo["position_y"].get<int>() - mc_y) / mapScaleDown,
                                    roomInfo["size_x"].get<int>() / mapScaleDown,
                                    roomInfo["size_y"].get<int>() / mapScaleDown),
                                QColor(32,50,100));

            painter.setPen(QPen(QColor(64,100,200), 5));
            painter.drawRect(QRect((roomInfo["position_x"].get<int>() - mc_x) / mapScaleDown,
                             (roomInfo["position_y"].get<int>() - mc_y) / mapScaleDown,
                             roomInfo["size_x"].get<int>() / mapScaleDown,
                             roomInfo["size_y"].get<int>() / mapScaleDown));
        }

        for (nlohmann::json roomInfo : tempToDraw["mapRoomsInfo"]) {
            if (!roomInfo["vDoorsInfo"].is_null()) {
                for (nlohmann::json vd : roomInfo["vDoorsInfo"]) {
                    painter.fillRect(QRect((vd["x"].get<int>() + roomInfo["position_x"].get<int>() - mc_x) / mapScaleDown,
                            (vd["y"].get<int>() + roomInfo["position_y"].get<int>() - mc_y) / mapScaleDown,
                            vd["w"].get<int>() / mapScaleDown,
                            vd["h"].get<int>() / mapScaleDown),
                            QColor("darkGray"));
                }
            }
            if (!roomInfo["hDoorsInfo"].is_null()) {
                for (nlohmann::json hd : roomInfo["hDoorsInfo"]) {
                    painter.fillRect(QRect((hd["x"].get<int>() + roomInfo["position_x"].get<int>() - mc_x) / mapScaleDown,
                            (hd["y"].get<int>() + roomInfo["position_y"].get<int>() - mc_y) / mapScaleDown,
                            hd["w"].get<int>() / mapScaleDown,
                            hd["h"].get<int>() / mapScaleDown),
                            QColor("darkGray"));
                }
            }
        }

        painter.setPen(QPen(QColor(64,192,64,100), 5));
        painter.drawRect(QRect((c_x - mc_x) / mapScaleDown, (c_y - mc_y) / mapScaleDown,
                            tempToDraw["mapCamera_size_x"], tempToDraw["mapCamera_size_y"]));
        if (!tempToDraw["minimosPath"].is_null()) {
            QImage minimos(QString::fromStdString(tempToDraw["minimosPath"].get<std::string>()));

            painter.drawImage(QRect((tempToDraw["samos_x"].get<int>() + tempToDraw["samos_box_x"].get<int>() + tempToDraw["samos_box_w"].get<int>() / 2 - mc_x) / mapScaleDown - minimos.width() / 2,
                                    (tempToDraw["samos_y"].get<int>() + tempToDraw["samos_box_y"].get<int>() + tempToDraw["samos_box_h"].get<int>() / 2 - mc_y) / mapScaleDown - minimos.height() / 2,
                                    minimos.width(),
                                    minimos.height()),
                              minimos);
        }
    }
    if (tempToDraw["showDebugInfo"]) {
        painter.fillRect(QRect(70, 70, 250, 550), QBrush(QColor(0,0,0,150)));

        painter.setPen(QColor("white"));

        painter.drawText(QPoint(80, 90), QString::fromStdString("X : " + std::to_string(tempToDraw["samos_x"].get<double>())));
        painter.drawText(QPoint(80, 110), QString::fromStdString("Y : " + std::to_string(tempToDraw["samos_y"].get<double>())));
        painter.drawText(QPoint(80, 130), QString::fromStdString("VX : " + std::to_string(tempToDraw["samos_vx"].get<double>())));
        painter.drawText(QPoint(80, 150), QString::fromStdString("VY : " + std::to_string(tempToDraw["samos_vy"].get<double>())));
        painter.drawText(QPoint(80, 170), QString::fromStdString("Box X : " + std::to_string(tempToDraw["samos_box_x"].get<int>())));
        painter.drawText(QPoint(80, 190), QString::fromStdString("Box Y : " + std::to_string(tempToDraw["samos_box_y"].get<int>())));
        painter.drawText(QPoint(80, 210), QString::fromStdString("Box Width : " + std::to_string(tempToDraw["samos_box_w"].get<int>())));
        painter.drawText(QPoint(80, 230), QString::fromStdString("Box Height : " + std::to_string(tempToDraw["samos_box_h"].get<int>())));
        painter.drawText(QPoint(80, 250), QString::fromStdString("State : " + tempToDraw["samos_state"].get<std::string>()));
        painter.drawText(QPoint(80, 270), QString::fromStdString("Facing : " + tempToDraw["samos_facing"].get<std::string>()));
        painter.drawText(QPoint(80, 290), QString::fromStdString("Speed retained : " + std::to_string(tempToDraw["samos_speedRetained"].get<double>())));
        painter.drawText(QPoint(80, 310), QString::fromStdString("Retain time : " + std::to_string(tempToDraw["samos_retainTime"].get<double>())));
        painter.drawText(QPoint(80, 330), QString::fromStdString("On ground : " + std::to_string(tempToDraw["samos_onGround"].get<bool>())));
        painter.drawText(QPoint(80, 350), QString::fromStdString("Room ID : " + std::to_string(tempToDraw["samos_room"].get<int>())));
        painter.drawText(QPoint(80, 370), QString::fromStdString("I-Time : " + std::to_string(tempToDraw["samos_iTime"].get<double>())));
        painter.drawText(QPoint(80, 390), QString::fromStdString("Lag time : " + std::to_string(tempToDraw["samos_lagTime"].get<double>())));
        painter.drawText(QPoint(80, 410), QString::fromStdString("Shoot cooldown : " + std::to_string(tempToDraw["samos_shootTime"].get<double>())));
        painter.drawText(QPoint(80, 430), QString::fromStdString("Switch cooldown : " + std::to_string(tempToDraw["samos_switchDelay"].get<double>())));
        painter.drawText(QPoint(80, 450), QString::fromStdString("Fast falling : " + std::to_string(tempToDraw["samos_fastFalling"].get<bool>())));
        painter.drawText(QPoint(80, 470), QString::fromStdString("Jump time : " + std::to_string(tempToDraw["samos_jumpTime"].get<double>())));
        painter.drawText(QPoint(80, 490), QString::fromStdString("VX prior dash: " + std::to_string(tempToDraw["samos_priorDash_vx"].get<double>())));
        painter.drawText(QPoint(80, 510), QString::fromStdString("VY prior dash: " + std::to_string(tempToDraw["samos_priorDash_vy"].get<double>())));
        painter.drawText(QPoint(80, 530), QString::fromStdString("Dash time : " + std::to_string(tempToDraw["samos_dashTime"].get<double>())));
        painter.drawText(QPoint(80, 550), QString::fromStdString("Dash cooldown : " + std::to_string(tempToDraw["samos_dashCoolDown"].get<double>())));
        painter.drawText(QPoint(80, 570), QString::fromStdString("Dash direction : " + tempToDraw["samos_dashDirection"].get<std::string>()));
        painter.drawText(QPoint(80, 590), QString::fromStdString("Frame count : " + std::to_string(tempToDraw["samos_frameCount"].get<int>())));
        painter.drawText(QPoint(80, 610), QString::fromStdString("TAS Current line frame count : " + std::to_string(tempToDraw["tas_lineFrameCount"].get<int>())));
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

Game *MainWindow::getGame() const
{
    return game;
}

void MainWindow::setGame(Game *newGame)
{
    game = newGame;
}

bool MainWindow::getRender() const
{
    return render;
}

void MainWindow::setRender(bool newRender)
{
    render = newRender;
}
