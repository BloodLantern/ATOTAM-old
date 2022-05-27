#ifndef MAP_H
#define MAP_H

#include "nlohmann/json.hpp"
#include <string>

class Map
{
public:
    Map(std::string filePath);
private:
    std::string name;
    std::string filePath;
};

#endif // MAP_H
