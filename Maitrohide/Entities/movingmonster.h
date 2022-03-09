#ifndef MOVINGMONSTER_H
#define MOVINGMONSTER_H

#include "monster.h"
#include "movingentity.h"

class MovingMonster : public Monster, public MovingEntity
{
public:
    MovingMonster();
};

#endif // MOVINGMONSTER_H
