#include "physics_ode.h"
#include "raymath.h"

#include <ode/odemath.h>
#include <ode/collision.h>
#include <ode/objects.h>


// a space can have multiple "worlds" for example you might have different
// sub levels that never interact, or the inside and outside of a building

// During collision step is saving last used instance for movement callbacks
PhysicsInstance* current_instance;

// Check ray collision against a space
void RaycastPhysicsCallback(void* data, dGeomID Geometry1, dGeomID Geometry2) {
    PhysicsInstance* instance = (PhysicsInstance*)data;

    // Check collisions
    dContact Contacts[MAX_CONTACTS];
    int Count = dCollide(Geometry1, Geometry2, MAX_CONTACTS, &Contacts[0].geom, sizeof(dContact));
    for (int i = 0; i < Count; i++) {

        // Check depth against current closest hit
        if (Contacts[i].geom.depth < instance->ray_cast.distance) {
            instance->ray_cast.position = *(Vector3*)(Contacts[i].geom.pos);
            instance->ray_cast.distance = Contacts[i].geom.depth;
            instance->ray_cast.other_geom = Contacts[i].geom.g2;
            instance->ray_cast.other_body = dGeomGetBody(Contacts[i].geom.g2);
            
            // TODO: Get collision geom / body
            // TODO: Get collision layer
        }
    }
}

// Performs raycasting on a space and returns the point of collision. Return false for no hit.
bool RaycastPhysics(PhysicsInstance* instance, const Vector3 start, Vector3 end, unsigned layer, unsigned mask) {

    // Calculate direction
    dVector3 dir;
    dSubtractVectors3(dir, *(dVector3*)&end, *(dVector3*)&start);

    // Get length
    dReal length = dCalcVectorLength3(dir);
    dReal inverse_length = dRecip(length);

    // Normalize
    dScaleVector3(dir, inverse_length);

    // Create ray inside physics instance
    instance->ray_cast.ray = dCreateRay(0, length);
    dGeomSetCategoryBits(instance->ray_cast.ray, layer);
    dGeomSetCollideBits(instance->ray_cast.ray, mask);

    dGeomRaySet(instance->ray_cast.ray, start.x, start.y, start.z, dir[0], dir[1], dir[2]);

    // Check collisions
    instance->ray_cast.distance = dInfinity;
    dSpaceCollide2(instance->ray_cast.ray, (dGeomID)instance->space, instance, &RaycastPhysicsCallback);

    // Cleanup
    dGeomDestroy(instance->ray_cast.ray);

    // Check for hit
    if (instance->ray_cast.distance != dInfinity) {
        return true;
    }

    return false;
}



// set a raylib model matrix from an ODE rotation matrix and position
void SetPhysicsTransform(const float pos[3], const float R[12], Matrix* matrix){
    matrix->m0 = R[0];
    matrix->m1 = R[4];
    matrix->m2 = R[8];
    matrix->m3 = 0;
    matrix->m4 = R[1];
    matrix->m5 = R[5];
    matrix->m6 = R[9];
    matrix->m7 = 0;
    matrix->m8 = R[2];
    matrix->m9 = R[6];
    matrix->m10 = R[10];
    matrix->m11 = 0;
    matrix->m12 = pos[0];
    matrix->m13 = pos[1];
    matrix->m14 = pos[2];
    matrix->m15 = 1;
}

void setTransformCylinder(const float pos[3], const float R[12], Matrix* matrix, float length)
{
    Matrix m;
    m.m0 = R[0];
    m.m1 = R[4];
    m.m2 = R[8];
    m.m3 = 0;
    m.m4 = R[1];
    m.m5 = R[5];
    m.m6 = R[9];
    m.m7 = 0;
    m.m8 = R[2];
    m.m9 = R[6];
    m.m10 = R[10];
    m.m11 = 0;
    m.m12 = pos[0];
    m.m13 = pos[1];
    m.m14 = pos[2];
    m.m15 = 1;

    // rotate because the cylinder axis looks diferent
    Matrix r = MatrixRotateX(DEG2RAD * 90);
    Matrix nMatrix = MatrixMultiply(r, m);

    // move the origin of the model to the center
    // -1.5 is because is half o 3 (the length of the cylinder)
    Matrix t = MatrixTranslate(0, length * -0.5f, 0);
    nMatrix = MatrixMultiply(t, nMatrix);

    matrix->m0 = nMatrix.m0;
    matrix->m1 = nMatrix.m1;
    matrix->m2 = nMatrix.m2;
    matrix->m3 = nMatrix.m3;
    matrix->m4 = nMatrix.m4;
    matrix->m5 = nMatrix.m5;
    matrix->m6 = nMatrix.m6;
    matrix->m7 = nMatrix.m7;
    matrix->m8 = nMatrix.m8;
    matrix->m9 = nMatrix.m9;
    matrix->m10 = nMatrix.m10;
    matrix->m11 = nMatrix.m11;
    matrix->m12 = nMatrix.m12;
    matrix->m13 = nMatrix.m13;
    matrix->m14 = nMatrix.m14;
    matrix->m15 = nMatrix.m15;
}

void InitPhysics() {
    // TODO: move dInitODE2 to global initialization
    dInitODE2(0);
}

PhysicsInstance CreatePhysics() {
    InitPhysics();
    
    PhysicsInstance instance = { 0 };
    instance.world = dWorldCreate();
    instance.space = dHashSpaceCreate(NULL);
    instance.contact_group = dJointGroupCreate(0);
    dWorldSetGravity(instance.world, 0, -9.8, 0);

    return instance;
}

void ClosePhysics() {
    dCloseODE();
}

void DestroyPhysics(PhysicsInstance* instance) {
    //TODO: free(planeGeom.indexes);

    if (instance == NULL) {
        return;
    }
    dJointGroupEmpty(instance->contact_group);
    dJointGroupDestroy(instance->contact_group);
    dSpaceDestroy(instance->space);
    dWorldDestroy(instance->world);
}

static void PhysicsCollisionCallback(void* data, dGeomID o1, dGeomID o2){
    int i;

    PhysicsInstance* instance = (PhysicsInstance*)data;
    current_instance = instance;

    // if (o1->body && o2->body) return;

    // exit without doing anything if the two bodies are connected by a joint
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact))
        return;
    bool is_player = b1 == instance->player.body || b2 == instance->player.body;
    dContact contact[MAX_CONTACTS]; // up to MAX_CONTACTS contacts per body-body
    for (i = 0; i < MAX_CONTACTS; i++) {
        /*contact[i].surface.mode = dContactBounce | dContactSoftCFM | dContactApprox1;*/
        contact[i].surface.mode = is_player ? (dContactSlip1 | dContactSlip2) : dContactBounce;
        contact[i].surface.mu = dInfinity;
        contact[i].surface.bounce = 0.0;
        contact[i].surface.bounce_vel = 0.1;
        /*contact[i].surface.soft_cfm = 0.01;*/
    }
    int numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom,
        sizeof(dContact));
    if (numc) {
        dMatrix3 RI;
        dRSetIdentity(RI);
        for (i = 0; i < numc; i++) {
            dJointID c = dJointCreateContact(instance->world, instance->contact_group, contact + i);
            dJointAttach(c, b1, b2);
        }
    }
}

void UpdatePhysics(PhysicsInstance* instance, float delta_time) {
    // check for collisions
    dSpaceCollide(instance->space, instance, &PhysicsCollisionCallback);

    // step the world
    if (delta_time > 0.f) {
        dWorldQuickStep(instance->world, delta_time);
    }
    dJointGroupEmpty(instance->contact_group);
}



bool IsPhysicsPairColliding(PhysicsInstance* instance, dGeomID a, dGeomID b) {
    return dCollide(a, b, 1, &instance->contact_geom, sizeof(dContactGeom));
}

// TODO: Detect ground by collision normal
bool IsPhysicsObjectOnGround(PhysicsInstance* instance, dBodyID body) {
    int joint_count = dBodyGetNumJoints(body);
    if (joint_count == 0) { return false; }
    for (int i = 0; i < joint_count; i++) {
        dJointID joint = dBodyGetJoint(body, i);

    }
    return true;
}