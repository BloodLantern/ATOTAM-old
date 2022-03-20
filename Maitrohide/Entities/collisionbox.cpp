#include "collisionbox.h"

CollisionBox::CollisionBox(int x, int y, int width, int height) : x(x), y(y), width(width), height(height)
{

}

CollisionBox::CollisionBox(int width, int height) : width(width), height(height)
{

}

int CollisionBox::getHeight() const
{
    return height;
}

void CollisionBox::setHeight(int newHeight)
{
    height = newHeight;
}

int CollisionBox::getWidth() const
{
    return width;
}

void CollisionBox::setWidth(int newWidth)
{
    width = newWidth;
}

int CollisionBox::getY() const
{
    return y;
}

void CollisionBox::setY(int newY)
{
    y = newY;
}

int CollisionBox::getX() const
{
    return x;
}

void CollisionBox::setX(int newX)
{
    x = newX;
}
