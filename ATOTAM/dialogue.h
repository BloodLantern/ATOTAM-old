#ifndef DIALOGUE_H
#define DIALOGUE_H

#include "nlohmann/json.hpp"
#include <string>
#include <vector>

class Dialogue
{
public:
    Dialogue(nlohmann::json textArray, std::string talking);
    Dialogue();
    bool isNull();

    const std::string &getTalking() const;
    void setTalking(const std::string &newTalking);

    const std::vector<std::string> &getText() const;
    void setText(const std::vector<std::string> &newText);
    unsigned int getTextAdvancement() const;
    void setTextAdvancement(unsigned int newTextAdvancement);

private:
    std::vector<std::string> text; // The text of the dialogue box
    std::string talking; // Who is currently talking
    unsigned int textAdvancement;
};

unsigned int operator++(Dialogue d);

#endif // DIALOGUE_H
