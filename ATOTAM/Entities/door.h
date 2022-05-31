#ifndef DOOR_H
#define DOOR_H

#include "area.h"

class Door : public Area
{
public:
    Door(double x, double y, std::string name);
    ~Door();

    int getEndingRoom() const;
    void setEndingRoom(int newEndingRoom);

private:
    int endingRoom;
};

#endif // DOOR_H
