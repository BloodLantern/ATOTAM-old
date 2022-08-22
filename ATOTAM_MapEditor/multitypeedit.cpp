#include "multitypeedit.h"

MultiTypeEdit::MultiTypeEdit(Map *map, Entity *entity)
    : Edit(map)
    , entity(entity)
    , roomEdit(false)
{

}

MultiTypeEdit::MultiTypeEdit(Map *map, int roomId)
    : Edit(map)
    , roomId(roomId)
    , roomEdit(true)
{

}

MultiTypeEdit::~MultiTypeEdit()
{
}

Entity *MultiTypeEdit::getEntity() const
{
    return entity;
}

void MultiTypeEdit::setEntity(Entity *newEntity)
{
    entity = newEntity;
}

int MultiTypeEdit::getRoomId() const
{
    return roomId;
}

void MultiTypeEdit::setRoomId(int newRoomId)
{
    roomId = newRoomId;
}

bool MultiTypeEdit::getRoomEdit() const
{
    return roomEdit;
}

void MultiTypeEdit::setRoomEdit(bool newRoomEdit)
{
    roomEdit = newRoomEdit;
}
