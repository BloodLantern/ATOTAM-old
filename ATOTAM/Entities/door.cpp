#include "door.h"

Door::Door(double x, double y, std::string name)
    : Area(x, y, name)
{

}

Door::~Door()
{

}

QPoint Door::getStartingRoom() const
{
    return startingRoom;
}

void Door::setStartingRoom(QPoint newStartingRoom)
{
    startingRoom = newStartingRoom;
}

QPoint Door::getEndingRoom() const
{
    return endingRoom;
}

void Door::setEndingRoom(QPoint newEndingRoom)
{
    endingRoom = newEndingRoom;
}
