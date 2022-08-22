
#include "edit.h"
#ifndef EDITORPREVIEW_H
#define EDITORPREVIEW_H

#include "../ATOTAM/Entities/entity.h"
#include "../ATOTAM/map.h"
#include <QWidget>

class EditorPreview : public QWidget
{
public:
    EditorPreview(Map* map,
                           QImage* errorTexture,
                           QImage* emptyTexture,
                           int renderingMultiplier,
                           nlohmann::json editorJson,
                           double physicsFrameRate);
    ~EditorPreview();

    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

    nlohmann::json loadJson(std::string);
    void saveJson(nlohmann::json json, std::string fileName);
    QPoint getClickAreaOffset(Entity*);
    std::pair<int, int> getClickAreaSize(Entity*);
    void updateCursor(QPoint);
    void undoEdit();
    void redoEdit();
    void duplicateObject();
    void deleteObject();
    void clearUnmadeEdits();
    void addObject(Entity*);

    nlohmann::json &getEditorJson();
    void setEditorJson(nlohmann::json &newEditorJson);
    const std::string &getAssetsPath() const;
    void setAssetsPath(const std::string &newAssetsPath);
    QPoint getCamera() const;
    void setCamera(QPoint newCamera);
    Map &getCurrentMap();
    void setCurrentMap(const Map &newCurrentMap);
    int getRoomId() const;
    void setRoomId(int newRoomId);
    double getZoomFactor() const;
    void setZoomFactor(double newZoomFactor);
    Entity *getSelected() const;
    void setSelected(Entity *newSelected);
    std::vector<Edit*> &getEdits();
    void setEdits(std::vector<Edit*> &newEdits);

private:
    void drawEntity(Entity* ent, QPainter* painter);
    void getInputs();

    std::vector<Edit*> edits;
    nlohmann::json editorJson = nlohmann::json();
    Map currentMap = Map();
    int roomId = 0;
    std::string assetsPath;
    QPoint lastMousePosition;

    // paintEvent fields
    std::vector<Entity*> entities = std::vector<Entity*>();
    std::vector<Entity*> visibleEntities = std::vector<Entity*>();
    QImage* errorTexture = nullptr;
    QImage* emptyTexture = nullptr;
    QPoint camera = QPoint();
    double zoomFactor = 1;
    int renderingMultiplier = 2;

    // Camera movement
    QPoint cameraStart = QPoint();
    QPoint clickStart = QPoint();
    bool dragging = false;
    bool farEnough = false;

    // Selection
    Entity* selected = nullptr;

    // Keyboard inputs
    std::map<std::string, bool> inputList;
    std::map<std::string, double> inputTime;
    double physicsFrameRate;
};

#endif // EDITORPREVIEW_H
