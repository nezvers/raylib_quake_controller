#include "app_state.h"
#include "raylib.h"
#include "sounds.h"
#include "music.h"

static void Enter();
static void Update();
static void Exit();
AppState app_boot = {Enter, Update, Exit};

static void Enter(){
    // Set configuration flags for window creation
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_BORDERLESS_WINDOWED_MODE);
    InitWindow(screen_width, screen_height, window_title);
    SetWindowMinSize(game_width, game_height);
#ifndef PLATFORM_WEB
    SetTargetFPS(60);
#endif

    ResizeRenderSize();


    InitAudioDevice();
    SetMasterVolume(100.0);
    InitGameSounds();

    //TransitionAppState(&app_gameplay);
     TransitionAppState(&app_demo_room);
}

static void Update(){

}

static void Exit(){

}