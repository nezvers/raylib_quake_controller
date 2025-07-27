#ifndef SHADERS_H
#define SHADERS_H

#include "raylib.h"
#include "rlights.h"

// TODO: add shadows https://noino.substack.com/p/raylib-graphics-shading
// https://github.com/WEREMSOFT/c99-raylib-shadowmap

typedef struct {
    int ambientLoc;
    int fogDensityLoc;
    int lightCountLoc;
    float fogDensity;
    Shader shader;
    Light* light_list;
} ShaderAttributes;

ShaderAttributes CreateShader(int shader_id);

void UpdateShader(ShaderAttributes* attrib, Camera* camera);

void CreateShadersLight(int type, Vector3 position, Vector3 target, Color color, float strength, int index, ShaderAttributes** shader_attribute);

#endif // SHADERS_H
