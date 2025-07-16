#include "camera.h"
#include "raymath.h"


CameraFPS CreateCamera(Vector3 position, Vector2* rotation, Vector3* look_dir) {
    Camera camera = (Camera){ 0 };
    camera.fovy = 60.f;
    camera.projection = CAMERA_PERSPECTIVE;

    CameraFPS cam = { 0 };
    cam.bob_timer = 0.f;
    cam.walk_lerp = 0.f;
    cam.lean = Vector2Zero();
    cam.camera = camera;
    PlayerInput input = (PlayerInput){ 0 };
    UpdateFPSCameraAnimated(&cam, position, rotation, 0.f, &input, false, look_dir);

    return cam;
}

// TODO: receive input struct or something
void UpdateFPSCameraAnimated(CameraFPS* cam, Vector3 position, Vector2* rotation, float delta, PlayerInput* input, bool grounded, Vector3* look_dir) {

    cam->camera.position = position;

    const Vector3 up = (Vector3){ 0.f, 1.f, 0.f };
    const Vector3 target_offset = (Vector3){ 0.f, 0.f, -1.f };


    /* Left & Right */
    Vector3 yaw = Vector3RotateByAxisAngle(target_offset, up, rotation->x + cam->offset.x);

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
    *look_dir = Vector3RotateByAxisAngle(yaw, right, -rotation->y - cam->lean.y + cam->offset.y);

    // Head animation

    if (grounded && (input->y != 0 || input->x != 0)) {
        cam->bob_timer += delta * 3.f;
        cam->walk_lerp = Lerp(cam->walk_lerp, 1.f, 10.f * delta);
        cam->camera.fovy = Lerp(cam->camera.fovy, 55.f, 5.f * delta);
    }
    else {
        cam->walk_lerp = Lerp(cam->walk_lerp, 0.f, 10.f * delta);
        cam->camera.fovy = Lerp(cam->camera.fovy, 60.f, 5.f * delta);
    }

    cam->lean.x = Lerp(cam->lean.x, input->x * 0.02f, 10.f * delta);
    cam->lean.y = Lerp(cam->lean.y, input->y * 0.015f, 10.f * delta);

    // Rotate up direction around forward axis
    float _sin = sin(cam->bob_timer * PI);
    float _cos = cos(cam->bob_timer * PI);
    const float BOB_ROTATION = 0.01f;
    cam->camera.up = Vector3RotateByAxisAngle(up, *look_dir, _sin * BOB_ROTATION + cam->lean.x);

    /* BOB */
    const float BOB_SIDE = 0.1f;
    const float BOB_UP = 0.15f;
    Vector3 bobbing = Vector3Scale(right, _sin * BOB_SIDE);
    bobbing.y = fabsf(_cos * BOB_UP);
    cam->camera.position = Vector3Add(cam->camera.position, Vector3Scale(bobbing, cam->walk_lerp));

    cam->camera.target = Vector3Add(cam->camera.position, *look_dir);
    
}