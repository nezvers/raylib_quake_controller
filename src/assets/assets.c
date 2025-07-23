#include "assets.h"


#define DEFINE_SOUNDS_RESOURCE_STRING(id_enum, resource_string, volume_reference) resource_string,
const char* snd_file_list[SOUNDS_COUNT] = {
    FOR_EACH_SOUND_ENTRY(DEFINE_SOUNDS_RESOURCE_STRING)
};


#define DEFINE_MODELS_RESOURCE_STRING(id_enum, resource_string) resource_string,
const char* mdl_file_list[MODELS_COUNT] = {
    FOR_EACH_MODEL_ENTRY(DEFINE_MODELS_RESOURCE_STRING)
};


#define DEFINE_SHADER_VS_RESOURCE_STRING(id_enum, resource_vs_string, resource_fs_string) resource_vs_string,
const char* sdr_vs_file_list[SHADER_COUNT] = {
    FOR_EACH_SHADER_ENTRY(DEFINE_SHADER_VS_RESOURCE_STRING)
};


#define DEFINE_SHADER_FS_RESOURCE_STRING(id_enum, resource_vs_string, resource_fs_string) resource_fs_string,
const char* sdr_fs_file_list[SHADER_COUNT] = {
    FOR_EACH_SHADER_ENTRY(DEFINE_SHADER_FS_RESOURCE_STRING)
};
