#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "input.h"

Camera CreateCamera(Vector3 position, Vector2* rotation);

/* Sets camera rotation and animation */
void UpdateFPSCameraAnimated(Camera* camera, Vector3 position, Vector2* rotation, float delta, PlayerInput input, bool grounded);

#endif // CAMERA_H