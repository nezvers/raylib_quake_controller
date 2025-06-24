#ifndef MUSIC_H
#define MUSIC_H
#include "raylib.h"

extern Music music;
extern int music_volume;

void SetAppMusicVolume(int value);

void SetNewMusicFile(const char* file_name);

void SetNewMusicBuffer(unsigned char* data, size_t size);

#endif // MUSIC_H