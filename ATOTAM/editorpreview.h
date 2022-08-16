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

    void paintEvent(QPaintEvent*);

private:
    std::vector<Entity*>* entities;
    QImage* errorTexture;
    QImage* emptyTexture;
    QPoint camera;
    int zoomFactor = 1;
    int* renderingMultiplier;
};

#endif // EDITORPREVIEW_H
