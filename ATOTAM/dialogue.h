#ifndef DIALOGUE_H
#define DIALOGUE_H

#define JSON_DIAGNOSTICS 1 // Json extended error messages
#include "nlohmann/json.hpp"
#include <string>
#include <vector>
#include <Entities/entity.h>

class Dialogue
{
public:
    Dialogue(nlohmann::json textArray, Entity* talking);
    Dialogue(nlohmann::json textArray, Entity* talking, std::string talkingName);
    Dialogue(); // Creates a null Dialogue
    bool isNull();

    const std::vector<std::string> &getText() const;
    void setText(const std::vector<std::string> &newText);

    unsigned int getTextAdvancement() const;
    void setTextAdvancement(unsigned int newTextAdvancement);

    Entity *getTalking() const;
    void setTalking(Entity *newTalking);

    const std::string &getTalkingName() const;
    void setTalkingName(const std::string &newTalkingName);

private:
    std::vector<std::string> text; // The text of the dialogue box
    Entity* talking = nullptr; // Who is currently talking
    std::string talkingName = "";
    unsigned int textAdvancement = 0;
};

unsigned int operator++(Dialogue d);

#endif // DIALOGUE_H
