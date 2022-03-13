#include "renderer.h"

Renderer::Renderer(QPaintDevice *paintSupport)
    : m_painter(new QPainter(paintSupport))
    , rendering({})
{

}

void Renderer::render(Renderable *renderable)
{
    m_painter->drawImage(renderable->getX(), renderable->getY(), renderable->getTexture());
}
