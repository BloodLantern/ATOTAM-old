#ifndef DOOR_H
#define DOOR_H

#include "area.h"

class Door : public Area
{
public:
    Door(double x, double y, std::string name);
    ~Door();

    nlohmann::json getJsonRepresentation(bool defaultValues);
    void setJsonValues(nlohmann::json);

    int getEndingRoom() const;
    void setEndingRoom(int newEndingRoom);

private:
    int endingRoom = 0;
};

#endif // DOOR_H
