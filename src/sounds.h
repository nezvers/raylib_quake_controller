#ifndef SOUNDS_H
#define SOUNDS_H

#include "raylib.h"


#define FOR_EACH_SOUND_ENTRY(SOUND_FN) \
    SOUND_FN(SND_JUMP_HUH, RESOURCES_PATH"huh_jump.wav", 0.02f)\
    SOUND_FN(SND_GUN_0, RESOURCES_PATH"gun_0.wav", 0.02f)\
    SOUND_FN(SND_GUN_1, RESOURCES_PATH"gun_1.wav", 0.02f)\

#define DEFINE_ENUM_ID(id_enum, resource_string, volume_reference) id_enum,

enum SoundTypes {
    FOR_EACH_SOUND_ENTRY(DEFINE_ENUM_ID)
    SOUNDS_COUNT,
};

extern int sounds_volume;
extern Sound sound_list[SOUNDS_COUNT];

void InitGameSounds();

void SetAppSoundVolume(int value);

void PlayAppSound(int index);

void RandomAppSoundPitch(int index, float min_pitch, float max_pitch);

#endif // SOUNDS_H