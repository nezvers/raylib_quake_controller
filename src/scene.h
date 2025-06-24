#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"
#include "rlights.h"
#include "physics_ode.h"

#define MODEL_COUNT 5


typedef struct {
    int ambientLoc;
    int fogDensityLoc;
    float fogDensity;
    Light light;
}ShaderAttributes;
Shader CreateShader(ShaderAttributes* attrib);

void CreateScene();

void UpdateScene(Camera* camera);

void DrawScene();

void UnloadScene();

#endif // SCENE_H