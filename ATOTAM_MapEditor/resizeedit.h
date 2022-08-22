#ifndef RESIZEEDIT_H
#define RESIZEEDIT_H

#include "multitypeedit.h"
#include "moveedit.h"

class ResizeEdit : public MultiTypeEdit
{
public:
    enum Direction {Left, Right, Up, Down, UpLeft, UpRight, DownLeft, DownRight};

    ResizeEdit(Map*, Entity*entity, Direction direction, std::pair<int, int>* delta = new std::pair<int, int>());
    ResizeEdit(Map*, int roomId, Direction direction, std::pair<int, int>* delta = new std::pair<int, int>());
    virtual ~ResizeEdit();

    MoveEdit *getMove() const;
    void setMove(MoveEdit *newMove);
    std::pair<int, int> *getDelta() const;
    void setDelta(std::pair<int, int> *newDelta);
    Direction getDirection() const;
    void setDirection(Direction newDirection);

    // Edit interface
    void unmake() override;
    void make() override;

private:

    // General
    std::pair<int, int>* delta = nullptr;
    MoveEdit* move = nullptr;
    Direction direction = Right;
};

#endif // RESIZEEDIT_H
