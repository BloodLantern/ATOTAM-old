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
