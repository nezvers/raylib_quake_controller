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

enum INDEX {
    PLANE = 0,
    PLAYER,
    OBJS,
    PLAYER_BULLET,
    ALL,
    LAST_INDEX_CNT
};


extern dSpaceID space;
extern Body playerBody;
extern const int catBits[LAST_INDEX_CNT];


// when objects potentially collide this callback is called
// you can rule out certain collisions or use different surface parameters
// depending what object types collide.... lots of flexibility and power here!
#define MAX_CONTACTS 8

void CreatePhysics();
void DestroyPhysics();
void UpdatePhysics(float delta_time);
void DrawPhysics(Model plane, Model sphere, Model box);


PlaneGeom createStaticMesh(dSpaceID space, Model plane);

void drawBodyCylinder(dBodyID body, Model cylinder);

void drawBodyModel(dBodyID body, Model model);

/* COLLIDER API */
dGeomID CreatePhysicsPlane(Vector3 position, Vector3 normal, unsigned layer, unsigned mask);

dGeomID CreatePhysicsBox(Vector3 position, Vector3 size, unsigned layer, unsigned mask);

bool IsPhysicsPairColliding(dGeomID a, dGeomID b);

// TEMP API
Body CreatePhysicsPlayerBody(Vector3 position);

#endif // PHYSICS_ODE_H