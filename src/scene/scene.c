
#include "scene.h"
#include "scene_create.h"
#include "raymath.h"
#include "stb_ds.h"
#include "input.h"
#include "models.h"
#include "debug_draw.h"
#include "physics_ode.h" // for setting transforms in draw
#include "assets.h"


Scene demo_scene;


// Mockup for loading a scene
void CreateModels() {
    int texture_ID = 0;
    arrput(demo_scene.texture_list, LoadTexture(tex_file_list[TEX_TEXEL_CHECKER]));

    arrput(demo_scene.shader_list, CreateShader(SDR_GENERIC));
    arrput(demo_scene.shader_list, CreateShader(SDR_SKINNING));

    Vector3 light_pos = (Vector3){ 0, 4, 0 };
    Vector3 light_target = light_pos;
    Color light_color = (Color){ 5,5,5,255 };
    float light_strength = 0.5f;
    CreateShadersLight(LIGHT_POINT, light_pos, light_target, light_color, light_strength, 0, &demo_scene.shader_list);
    light_strength = 0.1f;
    CreateShadersLight(LIGHT_POINT, light_pos, light_target, light_color, light_strength, 1, &demo_scene.shader_list);

    // Static
    int shader_ID = 0;
    int  plane_model = CreateModelPlane(&demo_scene.model_list, (Vector2) { 100.f, 100.f }, demo_scene.shader_list[shader_ID].shader, demo_scene.texture_list[texture_ID]);
    dGeomID plane_geom = CreatePhysicsPlaneStatic(&demo_scene.physics, Vector3Zero(), (Vector3) { 0, 1, 0 }, PHYS_SOLID, PHYS_ALL);
    SceneAddPlaneStatic(&demo_scene, (Vector3) { 0.f, 0.f, 0.f }, plane_model, plane_geom);

    const Vector3 tower_size = (Vector3){ 16.f, 32.f, 16.f };
    int tower_model = CreateModelBox(&demo_scene.model_list, tower_size, demo_scene.shader_list[shader_ID].shader, demo_scene.texture_list[texture_ID]);
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
    int box_model = CreateModelBox(&demo_scene.model_list, box_size, demo_scene.shader_list[shader_ID].shader, demo_scene.texture_list[texture_ID]);
    dBodyID box_body = CreatePhysicsBodyBoxDynamic(&demo_scene.physics, box_position, box_rotation, box_size, PHYS_DYNAMIC, PHYS_ALL);
    SceneAddBoxDynamic(&demo_scene, box_model, box_body);

    const float sphere_radius = 0.5f;
    const Vector3 sphere_rotation = (Vector3){ 0, 0, 0 };
    Vector3 sphere_position = (Vector3){ 0.f, 2.f, -0.f };
    int sphere_model = CreateModelSphere(&demo_scene.model_list, sphere_radius, demo_scene.shader_list[shader_ID].shader, demo_scene.texture_list[texture_ID]);
    dBodyID sphere_body = CreatePhysicsBodySphereDynamic(&demo_scene.physics, sphere_position, sphere_rotation, sphere_radius, PHYS_DYNAMIC, PHYS_ALL);
    SceneAddSphereDynamic(&demo_scene, sphere_model, sphere_body);

    // Platform
    const Vector3 platform_size = (Vector3){ 4.f, 1.f, 4.f };
    const Vector3 platform_rotation = (Vector3){ 0, 0, 0 };
    Vector3 platform_position = (Vector3){ 0.f, -0.5f, -16.f };
    PlatformMovementAnimation platform_animation = (PlatformMovementAnimation){ platform_position, (Vector3) { 0.f, 8.f, -16.f }, platform_position, 0, 0.2f, &demo_scene.delta_time };
    int platform_model = CreateModelBox(&demo_scene.model_list, platform_size, demo_scene.shader_list[shader_ID].shader, demo_scene.texture_list[texture_ID]);
    dBodyID platform_body = CreatePhysicsBoxAnimated(&demo_scene.physics, platform_position, platform_rotation, platform_size, PHYS_SOLID, 0);
    SceneAddPlatform(&demo_scene, platform_model, platform_body, platform_animation);

    // ANIMATED MODELS
    shader_ID = 1;
    int master_id = CreateAnimatedMasterModel(&demo_scene.master_model_list, mdl_file_list[MDL_ANIMATED_CUBE], demo_scene.shader_list[shader_ID].shader, demo_scene.texture_list[texture_ID]);
    int animation_set_id = CreateAnimationSet(&demo_scene.animation_set_list, mdl_file_list[MDL_ANIMATED_CUBE], &demo_scene.master_model_list[master_id]);
    int master_instance_id = CreateMasterModelInstance(&demo_scene.instance_model_list, &demo_scene.master_model_list[master_id]);
    int animated_inst_id = CreateAnimatedInstance(&demo_scene.animated_instance_list, &demo_scene.instance_model_list[master_instance_id], &demo_scene.animation_set_list[animation_set_id]);
    demo_scene.animated_instance_list[animated_inst_id].transform = rlmPQSTranslation(-6.f, 3, 0); // Test position
    demo_scene.animated_instance_list[animated_inst_id].transform.rotation = QuaternionFromAxisAngle((Vector3) { 0, 1, 0 }, 180 * DEG2RAD); // Test rotation
}

void CreateScene() {
    demo_scene = (Scene){ 0 };
    demo_scene.physics = CreatePhysics();
    // TODO: Load from scene prefab/ save file
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

    // Player's light follow
    Vector3 player_light_position = Vector3Add(demo_scene.player.position, (Vector3) { 0, 2, 0 });

    // Light flicker
    static float light_t = 0.f;
    light_t += GetFrameTime();
    float light_strength = sin(light_t) * 0.3f + 0.6f;

    for (int i = 0; i < arrlen(demo_scene.shader_list); i++) {
        rlmAnimatedModelInstance* shader_instance = &demo_scene.shader_list[i];

        demo_scene.shader_list[i].light_list[0].position = player_light_position;
        demo_scene.shader_list[i].light_list[0].dirty |= LIGHT_DIRTY_POSITION;

        Light* center_light = &demo_scene.shader_list[i].light_list[1];
        center_light->strength = light_strength;
        center_light->dirty |= LIGHT_DIRTY_STRENGTH;

        UpdateShader(shader_instance, &demo_scene.camera.camera);
    }

    // Animated models
    for (int i = 0; i < arrlen(demo_scene.animated_instance_list); i++) {
        rlmAnimatedModelInstance* instance = &demo_scene.animated_instance_list[i];
        rlmAdvanceAnimationInstance(instance, delta);
    }

    if (IsKeyPressed(KEY_ENTER)){
        for (int i = 0; i < arrlen(demo_scene.animated_instance_list); i++) {
            demo_scene.animated_instance_list[i].interpolate = !demo_scene.animated_instance_list[i].interpolate;
            TraceLog(LOG_INFO, "Animating %s\n", demo_scene.animated_instance_list[i].interpolate ? "ON" : "OFF");
        }
    }

    if (IsKeyPressed(KEY_RIGHT)) {
        for (int i = 0; i < arrlen(demo_scene.animated_instance_list); i++) {
            demo_scene.animated_instance_list[i].currentSequence = (demo_scene.animated_instance_list[i].currentSequence + 1) % demo_scene.animated_instance_list[i].sequences->sequenceCount;
            rlmSetAnimationInstanceSequence(&demo_scene.animated_instance_list[i], demo_scene.animated_instance_list[i].currentSequence);
            TraceLog(LOG_INFO, "Current sequence %d\n", demo_scene.animated_instance_list[i].currentSequence);
        }
    }

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

    // Animated models
    for (int i = 0; i < arrlen(demo_scene.animated_instance_list); i++) {
        rlmAnimatedModelInstance* instance = &demo_scene.animated_instance_list[i];
        rlmDrawModelWithPose(*instance->model, instance->transform, &instance->currentPose);
    }

    DrawSphere((Vector3) { 0.f, 300.f, -300.f}, 100.f, RED);

    DrawDebugDraw();

    EndMode3D();
}

void UnloadScene() {
    arrfree(demo_scene.static_list);
    arrfree(demo_scene.dynamic_list);
    arrfree(demo_scene.platform_list);
    arrfree(demo_scene.platform_animation_list);
    arrfree(demo_scene.master_model_list);

    // ?? : Elements Don't have to be unloaded
    arrfree(demo_scene.animated_instance_list);

    for (int i = 0; i < arrlen(demo_scene.texture_list); i++) {
        UnloadTexture(demo_scene.texture_list[i]);
    }
    arrfree(demo_scene.texture_list);

    for (int i = 0; i < arrlen(demo_scene.model_list); i++) {
        UnloadModel(demo_scene.model_list[i]);
    }
    arrfree(demo_scene.model_list);

    for (int i = 0; i < arrlen(demo_scene.master_model_list); i++) {
        rlmUnloadModel(&demo_scene.master_model_list[i]);
    }
    arrfree(demo_scene.master_model_list);

    for (int i = 0; i < arrlen(demo_scene.animation_set_list); i++) {
        rlmUnloadAnimationSet(&demo_scene.animation_set_list[i]);
    }
    arrfree(demo_scene.animation_set_list);

    for (int i = 0; i < arrlen(demo_scene.shader_list); i++) {
        arrfree(demo_scene.shader_list[i].light_list);
    }
    arrfree(demo_scene.shader_list);

    DestroyPhysics(NULL);
}
