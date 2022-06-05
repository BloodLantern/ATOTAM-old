#include "dialogue.h"

Dialogue::Dialogue(nlohmann::json textArray, std::string talking)
    : talking(talking)
{
    std::vector<std::string> text;
    for (std::string str : textArray)
        text.push_back(str);
    setText(text);
}

Dialogue::Dialogue()
    : text(std::vector<std::string>()), talking("")
{

}

bool Dialogue::isNull()
{
    return text.size() == 0 && talking == "";
}

const std::string &Dialogue::getTalking() const
{
    return talking;
}

void Dialogue::setTalking(const std::string &newTalking)
{
    talking = newTalking;
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

unsigned int operator++(Dialogue d)
{
    d.setTextAdvancement(d.getTextAdvancement() + 1);
    return d.getTextAdvancement();
}
