#include "editorpreview.h"
#include "moveedit.h"
#include "qpainter.h"
#include <QMainWindow>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QCursor>
#include <iostream>

EditorPreview::EditorPreview(Map* map, QImage* errorTexture, QImage* emptyTexture, int* renderingMultiplier, nlohmann::json editorJson)
    : editorJson(editorJson)
    , currentMap(*map)
    , entities(map->loadRooms())
    , errorTexture(errorTexture)
    , emptyTexture(emptyTexture)
    , renderingMultiplier(renderingMultiplier)
{
    setMouseTracking(true);
    camera.setX(editorJson["lastLaunch"]["map"]["camera"][0]);
    camera.setY(editorJson["lastLaunch"]["map"]["camera"][1]);
}

nlohmann::json EditorPreview::loadJson(std::string fileName)
{
    std::ifstream file(assetsPath + "/" + fileName + ".json");
    nlohmann::json temp;
    file >> temp;
    return temp;
}

void EditorPreview::saveJson(nlohmann::json json, std::string fileName)
{
    std::ofstream file(assetsPath + "/" + fileName + ".json");
    file << json.dump(4);
    file.close();
}

QPoint EditorPreview::getClickAreaOffset(Entity *ent)
{
    if (ent->getTexture() != emptyTexture)
        return ent->getTexture()->offset();
    else
        return QPoint(ent->getBox()->getX(), ent->getBox()->getY());
}

std::pair<int, int> EditorPreview::getClickAreaSize(Entity *ent)
{
    if (ent->getTexture() != emptyTexture)
        return std::pair<int, int>(ent->getTexture()->width(), ent->getTexture()->height());
    else
        return std::pair<int, int>(ent->getBox()->getWidth(), ent->getBox()->getHeight());
}

void EditorPreview::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    visibleEntities.clear();
    std::vector<Entity*> currentRoom;

    for (std::vector<Entity*>::iterator ent = entities.begin(); ent != entities.end(); ent++) {
        // Make sure not to use a null pointer in case there is one
        if (*ent == nullptr)
            continue;

        // The current room's entities are drawn on top
        if ((*ent)->getRoomId() == roomId) {
            currentRoom.push_back(*ent);
            continue;
        }

        // The selected entity is drawn on top
        if (*ent == selected)
            continue;

        drawEntity(*ent, &painter);
    }

    // Draw current room's entities on top
    for (std::vector<Entity*>::iterator ent = currentRoom.begin(); ent != currentRoom.end(); ent++)
        drawEntity(*ent, &painter);

    if (selected != nullptr) {
        // Draw the selected entity on top
        drawEntity(selected, &painter);

        // Selected entity overlay
        painter.fillRect((selected->getX() + selected->getTexture()->offset().x() - camera.x()) * zoomFactor,
                        (selected->getY() + selected->getTexture()->offset().y() - camera.y()) * zoomFactor,
                        selected->getTexture()->width() * zoomFactor * (*renderingMultiplier),
                        selected->getTexture()->height() * zoomFactor * (*renderingMultiplier),
                        QColor(0, 0, 255, editorJson["values"]["selectedEntitiesOverlayOpacity"]));
    }

    painter.end();
}

void EditorPreview::drawEntity(Entity* ent, QPainter* painter)
{
    // If the texture is null, set it to the empty texture
    if (ent->getTexture() == nullptr)
        ent->setTexture(emptyTexture);

    if (ent->getTexture() == emptyTexture) {
        // Draw the outline of the entity to make sure it is visible anyway
        painter->setPen(QColor(0, 255, 0, editorJson["values"]["invisibleEntitiesHitboxOpacity"]));
        painter->drawRect(ent->getX() + ent->getBox()->getX(),
                         ent->getY() + ent->getBox()->getY(),
                         ent->getBox()->getWidth(),
                         ent->getBox()->getHeight());
    }

    // Make sure not to draw the Entities that aren't visible
    if (ent->getX() + ent->getTexture()->offset().x() + ent->getTexture()->width() * (*renderingMultiplier) < camera.x() // If too much on the left
            || ent->getX() + ent->getTexture()->offset().x() > camera.x() + width() / zoomFactor // If too much on the right
            || ent->getY() + ent->getTexture()->offset().y() + ent->getTexture()->height() * (*renderingMultiplier) < camera.y() // If too high
            || ent->getY() + ent->getTexture()->offset().y() > camera.y() + height() / zoomFactor) // If too low
        return;

    // This Entity is visible so add it to the visible Entities vector
    visibleEntities.push_back(ent);

    // Try to draw the texture: if it fails, set it to the error texture and try again
    try {
        if (ent->getRoomId() != roomId) {
            // Dark overlay over other room's entities
            QColor base_color(50, 50, 50, editorJson["values"]["otherRoomEntityOverlayOpacity"]);

            QImage new_image = *ent->getTexture();
            QPainter pa(&new_image);
            pa.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            pa.fillRect(new_image.rect(), base_color);
            pa.end();
            painter->drawImage(QRect((ent->getX() + ent->getTexture()->offset().x() - camera.x()) * zoomFactor,
                                    (ent->getY() + ent->getTexture()->offset().y() - camera.y()) * zoomFactor,
                                    ent->getTexture()->width() * zoomFactor * (*renderingMultiplier),
                                    ent->getTexture()->height() * zoomFactor * (*renderingMultiplier)),
                                    new_image);
        } else
            painter->drawImage(QRect((ent->getX() + ent->getTexture()->offset().x() - camera.x()) * zoomFactor,
                                    (ent->getY() + ent->getTexture()->offset().y() - camera.y()) * zoomFactor,
                                    ent->getTexture()->width() * zoomFactor * (*renderingMultiplier),
                                    ent->getTexture()->height() * zoomFactor * (*renderingMultiplier)),
                                    *ent->getTexture());
    } catch (...) {
        ent->setTexture(errorTexture);
        painter->drawImage(QRect((ent->getX() + ent->getTexture()->offset().x() - camera.x()) * zoomFactor,
                                (ent->getY() + ent->getTexture()->offset().y() - camera.y()) * zoomFactor,
                                ent->getTexture()->width() * zoomFactor * (*renderingMultiplier),
                                ent->getTexture()->height() * zoomFactor * (*renderingMultiplier)),
                                *ent->getTexture());
    }
}

void EditorPreview::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        dragging = true;
        clickStart = event->pos();
        cameraStart = camera;
        event->accept();
    } else if (event->button() == Qt::LeftButton) {
        Entity* clicked = nullptr;
        int maxDistance = editorJson["values"]["resizeMaximumCursorDistance"];
        // Try to select the entity at the mouse position
        for (std::vector<Entity*>::iterator ent = visibleEntities.begin(); ent != visibleEntities.end(); ent++)
                if (event->pos().x() < ((*ent)->getX() + getClickAreaOffset(*ent).x() + getClickAreaSize(*ent).first * 2 - camera.x() + maxDistance) * zoomFactor // If on the left of the right edge of the entity's texture
                        && event->pos().x() > ((*ent)->getX() + getClickAreaOffset(*ent).x() - camera.x() - maxDistance) * zoomFactor // If on the right of the left edge of the entity's texture
                        && event->pos().y() < ((*ent)->getY() + getClickAreaOffset(*ent).y() + getClickAreaSize(*ent).second * 2 - camera.y() + maxDistance) * zoomFactor // If over the bottom edge of the entity's texture
                        && event->pos().y() > ((*ent)->getY() + getClickAreaOffset(*ent).y() - camera.y() - maxDistance) * zoomFactor) // If under the top edge of the entity's texture
                    clicked = *ent;

        if (clicked == nullptr) {
            // Deselection
            selected = nullptr;
            setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        } else {
            // Selection
            selected = clicked;
            dragging = true;
            clickStart = event->pos();
            selectedStart = QPoint(selected->getX(), selected->getY());
        }

        event->accept();
        update();
    } else
        event->ignore();
}

void EditorPreview::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton || event->button() == Qt::LeftButton) {
        dragging = false;
        farEnough = false;
    }

    if (event->button() == Qt::LeftButton)
        if (selected != nullptr) {
            if (selectedStart != QPoint(selected->getX(), selected->getY())) {
                MoveEdit* move = new MoveEdit(&currentMap, selected, selectedStart, QPoint(selected->getX(), selected->getY()));
                move->make();
                edits.push_back(move);
            }
        }
}

void EditorPreview::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::RightButton) && dragging) {
        QPoint pos = event->pos();
        // If the mouse was moved far enough
        if (std::sqrt(std::pow(pos.x() - clickStart.x(), 2) + std::pow(pos.y() - clickStart.y(), 2)) > editorJson["values"]["cameraMinimumMove"])
            farEnough = true;

        if (farEnough) {
            // Move camera
            camera.setX(cameraStart.x() + (clickStart.x() - pos.x()) / zoomFactor);
            camera.setY(cameraStart.y() + (clickStart.y() - pos.y()) / zoomFactor);
            event->accept();
            update();
        }
    } else if ((event->buttons() & Qt::LeftButton) && dragging) {
        QPoint pos = event->pos();
        // If the Shift key is held
        if (inputList["disableMovingSteps"]) {
            // Move object without steps
            selected->setX(selectedStart.x() - (clickStart.x() - pos.x()) / zoomFactor);
            selected->setY(selectedStart.y() - (clickStart.y() - pos.y()) / zoomFactor);
            event->accept();
            update();
        } else {
            // Move object with steps
            selected->setX(selectedStart.x() - (((int)(std::round(clickStart.x() - pos.x()) / zoomFactor)) / editorJson["values"]["selectedEntityMoveStep"].get<int>()) * editorJson["values"]["selectedEntityMoveStep"].get<int>());
            selected->setY(selectedStart.y() - (((int)(std::round(clickStart.y() - pos.y()) / zoomFactor)) / editorJson["values"]["selectedEntityMoveStep"].get<int>()) * editorJson["values"]["selectedEntityMoveStep"].get<int>());
            event->accept();
            update();
        }
    } else {
        if (selected) {
            // Setting up variables
            int maxDistance = editorJson["values"]["resizeMaximumCursorDistance"];
            // Relative to the entity
            QPoint relativeCursorPos = event->pos() - QPoint(selected->getX(), selected->getY()) + camera;

            bool left = false;
            if (relativeCursorPos.x() < maxDistance * zoomFactor // Not too much on the right
                    && relativeCursorPos.x() > -maxDistance * zoomFactor // Not too much on the left
                    && relativeCursorPos.y() > -maxDistance * zoomFactor // Not too high
                    && relativeCursorPos.y() < (selected->getBox()->getHeight() + maxDistance) * zoomFactor) // Not too low
                left = true;
            bool right = false;
            if (relativeCursorPos.x() < (maxDistance + selected->getBox()->getWidth()) * zoomFactor // Not too much on the right
                    && relativeCursorPos.x() > (-maxDistance + selected->getBox()->getWidth()) * zoomFactor // Not too much on the right
                    && relativeCursorPos.y() > -maxDistance * zoomFactor // Not too high
                    && relativeCursorPos.y() < (selected->getBox()->getHeight() + maxDistance) * zoomFactor) // Not too low
                right = true;
            bool up = false;
            if (relativeCursorPos.y() < maxDistance * zoomFactor // Not too high
                    && relativeCursorPos.y() > -maxDistance * zoomFactor // Not too low
                    && relativeCursorPos.x() > -maxDistance * zoomFactor // Not too much on the left
                    && relativeCursorPos.x() < (selected->getBox()->getWidth() + maxDistance) * zoomFactor) // Not too much on the right
                up = true;
            bool down = false;
            if (relativeCursorPos.y() < (maxDistance + selected->getBox()->getHeight()) * zoomFactor // Not too high
                    && relativeCursorPos.y() > (-maxDistance + selected->getBox()->getHeight()) * zoomFactor // Not too low
                    && relativeCursorPos.x() > -maxDistance * zoomFactor // Not too much on the left
                    && relativeCursorPos.x() < (selected->getBox()->getWidth() + maxDistance) * zoomFactor) // Not too much on the right
                down = true;

            if ((left && up) || (right && down))
                setCursor(QCursor(Qt::CursorShape::SizeFDiagCursor));
            else if ((left && down) || (right && up))
                setCursor(QCursor(Qt::CursorShape::SizeBDiagCursor));
            else if (left || right)
                setCursor(QCursor(Qt::CursorShape::SizeHorCursor));
            else if (up || down)
                setCursor(QCursor(Qt::CursorShape::SizeVerCursor));
            else
                setCursor(QCursor(Qt::CursorShape::ArrowCursor));

            event->accept();
        }
    }
}

void EditorPreview::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0) {
        // Camera offset after zoom
        camera.setX(camera.x() + event->position().x() * 0.2 / zoomFactor);
        camera.setY(camera.y() + event->position().y() * 0.2 / zoomFactor);
        // Zoom in
        zoomFactor *= editorJson["values"]["zoomMultiplier"].get<float>();
    } else if (event->angleDelta().y() < 0) {
        // Camera offset after zoom
        camera.setX(camera.x() - event->position().x() * 0.25 / zoomFactor);
        camera.setY(camera.y() - event->position().y() * 0.25 / zoomFactor);
        // Zoom out
        zoomFactor /= editorJson["values"]["zoomMultiplier"].get<float>();
    } else
        event->ignore();

    if (event->angleDelta().y() != 0) {
        // Zoom was modified
        event->accept();
        update();
    }
}

nlohmann::json &EditorPreview::getEditorJson()
{
    return editorJson;
}

void EditorPreview::setEditorJson(nlohmann::json &newEditorJson)
{
    editorJson = newEditorJson;
}

std::map<std::string, double>* EditorPreview::getInputTime()
{
    return &inputTime;
}

void EditorPreview::setInputTime(std::map<std::string, double>* newInputTime)
{
    inputTime = *newInputTime;
}

std::map<std::string, bool>* EditorPreview::getInputList()
{
    return &inputList;
}

void EditorPreview::setInputList(std::map<std::string, bool>* newInputList)
{
    inputList = *newInputList;
}

const std::string &EditorPreview::getAssetsPath() const
{
    return assetsPath;
}

void EditorPreview::setAssetsPath(const std::string &newAssetsPath)
{
    assetsPath = newAssetsPath;
}

QPoint EditorPreview::getCamera() const
{
    return camera;
}

void EditorPreview::setCamera(QPoint newCamera)
{
    camera = newCamera;
}

const Map &EditorPreview::getCurrentMap() const
{
    return currentMap;
}

void EditorPreview::setCurrentMap(const Map &newCurrentMap)
{
    currentMap = newCurrentMap;
}

int EditorPreview::getRoomId() const
{
    return roomId;
}

void EditorPreview::setRoomId(int newRoomId)
{
    roomId = newRoomId;
}

double EditorPreview::getZoomFactor() const
{
    return zoomFactor;
}

void EditorPreview::setZoomFactor(double newZoomFactor)
{
    zoomFactor = newZoomFactor;
}

Entity *EditorPreview::getSelected() const
{
    return selected;
}

void EditorPreview::setSelected(Entity *newSelected)
{
    selected = newSelected;
}

const std::vector<Edit*> &EditorPreview::getEdits() const
{
    return edits;
}

void EditorPreview::setEdits(const std::vector<Edit*> &newEdits)
{
    edits = newEdits;
}
