#ifndef SAVEPOINT_H
#define SAVEPOINT_H

#include "area.h"

class Savepoint : public Area
{
public:
    Savepoint(double x, double y, int spID, std::string mapName);
    ~Savepoint();

    int getSavepointID() const;
    void setSavepointID(int newSavepointID);

    const std::string &getMapName() const;
    void setMapName(const std::string &newMapName);

private:
    int savepointID = 0;
    std::string mapName = "";
};

#endif // SAVEPOINT_H
