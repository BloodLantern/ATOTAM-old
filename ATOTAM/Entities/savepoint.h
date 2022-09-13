#ifndef SAVEPOINT_H
#define SAVEPOINT_H

#include "npc.h"

class Savepoint : public NPC
{
public:
    Savepoint(double x, double y, int spID, std::string mapName);
    Savepoint(const Savepoint&);
    ~Savepoint();

    nlohmann::json getJsonRepresentation(bool defaultValues);
    void setJsonValues(nlohmann::json);

    int getSavepointID() const;
    void setSavepointID(int newSavepointID);

    const std::string &getMapName() const;
    void setMapName(const std::string &newMapName);

private:
    int savepointID = 0;
    std::string mapName = "";
};

#endif // SAVEPOINT_H
