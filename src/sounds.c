#include "sounds.h"
#include "app_state.h"
#include "raymath.h"
#include "assets.h"


Sound sound_list[SOUNDS_COUNT];
int sounds_volume = 5;


#define DEFINE_SOUNDS_DEFAULT_VOLUME(id_enum, resource_string, volume_reference) volume_reference,
static const float default_volumes[SOUNDS_COUNT] = {
    FOR_EACH_SOUND_ENTRY(DEFINE_SOUNDS_DEFAULT_VOLUME)
};


void InitGameSounds() {
    for (int i = 0; i < SOUNDS_COUNT; i++) {
        sound_list[i] = LoadSound(snd_file_list[i]);
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

void RandomAppSoundPitch(int index, float min_pitch, float max_pitch) {
    float t = (float)GetRandomValue(0, 100) * 0.01f;
    float pitch = Lerp(min_pitch, max_pitch, t);
    SetSoundPitch(sound_list[index], pitch);
}