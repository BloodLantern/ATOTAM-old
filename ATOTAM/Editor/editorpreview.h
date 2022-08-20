
#include "edit.h"
#ifndef EDITORPREVIEW_H
#define EDITORPREVIEW_H

#include "Entities/entity.h"
#include "map.h"
#include <QWidget>

class EditorPreview : public QWidget
{
public:
    EditorPreview(Map* map,
                           QImage* errorTexture,
                           QImage* emptyTexture,
                           int* renderingMultiplier,
                           nlohmann::json editorJson);

    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;

    nlohmann::json loadJson(std::string);
    void saveJson(nlohmann::json json, std::string fileName);
    QPoint getClickAreaOffset(Entity*);
    std::pair<int, int> getClickAreaSize(Entity*);

    nlohmann::json &getEditorJson();
    void setEditorJson(nlohmann::json &newEditorJson);
    std::map<std::string, bool>* getInputList();
    void setInputList(std::map<std::string, bool>* newInputList);
    std::map<std::string, double>* getInputTime();
    void setInputTime(std::map<std::string, double>* newInputTime);
    const std::string &getAssetsPath() const;
    void setAssetsPath(const std::string &newAssetsPath);
    QPoint getCamera() const;
    void setCamera(QPoint newCamera);
    const Map &getCurrentMap() const;
    void setCurrentMap(const Map &newCurrentMap);
    int getRoomId() const;
    void setRoomId(int newRoomId);
    double getZoomFactor() const;
    void setZoomFactor(double newZoomFactor);
    Entity *getSelected() const;
    void setSelected(Entity *newSelected);
    const std::vector<Edit*> &getEdits() const;
    void setEdits(const std::vector<Edit*> &newEdits);

private:
    void drawEntity(Entity* ent, QPainter* painter);

    std::vector<Edit*> edits;
    nlohmann::json editorJson = nlohmann::json();
    Map currentMap = Map();
    int roomId = 0;
    std::string assetsPath;

    // paintEvent fields
    std::vector<Entity*> entities = std::vector<Entity*>();
    std::vector<Entity*> visibleEntities = std::vector<Entity*>();
    QImage* errorTexture = nullptr;
    QImage* emptyTexture = nullptr;
    QPoint camera = QPoint();
    double zoomFactor = 1;
    int* renderingMultiplier = nullptr;

    // Camera movement
    QPoint cameraStart = QPoint();
    QPoint clickStart = QPoint();
    bool dragging = false;
    bool farEnough = false;

    // Selection
    QPoint selectedStart = QPoint();
    Entity* selected = nullptr;

    // QWidget interface

    // Keyboard inputs
    std::map<std::string, bool> inputList;
    std::map<std::string, double> inputTime;

    // QWidget interface
};

#endif // EDITORPREVIEW_H
