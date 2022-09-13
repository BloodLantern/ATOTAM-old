#ifndef DOOR_H
#define DOOR_H

#include "area.h"

class Door : public Area
{
public:
    Door(double x, double y, std::string name);
    Door(const Door&);
    ~Door();

    nlohmann::json getJsonRepresentation(bool defaultValues);
    void setJsonValues(nlohmann::json);

    std::string getEndingRoom() const;
    void setEndingRoom(std::string newEndingRoom);

private:
    std::string endingRoom = "0";
};

#endif // DOOR_H
