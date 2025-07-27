#include "shaders.h"
#include "stb_ds.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "assets.h"

void CreateShadersLight(int type, Vector3 position, Vector3 target, Color color, float strength, int i, ShaderAttributes** shader_attribute) {
    ShaderAttributes* attribute_list = *shader_attribute;

    for (int i = 0; i < arrlen(attribute_list); i++) {
        Light light = CreateLight(LIGHT_POINT, position, target, color, strength, i, (*shader_attribute)->shader);
        arrput(attribute_list[i].light_list, light);

        int light_count = arrlen(attribute_list[i].light_list);
        SetShaderValue(attribute_list[i].shader, attribute_list[i].lightCountLoc, &light_count, SHADER_UNIFORM_INT);
    }
}

// TODO: Separate shader functions into it's own module
ShaderAttributes CreateShader(int shader_id) {
    ShaderAttributes attrib = { 0 };
    attrib.light_list = NULL;

    // Load shader and set up some uniforms
    attrib.shader = LoadShader(sdr_vs_file_list[shader_id], sdr_fs_file_list[shader_id]);
    attrib.shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(attrib.shader, "matModel");
    attrib.shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(attrib.shader, "viewPos");

    attrib.ambientLoc = GetShaderLocation(attrib.shader, "ambient");
    SetShaderValue(attrib.shader, attrib.ambientLoc, (float[4]) { 0.001f, 0.001f, 0.001f, 1.0f }, SHADER_UNIFORM_VEC4);

    // TODO: add fog back
    /*
    attrib.fogDensity = 0.025f;
    attrib.fogDensityLoc = GetShaderLocation(attrib.shader, "fogDensity");
    SetShaderValue(attrib.shader, attrib.fogDensityLoc, &attrib.fogDensity, SHADER_UNIFORM_FLOAT);
    */

    attrib.lightCountLoc = GetShaderLocation(attrib.shader, "lightCount");
    int light_count = arrlen(attrib.light_list);
    SetShaderValue(attrib.shader, attrib.lightCountLoc, &light_count, SHADER_UNIFORM_INT);

    return attrib;
}


void UpdateShader(ShaderAttributes* attrib, Camera* camera) {
    // TODO: add fog back
    //SetShaderValue(attrib->shader, attrib->fogDensityLoc, &attrib->fogDensity, SHADER_UNIFORM_FLOAT);

    // Update the light shader with the camera view position
    SetShaderValue(attrib->shader, attrib->shader.locs[SHADER_LOC_VECTOR_VIEW], &camera->position.x, SHADER_UNIFORM_VEC3);

    int light_count = arrlen(attrib->light_list);
    SetShaderValue(attrib->shader, attrib->lightCountLoc, &light_count, SHADER_UNIFORM_INT);

    for (int i = 0; i < light_count; i++) {
        UpdateLightValues(attrib->shader, attrib->light_list[i]);
        attrib->light_list[i].dirty = 0;
    }
}
