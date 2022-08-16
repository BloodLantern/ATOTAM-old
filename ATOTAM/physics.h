#ifndef PHYSICS_H
#define PHYSICS_H

#include <Entities/entity.h>
#include <Entities/projectile.h>

#include "Entities/area.h"
#include "Entities/dynamicobj.h"
#include "Entities/entity.h"
#include "Entities/living.h"
#include "Entities/monster.h"
#include "Entities/npc.h"
#include "Entities/samos.h"
#include "Entities/terrain.h"

class Physics
{
public:
    static double gravity; //p.s^-2
    static double frameRate; //fps

    static void updatePhysics();
    static std::vector<Entity*> handleCollision(Entity* obj1, Entity* obj2);
    static bool updateProjectile(Projectile* p);
    static bool canChangeBox(Entity* e, CollisionBox* b);
    static void updateSamos();
};

#endif // PHYSICS_H
