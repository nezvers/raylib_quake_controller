
#include "physics_create.h"
#include "raymath.h"
#include "easing.h"

TrimeshData CreatePhysicsTrimeshData(Model plane) {
    int vertex_count = plane.meshes[0].vertexCount;
    int* indexes = RL_MALLOC(vertex_count * sizeof(int));
    if (indexes == NULL) { return (TrimeshData) { 0 }; }
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

    return (TrimeshData) { trimesh_data, indexes };
}

dGeomID CreatePhysicsMesh(PhysicsInstance* instance, TrimeshData* trimesh_data, unsigned layer, unsigned mask) {
    dGeomID geom_id = dCreateTriMesh(instance->space, trimesh_data->trimesh, NULL, NULL, NULL);
    dGeomSetCategoryBits(geom_id, layer);
    dGeomSetCollideBits(geom_id, mask);
    return geom_id;
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

dGeomID CreatePhysicsBoxStatic(PhysicsInstance* instance, Vector3 position, Vector3 rotation, Vector3 size, unsigned layer, unsigned mask) {
    dGeomID geometry_id = dCreateBox(instance->space, size.x, size.y, size.z);
    dGeomSetPosition(geometry_id, position.x, position.y, position.z);
    dGeomSetCategoryBits(geometry_id, layer);
    dGeomSetCollideBits(geometry_id, mask);

    dMatrix3 r;
    dRFromEulerAngles(r, rotation.x, rotation.y, rotation.z);
    dGeomSetRotation(geometry_id, r);
    return geometry_id;
}

dBodyID CreatePhysicsBodyBoxDynamic(PhysicsInstance* instance, Vector3 position, Vector3 rotation, Vector3 size, unsigned layer, unsigned mask) {
    dBodyID obj = dBodyCreate(instance->world);
    dGeomID geom;
    dMatrix3 r;
    dMass m;

    geom = dCreateBox(instance->space, size.x, size.y, size.z);
    dGeomSetCategoryBits(geom, layer);
    dGeomSetCollideBits(geom, mask);

    dGeomSetBody(geom, obj);
    dMassSetBoxTotal(&m, 1, 0.5, 0.5, 0.5);
    dBodySetMass(obj, &m);

    dRFromEulerAngles(r, rotation.x, rotation.y, rotation.z);
    dBodySetRotation(obj, r);

    //dBodySetDamping(obj, 0.1f, 0.1f); // TODO: Doesn't fix sphere rolling speed

    dBodySetPosition(obj, position.x, position.y, position.z);
    return obj;
}

void PlatformCallback(dBodyID body_id) {
    PlatformMovementAnimation* plat_anim = dBodyGetData(body_id);
    if (plat_anim == NULL) { return; }

    plat_anim->t += *plat_anim->delta * plat_anim->multiply;
    plat_anim->t -= (int)plat_anim->t;

    float t = plat_anim->t * 2.f;
    if (t > 1.f) { t = 2.f - t; }
    t = cubicEaseInOutf(t);

    Vector3 pos = Vector3Lerp(plat_anim->pos1, plat_anim->pos2, t);
    Vector3 diff = Vector3Subtract(pos, plat_anim->prev);
    if (*plat_anim->delta < 0.0001f) { return; }
    Vector3 velocity = Vector3Scale(diff, 1.f / *plat_anim->delta);
    plat_anim->prev = pos;
    dBodySetPosition(body_id, pos.x, pos.y, pos.z);
    dBodySetLinearVel(body_id, velocity.x, velocity.y, velocity.z);
}

dBodyID CreatePhysicsBoxAnimated(PhysicsInstance* instance, Vector3 position, Vector3 rotation, Vector3 size, unsigned layer, unsigned mask) {
    dBodyID obj = dBodyCreate(instance->world);
    dGeomID geom;
    dMatrix3 r;
    dMass m;

    geom = dCreateBox(instance->space, size.x, size.y, size.z);
    dGeomSetCategoryBits(geom, layer);
    dGeomSetCollideBits(geom, mask);

    dGeomSetBody(geom, obj);
    dMassSetBoxTotal(&m, 1, 0.5, 0.5, 0.5);
    dBodySetMass(obj, &m);

    dRFromEulerAngles(r, rotation.x, rotation.y, rotation.z);
    dBodySetRotation(obj, r);

    dBodySetKinematic(obj);
    dBodySetMovedCallback(obj, PlatformCallback);
    //dBodySetDamping(obj, 0.1f, 0.1f); // TODO: Doesn't fix sphere rolling speed

    dBodySetPosition(obj, position.x, position.y, position.z);
    return obj;
}

void RollingDamping(dBodyID body) {
    if (dBodyGetNumJoints(body) == 0) { return; }
    float* angular = dBodyGetAngularVel(body);
    dScaleVector3(angular, 0.95);
    dBodySetAngularVel(body, angular[0], angular[1], angular[2]);
}

dBodyID CreatePhysicsBodySphereDynamic(PhysicsInstance* instance, Vector3 position, Vector3 rotation, float radius, unsigned layer, unsigned mask) {
    dBodyID obj = dBodyCreate(instance->world);
    dGeomID geom;
    dMatrix3 r;
    dMass m;

    geom = dCreateSphere(instance->space, radius);
    dMassSetSphereTotal(&m, 1, radius);

    // set the bodies mass and the newly created geometry
    dGeomSetBody(geom, obj);
    dBodySetMass(obj, &m);

    dGeomSetCategoryBits(geom, layer);
    dGeomSetCollideBits(geom, mask);

    dRFromEulerAngles(r, rotation.x, rotation.y, rotation.z);
    dBodySetRotation(obj, r);

    dBodySetPosition(obj, position.x, position.y, position.z);
    dBodySetMovedCallback(obj, RollingDamping);
    return obj;
}


// TODO: 
PhysicsCharacter CreatePhysicsPlayerBody(PhysicsInstance* instance, Vector3 position) {
    dMass m;
    dMatrix3 r;
    dBodyID obj = dBodyCreate(instance->world);
    dGeomID capsule_geom = dCreateCapsule(instance->space, 0.5, 1.0);

    // foot sphere is a disabled geometry just for checking collisions
    dGeomID foot_geom = dCreateSphere(instance->space, 0.75);
    dGeomDisable(foot_geom);
    dGeomSetCategoryBits(foot_geom, PHYS_TRIGGER);
    dGeomSetCollideBits(foot_geom, PHYS_ALL & ~PHYS_BULLET & ~PHYS_PLAYER);

    // capsule torso
    dMassSetCapsuleTotal(&m, 1, 3, 0.5, 1.0);
    dBodySetMass(obj, &m);
    dGeomSetBody(capsule_geom, obj);

    // foot sphere
    dGeomSetBody(foot_geom, obj);
    dGeomSetOffsetPosition(foot_geom, 0, 0, 0.5);

    // give the body a position and rotation
    dBodySetPosition(obj, position.x, position.y, position.z);
    dRFromAxisAndAngle(r, 1.0f, 0, 0, 90.0f * DEG2RAD);
    dBodySetRotation(obj, r);

    dBodySetMaxAngularSpeed(obj, 0);

    // collision mask
    dGeomSetCategoryBits(capsule_geom, PHYS_PLAYER);
    dGeomSetCollideBits(capsule_geom, PHYS_ALL & (~PHYS_BULLET));
    dBodySetGravityMode(obj, 8); // TODO: use this to remove gravity only to player
    int mode = dBodyGetGravityMode(obj);

    PhysicsCharacter player_body = (PhysicsCharacter){ .body = obj, .geom = capsule_geom, .footGeom = foot_geom };
    instance->player = player_body;
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
