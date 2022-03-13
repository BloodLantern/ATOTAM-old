#ifndef RENDERER_H
#define RENDERER_H

#include "renderable.h"
#include <QPainter>
#include <QPaintDevice>

class Renderer
{
public:
    Renderer(QPaintDevice *paintSupport);
    void render(Renderable *renderable);
private:
    QPainter *m_painter;
};

#endif // RENDERER_H
