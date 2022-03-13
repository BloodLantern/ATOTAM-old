#include "renderer.h"

Renderer::Renderer(QPaintDevice *paintSupport)
    : m_painter(new QPainter(paintSupport))
{

}
