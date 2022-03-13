#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QImage>

class Renderable
{
public:
    Renderable(QImage *texture, int x = 0, int y = 0);

    int getX() const;
    void setX(int newX);
    int getY() const;
    void setY(int newY);
    const QImage &getTexture() const;
    void setTexture(const QImage &newTexture);

private:
    QImage m_texture;
    int x;
    int y;
};

#endif // RENDERABLE_H
