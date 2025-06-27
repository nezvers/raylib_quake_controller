#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"
#include "rlights.h"
#include "physics_ode.h"
#include "character.h"

#define MODEL_COUNT 5
#define TEXTURE_COUNT 1
#define SHADER_COUNT 1


typedef struct {
    int ambientLoc;
    int fogDensityLoc;
    float fogDensity;
    Light light;
}ShaderAttributes;

typedef struct {
    Vector3 position;
    Vector3 rotation;
    dGeomID geom;
    Model model;
} StaticMesh;

typedef struct {
    Camera camera;
    Character player;
    StaticMesh* static_list;
    //StaticMesh static_list[MODEL_COUNT];
    Texture2D textures[TEXTURE_COUNT];
    Shader shaders[SHADER_COUNT];
}Scene;

Shader CreateShader(ShaderAttributes* attrib);

void CreateScene();

void UpdateScene(Camera* camera);

void DrawScene();

void UnloadScene();

#endif // SCENE_H