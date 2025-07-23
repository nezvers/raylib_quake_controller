#ifndef SHADERS_H
#define SHADERS_H

#include "raylib.h"
#include "rlights.h"

typedef struct {
    int ambientLoc;
    int fogDensityLoc;
    int lightCountLoc;
    float fogDensity;
    Shader shader;
    Light* light_list;
} ShaderAttributes;

ShaderAttributes CreateShader();

void UpdateShader(ShaderAttributes* attrib, Camera* camera);

#endif // SHADERS_H
