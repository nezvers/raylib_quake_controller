
#include "scene.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"


Scene demo_scene;
Model plane;
Model box;
Model sphere;
Texture texture;
Texture2D texturePlane;

void SceneAddPlane(Scene* scene, Vector2 size, Vector3 position, Shader shadr, Texture texture) {
    StaticMesh _static_mesh = { 0 };
    _static_mesh.model = LoadModelFromMesh(GenMeshPlane(size.x, size.y, 1.f, 1.f));
    _static_mesh.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    _static_mesh.model.materials[0].shader = shadr;
    _static_mesh.position = position;

    _static_mesh.geom = CreatePhysicsPlane(Vector3Zero(), (Vector3) { 0, 1, 0 }, catBits[PLANE], catBits[ALL]);
    // TODO: Generate physical collider
    arrput(scene->static_list, _static_mesh);
}

void SceneAddCube(Scene* scene, Vector3 size, Vector3 position, Shader shadr, Texture texture) {
    StaticMesh _static_mesh = { 0 };
    _static_mesh.model = LoadModelFromMesh(GenMeshCube(size.x, size.y, size.z));
    _static_mesh.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    _static_mesh.model.materials[0].shader = shadr;
    _static_mesh.position = position;

    _static_mesh.geom = CreatePhysicsBox(position, size, catBits[PLANE], catBits[ALL]);

    // TODO: Generate physical collider
    arrput(scene->static_list, _static_mesh);
}

void CreateModels() {
    demo_scene.static_list = NULL;
    // Assign texture and shader
    demo_scene.textures = NULL;
    Texture2D tex_cheker = LoadTexture(RESOURCES_PATH"texel_checker.png");
    arrput(demo_scene.textures, tex_cheker);


    ShaderAttributes shader_attrib = CreateShader();
    demo_scene.shaders = NULL;
    arrput(demo_scene.shaders, shader_attrib);

    // Ground
    SceneAddPlane(&demo_scene, (Vector2) { 100.f, 100.f }, (Vector3) { 0.f, 0.f, 0.f }, demo_scene.shaders[0].shader, tex_cheker);
    SceneAddCube(&demo_scene, (Vector3) { 16.f, 32.f, 16.f }, (Vector3) { 16.f, 16.f, 16.f }, demo_scene.shaders[0].shader, tex_cheker);
    SceneAddCube(&demo_scene, (Vector3) { 16.f, 32.f, 16.f }, (Vector3) { -16.f, 16.f, 16.f }, demo_scene.shaders[0].shader, tex_cheker);
    SceneAddCube(&demo_scene, (Vector3) { 16.f, 32.f, 16.f }, (Vector3) { -16.f, 16.f, -16.f }, demo_scene.shaders[0].shader, tex_cheker);
    SceneAddCube(&demo_scene, (Vector3) { 16.f, 32.f, 16.f }, (Vector3) { 16.f, 16.f, -16.f }, demo_scene.shaders[0].shader, tex_cheker);

    Texture _tex = LoadTexture(RESOURCES_PATH"texel_checker.png");
    texturePlane = LoadTexture(RESOURCES_PATH"grass-texture.png");

    plane = LoadModel(RESOURCES_PATH"grass-plane.obj");
    box = LoadModelFromMesh(GenMeshCube(1, 1, 1));
    sphere = LoadModelFromMesh(GenMeshSphere(.5, 32, 32));

    box.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = _tex;
    sphere.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = _tex;
    plane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texturePlane;
}

void CreateScene() {
    CreatePhysics();
    CreateModels();
    demo_scene.player = CreateBody(Vector3Zero());
}

void UpdateShader(ShaderAttributes* attrib, Camera* camera) {
    SetShaderValue(attrib->shader, attrib->fogDensityLoc, &attrib->fogDensity, SHADER_UNIFORM_FLOAT);

    // Update the light shader with the camera view position
    SetShaderValue(attrib->shader, attrib->shader.locs[SHADER_LOC_VECTOR_VIEW], &camera->position.x, SHADER_UNIFORM_VEC3);
}

void UpdateScene(Camera* camera) {
    UpdateShader(&demo_scene.shaders[0], camera);
}

void DrawScene() {
    // Draw level
    for (int i = 0; i < arrlen(demo_scene.static_list); i++) {
        DrawModel(demo_scene.static_list[i].model, demo_scene.static_list[i].position, 1.0f, WHITE);
    }
    DrawSphere((Vector3) { 0.f, 300.f, -300.f}, 100.f, RED);
    //DrawModel(plane, (Vector3) { 0, 0, 0 }, 1.0f, WHITE);
    DrawPhysics(plane, sphere, box);
}

void UnloadScene() {
    arrfree(demo_scene.static_list);
    arrfree(demo_scene.shaders);
    for (int i = 0; i < arrlen(demo_scene.textures); i++) {
        UnloadTexture(demo_scene.textures[i]);
    }
    arrfree(demo_scene.textures);
    for (int i = 0; i < arrlen(demo_scene.static_list); i++) {
        UnloadModel(demo_scene.static_list[i].model);
    }
    arrfree(demo_scene.static_list);

    UnloadTexture(texture);
    UnloadTexture(texturePlane);
    UnloadModel(plane);
    UnloadModel(sphere);
    UnloadModel(box);

    DestroyPhysics();
}

ShaderAttributes CreateShader() {
    ShaderAttributes attrib = { 0 };
    // Load shader and set up some uniforms
    attrib.shader = LoadShader(RESOURCES_PATH"lighting.vs", RESOURCES_PATH"fog.fs");
    attrib.shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(attrib.shader, "matModel");
    attrib.shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(attrib.shader, "viewPos");

    // Ambient light level
    attrib.ambientLoc = GetShaderLocation(attrib.shader, "ambient");
    SetShaderValue(attrib.shader, attrib.ambientLoc, (float[4]) { 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);

    attrib.fogDensity = 0.025f;
    attrib.fogDensityLoc = GetShaderLocation(attrib.shader, "fogDensity");
    SetShaderValue(attrib.shader, attrib.fogDensityLoc, &attrib.fogDensity, SHADER_UNIFORM_FLOAT);

    // Using just 1 point lights
    attrib.light = CreateLight(LIGHT_POINT, (Vector3) { 0, 4, 0 }, Vector3Zero(), WHITE, attrib.shader);

    return attrib;
}


bool IsPlayerGrounded() {
    return IsPhysicsPairColliding(demo_scene.player.phys.footGeom, demo_scene.static_list[0].geom);
}