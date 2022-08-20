#ifndef MOVEEDIT_H
#define MOVEEDIT_H

#include "edit.h"

class MoveEdit : public Edit
{
public:
    MoveEdit(Map* map, Entity* entity, QPoint startingPos, QPoint endingPos);
    MoveEdit(Map* map, int roomId, QPoint startingPos, QPoint endingPos);

private:
    Entity* entity = nullptr;
    int roomId = 0;

    // General
    bool roomMove = false;
    QPoint startingPos = QPoint();
    QPoint endingPos = QPoint();

    // Edit interface
public:
    void unmake() override;
    void make() override;
};

#endif // MOVEEDIT_H
