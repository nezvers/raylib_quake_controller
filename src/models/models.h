#ifndef MODELS_H
#define MODELS_H

#include "raylib.h"

int CreateModelPlane(Model** model_list, Vector2 size, Shader shadr, Texture texture);

int CreateModelBox(Model** model_list, Vector3 size, Shader shadr, Texture texture);

int CreateModelSphere(Model** model_list, float radius, Shader shadr, Texture texture);

#endif // MODELS_H