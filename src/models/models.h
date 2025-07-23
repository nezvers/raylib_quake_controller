#ifndef MODELS_H
#define MODELS_H

#include "raylib.h"
#include "scene.h"

int CreateModelPlane(Scene* scene, Vector2 size, Shader shadr, Texture texture);

int CreateModelBox(Scene* scene, Vector3 size, Shader shadr, Texture texture);

int CreateModelSphere(Scene* scene, float radius, Shader shadr, Texture texture);

#endif // MODELS_H