#ifndef NPC_H
#define NPC_H

#include "living.h"

class NPC : public Living
{
public:
    NPC(double x, double y, std::string facing, std::string name);

    const nlohmann::json &getJson() const;
    void setJson(const nlohmann::json &newJson);

    const std::string &getNpcType() const;
    void setNpcType(const std::string &newNpcType);

    unsigned int getTimesInteracted() const;
    void setTimesInteracted(unsigned int newTimesInteracted);

    unsigned int getMaxInteractions() const;
    void setMaxInteractions(unsigned int newMaxInteractions);

private:
    nlohmann::json json;
    std::string npcType = "";
    unsigned int timesInteracted = 0;
    unsigned int maxInteractions = 0;
};

#endif // NPC_H
