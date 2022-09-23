
#include "edit.h"
#include "propertiesmodel.h"
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
                           double physicsFrameRate,
                           std::string assetsPath);
    ~EditorPreview();

    void getInputs();
    nlohmann::json loadJson(std::string);
    void saveJson(nlohmann::json json, std::string fileName);
    QPoint getClickAreaOffset(Entity*);
    std::pair<int, int> getClickAreaSize(Entity*);
    void updateCursor(QPoint);

    void undoEdit();
    void redoEdit();
    void duplicateObject();
    void deleteObject();
    void moveObjectByOnePixel(std::string direction);
    void resetSizeObject();

    void clearUnmadeEdits();
    void addObject(Entity*);
    // Clears Entity list. Also deletes the pointers
    void clearEntities();

    nlohmann::json &getEditorJson();
    void setEditorJson(nlohmann::json &newEditorJson);
    const std::string &getAssetsPath() const;
    void setAssetsPath(const std::string &newAssetsPath);
    QPoint getCamera() const;
    void setCamera(QPoint newCamera);
    Map* getCurrentMap();
    void setCurrentMap(const Map &newCurrentMap);
    std::string getRoomId() const;
    void setRoomId(std::string newRoomId);
    double getZoomFactor() const;
    void setZoomFactor(double newZoomFactor);
    Entity *getSelected() const;
    void setSelected(Entity *newSelected);
    Entity **getSelectedPointer();
    std::vector<Edit*> &getEdits();
    void setEdits(std::vector<Edit*> &newEdits);
    std::map<std::string, bool> &getInputList();
    void setInputList(std::map<std::string, bool> &newInputList);
    PropertiesModel *getProperties() const;
    void setProperties(PropertiesModel *newProperties);

    const std::vector<Entity *> &getEntities() const;
    void setEntities(const std::vector<Entity *> &newEntities);

public slots:
    void updateProperty(std::string key, std::string value);

private:
    void drawEntity(Entity* ent, QPainter* painter);
    void removeNullEntities();
    void updateProperties();

    std::vector<Edit*> edits;
    std::string assetsPath;
    nlohmann::json editorJson = nlohmann::json();
    nlohmann::json windowsKeyCodes = nlohmann::json();
    Map currentMap = Map();
    std::string roomId = "0";
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
    PropertiesModel* properties = nullptr;

    // Keyboard inputs
    std::map<std::string, bool> inputList;
    std::map<std::string, double> inputTime;
    double physicsFrameRate;

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
};

#endif // EDITORPREVIEW_H
