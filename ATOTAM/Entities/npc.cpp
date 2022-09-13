#include "npc.h"

NPC::NPC(double x, double y, std::string facing, std::string name)
    : Living(x, y, facing, name)
{
    json = values["names"][name];
    npcType = json["npcType"];
}

NPC::NPC(const NPC &n)
    : NPC(n.getX(), n.getY(), n.getFacing(), n.getName())
{
    setCurrentAnimation(updateAnimation());
    setFrame(0);
    updateTexture();
}

NPC::~NPC()
{

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

unsigned int NPC::getMaxInteractions() const
{
    return maxInteractions;
}

void NPC::setMaxInteractions(unsigned int newMaxInteractions)
{
    maxInteractions = newMaxInteractions;
}
