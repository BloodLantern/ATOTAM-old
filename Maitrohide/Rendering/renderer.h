#ifndef RENDERER_H
#define RENDERER_H

#include "renderable.h"

#include <QPainter>
#include <QPaintDevice>
#include <vector>

class Renderer
{
public:
    Renderer(QPaintDevice *paintSupport);
    void render(Renderable *renderable);
private:
    QPainter *m_painter;
    std::vector<Renderable*> rendering;
};

#endif // RENDERER_H
