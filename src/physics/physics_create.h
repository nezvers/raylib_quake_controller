#ifndef PHYSICS_CREATE_H
#define PHYSICS_CREATE_H

#include "physics.h"

typedef struct {
    Vector3 pos1;
    Vector3 pos2;
    Vector3 prev;
    float t;
    float multiply;
    float* delta;
} PlatformMovementAnimation;

dGeomID CreatePhysicsPlaneStatic(PhysicsInstance* instance, Vector3 position, Vector3 normal, unsigned layer, unsigned mask);

dGeomID CreatePhysicsBoxStatic(PhysicsInstance* instance, Vector3 position, Vector3 rotation, Vector3 size, unsigned layer, unsigned mask);

dBodyID CreatePhysicsBoxAnimated(PhysicsInstance* instance, Vector3 position, Vector3 rotation, Vector3 size, unsigned layer, unsigned mask);

dBodyID CreatePhysicsBodyBoxDynamic(PhysicsInstance* instance, Vector3 position, Vector3 rotation, Vector3 size, unsigned layer, unsigned mask);

dBodyID CreatePhysicsBodySphereDynamic(PhysicsInstance* instance, Vector3 position, Vector3 rotation, float radius, unsigned layer, unsigned mask);

TrimeshData CreatePhysicsTrimeshData(Model plane);

dGeomID CreatePhysicsMesh(PhysicsInstance* instance, TrimeshData* trimesh_data, unsigned layer, unsigned mask);

// TEMP API
PhysicsCharacter CreatePhysicsPlayerBody(PhysicsInstance* instance, Vector3 position);

#endif // PHYSICS_CREATE_H