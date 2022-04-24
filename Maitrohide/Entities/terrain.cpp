#include "terrain.h"

Terrain::Terrain(double x, double y, CollisionBox* box, QImage* texture) : Entity(x, y, box, texture, EntityType::Terrain, false, Direction::None, false)
{

}

Terrain::~Terrain()
{

}
