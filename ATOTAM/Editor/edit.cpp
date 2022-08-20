#include "edit.h"

Edit::Edit(Map *map)
    : map(map)
{

}

bool Edit::getMade() const
{
    return made;
}

void Edit::setMade(bool newMade)
{
    made = newMade;
}
