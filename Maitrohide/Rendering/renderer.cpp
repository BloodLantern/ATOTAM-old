#include "renderer.h"

Renderer::Renderer(QPaintDevice *paintSupport)
{
    QPainter painter(paintSupport);
    m_painter = &painter;
}
