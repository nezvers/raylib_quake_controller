#ifndef SOUNDS_H
#define SOUNDS_H

#include "raylib.h"

extern int sounds_volume;

void InitGameSounds();

void SetAppSoundVolume(int value);

void PlayAppSound(int index);

void RandomAppSoundPitch(int index, float min_pitch, float max_pitch);

#endif // SOUNDS_H