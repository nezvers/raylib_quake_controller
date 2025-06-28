#include "camera.h"
#include "raymath.h"


#define CROUCH_HEIGHT 0.f
#define STAND_HEIGHT 1.f
#define BOTTOM_HEIGHT 0.5f

float bob_timer;
float walk_lerp;
float headLerp;
Vector2 lean;

Camera CreateCamera(Vector3 position, Vector2* rotation) {
    Camera camera = (Camera){ 0 };
    camera.fovy = 60.f;
    camera.projection = CAMERA_PERSPECTIVE;

    bob_timer = 0.f;
    walk_lerp = 0.f;
    headLerp = STAND_HEIGHT;
    lean = Vector2Zero();
    UpdateCameraAngle(&camera, position, rotation, 0.f, false, false, false, false);

    return camera;
}

// TODO:
void UpdateCameraAngle(Camera* camera, Vector3 position, Vector2* rotation, float delta, bool forward, bool sideway, bool crouching, bool grounded) {
    camera->position = (Vector3){
            position.x,
            position.y + (BOTTOM_HEIGHT + headLerp),
            position.z,
    };

    const Vector3 up = (Vector3){ 0.f, 1.f, 0.f };
    const Vector3 target_offset = (Vector3){ 0.f, 0.f, -1.f };

    headLerp = Lerp(headLerp, (crouching ? CROUCH_HEIGHT : STAND_HEIGHT), 20.f * delta);
    if (grounded && (forward != 0 || sideway != 0)) {
        bob_timer += delta * 3.f;
        walk_lerp = Lerp(walk_lerp, 1.f, 10.f * delta);
        camera->fovy = Lerp(camera->fovy, 55.f, 5.f * delta);
    }
    else {
        walk_lerp = Lerp(walk_lerp, 0.f, 10.f * delta);
        camera->fovy = Lerp(camera->fovy, 60.f, 5.f * delta);
    }

    lean.x = Lerp(lean.x, sideway * 0.02f, 10.f * delta);
    lean.y = Lerp(lean.y, forward * 0.015f, 10.f * delta);


    /* Left & Right */
    Vector3 yaw = Vector3RotateByAxisAngle(target_offset, up, rotation->x);

    {
        // Clamp view up
        float maxAngleUp = Vector3Angle(up, yaw);
        maxAngleUp -= 0.001f; // avoid numerical errors
        if (-(rotation->y) > maxAngleUp) { rotation->y = -maxAngleUp; }

        // Clamp view down
        float maxAngleDown = Vector3Angle(Vector3Negate(up), yaw);
        maxAngleDown *= -1.0f; // downwards angle is negative
        maxAngleDown += 0.001f; // avoid numerical errors
        if (-(rotation->y) < maxAngleDown) { rotation->y = -maxAngleDown; }
    }

    /* Up & Down */
    Vector3 right = Vector3Normalize(Vector3CrossProduct(yaw, up));

    // Rotate view vector around right axis
    Vector3 pitch = Vector3RotateByAxisAngle(yaw, right, -rotation->y - lean.y);

    // Head animation
    // Rotate up direction around forward axis
    float _sin = sin(bob_timer * PI);
    float _cos = cos(bob_timer * PI);
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