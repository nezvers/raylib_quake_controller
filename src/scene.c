
#include "scene.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "stb_ds.h"
#include "camera.h"
#include "input.h"
#include "models.h"


Scene demo_scene;

int SceneAddPlaneStatic(Scene* scene, Vector3 position, int model_id, dGeomID geom_id) {
    StaticMesh static_mesh = { 0 };
    static_mesh.model = model_id;
    static_mesh.position = position;
    static_mesh.geom = geom_id;

    int i = arrlen(scene->static_list);
    arrput(scene->static_list, static_mesh);
    if (arrlen(scene->static_list) > i) {
        return i;
    }
    return -1;
}

int SceneAddCubeStatic(Scene* scene, Vector3 position, int model_id, dGeomID geom_id) {
    StaticMesh static_mesh = { 0 };
    static_mesh.model = model_id;
    static_mesh.position = position;
    static_mesh.geom = geom_id;

    int i = arrlen(scene->static_list);
    arrput(scene->static_list, static_mesh);
    if (arrlen(scene->static_list) > i) {
        return i;
    }
    return -1;
}

int SceneAddBoxDynamic(Scene* scene, int model_id, dBodyID geom_id) {
    DynamicMesh dynamic_mesh = { 0 };
    dynamic_mesh.model = model_id;
    dynamic_mesh.body = geom_id;

    int i = arrlen(scene->dynamic_list);
    arrput(scene->dynamic_list, dynamic_mesh);
    if (arrlen(scene->dynamic_list) > i) {
        return i;
    }
    return -1;
}

int SceneAddSphereDynamic(Scene* scene, int model_id, dBodyID geom_id) {
    DynamicMesh dynamic_mesh = { 0 };
    dynamic_mesh.model = model_id;
    dynamic_mesh.body = geom_id;

    int i = arrlen(scene->dynamic_list);
    arrput(scene->dynamic_list, dynamic_mesh);
    if (arrlen(scene->dynamic_list) > i) {
        return i;
    }
    return -1;
}


// Mockup for loading a scene
void CreateModels() {
    demo_scene.static_list = NULL;
    demo_scene.dynamic_list = NULL;

    // Assign texture and shader
    demo_scene.texture_list = NULL;
    Texture2D tex_cheker = LoadTexture(RESOURCES_PATH"texel_checker.png");
    arrput(demo_scene.texture_list, tex_cheker);

    demo_scene.shader_list = NULL;
    ShaderAttributes shader_attrib = CreateShader();
    arrput(demo_scene.shader_list, shader_attrib);

    // Ground
    demo_scene.model_list = NULL;

    // Static
    const int shader_ID = 0;
    int  plane_model = CreateModelPlane(&demo_scene, (Vector2) { 100.f, 100.f }, demo_scene.shader_list[shader_ID].shader, tex_cheker);
    dGeomID plane_geom = CreatePhysicsPlaneStatic(Vector3Zero(), (Vector3) { 0, 1, 0 }, PHYS_SOLID, PHYS_ALL);
    SceneAddPlaneStatic(&demo_scene, (Vector3) { 0.f, 0.f, 0.f }, plane_model, plane_geom);

    const Vector3 tower_size = (Vector3){ 16.f, 32.f, 16.f };
    int tower_model = CreateModelBox(&demo_scene, tower_size, demo_scene.shader_list[shader_ID].shader, tex_cheker);
    Vector3 tower_position = (Vector3){ 16.f, 16.f, 16.f };
    dGeomID tower_geom = CreatePhysicsBoxStatic(tower_position, tower_size, PHYS_SOLID, PHYS_ALL);
    SceneAddCubeStatic(&demo_scene, tower_position, tower_model, tower_geom);

    tower_position = (Vector3){ 16.f, 16.f, -16.f };
    tower_geom = CreatePhysicsBoxStatic(tower_position, tower_size, PHYS_SOLID, PHYS_ALL);
    SceneAddCubeStatic(&demo_scene, tower_position, tower_model, tower_geom);

    tower_position = (Vector3){ -16.f, 16.f, 16.f };
    tower_geom = CreatePhysicsBoxStatic(tower_position, tower_size, PHYS_SOLID, PHYS_ALL);
    SceneAddCubeStatic(&demo_scene, tower_position, tower_model, tower_geom);

    tower_position = (Vector3){ -16.f, 16.f, -16.f };
    tower_geom = CreatePhysicsBoxStatic(tower_position, tower_size, PHYS_SOLID, PHYS_ALL);
    SceneAddCubeStatic(&demo_scene, tower_position, tower_model, tower_geom);

    // Dynamic
    const Vector3 box_size = (Vector3){ 1.f, 1.f, 1.f };
    const Vector3 box_rotation = (Vector3){ 0, 0, 0 };
    Vector3 box_position = (Vector3){ 0.f, 1.f, -0.f };
    int box_model = CreateModelBox(&demo_scene, box_size, demo_scene.shader_list[shader_ID].shader, tex_cheker);
    dBodyID box_body = CreatePhysicsBodyBoxDynamic(box_position, box_rotation, box_size, PHYS_DYNAMIC, PHYS_ALL);
    SceneAddBoxDynamic(&demo_scene, box_model, box_body);

    const float sphere_radius = 0.5f;
    const Vector3 sphere_rotation = (Vector3){ 0, 0, 0 };
    Vector3 sphere_position = (Vector3){ 0.f, 2.f, -0.f };
    int sphere_model = CreateModelSphere(&demo_scene, sphere_radius, demo_scene.shader_list[shader_ID].shader, tex_cheker);
    dBodyID sphere_body = CreatePhysicsBodySphereDynamic(sphere_position, sphere_rotation, sphere_radius, PHYS_DYNAMIC, PHYS_ALL);
    SceneAddSphereDynamic(&demo_scene, sphere_model, sphere_body);
}

void CreateScene() {
    CreatePhysics();
    CreateModels();
    demo_scene.player = CreateBody((Vector3) {0,1,3}, Vector2Zero());
    demo_scene.camera = CreateCamera(demo_scene.player.position, &demo_scene.player.rotation);
}

void UpdateShader(ShaderAttributes* attrib, Camera* camera) {
    SetShaderValue(attrib->shader, attrib->fogDensityLoc, &attrib->fogDensity, SHADER_UNIFORM_FLOAT);

    static float light_t = 0.f;
    light_t += GetFrameTime();
    attrib->lightStrength = sin(light_t) * 0.3f + 0.6f;
    SetShaderValue(attrib->shader, attrib->strengthLoc, &attrib->lightStrength, SHADER_UNIFORM_FLOAT);

    // Update the light shader with the camera view position
    SetShaderValue(attrib->shader, attrib->shader.locs[SHADER_LOC_VECTOR_VIEW], &camera->position.x, SHADER_UNIFORM_VEC3);
}

void UpdateScene(float delta) {
    PlayerInput player_input = UpdateInput();
    demo_scene.player.rotation.x -= player_input.mouse.x;
    demo_scene.player.rotation.y += player_input.mouse.y;

    UpdateBody(&demo_scene.player, demo_scene.player.rotation.x, player_input);

    UpdatePhysics(delta);

    float* pos = (float*)dBodyGetPosition(demo_scene.player.phys.body);
    demo_scene.player.position = (Vector3){ pos[0], pos[1], pos[2] };
    
    UpdateFPSCameraAnimated(&demo_scene.camera, demo_scene.player.position, &demo_scene.player.rotation, delta, player_input, demo_scene.player.is_grounded);

    UpdateShader(&demo_scene.shader_list[0], &demo_scene.camera);
}

void DrawScene() {
    ClearBackground(RAYWHITE);

    BeginMode3D(demo_scene.camera);

    // Draw level
    for (int i = 0; i < arrlen(demo_scene.static_list); i++) {
        
        DrawModel(demo_scene.model_list[demo_scene.static_list[i].model], demo_scene.static_list[i].position, 1.0f, WHITE);
    }
    for (int i = 0; i < arrlen(demo_scene.dynamic_list); i++) {
        DynamicMesh dynamic_mesh = demo_scene.dynamic_list[i];
        SetPhysicsTransform(
            (float*)dBodyGetPosition(dynamic_mesh.body),
            (float*)dBodyGetRotation(dynamic_mesh.body),
            &demo_scene.model_list[dynamic_mesh.model].transform);
        DrawModel(demo_scene.model_list[dynamic_mesh.model], Vector3Zero(), 1.0f, WHITE);
    }

    DrawSphere((Vector3) { 0.f, 300.f, -300.f}, 100.f, RED);

    EndMode3D();
}

void UnloadScene() {
    arrfree(demo_scene.static_list);
    arrfree(demo_scene.shader_list);
    for (int i = 0; i < arrlen(demo_scene.texture_list); i++) {
        UnloadTexture(demo_scene.texture_list[i]);
    }
    arrfree(demo_scene.texture_list);
    for (int i = 0; i < arrlen(demo_scene.model_list); i++) {
        UnloadModel(demo_scene.model_list[i]);
    }
    arrfree(demo_scene.static_list);

    /*
    UnloadTexture(texture);
    UnloadTexture(texturePlane);
    UnloadModel(plane);
    UnloadModel(sphere);
    UnloadModel(box);
    */

    DestroyPhysics();
}

ShaderAttributes CreateShader() {
    ShaderAttributes attrib = { 0 };
    // Load shader and set up some uniforms
    attrib.shader = LoadShader(RESOURCES_PATH"lighting.vs", RESOURCES_PATH"lighting.fs");
    attrib.shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(attrib.shader, "matModel");
    attrib.shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(attrib.shader, "viewPos");

    // Ambient light level
    attrib.ambientLoc = GetShaderLocation(attrib.shader, "ambient");
    SetShaderValue(attrib.shader, attrib.ambientLoc, (float[4]) { 0.001f, 0.001f, 0.001f, 1.0f }, SHADER_UNIFORM_VEC4);

    attrib.fogDensity = 0.025f;
    attrib.fogDensityLoc = GetShaderLocation(attrib.shader, "fogDensity");
    SetShaderValue(attrib.shader, attrib.fogDensityLoc, &attrib.fogDensity, SHADER_UNIFORM_FLOAT);

    attrib.lightStrength = 0.1f;
    attrib.strengthLoc = GetShaderLocation(attrib.shader, "strength");
    SetShaderValue(attrib.shader, attrib.strengthLoc, &attrib.lightStrength, SHADER_UNIFORM_FLOAT);

    // Using just 1 point lights
    attrib.light = CreateLight(LIGHT_POINT, (Vector3) { 0, 4, 0 }, (Vector3) { 0, 4, 0 }, (Color) {5,5,5,255}, attrib.shader);

    return attrib;
}


bool IsPlayerGrounded() {
    return IsPhysicsPairColliding(demo_scene.player.phys.footGeom, demo_scene.static_list[0].geom);
}