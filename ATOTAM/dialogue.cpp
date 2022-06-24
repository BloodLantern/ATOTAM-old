#include "dialogue.h"

Dialogue::Dialogue(nlohmann::json textArray, Entity* talking)
    : talking(talking), talkingName(talking->getName())
{
    // Text
    std::vector<std::string> text;
    for (std::string str : textArray)
        text.push_back(str);
    setText(text);
}

Dialogue::Dialogue(nlohmann::json textArray, Entity* _talking, std::string _talkingName)
    : talking(_talking), talkingName(_talkingName)
{
    std::vector<std::string> text;
    for (std::string str : textArray)
        text.push_back(str);
    setText(text);
}

Dialogue::Dialogue()
    : text(std::vector<std::string>()), talking(nullptr), talkingName("")
{

}

bool Dialogue::isNull()
{
    return text.size() == 0 && talkingName == "";
}

const std::vector<std::string> &Dialogue::getText() const
{
    return text;
}

void Dialogue::setText(const std::vector<std::string> &newText)
{
    text = newText;
}

unsigned int Dialogue::getTextAdvancement() const
{
    return textAdvancement;
}

void Dialogue::setTextAdvancement(unsigned int newTextAdvancement)
{
    textAdvancement = newTextAdvancement;
}

Entity *Dialogue::getTalking() const
{
    return talking;
}

void Dialogue::setTalking(Entity *newTalking)
{
    talking = newTalking;
}

const std::string &Dialogue::getTalkingName() const
{
    return talkingName;
}

void Dialogue::setTalkingName(const std::string &newTalkingName)
{
    talkingName = newTalkingName;
}

unsigned int operator++(Dialogue d)
{
    d.setTextAdvancement(d.getTextAdvancement() + 1);
    return d.getTextAdvancement();
}
