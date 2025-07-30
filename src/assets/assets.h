#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"


/* SOUNDS */
#define FOR_EACH_SOUND_ENTRY(SOUND_FN) \
    SOUND_FN(SND_JUMP_HUH,  RESOURCES_PATH"sounds/snd_huh_jump.wav",    0.02f)\
    SOUND_FN(SND_LANDING,   RESOURCES_PATH"sounds/snd_landing.wav",     0.01f)\
    SOUND_FN(SND_GUN_0,     RESOURCES_PATH"sounds/snd_gun_0.wav",       0.02f)\
    SOUND_FN(SND_GUN_1,     RESOURCES_PATH"sounds/snd_gun_1.wav",       0.02f)\

#define DEFINE_SND_ENUM_ID(id_enum, resource_string, volume_reference) id_enum,

enum SoundsEnum {
    FOR_EACH_SOUND_ENTRY(DEFINE_SND_ENUM_ID)
    SOUNDS_COUNT,
};

extern const char* snd_file_list[SOUNDS_COUNT];
extern Sound sound_list[SOUNDS_COUNT];


/* TEXTURES */
#define FOR_EACH_TEXTURE_ENTRY(TEXTURE_FN) \
    TEXTURE_FN(TEX_TEXEL_CHECKER, RESOURCES_PATH"images/texel_checker.png")\

#define DEFINE_TEX_ENUM_ID(id_enum, resource_string) id_enum,

enum TextureEnum {
    FOR_EACH_TEXTURE_ENTRY(DEFINE_TEX_ENUM_ID)
    TEXTURE_COUNT,
};

extern const char* tex_file_list[TEXTURE_COUNT];


/* MODELS */
#define FOR_EACH_MODEL_ENTRY(MODEL_FN) \
    MODEL_FN(MDL_ANIMATED_CUBE, RESOURCES_PATH"glb/animated_cube.glb")\
    MODEL_FN(MDL_ROBOT, RESOURCES_PATH"glb/robot.glb")\
    MODEL_FN(MDL_BONE_CHAIN, RESOURCES_PATH"glb/bone_chain.glb")\

#define DEFINE_MDL_ENUM_ID(id_enum, resource_string) id_enum,

enum ModelEnum {
    FOR_EACH_MODEL_ENTRY(DEFINE_MDL_ENUM_ID)
    MODELS_COUNT,
};

extern const char* mdl_file_list[MODELS_COUNT];


/* SHADERS */
#define FOR_EACH_SHADER_ENTRY(SHADER_FN) \
    SHADER_FN(SDR_GENERIC, RESOURCES_PATH"shaders/lighting.vs", RESOURCES_PATH"shaders/lighting.fs")\
    SHADER_FN(SDR_SKINNING, RESOURCES_PATH"shaders/skinning_light.vs", RESOURCES_PATH"shaders/skinning_light.fs")\

#define DEFINE_SDR_ENUM_ID(id_enum, resource_vs_string, resource_fs_string) id_enum,

enum ShaderEnum {
    FOR_EACH_SHADER_ENTRY(DEFINE_SDR_ENUM_ID)
    SHADER_COUNT,
};

extern const char* sdr_vs_file_list[SHADER_COUNT];
extern const char* sdr_fs_file_list[SHADER_COUNT];


#endif // ASSETS_H