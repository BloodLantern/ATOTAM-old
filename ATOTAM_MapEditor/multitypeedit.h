#ifndef MULTITYPEEDIT_H
#define MULTITYPEEDIT_H

#include "edit.h"

class MultiTypeEdit : public Edit
{
public:
    MultiTypeEdit(Map* map, Entity* entity);
    MultiTypeEdit(Map* map, int roomId);
    virtual ~MultiTypeEdit();

    Entity *getEntity() const;
    void setEntity(Entity *newEntity);
    int getRoomId() const;
    void setRoomId(int newRoomId);
    bool getRoomEdit() const;
    void setRoomEdit(bool newRoomEdit);

protected:
    Entity* entity = nullptr;
    int roomId = 0;
    bool roomEdit = false;
};

#endif // MULTITYPEEDIT_H
