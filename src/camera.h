#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"

Camera CreateCamera(Vector3 position, Vector2* rotation);

/* Sets camera rotation and animation */
void UpdateCameraAngle(Camera* camera, Vector3 position, Vector2* rotation, float delta, bool forward, bool sideway, bool crouching, bool grounded);

#endif // CAMERA_H