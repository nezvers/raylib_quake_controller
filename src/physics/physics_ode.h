#ifndef PHYSICS_ODE_H
#define PHYSICS_ODE_H

#include "physics.h"


bool IsPhysicsPairColliding(PhysicsInstance* instance, dGeomID a, dGeomID b);

void SetPhysicsTransform(const float pos[3], const float R[12], Matrix* matrix);

bool RaycastPhysics(PhysicsInstance* instance, const Vector3 start, Vector3 end, unsigned layer, unsigned mask);

#endif // PHYSICS_ODE_H