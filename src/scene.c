
#include "scene.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "stb_ds.h"
#include "input.h"
#include "models.h"
#include "debug_draw.h"
#include "physics_ode.h" // for setting transforms in draw


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

int SceneAddPlatform(Scene* scene, int model_id, dBodyID geom_id, PlatformMovementAnimation anim) {
    DynamicMesh dynamic_mesh = { 0 };
    dynamic_mesh.model = model_id;
    dynamic_mesh.body = geom_id;

    int anim_i = arrlen(scene->platform_animation_list);
    arrput(scene->platform_animation_list, anim);
    if (arrlen(scene->platform_animation_list) > anim_i) {
        dBodySetData(dynamic_mesh.body, &scene->platform_animation_list[anim_i]);
    }

    int i = arrlen(scene->platform_list);
    arrput(scene->platform_list, dynamic_mesh);
    if (arrlen(scene->platform_list) > i) {
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
    demo_scene.platform_list = NULL;
    demo_scene.platform_animation_list = NULL;
    demo_scene.delta_time = 0.f;

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
    dGeomID plane_geom = CreatePhysicsPlaneStatic(&demo_scene.physics, Vector3Zero(), (Vector3) { 0, 1, 0 }, PHYS_SOLID, PHYS_ALL);
    SceneAddPlaneStatic(&demo_scene, (Vector3) { 0.f, 0.f, 0.f }, plane_model, plane_geom);

    const Vector3 tower_size = (Vector3){ 16.f, 32.f, 16.f };
    int tower_model = CreateModelBox(&demo_scene, tower_size, demo_scene.shader_list[shader_ID].shader, tex_cheker);
    Vector3 tower_position = (Vector3){ 16.f, 16.f, 16.f };
    dGeomID tower_geom = CreatePhysicsBoxStatic(&demo_scene.physics, tower_position, (Vector3) { 0 }, tower_size, PHYS_SOLID, PHYS_ALL);
    SceneAddCubeStatic(&demo_scene, tower_position, tower_model, tower_geom);

    tower_position = (Vector3){ 16.f, 16.f, -16.f };
    tower_geom = CreatePhysicsBoxStatic(&demo_scene.physics, tower_position, (Vector3) { 0 }, tower_size, PHYS_SOLID, PHYS_ALL);
    SceneAddCubeStatic(&demo_scene, tower_position, tower_model, tower_geom);

    tower_position = (Vector3){ -16.f, 16.f, 16.f };
    tower_geom = CreatePhysicsBoxStatic(&demo_scene.physics, tower_position, (Vector3) { 0 }, tower_size, PHYS_SOLID, PHYS_ALL);
    SceneAddCubeStatic(&demo_scene, tower_position, tower_model, tower_geom);

    tower_position = (Vector3){ -16.f, 16.f, -16.f };
    tower_geom = CreatePhysicsBoxStatic(&demo_scene.physics, tower_position, (Vector3) { 0 }, tower_size, PHYS_SOLID, PHYS_ALL);
    SceneAddCubeStatic(&demo_scene, tower_position, tower_model, tower_geom);

    // Dynamic
    const Vector3 box_size = (Vector3){ 1.f, 1.f, 1.f };
    const Vector3 box_rotation = (Vector3){ 0, 0, 0 };
    Vector3 box_position = (Vector3){ 0.f, 1.f, -0.f };
    int box_model = CreateModelBox(&demo_scene, box_size, demo_scene.shader_list[shader_ID].shader, tex_cheker);
    dBodyID box_body = CreatePhysicsBodyBoxDynamic(&demo_scene.physics, box_position, box_rotation, box_size, PHYS_DYNAMIC, PHYS_ALL);
    SceneAddBoxDynamic(&demo_scene, box_model, box_body);

    const float sphere_radius = 0.5f;
    const Vector3 sphere_rotation = (Vector3){ 0, 0, 0 };
    Vector3 sphere_position = (Vector3){ 0.f, 2.f, -0.f };
    int sphere_model = CreateModelSphere(&demo_scene, sphere_radius, demo_scene.shader_list[shader_ID].shader, tex_cheker);
    dBodyID sphere_body = CreatePhysicsBodySphereDynamic(&demo_scene.physics, sphere_position, sphere_rotation, sphere_radius, PHYS_DYNAMIC, PHYS_ALL);
    SceneAddSphereDynamic(&demo_scene, sphere_model, sphere_body);

    // Platform
    const Vector3 platform_size = (Vector3){ 4.f, 1.f, 4.f };
    const Vector3 platform_rotation = (Vector3){ 0, 0, 0 };
    Vector3 platform_position = (Vector3){ 0.f, -0.5f, -16.f };
    PlatformMovementAnimation platform_animation = (PlatformMovementAnimation){ platform_position, (Vector3) { 0.f, 8.f, -16.f }, platform_position, 0, 0.2f, &demo_scene.delta_time };
    int platform_model = CreateModelBox(&demo_scene, platform_size, demo_scene.shader_list[shader_ID].shader, tex_cheker);
    dBodyID platform_body = CreatePhysicsBoxAnimated(&demo_scene.physics, platform_position, platform_rotation, platform_size, PHYS_SOLID, 0);
    SceneAddPlatform(&demo_scene, platform_model, platform_body, platform_animation);
}

void CreateScene() {
    // TODO: Load from scene prefab/ save file
    demo_scene.physics = CreatePhysics();
    CreateModels();
    Vector3 player_position = (Vector3){ 0,1,3 };
    demo_scene.player = CreateCharacter(player_position, Vector2Zero(), CreatePhysicsPlayerBody(&demo_scene.physics, player_position));
    player_position.y += demo_scene.player.head_lerp;
    demo_scene.camera = CreateCamera(player_position, &demo_scene.player.rotation, &demo_scene.player.look_dir);
}

void UpdateScene(float delta) {
    demo_scene.delta_time = delta;
    UpdateDebugDraw(delta);
    PlayerInput player_input = UpdateInput();
    demo_scene.player.rotation.x -= player_input.mouse.x;
    demo_scene.player.rotation.y += player_input.mouse.y;

    UpdateCharacter(&demo_scene.physics, &demo_scene.player, demo_scene.player.rotation.x, &player_input, delta);

    UpdatePhysics(&demo_scene.physics, delta);


    UpdateCharacterPlayer(&demo_scene.physics, &demo_scene.player, &player_input, &demo_scene.camera, delta);

    UpdateShader(&demo_scene.shader_list[0], &demo_scene.camera.camera);

}

void DrawScene() {
    ClearBackground(RAYWHITE);

    BeginMode3D(demo_scene.camera.camera);

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
    for (int i = 0; i < arrlen(demo_scene.platform_list); i++) {
        DynamicMesh dynamic_mesh = demo_scene.platform_list[i];
        Model *model = &demo_scene.model_list[dynamic_mesh.model];
        SetPhysicsTransform(
            (float*)dBodyGetPosition(dynamic_mesh.body),
            (float*)dBodyGetRotation(dynamic_mesh.body),
            &(model->transform));
        DrawModel(*model, Vector3Zero(), 1.0f, WHITE);
    }

    DrawSphere((Vector3) { 0.f, 300.f, -300.f}, 100.f, RED);

    DrawDebugDraw();

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

    DestroyPhysics(NULL);
}

ShaderAttributes CreateShader() {
    ShaderAttributes attrib = { 0 };
    attrib.light_list = NULL;

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

    // LIGHT INSTANCE
    Vector3 light_pos = (Vector3){ 0, 4, 0 };
    Vector3 light_target = light_pos;
    Color light_color = (Color){ 5,5,5,255 };
    float light_strength = 0.1f;
    Light light_inst = CreateLight(LIGHT_POINT, light_pos, light_target, light_color, light_strength, attrib.shader);
    arrput(attrib.light_list, light_inst);

    // Initialize light count
    attrib.lightCountLoc = GetShaderLocation(attrib.shader, "lightCount");
    int light_count = arrlen(attrib.light_list);
    SetShaderValue(attrib.shader, attrib.lightCountLoc, &light_count, SHADER_UNIFORM_INT);

    return attrib;
}

// TODO: GTFO
void UpdateShader(ShaderAttributes* attrib, Camera* camera) {
    SetShaderValue(attrib->shader, attrib->fogDensityLoc, &attrib->fogDensity, SHADER_UNIFORM_FLOAT);

    // Update the light shader with the camera view position
    SetShaderValue(attrib->shader, attrib->shader.locs[SHADER_LOC_VECTOR_VIEW], &camera->position.x, SHADER_UNIFORM_VEC3);

    int light_count = arrlen(attrib->light_list);
    SetShaderValue(attrib->shader, attrib->lightCountLoc, &light_count, SHADER_UNIFORM_INT);

    for (int i = 0; i < light_count; i++) {
        Light* inst = &attrib->light_list[i];
        static float light_t = 0.f;
        light_t += GetFrameTime();
        inst->strength = sin(light_t) * 0.3f + 0.6f;

        UpdateLightValues(attrib->shader, *inst);
    }
}


bool IsCharacterGrounded(PhysicsInstance* instance, Character* character) {
    return IsPhysicsPairColliding(&demo_scene.physics, character->phys.footGeom, instance->space);
}