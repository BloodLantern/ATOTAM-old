#ifndef COLLISIONBOX_H
#define COLLISIONBOX_H


class CollisionBox
{
public:
    CollisionBox(int x, int y, int width, int height);
    CollisionBox(int width, int height);

    int getHeight() const;
    void setHeight(int newHeight);

    int getWidth() const;
    void setWidth(int newWidth);

    int getY() const;
    void setY(int newY);

    int getX() const;
    void setX(int newX);

private:
    int x = 0; //offset in px
    int y = 0; //offset in px
    int width = 0; //in px
    int height = 0; //in px
};

bool operator!=(CollisionBox a, CollisionBox b);

#endif // COLLISIONBOX_H
