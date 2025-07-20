#include "assets.h"


#define DEFINE_SOUNDS_RESOURCE_STRING(id_enum, resource_string, volume_reference) resource_string,
const char* snd_file_list[SOUNDS_COUNT] = {
    FOR_EACH_SOUND_ENTRY(DEFINE_SOUNDS_RESOURCE_STRING)
};


#define DEFINE_MODELS_RESOURCE_STRING(id_enum, resource_string) resource_string,
const char* mdl_file_list[MODELS_COUNT] = {
    FOR_EACH_MODEL_ENTRY(DEFINE_MODELS_RESOURCE_STRING)
};