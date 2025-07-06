#include "physics_ode.h"
#include "raymath.h"
#include "ode_util.h"
#include <ode/odemath.h>
#include <ode/collision.h>
#include <ode/objects.h>


// a space can have multiple "worlds" for example you might have different
// sub levels that never interact, or the inside and outside of a building


// Check ray collision against a space
void RayCallback(void* data, dGeomID Geometry1, dGeomID Geometry2) {
    PhysicsInstance* instance = (PhysicsInstance*)data;

    // Check collisions
    dContact Contacts[MAX_CONTACTS];
    int Count = dCollide(Geometry1, Geometry2, MAX_CONTACTS, &Contacts[0].geom, sizeof(dContact));
    for (int i = 0; i < Count; i++) {

        // Check depth against current closest hit
        if (Contacts[i].geom.depth < instance->ray_cast.distance) {
            instance->ray_cast.position = *(Vector3*)(Contacts[i].geom.pos);
            instance->ray_cast.distance = Contacts[i].geom.depth;
        }
    }
}

// Performs raycasting on a space and returns the point of collision. Return false for no hit.
bool RaycastQuery(PhysicsInstance* instance, const Vector3 start, Vector3 end) {

    // Calculate direction
    dVector3 dir;
    dSubtractVectors3(dir, *(dVector3*)&end, *(dVector3*)&start);

    // Get length
    dReal length = dCalcVectorLength3(dir);
    dReal inverse_length = dRecip(length);

    // Normalize
    dScaleVector3(dir, inverse_length);

    // Create ray
    instance->ray_cast.ray = dCreateRay(0, length);
    dGeomRaySet(instance->ray_cast.ray, start.x, start.y, start.z, dir[0], dir[1], dir[2]);

    // Check collisions
    instance->ray_cast.distance = dInfinity;
    dSpaceCollide2(instance->ray_cast.ray, (dGeomID)instance->space, instance, &RayCallback);

    // Cleanup
    dGeomDestroy(instance->ray_cast.ray);

    // Check for hit
    if (instance->ray_cast.distance != dInfinity) {
        return true;
    }

    return false;
}

PhysicsCharacter CreatePhysicsPlayerBody(PhysicsInstance* instance, Vector3 position) {
    dMass m;
    dMatrix3 R;
    dBodyID obj = dBodyCreate(instance->world);
    dGeomID geom = dCreateCapsule(instance->space, 0.5, 1.0);

    // foot sphere is a disabled geometry just for checking collisions
    dGeomID footGeom = dCreateSphere(instance->space, 0.75);
    dGeomDisable(footGeom);

    // capsule torso
    dMassSetCapsuleTotal(&m, 1, 3, 0.5, 1.0);
    dBodySetMass(obj, &m);
    dGeomSetBody(geom, obj);

    // foot sphere
    dGeomSetBody(footGeom, obj);
    dGeomSetOffsetPosition(footGeom, 0, 0, 0.5);

    // give the body a position and rotation
    dBodySetPosition(obj, position.x, position.y, position.z);
    dRFromAxisAndAngle(R, 1.0f, 0, 0, 90.0f * DEG2RAD);
    dBodySetRotation(obj, R);

    dBodySetMaxAngularSpeed(obj, 0);

    // collision mask
    dGeomSetCategoryBits(geom, PHYS_PLAYER);
    dGeomSetCollideBits(geom, PHYS_ALL & (~PHYS_BULLET));
    dBodySetGravityMode(obj, 8); // TODO: use this to remove gravity only to player
    int mode = dBodyGetGravityMode(obj);
    
    PhysicsCharacter player_body = (PhysicsCharacter){ .body = obj, .geom = geom, .footGeom = footGeom };
    instance->player = player_body;
    //glob_playerBody = player_body;
    return player_body;
}

// TODO: remove
dBodyID createBullet(PhysicsInstance* instance) {
    dBodyID obj = dBodyCreate(instance->world);
    dGeomID geom;
    dMass m;

    geom = dCreateSphere(instance->space, 0.1);
    dMassSetSphereTotal(&m, 10, 0.1);
    dGeomSetBody(geom, obj);

    // collision mask
    dGeomSetCategoryBits(geom, PHYS_BULLET);
    dGeomSetCollideBits(geom, PHYS_ALL & ~PHYS_PLAYER & ~PHYS_BULLET);

    dBodySetMass(obj, &m);
    dBodyDisable(obj);

    return obj;
}



TrimeshData CreatePhysicsTrimeshData(Model plane) {
    int vertex_count = plane.meshes[0].vertexCount;
    int* indexes = RL_MALLOC(vertex_count * sizeof(int));
    if (indexes == NULL) { return (TrimeshData) {0}; }
    for (int i = 0; i < vertex_count; i++) {
        indexes[i] = i;
    }
    dTriMeshDataID trimesh_data = dGeomTriMeshDataCreate();
    dGeomTriMeshDataBuildSingle(trimesh_data, plane.meshes[0].vertices,
        3 * sizeof(float), vertex_count,
        indexes, vertex_count,
        3 * sizeof(int));
    /*
    dGeomID geom_id = dCreateTriMesh(space, trimesh_data, NULL, NULL, NULL);
    dGeomSetCategoryBits(geom_id, PHYS_SOLID);
    dGeomSetCollideBits(geom_id, PHYS_ALL);
    */

    return (TrimeshData) { trimesh_data, indexes};
}

dGeomID CreatePhysicsMesh(PhysicsInstance* instance, TrimeshData* trimesh_data, unsigned layer, unsigned mask) {
    dGeomID geom_id = dCreateTriMesh(instance->space, trimesh_data->trimesh, NULL, NULL, NULL);
    dGeomSetCategoryBits(geom_id, layer);
    dGeomSetCollideBits(geom_id, mask);
    return geom_id;
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

static void nearCallback(void* data, dGeomID o1, dGeomID o2){
    PhysicsInstance* instance = (PhysicsInstance*)data;
    int i;
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
    dSpaceCollide(instance->space, instance, &nearCallback);

    // step the world
    if (delta_time > 0.f) {
        dWorldQuickStep(instance->world, delta_time);
    }
    dJointGroupEmpty(instance->contact_group);
}

/*
layer - bitmask which layers geometry belong to
mask - bitmask of layers geometry collide against
*/
dGeomID CreatePhysicsPlaneStatic(PhysicsInstance* instance, Vector3 position, Vector3 normal, unsigned layer, unsigned mask) {
    dGeomID geometry_id = dCreatePlane(instance->space, normal.x, normal.y, normal.z, 0);
    //dGeomSetPosition(geometry_id, position.x, position.y, position.z);

    dGeomSetCategoryBits(geometry_id, layer);
    dGeomSetCollideBits(geometry_id, mask);
    return geometry_id;
}

dGeomID CreatePhysicsBoxStatic(PhysicsInstance* instance, Vector3 position, Vector3 size, unsigned layer, unsigned mask) {
    dGeomID geometry_id = dCreateBox(instance->space, size.x, size.y, size.z);
    dGeomSetPosition(geometry_id, position.x, position.y, position.z);
    dGeomSetCategoryBits(geometry_id, layer);
    dGeomSetCollideBits(geometry_id, mask);
    return geometry_id;
}

dBodyID CreatePhysicsBodyBoxDynamic(PhysicsInstance* instance, Vector3 position, Vector3 rotation, Vector3 size, unsigned layer, unsigned mask) {
    dBodyID obj = dBodyCreate(instance->world);
    dGeomID geom;
    dMatrix3 R;
    dMass m;

    geom = dCreateBox(instance->space, size.x, size.y, size.z);
    dMassSetBoxTotal(&m, 1, 0.5, 0.5, 0.5);

    // set the bodies mass and the newly created geometry
    dGeomSetBody(geom, obj);
    dBodySetMass(obj, &m);

    dGeomSetCategoryBits(geom, layer);
    dGeomSetCollideBits(geom, mask);

    dRFromEulerAngles(R, rotation.x, rotation.y, rotation.z);
    dBodySetRotation(obj, R);

    dBodySetPosition(obj, position.x, position.y, position.z);
    return obj;
}

dBodyID CreatePhysicsBodySphereDynamic(PhysicsInstance* instance, Vector3 position, Vector3 rotation, float radius, unsigned layer, unsigned mask) {
    dBodyID obj = dBodyCreate(instance->world);
    dGeomID geom;
    dMatrix3 R;
    dMass m;

    geom = dCreateSphere(instance->space, radius);
    dMassSetSphereTotal(&m, 1, radius);

    // set the bodies mass and the newly created geometry
    dGeomSetBody(geom, obj);
    dBodySetMass(obj, &m);

    dGeomSetCategoryBits(geom, layer);
    dGeomSetCollideBits(geom, mask);

    dRFromEulerAngles(R, rotation.x, rotation.y, rotation.z);
    dBodySetRotation(obj, R);

    dBodySetPosition(obj, position.x, position.y, position.z);
    return obj;
}

bool IsPhysicsPairColliding(PhysicsInstance* instance, dGeomID a, dGeomID b) {
    return dCollide(a, b, 1, &instance->contact_geom, sizeof(dContactGeom));
}