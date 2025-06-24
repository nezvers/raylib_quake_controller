#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "stddef.h"
#include "stdbool.h"

typedef struct{
    void (*enter)();
    void (*update)();
    void (*exit)();
    void (*draw)();
    void (*gui)();
} AppState;


extern char* window_title;
extern int screen_width;
extern int screen_height;
extern Color clear_color;
extern bool is_fullscreen;
#define MAX_VOLUME 11
extern int sounds_volume;
extern int music_volume;

extern int game_width;
extern int game_height;
extern RenderTexture2D target;
extern Rectangle source_rect;
extern Rectangle dest_rect;

enum ViewportType {KEEP_ASPECT, KEEP_HEIGHT, KEEP_WIDTH};
extern int viewport_type;

extern AppState* current_app_state;
extern AppState app_boot; // <= Initial state
extern AppState app_demo_room;

void TransitionAppState(AppState* value);
void ResizeRenderSize();
void DrawRenderTexture();

#endif // GAME_H