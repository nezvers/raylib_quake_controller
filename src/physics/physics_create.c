
#include "physics_create.h"

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
    dBodySetMovedCallback(obj, RollingDamping);
    return obj;
}

// TODO: 
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
