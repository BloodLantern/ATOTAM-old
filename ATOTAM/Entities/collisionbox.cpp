#include "collisionbox.h"

CollisionBox::CollisionBox(int x, int y, int width, int height) : x(x), y(y), width(width), height(height)
{

}

CollisionBox::CollisionBox(int width, int height) : x(0), y(0), width(width), height(height)
{

}

int CollisionBox::getHeight()
{
    return height;
}

void CollisionBox::setHeight(int newHeight)
{
    height = newHeight;
}

int CollisionBox::getWidth()
{
    return width;
}

void CollisionBox::setWidth(int newWidth)
{
    width = newWidth;
}

int CollisionBox::getY()
{
    return y;
}

void CollisionBox::setY(int newY)
{
    y = newY;
}

int CollisionBox::getX()
{
    return x;
}

void CollisionBox::setX(int newX)
{
    x = newX;
}

bool operator!=(CollisionBox a, CollisionBox b) {
    return (a.getX() != b.getX() || a.getY() != b.getY() || a.getHeight() != b.getHeight() || a.getWidth() != b.getWidth());
}
