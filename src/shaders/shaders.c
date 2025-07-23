#include "shaders.h"
#include "stb_ds.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

// TODO: Separate shader functions into it's own module
ShaderAttributes CreateShader() {
    ShaderAttributes attrib = { 0 };
    attrib.light_list = NULL;

    // Load shader and set up some uniforms
    attrib.shader = LoadShader(RESOURCES_PATH"shaders/lighting.vs", RESOURCES_PATH"shaders/lighting.fs");
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
    for (int i = 0; i < MAX_LIGHTS; i++) {
        Light light_inst = CreateLight(LIGHT_POINT, light_pos, light_target, light_color, light_strength, i, attrib.shader);
        arrput(attrib.light_list, light_inst);
        light_inst.enabled = false;
        light_inst.dirty = LIGHT_DIRTY_ENABLED;
    }

    // player
    attrib.light_list[0].enabled = true;
    attrib.light_list[0].strength = 0.5f;
    attrib.light_list[0].dirty = LIGHT_DIRTY_STRENGTH;

    // center
    attrib.light_list[1].enabled = true;

    // Initialize light count
    attrib.lightCountLoc = GetShaderLocation(attrib.shader, "lightCount");
    int light_count = arrlen(attrib.light_list);
    SetShaderValue(attrib.shader, attrib.lightCountLoc, &light_count, SHADER_UNIFORM_INT);

    return attrib;
}


void UpdateShader(ShaderAttributes* attrib, Camera* camera) {
    SetShaderValue(attrib->shader, attrib->fogDensityLoc, &attrib->fogDensity, SHADER_UNIFORM_FLOAT);

    // Update the light shader with the camera view position
    SetShaderValue(attrib->shader, attrib->shader.locs[SHADER_LOC_VECTOR_VIEW], &camera->position.x, SHADER_UNIFORM_VEC3);

    int light_count = arrlen(attrib->light_list);
    SetShaderValue(attrib->shader, attrib->lightCountLoc, &light_count, SHADER_UNIFORM_INT);

    for (int i = 0; i < light_count; i++) {
        UpdateLightValues(attrib->shader, attrib->light_list[i]);
        attrib->light_list[i].dirty = 0;
    }
}
