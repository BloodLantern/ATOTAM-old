#include "dialogue.h"

Dialogue::Dialogue(std::string text, std::string talking)
    : text(text), talking(talking)
{

}

Dialogue::Dialogue()
    : text(""), talking("")
{

}

bool Dialogue::isNull()
{
    return text == "" && talking == "";
}

const std::string &Dialogue::getText() const
{
    return text;
}

void Dialogue::setText(const std::string &newText)
{
    text = newText;
}

const std::string &Dialogue::getTalking() const
{
    return talking;
}

void Dialogue::setTalking(const std::string &newTalking)
{
    talking = newTalking;
}
