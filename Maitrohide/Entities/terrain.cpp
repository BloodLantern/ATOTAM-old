#include "terrain.h"

Terrain::Terrain(double x, double y, CollisionBox* box, QImage* texture, std::string name) : Entity(x, y, box, texture, "Terrain", false, "None", 0.0, name, false)
{
    setLastFrameFacing("None");
}

Terrain::~Terrain()
{

}
