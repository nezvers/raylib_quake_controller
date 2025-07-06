#include "character.h"
#include "raylib.h"
#include "raymath.h"
#include "scene.h"
#include "debug_draw.h"


#define CROUCH_HEIGHT 0.f
#define STAND_HEIGHT 1.f
#define BOTTOM_HEIGHT 0.5f


void UpdateCharacter(PhysicsInstance* instance, Character* body, float rot, PlayerInput input, float delta) {

    body->head_lerp = Lerp(body->head_lerp, (input.crouch ? CROUCH_HEIGHT : STAND_HEIGHT), 20.f * delta);

    Vector2 input_dir = (Vector2){ (float)input.x, (float)-input.y };
#if defined(NORMALIZE_INPUT)
    // Slow down diagonal movement
    if (input.x != 0 & input.y != 0) {
        input_dir = Vector2Normalize(input_dir);
    }
#endif

    body->is_grounded = IsPlayerGrounded(instance, body); // <= enables jumping

    float* phys_velocity = dBodyGetLinearVel(body->phys.body);
    body->velocity = (Vector3){ phys_velocity[0], phys_velocity[1], phys_velocity[2]};

    body->velocity.y -= GRAVITY * delta - 9.8 * delta; // 9.8 is a hack to counter physics gravity on top of character controller gravity.

    if (body->is_grounded && input.jump) {
        body->velocity.y = JUMP_FORCE;
        body->is_grounded = false;
        PlayAppSound(JUMP_HUH);
    }

    Vector3 front_vec = (Vector3){ sin(rot), 0.f, cos(rot) };
    Vector3 right_vec = (Vector3){ cos(-rot), 0.f, sin(-rot) };

    Vector3 desired_dir = {
        input_dir.x * right_vec.x + input_dir.y * front_vec.x,
        0.f,
        input_dir.x * right_vec.z + input_dir.y * front_vec.z,
    };

    // Smooth out a direction change
    body->dir = Vector3Lerp(body->dir, desired_dir, CONTROL * delta);

    float decel = body->is_grounded ? FRICTION : AIR_DRAG;
    Vector3 hvel = (Vector3){
        body->velocity.x * decel,
        0.f,
        body->velocity.z * decel
    };

    float hvel_length = Vector3Length(hvel); // a.k.a. magnitude
    if (hvel_length < MAX_SPEED * 0.01f) {
        hvel = (Vector3){ 0 };
    }

    /* This is what creates strafing */
    float speed = Vector3DotProduct(hvel, body->dir);

    /*
    Whenever the amount of acceleration to add is clamped by the maximum acceleration constant,
    a Player can make the speed faster by bringing the direction closer to horizontal velocity angle
    More info here: https://youtu.be/v3zT3Z5apaM?t=165
    */
    float max_speed = input.crouch ? CROUCH_SPEED : MAX_SPEED;
    float accel = Clamp(max_speed - speed, 0.f, MAX_ACCEL * delta);
    hvel.x += body->dir.x * accel;
    hvel.z += body->dir.z * accel;

    body->velocity.x = hvel.x;
    body->velocity.z = hvel.z;

    dBodySetLinearVel(body->phys.body, body->velocity.x, body->velocity.y, body->velocity.z);
}

void UpdateCharacterPlayer(PhysicsInstance* instance, Character* body, float rot, PlayerInput input, float delta) {
    if (input.shoot) {
        Vector3 position = demo_scene.camera.position;
        AppendDebugDrawSphere(position, 0.2f, SKYBLUE, 5.f);
    }
    UpdateCharacter(instance, body, rot, input, delta);
}

Character CreateCharacter(Vector3 position, Vector2 rotation, PhysicsCharacter phys) {
    float head_offset = BOTTOM_HEIGHT + STAND_HEIGHT;
    Character character = (Character){ position, Vector3Zero(), Vector3Zero(), rotation, false, head_offset, sound_list[JUMP_HUH], phys };
    return character;
}
