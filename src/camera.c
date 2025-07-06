#include "camera.h"
#include "raymath.h"



typedef struct {
    float bob_timer;
    float walk_lerp;
    Vector2 lean;
} CameraAnimFPS;

CameraAnimFPS camera_anim;

Camera CreateCamera(Vector3 position, Vector2* rotation) {
    Camera camera = (Camera){ 0 };
    camera.fovy = 60.f;
    camera.projection = CAMERA_PERSPECTIVE;

    camera_anim.bob_timer = 0.f;
    camera_anim.walk_lerp = 0.f;
    camera_anim.lean = Vector2Zero();
    UpdateFPSCameraAnimated(&camera, position, rotation, 0.f, (PlayerInput) {0}, false);

    return camera;
}

// TODO: receive input struct or something
void UpdateFPSCameraAnimated(Camera* camera, Vector3 position, Vector2* rotation, float delta, PlayerInput input, bool grounded) {

    camera->position = position;

    const Vector3 up = (Vector3){ 0.f, 1.f, 0.f };
    const Vector3 target_offset = (Vector3){ 0.f, 0.f, -1.f };


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
    Vector3 pitch = Vector3RotateByAxisAngle(yaw, right, -rotation->y - camera_anim.lean.y);

    // Head animation

    if (grounded && (input.y != 0 || input.x != 0)) {
        camera_anim.bob_timer += delta * 3.f;
        camera_anim.walk_lerp = Lerp(camera_anim.walk_lerp, 1.f, 10.f * delta);
        camera->fovy = Lerp(camera->fovy, 55.f, 5.f * delta);
    }
    else {
        camera_anim.walk_lerp = Lerp(camera_anim.walk_lerp, 0.f, 10.f * delta);
        camera->fovy = Lerp(camera->fovy, 60.f, 5.f * delta);
    }

    camera_anim.lean.x = Lerp(camera_anim.lean.x, input.x * 0.02f, 10.f * delta);
    camera_anim.lean.y = Lerp(camera_anim.lean.y, input.y * 0.015f, 10.f * delta);

    // Rotate up direction around forward axis
    float _sin = sin(camera_anim.bob_timer * PI);
    float _cos = cos(camera_anim.bob_timer * PI);
    const float BOB_ROTATION = 0.01f;
    camera->up = Vector3RotateByAxisAngle(up, pitch, _sin * BOB_ROTATION + camera_anim.lean.x);

    /* BOB */
    const float BOB_SIDE = 0.1f;
    const float BOB_UP = 0.15f;
    Vector3 bobbing = Vector3Scale(right, _sin * BOB_SIDE);
    bobbing.y = fabsf(_cos * BOB_UP);
    camera->position = Vector3Add(camera->position, Vector3Scale(bobbing, camera_anim.walk_lerp));

    camera->target = Vector3Add(camera->position, pitch);
}