
#include "scene.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

ShaderAttributes shader_attrib;
Shader shader;

Model model_list[MODEL_COUNT];
Vector3 position_list[MODEL_COUNT];
Model plane;
Model box;
Model sphere;
Texture texture;
Texture2D texturePlane;

void CreateModels() {
    // Ground
    model_list[0] = LoadModelFromMesh(GenMeshPlane(100.f, 100.f, 1.f, 1.f));
    position_list[0] = (Vector3){ 0.f, 0.f, 0.f };

    // Boxes
    model_list[1] = LoadModelFromMesh(GenMeshCube(16.f, 32.f, 16.f));
    position_list[1] = (Vector3){ 16.f, 16.f, 16.f };

    model_list[2] = LoadModelFromMesh(GenMeshCube(16.f, 32.f, 16.f));
    position_list[2] = (Vector3){ -16.f, 16.f, 16.f };

    model_list[3] = LoadModelFromMesh(GenMeshCube(16.f, 32.f, 16.f));
    position_list[3] = (Vector3){ -16.f, 16.f, -16.f };

    model_list[4] = LoadModelFromMesh(GenMeshCube(16.f, 32.f, 16.f));
    position_list[4] = (Vector3){ 16.f, 16.f, -16.f };

    // Assign texture and shader
    texture = LoadTexture(RESOURCES_PATH"texel_checker.png");
    texturePlane = LoadTexture(RESOURCES_PATH"grass-texture.png");

    for (int i = 0; i < MODEL_COUNT; i++) {
        model_list[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
        model_list[i].materials[0].shader = shader;
    }
    plane = LoadModel(RESOURCES_PATH"grass-plane.obj");
    box = LoadModelFromMesh(GenMeshCube(1, 1, 1));
    sphere = LoadModelFromMesh(GenMeshSphere(.5, 32, 32));

    box.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    sphere.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    plane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texturePlane;
}

void CreateScene() {
    shader = CreateShader(&shader_attrib);
    CreateModels();
    CreatePhysics(&plane);
}

void UpdateShader(Shader* shader, Camera* camera, ShaderAttributes* attrib) {
    SetShaderValue(*shader, attrib->fogDensityLoc, &attrib->fogDensity, SHADER_UNIFORM_FLOAT);

    // Update the light shader with the camera view position
    SetShaderValue(*shader, shader->locs[SHADER_LOC_VECTOR_VIEW], &camera->position.x, SHADER_UNIFORM_VEC3);
}

void UpdateScene(Camera* camera) {
    UpdateShader(&shader, camera, &shader_attrib);
}

void DrawScene() {
    // Draw level
    for (int i = 0; i < MODEL_COUNT; i++) {
        DrawModel(model_list[i], position_list[i], 1.0f, WHITE);
    }
    DrawSphere((Vector3) { 0.f, 300.f, -300.f}, 100.f, RED);
    //DrawModel(plane, (Vector3) { 0, 0, 0 }, 1.0f, WHITE);
    DrawPhysics(plane, sphere, box);
}

void UnloadScene() {
    UnloadShader(shader);
    UnloadTexture(texture);
    UnloadTexture(texturePlane);
    UnloadModel(plane);
    UnloadModel(sphere);
    UnloadModel(box);
    for (int i = 0; i < MODEL_COUNT; i++) {
        UnloadModel(model_list[i]);
    }
    DestroyPhysics();
}

Shader CreateShader(ShaderAttributes* attrib) {
    // Load shader and set up some uniforms
    Shader shader = LoadShader(RESOURCES_PATH"lighting.vs", RESOURCES_PATH"fog.fs");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    // Ambient light level
    attrib->ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, attrib->ambientLoc, (float[4]) { 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);

    attrib->fogDensity = 0.025f;
    attrib->fogDensityLoc = GetShaderLocation(shader, "fogDensity");
    SetShaderValue(shader, attrib->fogDensityLoc, &attrib->fogDensity, SHADER_UNIFORM_FLOAT);

    // Using just 1 point lights
    attrib->light = CreateLight(LIGHT_POINT, (Vector3) { 0, 4, 0 }, Vector3Zero(), WHITE, shader);

    return shader;
}