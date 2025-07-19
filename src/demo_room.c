#include "app_state.h"
#include "raylib.h"
#include "raymath.h"
#include "sounds.h"
#include "character.h"
#include "scene.h"


static void Enter();
static void Update();
static void LoadUpdate();
static void Exit();
static void Draw();
static void Gui();
AppState app_demo_room = { Enter, LoadUpdate, Exit, Draw, Gui };


//Character player;
//Camera camera;



bool is_mouse_disabled = false;

static void Enter() {
    SetExitKey(0);

    // Sets level physics and models
    // TODO: manage scene loading
    CreateScene();

    DisableCursor();  // Limit cursor to relative movement inside the window
    is_mouse_disabled = true;
}

static void LoadUpdate() {
    // Skip first update frame, stuff are loading in and make longer frame time
    app_demo_room.update = Update;
}

static void Update() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (is_mouse_disabled) {
            EnableCursor();
            is_mouse_disabled = false;
        }
        else {
            DisableCursor();
            is_mouse_disabled = true;
        }
    }

    float delta = GetFrameTime();
    // TODO: inputs as struct
    if (is_mouse_disabled) {
        UpdateScene(delta);
    }
}

static void Draw() {

    DrawScene();
}

static void Gui(){
    // Draw info box
    DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(5, 5, 330, 100, BLUE);

    DrawText("Camera controls:", 15, 15, 10, BLACK);
    DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
    DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
    DrawText(TextFormat("- Velocity Len: (%06.3f)", Vector2Length((Vector2) { demo_scene.player.velocity.x, demo_scene.player.velocity.z })), 15, 60, 10, BLACK);

    // Crosshair
    int center_x = screen_width >> 1;
    int center_y = screen_height >> 1;
    const int extent = 5;
    const Color croshair_color = PURPLE;
    DrawLine(center_x - extent, center_y - extent, center_x - 2, center_y - 2, croshair_color);
    DrawLine(center_x - extent, center_y + extent + 1, center_x - 2, center_y + 2 + 1, croshair_color);
    DrawLine(center_x + extent + 1, center_y - extent, center_x + 2 + 1, center_y - 2, croshair_color);
    DrawLine(center_x + extent + 1, center_y + extent + 1, center_x + 2 + 1, center_y + 2 + 1, croshair_color);
}

static void Exit() {
    SetExitKey(KEY_ESCAPE);
    EnableCursor();
    is_mouse_disabled = false;
    UnloadScene();
    UnloadSound(demo_scene.player.sound_jump);
}

