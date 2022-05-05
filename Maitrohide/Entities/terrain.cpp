#include "terrain.h"

Terrain::Terrain(double x, double y, CollisionBox* box, QImage* texture, std::string name) : Entity(x, y, box, texture, "Terrain", false, "None", false, name)
{

}

Terrain::~Terrain()
{

}
