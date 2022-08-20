#ifndef TERRAIN_H
#define TERRAIN_H

#include "entity.h"

class Terrain : public Entity
{
public:
    Terrain(double x, double y, CollisionBox* box, QImage* texture, std::string name);
    Terrain(double x, double y, std::string name);
    ~Terrain();
    const std::string &getMaterial() const;
    void setMaterial(const std::string &newMaterial);

    const std::string &getTerrainType() const;
    void setTerrainType(const std::string &newTerrainType);

private:
    std::string terrainType = "";
    std::string material = "";
};

#endif // TERRAIN_H
