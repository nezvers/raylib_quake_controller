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
    Shader shader;
    Light light;
}ShaderAttributes;

typedef struct {
    Vector3 position;
    Vector3 rotation;
    dGeomID geom;
    Model* model;
} StaticMesh;

typedef struct {
    Camera camera;
    Character player;
    StaticMesh* static_list;
    Texture2D* texture_list;
    ShaderAttributes* shader_list;
    Model* model_list;
}Scene;

extern Scene demo_scene;

ShaderAttributes CreateShader();

void CreateScene();

void UpdateScene(float delta);

void DrawScene();

void UnloadScene();

bool IsPlayerGrounded();

#endif // SCENE_H