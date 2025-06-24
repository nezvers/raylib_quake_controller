#include "sounds.h"
#include "app_state.h"


Sound sound_list[SOUNDS_COUNT];
int sounds_volume = 5;

#define DEFINE_SOUNDS_RESOURCE_STRING(id_enum, resource_string, volume_reference) resource_string,
static const char* file_list[SOUNDS_COUNT] = {
    FOR_EACH_SOUND_ENTRY(DEFINE_SOUNDS_RESOURCE_STRING)
};


#define DEFINE_SOUNDS_DEFAULT_VOLUME(id_enum, resource_string, volume_reference) volume_reference,
static const float default_volumes[SOUNDS_COUNT] = {
    FOR_EACH_SOUND_ENTRY(DEFINE_SOUNDS_DEFAULT_VOLUME)
};

void InitGameSounds() {
    for (int i = 0; i < SOUNDS_COUNT; i++) {
        sound_list[i] = LoadSound(file_list[i]);
    }

    SetAppSoundVolume(sounds_volume);
}

void SetAppSoundVolume(int value) {
    sounds_volume = value % MAX_VOLUME;
    for (int i = 0; i < SOUNDS_COUNT; i++) {
        SetSoundVolume(sound_list[i], default_volumes[i] * 0.1f * value);
    }
}

void PlayAppSound(int index) {
    PlaySound(sound_list[index]);
}