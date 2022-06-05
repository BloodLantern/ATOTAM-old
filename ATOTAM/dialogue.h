#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <string>



class Dialogue
{
public:
    Dialogue();

private:
    std::string text; // The text of the dialogue box
    std::string talking; // Who is currently talking
};

#endif // DIALOGUE_H
