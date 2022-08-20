#include "terrain.h"

Terrain::Terrain(double x, double y, CollisionBox* box, QImage* texture, std::string name)
    : Entity(x, y, box, texture, "Terrain", false, "None", 0.0, name, false)
{
    setLastFrameFacing("None");
}

Terrain::Terrain(double x, double y, std::string name) :
    Entity(x, y, "None", name)
{

}

Terrain::~Terrain()
{

}

const std::string &Terrain::getMaterial() const
{
    return material;
}

void Terrain::setMaterial(const std::string &newMaterial)
{
    material = newMaterial;
}

const std::string &Terrain::getTerrainType() const
{
    return terrainType;
}

void Terrain::setTerrainType(const std::string &newTerrainType)
{
    terrainType = newTerrainType;
}
