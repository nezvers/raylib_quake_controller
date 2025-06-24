#include "music.h"
#include "app_state.h"

Music music;
int music_volume;

void SetAppMusicVolume(int value) {
    music_volume = value % MAX_VOLUME;
    SetMusicVolume(music, 0.1 * music_volume);
}

void SetNewMusicFile(const char* file_name) {
    music = LoadMusicStream(file_name);
}

void SetNewMusicBuffer(unsigned char* data, size_t size) {
    music = LoadMusicStreamFromMemory(".xm", data, sizeof size);
}

