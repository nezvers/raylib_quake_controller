#include "app_state.h"
#include "raylib.h"
#include "raymath.h"
#include "sounds.h"
#include "character.h"
#include "scene.h"


static void Enter();
static void Update();
static void Exit();
static void Draw();
static void Gui();
AppState app_demo_room = { Enter, Update, Exit, Draw, Gui };


Vector2 sensitivity = { 0.001f, 0.001f };
Character player;
Camera camera;
Vector2 look_rotation = { 0 };
float bob_timer;
float walk_lerp;
float headLerp;
Vector2 lean;


bool is_mouse_disabled = false;

static void Enter() {
    SetExitKey(0);

    // Sets level physics and models
    CreateScene();

    player = CreateBody(Vector3Zero());
    camera = (Camera){ 0 };
    camera.fovy = 60.f;
    camera.projection = CAMERA_PERSPECTIVE;

    look_rotation = (Vector2){ 0 };
    bob_timer = 0.f;
    walk_lerp = 0.f;
    headLerp = STAND_HEIGHT;
    lean = Vector2Zero();

    float* pos = (float*)dBodyGetPosition(playerBody.body);
    player.position = (Vector3){pos[0], pos[1], pos[2]};
    camera.position = (Vector3){
            player.position.x,
            player.position.y + (BOTTOM_HEIGHT + headLerp),
            player.position.z,
    };
    UpdateCameraAngle(&camera, &look_rotation, bob_timer, walk_lerp, lean);

    DisableCursor();  // Limit cursor to relative movement inside the window
    is_mouse_disabled = true;
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

    if (is_mouse_disabled) {
        Vector2 mouse_delta = GetMouseDelta();
        look_rotation.x -= mouse_delta.x * sensitivity.x;
        look_rotation.y += mouse_delta.y * sensitivity.y;
    }

    char sideway = (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    char forward = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
    bool crouching = IsKeyDown(KEY_LEFT_CONTROL);
    UpdateBody(&player, look_rotation.x, sideway, forward, IsKeyPressed(KEY_SPACE), crouching);

    float delta = GetFrameTime();
    headLerp = Lerp(headLerp, (crouching ? CROUCH_HEIGHT : STAND_HEIGHT), 20.f * delta);

    UpdatePhysics(delta);

    float* pos = (float*)dBodyGetPosition(playerBody.body);
    player.position = (Vector3){ pos[0], pos[1], pos[2] };

    camera.position = (Vector3){
        player.position.x,
        player.position.y + (BOTTOM_HEIGHT + headLerp),
        player.position.z,
    };

    if (player.is_grounded && (forward != 0 || sideway != 0)) {
        bob_timer += delta * 3.f;
        walk_lerp = Lerp(walk_lerp, 1.f, 10.f * delta);
        camera.fovy = Lerp(camera.fovy, 55.f, 5.f * delta);
    }
    else {
        walk_lerp = Lerp(walk_lerp, 0.f, 10.f * delta);
        camera.fovy = Lerp(camera.fovy, 60.f, 5.f * delta);
    }

    lean.x = Lerp(lean.x, sideway * 0.02f, 10.f * delta);
    lean.y = Lerp(lean.y, forward * 0.015f, 10.f * delta);



    UpdateCameraAngle(&camera, &look_rotation, bob_timer, walk_lerp, lean);
    UpdateScene(&camera);
}

static void Draw() {
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    DrawScene();

    EndMode3D();
}

static void Gui(){
    // Draw info box
    DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(5, 5, 330, 100, BLUE);

    DrawText("Camera controls:", 15, 15, 10, BLACK);
    DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
    DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
    DrawText(TextFormat("- Velocity Len: (%06.3f)", Vector2Length((Vector2) { player.velocity.x, player.velocity.z })), 15, 60, 10, BLACK);
}

static void Exit() {
    SetExitKey(KEY_ESCAPE);
    EnableCursor();
    is_mouse_disabled = false;
    UnloadScene();
    UnloadSound(player.sound_jump);
}

