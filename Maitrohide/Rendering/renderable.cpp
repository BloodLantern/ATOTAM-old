#include "renderable.h"

Renderable::Renderable(QImage *texture, int x, int y)
    : m_texture(*texture)
    , x(x)
    , y(y)
{

}

int Renderable::getX() const
{
    return x;
}

void Renderable::setX(int newX)
{
    x = newX;
}

int Renderable::getY() const
{
    return y;
}

void Renderable::setY(int newY)
{
    y = newY;
}

const QImage &Renderable::getTexture() const
{
    return m_texture;
}

void Renderable::setTexture(const QImage &newTexture)
{
    m_texture = newTexture;
}
