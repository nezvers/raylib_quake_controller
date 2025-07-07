#include "app_state.h"
#define VIEWPORT_RECT_IMPLEMENTATION
#include "viewport_rect.h"

int screen_width = 1280;
int screen_height = 720;
char* window_title = "Raylib FPS Controller";
Color clear_color = { 255, 255, 255, 255 };
AppState* current_app_state = &app_boot;

int game_width = 640;
int game_height = 360;
RenderTexture2D target;

// The target's height is flipped (in the source Rectangle), due to OpenGL reasons
Rectangle source_rect;
Rectangle dest_rect;

int viewport_type = KEEP_HEIGHT;

void TransitionAppState(AppState* value){
    if (current_app_state != NULL && current_app_state->exit != NULL){
        current_app_state->exit();
    }
    current_app_state = value;
    if (current_app_state != NULL && current_app_state->enter != NULL){
        current_app_state->enter();
    }
}

void ResizeRenderSize(){
    screen_width = GetScreenWidth();
    screen_height = GetScreenHeight();

    switch(viewport_type){
        case KEEP_ASPECT:{
            KeepAspectCentered(screen_width, screen_height, game_width, game_height, &source_rect, &dest_rect);
            break;
        }
        case KEEP_HEIGHT:{
            KeepHeightCentered(screen_width, screen_height, game_width, game_height, &source_rect, &dest_rect);
            break;
        }
        case KEEP_WIDTH:{
            KeepWidthCentered(screen_width, screen_height, game_width, game_height, &source_rect, &dest_rect);
            break;
        }
    }
    UnloadRenderTexture(target);
    target = LoadRenderTexture(source_rect.width, -source_rect.height);
}

void DrawRenderTexture(){
        ClearBackground(BLACK);
        
        DrawTexturePro(target.texture, source_rect, dest_rect, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
}
