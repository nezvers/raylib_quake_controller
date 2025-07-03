#ifndef PHYSICS_ODE_H
#define PHYSICS_ODE_H

#include <ode/ode.h>
#include "raylib.h"

typedef struct PlaneBody {
    dGeomID geom;
    int* indexes;
} PlaneGeom;

typedef struct {
    dBodyID body;
    dGeomID geom;
    dGeomID footGeom;
} Body;


enum PHYSICS_LAYERS {
    PHYS_SOLID      = 0x0001, ///< Plane category >          0001
    PHYS_PLAYER     = 0x0002, ///< Player category >         0010
    PHYS_DYNAMIC    = 0x0004, ///< Objects category >        0100
    PHYS_BULLET     = 0x0008, ///< Player bullets category > 1000
    PHYS_ALL        = ~0L     ///< All categories >          11111111111111111111111111111111
};

extern dSpaceID space;
extern Body playerBody;


// when objects potentially collide this callback is called
// you can rule out certain collisions or use different surface parameters
// depending what object types collide.... lots of flexibility and power here!
#define MAX_CONTACTS 8

void CreatePhysics();
void DestroyPhysics();
void UpdatePhysics(float delta_time);


PlaneGeom createStaticMesh(dSpaceID space, Model plane);

/* COLLIDER API */
dGeomID CreatePhysicsPlaneStatic(Vector3 position, Vector3 normal, unsigned layer, unsigned mask);

dGeomID CreatePhysicsBoxStatic(Vector3 position, Vector3 size, unsigned layer, unsigned mask);

dBodyID CreatePhysicsBodyBoxDynamic(Vector3 position, Vector3 rotation, Vector3 size, unsigned layer, unsigned mask);

dBodyID CreatePhysicsBodySphereDynamic(Vector3 position, Vector3 rotation, float radius, unsigned layer, unsigned mask);

bool IsPhysicsPairColliding(dGeomID a, dGeomID b);

void SetPhysicsTransform(const float pos[3], const float R[12], Matrix* matrix);

// TEMP API
Body CreatePhysicsPlayerBody(Vector3 position);

#endif // PHYSICS_ODE_H