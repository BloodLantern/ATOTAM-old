#ifndef EDITORPREVIEW_H
#define EDITORPREVIEW_H

#include "Entities/entity.h"
#include <QWidget>

class EditorPreview : public QWidget
{
    Q_OBJECT
public:
    explicit EditorPreview(QWidget *parent,
                           std::vector<Entity*>* entities,
                           QImage* errorTexture,
                           QImage* emptyTexture,
                           int* renderingMultiplier);

    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;

private:
    // paintEvent fields
    std::vector<Entity*>* entities = nullptr;
    QImage* errorTexture = nullptr;
    QImage* emptyTexture = nullptr;
    QPoint camera = QPoint();
    double zoomFactor = 1;
    int* renderingMultiplier = nullptr;

    // Camera movement
    QPoint cameraStart = QPoint();
    QPoint clickStart = QPoint();
    bool dragging = false;

    // Selection
    Entity* selected = nullptr;

    // QWidget interface
};

#endif // EDITORPREVIEW_H
