#include "editorpreview.h"
#include "qpainter.h"

EditorPreview::EditorPreview(QWidget *parent, std::vector<Entity*>* entities, QImage* errorTexture, QImage* emptyTexture, int* renderingMultiplier)
    : QWidget{parent}
    , entities(entities)
    , errorTexture(errorTexture)
    , emptyTexture(emptyTexture)
    , renderingMultiplier(renderingMultiplier)
{

}

void EditorPreview::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    for (std::vector<Entity*>::iterator ent = entities->begin(); ent != entities->end(); ent++) {
        // Make sure not to use a null pointer in case there is one
        if (*ent == nullptr)
            continue;

        // If the texture is null, set it to the empty texture
        if ((*ent)->getTexture() == nullptr) {
            (*ent)->setTexture(emptyTexture);

            // Draw the outline of the entity to make sure it is visible anyway
            painter.setPen(QColor(0, 255, 0, 150));
            painter.drawRect((*ent)->getX(), (*ent)->getY(), (*ent)->getBox()->getWidth(), (*ent)->getBox()->getHeight());
        }

        // Make sure not to draw the Entities that aren't visible
        if ((*ent)->getX() + (*ent)->getTexture()->offset().x() + (*ent)->getTexture()->width() * zoomFactor < camera.x() // If too much on the left
                || (*ent)->getX() + (*ent)->getTexture()->offset().x() > camera.x() + 1920 // If too much on the right
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() + (*ent)->getTexture()->height() * zoomFactor < camera.y() // If too high
                || (*ent)->getY() + (*ent)->getTexture()->offset().y() > camera.y() + 1080) { // If too low
            continue;
        }

        //Try to draw the texture: if it fails, set it to the error texture and try again
        try {
            painter.drawImage(QRect(((*ent)->getX() + (*ent)->getTexture()->offset().x() - camera.x()) * zoomFactor,
                                    ((*ent)->getY() + (*ent)->getTexture()->offset().y() - camera.y()) * zoomFactor,
                                    (*ent)->getTexture()->width() * zoomFactor,
                                    (*ent)->getTexture()->height() * zoomFactor),
                                    *(*ent)->getTexture());
        } catch (...) {
            (*ent)->setTexture(errorTexture);
            painter.drawImage(QRect(((*ent)->getX() + (*ent)->getTexture()->offset().x() - camera.x()) * zoomFactor,
                                    ((*ent)->getY() + (*ent)->getTexture()->offset().y() - camera.y()) * zoomFactor,
                                    (*ent)->getTexture()->width() * zoomFactor,
                                    (*ent)->getTexture()->height() * zoomFactor),
                                    *(*ent)->getTexture());
        }
    }
}
