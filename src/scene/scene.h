#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"
#include "rlights.h"
#include "character.h"
#include "physics.h"
#include "physics_create.h"
#include "camera.h"
#include "rlModels.h"	
#include "rlModels_IO.h"

#define MODEL_COUNT 5
#define TEXTURE_COUNT 1
#define SHADER_COUNT 1

typedef struct {
    int ambientLoc;
    int fogDensityLoc;
    int lightCountLoc;
    float fogDensity;
    Shader shader;
    Light* light_list;
} ShaderAttributes;

typedef struct {
    Vector3 position;
    Vector3 rotation;
    dGeomID geom;
    int model;
} StaticMesh;

typedef struct {
    dBodyID body;
    int model;
} DynamicMesh;

typedef struct {
    StaticMesh* static_list;
    DynamicMesh* dynamic_list;
    DynamicMesh* platform_list;
    Texture2D* texture_list;
    ShaderAttributes* shader_list;
    Model* model_list;
    rlmModel* master_model_list;
    rlmModel* instance_model_list;
    rlmModelAnimationSet* animation_set_list;
    rlmAnimatedModelInstance* animated_instance_list;
    PlatformMovementAnimation* platform_animation_list;
    Character player;
    CameraFPS camera;
    PhysicsInstance physics;
    float delta_time;
} Scene;

extern Scene demo_scene;

void CreateScene();

void UpdateScene(float delta);

void DrawScene();

void UnloadScene();

ShaderAttributes CreateShader();

void UpdateShader(ShaderAttributes* attrib, Camera* camera);

#endif // SCENE_H