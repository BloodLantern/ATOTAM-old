#include "npc.h"

NPC::NPC(double x, double y, std::string facing, std::string name)
    : Living(x, y, facing, name)
{
    json = values["names"][name];
    npcType = json["npcType"];
}

const nlohmann::json &NPC::getJson() const
{
    return json;
}

void NPC::setJson(const nlohmann::json &newJson)
{
    json = newJson;
}

const std::string &NPC::getNpcType() const
{
    return npcType;
}

void NPC::setNpcType(const std::string &newNpcType)
{
    npcType = newNpcType;
}

unsigned int NPC::getTimesInteracted() const
{
    return timesInteracted;
}

void NPC::setTimesInteracted(unsigned int newTimesInteracted)
{
    timesInteracted = newTimesInteracted;
}
