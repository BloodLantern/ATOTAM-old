#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <string>



class Dialogue
{
public:
    Dialogue(std::string text, std::string talking);
    Dialogue();
    bool isNull();

    const std::string &getText() const;
    void setText(const std::string &newText);

    const std::string &getTalking() const;
    void setTalking(const std::string &newTalking);

private:
    std::string text; // The text of the dialogue box
    std::string talking; // Who is currently talking
};

#endif // DIALOGUE_H
