#include "door.h"

Door::Door(double x, double y, std::string name)
    : Area(x, y, name)
{
    setAreaType("Door");
}

Door::~Door()
{

}

int Door::getEndingRoom() const
{
    return endingRoom;
}

void Door::setEndingRoom(int newEndingRoom)
{
    endingRoom = newEndingRoom;
}
