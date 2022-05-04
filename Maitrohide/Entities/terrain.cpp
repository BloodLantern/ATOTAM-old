#include "terrain.h"

Terrain::Terrain(double x, double y, CollisionBox* box, QImage* texture, std::string name) : Entity(x, y, box, texture, EntityType::Terrain, false, Direction::None, false, name)
{

}

Terrain::~Terrain()
{

}
