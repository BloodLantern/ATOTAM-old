#ifndef PHYSICS_H
#define PHYSICS_H

#include <Entities/entity.h>
#include <Entities/projectile.h>

#include "Entities/dynamicobj.h"
#include "Entities/entity.h"
#include "Entities/terrain.h"
#include "game.h"
#include "map.h"
#include "save.h"

class Physics
{
public:
    static double gravity; //p.s^-2
    static double frameRate; //fps

    static std::tuple<std::string, std::vector<Entity*>, std::vector<Entity*>, Map, Save> updatePhysics(Game* game);
    static std::vector<Entity*> handleCollision(Entity* obj1, Entity* obj2);
    static bool updateProjectile(Projectile* p);
    static bool canChangeBox(Entity *e, CollisionBox *b, std::vector<Terrain*> *ts, std::vector<DynamicObj*> *ds, std::pair<int, int> roomS, std::pair<int, int> roomE);
    static bool canChangeBoxAxis(Entity *e, CollisionBox *b, std::vector<Terrain*> *ts, std::vector<DynamicObj*> *ds, std::pair<int, int> roomS, std::pair<int, int> roomE, bool alongY);
    static std::vector<Entity*> updateSamos(Game* game);
};

#endif // PHYSICS_H
