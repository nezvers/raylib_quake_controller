#ifndef PHYSICS_H
#define PHYSICS_H

#include <ode/ode.h>
#include "raylib.h"

typedef struct {
    dTriMeshDataID trimesh;
    int* indexes; // need to be freed
} TrimeshData;

typedef struct {
    dBodyID body;
    dGeomID geom;
    dGeomID footGeom;
} PhysicsCharacter;

typedef struct {
    Vector3 position; // x, y, z
    float distance;
    dGeomID other_geom;
    dBodyID other_body;
    dGeomID ray; // Used for raycast query
}RayCast;

// TODO: Pass for callbacks data pointer
typedef struct {
    dSpaceID space;
    dWorldID world; // TODO: use array
    dJointGroupID contact_group;
    dContactGeom contact_geom;
    PhysicsCharacter player;
    RayCast ray_cast; // Used for raycast query
} PhysicsInstance;

enum PHYSICS_LAYERS {
    PHYS_SOLID = 0x0001, ///< Plane category >          0001
    PHYS_MOVING = 0x0002, ///< Moving platforms category >          0001
    PHYS_PLAYER = 0x0004, ///< Player category >         0010
    PHYS_DYNAMIC = 0x0008, ///< Objects category >        0100
    PHYS_TRIGGER = 0x0010, ///< Trigger Zones > 1000
    PHYS_BULLET = 0x0020, ///< Player bullets category > 1000
    PHYS_ALL = ~0L     ///< All categories >          11111111111111111111111111111111
};


// when objects potentially collide this callback is called
// you can rule out certain collisions or use different surface parameters
// depending what object types collide.... lots of flexibility and power here!
#define MAX_CONTACTS 8

void InitPhysics();
void ClosePhysics();
PhysicsInstance CreatePhysics();
void DestroyPhysics(PhysicsInstance* instance);
void UpdatePhysics(PhysicsInstance* instance, float delta_time);


/* COLLIDER API */



#endif // PHYSICS_H