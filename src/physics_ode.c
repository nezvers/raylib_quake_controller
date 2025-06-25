#include "physics_ode.h"
#include "raymath.h"
#include "ode_util.h"

// a space can have multiple "worlds" for example you might have different
// sub levels that never interact, or the inside and outside of a building
dSpaceID space;
// these are used by the collision callback, while they could be passed
// via the collision callbacks user data, making the global is easier
dWorldID world;
dJointGroupID contactgroup;

#define numObj 200  // 100 boxes, 100 spheres, 100 cylinders
#define numBullets 50  // max bullets
PlaneGeom planeGeom;
dBodyID objects[numObj];
dBodyID bullets[numBullets];
dGeomID plane_geometry;
dGeomID tower_geometry[4];
PlayerBody playerBody;
dContactGeom contact;


static const int catBits[LAST_INDEX_CNT] = {
    0x0001, ///< Plane category >          0001
    0x0002, ///< Player category >         0010
    0x0004, ///< Objects category >        0100
    0x0008, ///< Player bullets category > 1000
    ~0L     ///< All categories >          11111111111111111111111111111111
};

dBodyID createRandomObject(dSpaceID space, dWorldID world, int type) {
    dBodyID obj = dBodyCreate(world);
    dGeomID geom;
    dMatrix3 R;
    dMass m;

    // create either a box or sphere with the apropriate mass
    if (type < numObj / 2) {
        geom = dCreateBox(space, 1, 1, 1);
        dMassSetBoxTotal(&m, 1, 0.5, 0.5, 0.5);
    }
    else {
        geom = dCreateSphere(space, 0.5);
        dMassSetSphereTotal(&m, 1, 0.5);
    }

    // set the bodies mass and the newly created geometry
    dGeomSetBody(geom, obj);
    dBodySetMass(obj, &m);

    // collision mask
    dGeomSetCategoryBits(geom, catBits[OBJS]);
    dGeomSetCollideBits(geom, catBits[ALL]);

    // give the body a random position and rotation
    dBodySetPosition(obj,
        dRandReal() * 10 - 5,
        dRandInt(10) + 4,
        dRandReal() * 10 - 5);

    dRFromAxisAndAngle(R,
        dRandReal() * 2.0 - 1.0,
        dRandReal() * 2.0 - 1.0,
        dRandReal() * 2.0 - 1.0,
        dRandReal() * 10.0 - 5.0);

    dBodySetRotation(obj, R);
    return obj;
}

PlayerBody createPlayerBody(dSpaceID space, dWorldID world) {
    dMass m;
    dMatrix3 R;
    dBodyID obj = dBodyCreate(world);
    dGeomID geom = dCreateCapsule(space, 0.5, 1.0);

    // foot sphere is a disabled geometry just for checking collisions
    dGeomID footGeom = dCreateSphere(space, 0.75);
    dGeomDisable(footGeom);

    // capsule torso
    dMassSetCapsuleTotal(&m, 1, 3, 0.5, 1.0);
    dBodySetMass(obj, &m);
    dGeomSetBody(geom, obj);

    // foot sphere
    dGeomSetBody(footGeom, obj);
    dGeomSetOffsetPosition(footGeom, 0, 0, 0.5);

    // give the body a position and rotation
    dBodySetPosition(obj, 0, 5, 15);
    dRFromAxisAndAngle(R, 1.0f, 0, 0, 90.0f * DEG2RAD);
    dBodySetRotation(obj, R);

    dBodySetMaxAngularSpeed(obj, 0);

    // collision mask
    dGeomSetCategoryBits(geom, catBits[PLAYER]);
    dGeomSetCollideBits(geom, catBits[ALL] & (~catBits[PLAYER_BULLET]));
    dBodySetGravityMode(geom, 8);

    return (PlayerBody) { .body = obj, .geom = geom, .footGeom = footGeom };
}

dBodyID createBullet(dSpaceID space, dWorldID world) {
    dBodyID obj = dBodyCreate(world);
    dGeomID geom;
    dMass m;

    geom = dCreateSphere(space, 0.1);
    dMassSetSphereTotal(&m, 10, 0.1);
    dGeomSetBody(geom, obj);

    // collision mask
    dGeomSetCategoryBits(geom, catBits[PLAYER_BULLET]);
    dGeomSetCollideBits(geom, catBits[ALL] & (~catBits[PLAYER]) & (~catBits[PLAYER_BULLET]));

    dBodySetMass(obj, &m);
    dBodyDisable(obj);

    return obj;
}

PlaneGeom createStaticPlane(dSpaceID space, Model plane) {
    int nV = plane.meshes[0].vertexCount;
    int* groundInd = RL_MALLOC(nV * sizeof(int));
    if (groundInd == NULL) { return (PlaneGeom) {0}; }
    for (int i = 0; i < nV; i++) {
        groundInd[i] = i;
    }
    dTriMeshDataID triData = dGeomTriMeshDataCreate();
    dGeomTriMeshDataBuildSingle(triData, plane.meshes[0].vertices,
        3 * sizeof(float), nV,
        groundInd, nV,
        3 * sizeof(int));
    dGeomID planeGeom = dCreateTriMesh(space, triData, NULL, NULL, NULL);
    dGeomSetCategoryBits(planeGeom, catBits[PLANE]);
    dGeomSetCollideBits(planeGeom, catBits[ALL]);

    return (PlaneGeom) { .geom = planeGeom, .indexes = groundInd };
}

// set a raylib model matrix from an ODE rotation matrix and position
void setTransform(const float pos[3], const float R[12], Matrix* matrix){
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
    // move the origin of the model to the center
    // -1.5 is because is half o 3 (the length of the cylinder)
    Matrix t = MatrixTranslate(0, length / 2 * -1, 0);
    Matrix nMatrix = MatrixMultiply(r, m);
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

void drawBodyCylinder(dBodyID body, Model cylinder) {
    float length = 1.0f;
    setTransformCylinder(
        (float*)dBodyGetPosition(body),
        (float*)dBodyGetRotation(body),
        &cylinder.transform,
        length);
    DrawModel(cylinder, (Vector3) { 0, 0, 0 }, 1.0f, WHITE);
}

void drawBodyModel(dBodyID body, Model model) {
    setTransform(
        (float*)dBodyGetPosition(body),
        (float*)dBodyGetRotation(body),
        &model.transform);
    DrawModel(model, (Vector3) { 0, 0, 0 }, 1.0f, WHITE);
}

void CreatePhysics(Model* plane) {
    // initialise and create the physics
    // TODO: move dInitODE2 to global initialization
    dInitODE2(0);
    world = dWorldCreate();
    space = dHashSpaceCreate(NULL);
    contactgroup = dJointGroupCreate(0);
    dWorldSetGravity(world, 0, 0, 0);

    //planeGeom = createStaticPlane(space, *plane);
    playerBody = createPlayerBody(space, world);

    for (int i = 0; i < numObj; i++) {
        objects[i] = createRandomObject(space, world, i);
    }

    for (int i = 0; i < numBullets; i++) {
        bullets[i] = createBullet(space, world);
    }
    plane_geometry = dCreatePlane(space, 0, 1, 0, 0);
    dGeomSetCategoryBits(plane_geometry, catBits[PLANE]);
    dGeomSetCollideBits(plane_geometry, catBits[ALL]);

    for (int i = 0; i < 4; i++) {
        tower_geometry[i] = dCreateBox(space, 16.f, 32.f, 16.f);
        dGeomSetCategoryBits(tower_geometry[i], catBits[PLANE]);
        dGeomSetCollideBits(tower_geometry[i], catBits[ALL]);
    }
    dGeomSetPosition(tower_geometry[0], 16.f, 16.f, 16.f);
    dGeomSetPosition(tower_geometry[1], -16.f, 16.f, 16.f);
    dGeomSetPosition(tower_geometry[2], -16.f, 16.f, -16.f);
    dGeomSetPosition(tower_geometry[3], 16.f, 16.f, -16.f);
}

void DestroyPhysics() {
    //free(planeGeom.indexes);
    dJointGroupEmpty(contactgroup);
    dJointGroupDestroy(contactgroup);
    dSpaceDestroy(space);
    dWorldDestroy(world);
    dCloseODE();
}

static void nearCallback(void* data, dGeomID o1, dGeomID o2){
    (void)data;
    int i;
    // if (o1->body && o2->body) return;

    // exit without doing anything if the two bodies are connected by a joint
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact))
        return;
    bool is_player = b1 == playerBody.body || b2 == playerBody.body;
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
            dJointID c = dJointCreateContact(world, contactgroup, contact + i);
            dJointAttach(c, b1, b2);
        }
    }
}

void UpdatePhysics(float delta_time) {
    // check for collisions
    dSpaceCollide(space, 0, &nearCallback);

    // step the world
    if (delta_time > 0.f) {
        float* phys_velocity;
        for (int i = 0; i < numObj; i++) {
            phys_velocity = dBodyGetLinearVel(objects[i]);
            dBodySetLinearVel(objects[i], phys_velocity[0], phys_velocity[1] - 9.8 * delta_time, phys_velocity[2]);
        }
        dWorldQuickStep(world, delta_time);
    }
    dJointGroupEmpty(contactgroup);
}

void DrawPhysics(Model plane, Model sphere, Model box) {
    for (int i = 0; i < numObj; i++) {
        if (i < numObj / 2) {
            drawBodyModel(objects[i], box);
        }
        else {
            drawBodyModel(objects[i], sphere);
        }
    }
    /*
    for (int i = 0; i < numBullets; i++) {
        dBodyID current_bullet_body = bullets[i];
        drawBodyModel(current_bullet_body, bullet);
    }
    DrawModel(aim, cameraAim.position, 1.0f, RED);
    */
}

bool IsPlayerGrounded() {
    return dCollide(plane_geometry, playerBody.footGeom, 1, &contact, sizeof(dContactGeom));
}