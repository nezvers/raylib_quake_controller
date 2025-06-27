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


// when objects potentially collide this callback is called
// you can rule out certain collisions or use different surface parameters
// depending what object types collide.... lots of flexibility and power here!
#define MAX_CONTACTS 8

void CreatePhysics(Model* plane);
void DestroyPhysics();
void UpdatePhysics(float delta_time);
void DrawPhysics(Model plane, Model sphere, Model box);


PlaneGeom createStaticPlane(dSpaceID space, Model plane);

void drawBodyCylinder(dBodyID body, Model cylinder);

void drawBodyModel(dBodyID body, Model model);

bool IsPlayerGrounded();

#endif // PHYSICS_ODE_H