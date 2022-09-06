#ifndef MOVEEDIT_H
#define MOVEEDIT_H

#include "multitypeedit.h"

class MoveEdit : public MultiTypeEdit
{
public:
    MoveEdit(Map* map, Entity* entity, QPoint delta = QPoint());
    MoveEdit(Map* map, int roomId, QPoint delta = QPoint());
    virtual ~MoveEdit();

    QPoint getDelta() const;
    void setDelta(QPoint newDelta);

    // Edit interface
    void unmake() override;
    void make() override;

private:
    // General
    QPoint delta = QPoint();
};

#endif // MOVEEDIT_H
