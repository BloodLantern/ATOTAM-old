#include "savepoint.h"

Savepoint::Savepoint(double x, double y, int spID, std::string mapName)
    : NPC(x, y, "Right", "Savepoint"),
      savepointID(spID),
      mapName(mapName)
{

}

Savepoint::~Savepoint()
{

}

nlohmann::json Savepoint::getJsonRepresentation()
{
    nlohmann::json result;
    result["x"] = getX();
    result["y"] = getY();
    if (getState() != "None")
        result["state"] = getState();
    if (getFacing() != "None")
        result["facing"] = getFacing();
    if (getHorizontalRepeat() != 1)
        result["horizontalRepeat"] = getHorizontalRepeat();
    if (getVerticalRepeat() != 1)
        result["verticalRepeat"] = getVerticalRepeat();
    result["spID"] = savepointID;
    return result;
}

void Savepoint::setJsonValues(nlohmann::json json)
{
    setX(json["x"]);
    setY(json["y"]);
    if (!json["state"].is_null())
        setState(json["state"]);
    if (!json["facing"].is_null())
        setFacing(json["facing"]);
    if (!json["horizontalRepeat"].is_null())
        setHorizontalRepeat(json["horizontalRepeat"]);
    if (!json["verticalRepeat"].is_null())
        setVerticalRepeat(json["verticalRepeat"]);
    savepointID = json["spID"];
}

int Savepoint::getSavepointID() const
{
    return savepointID;
}

void Savepoint::setSavepointID(int newSavepointID)
{
    savepointID = newSavepointID;
}

const std::string &Savepoint::getMapName() const
{
    return mapName;
}

void Savepoint::setMapName(const std::string &newMapName)
{
    mapName = newMapName;
}
