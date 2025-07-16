#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "input.h"

typedef struct {
    float bob_timer;
    float walk_lerp;
    Vector2 lean;
    Vector2 offset; // used for camera shake
    Camera camera;
} CameraFPS;

CameraFPS CreateCamera(Vector3 position, Vector2* rotation, Vector3* look_dir);

/* Sets camera rotation and animation */
void UpdateFPSCameraAnimated(CameraFPS* camera, Vector3 position, Vector2* rotation, float delta, PlayerInput* input, bool grounded, Vector3* look_dir);

#endif // CAMERA_H