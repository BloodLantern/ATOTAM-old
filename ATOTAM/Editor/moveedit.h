#ifndef MOVEEDIT_H
#define MOVEEDIT_H

#include "multitypeedit.h"

class MoveEdit : public MultiTypeEdit
{
public:
    MoveEdit(Map* map, Entity* entity, QPoint* delta = new QPoint());
    MoveEdit(Map* map, int roomId, QPoint* delta = new QPoint());
    virtual ~MoveEdit();

    QPoint *getDelta() const;
    void setDelta(QPoint *newDelta);

    // Edit interface
    void unmake() override;
    void make() override;

private:
    // General
    QPoint* delta = nullptr;
};

#endif // MOVEEDIT_H
