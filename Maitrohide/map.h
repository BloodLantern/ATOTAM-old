#ifndef MAP_H
#define MAP_H

#include "nlohmann/json.hpp"
#include <string>
#include <Entities/entity.h>

class Map
{
public:
    static std::vector<Entity*> loadMap(Map map); // Loads a map into a MainWindow
    Map(std::string file);
    std::string getStringRoom();
    const std::string &getName() const;
    void setName(const std::string &newName);

    const std::string &getFilePath() const;
    void setFilePath(const std::string &newFilePath);

    const nlohmann::json &getJson() const;
    void setJson(const nlohmann::json &newJson);

    QPoint getRoom() const;
    void setRoom(QPoint newRoom);

private:
    std::string name;
    std::string filePath;
    nlohmann::json json;
    QPoint room; // Default room
};

#endif // MAP_H
