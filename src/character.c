#include "character.h"
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "scene.h"


void UpdateBody(Character* body, float rot, char side, char forward, bool jumpPressed, bool crouchHold) {
    Vector2 input = (Vector2){ (float)side, (float)-forward };
#if defined(NORMALIZE_INPUT)
    // Slow down diagonal movement
    if (side != 0 & forward != 0) {
        input = Vector2Normalize(input);
    }
#endif

    /* Fancy collision system against "THE FLOOR" */
    body->is_grounded = IsPlayerGrounded(); // <= enables jumping
    

    float delta = GetFrameTime();

    float* phys_velocity = dBodyGetLinearVel(playerBody.body);
    body->velocity = (Vector3){ phys_velocity[0], phys_velocity[1], phys_velocity[2]};

    body->velocity.y -= GRAVITY * delta;

    if (body->is_grounded && jumpPressed) {
        body->velocity.y = JUMP_FORCE;
        body->is_grounded = false;
        //SetSoundPitch(sound_list[JUMP_HUH], 1.f + (GetRandomValue(-100, 100) * 0.001));
        //SetSoundVolume(sound_list[JUMP_HUH], 0.2f);
        PlayAppSound(JUMP_HUH);
    }

    Vector3 front_vec = (Vector3){ sin(rot), 0.f, cos(rot) };
    Vector3 right_vec = (Vector3){ cos(-rot), 0.f, sin(-rot) };

    Vector3 desired_dir = {
        input.x * right_vec.x + input.y * front_vec.x,
        0.f,
        input.x * right_vec.z + input.y * front_vec.z,
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
    float max_speed = crouchHold ? CROUCH_SPEED : MAX_SPEED;
    float accel = Clamp(max_speed - speed, 0.f, MAX_ACCEL * delta);
    hvel.x += body->dir.x * accel;
    hvel.z += body->dir.z * accel;

    body->velocity.x = hvel.x;
    body->velocity.z = hvel.z;

    dBodySetLinearVel(playerBody.body, body->velocity.x, body->velocity.y, body->velocity.z);

    /*
    body->position.x += body->velocity.x * delta;
    body->position.y += body->velocity.y * delta;
    body->position.z += body->velocity.z * delta;
    */
}

void UpdateCameraAngle(Camera* camera, Vector2* rot, float head_time, float walk_lerp, Vector2 lean) {
    const Vector3 up = (Vector3){ 0.f, 1.f, 0.f };
    const Vector3 target_offset = (Vector3){ 0.f, 0.f, -1.f };

    /* Left & Right */
    Vector3 yaw = Vector3RotateByAxisAngle(target_offset, up, rot->x);

    {
        // Clamp view up
        float maxAngleUp = Vector3Angle(up, yaw);
        maxAngleUp -= 0.001f; // avoid numerical errors
        if (-(rot->y) > maxAngleUp) { rot->y = -maxAngleUp; }

        // Clamp view down
        float maxAngleDown = Vector3Angle(Vector3Negate(up), yaw);
        maxAngleDown *= -1.0f; // downwards angle is negative
        maxAngleDown += 0.001f; // avoid numerical errors
        if (-(rot->y) < maxAngleDown) { rot->y = -maxAngleDown; }
    }

    /* Up & Down */
    Vector3 right = Vector3Normalize(Vector3CrossProduct(yaw, up));

    // Rotate view vector around right axis
    Vector3 pitch = Vector3RotateByAxisAngle(yaw, right, -rot->y - lean.y);

    // Head animation
    // Rotate up direction around forward axis
    float _sin = sin(head_time * PI);
    float _cos = cos(head_time * PI);
    const float BOB_ROTATION = 0.01f;
    camera->up = Vector3RotateByAxisAngle(up, pitch, _sin * BOB_ROTATION + lean.x);

    /* BOB */
    const float BOB_SIDE = 0.1f;
    const float BOB_UP = 0.15f;
    Vector3 bobbing = Vector3Scale(right, _sin * BOB_SIDE);
    bobbing.y = fabsf(_cos * BOB_UP);
    camera->position = Vector3Add(camera->position, Vector3Scale(bobbing, walk_lerp));

    camera->target = Vector3Add(camera->position, pitch);
}

Character CreateBody(Vector3 position) {
    return (Character) { position, Vector3Zero(), Vector3Zero(), false, sound_list[JUMP_HUH]};
}
