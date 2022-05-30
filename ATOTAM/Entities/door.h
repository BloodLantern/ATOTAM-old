#ifndef DOOR_H
#define DOOR_H

#include "area.h"

class Door : public Area
{
public:
    Door(double x, double y, std::string name);
    ~Door();
    QPoint getStartingRoom() const;
    void setStartingRoom(QPoint newStartingRoom);

    QPoint getEndingRoom() const;
    void setEndingRoom(QPoint newEndingRoom);

private:
    QPoint startingRoom;
    QPoint endingRoom;
};

#endif // DOOR_H
