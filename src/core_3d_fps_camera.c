/*******************************************************************************************
*
*   raylib [core] example - 3d camera first person
*
*   Example complexity rating: [★★☆☆] 2/4
*
*   Example originally created with raylib 1.3, last time updated with raylib 1.3
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2015-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"

#define MAX_COLUMNS 20
#define GROUND_SIZE 200
#define GROUND_SIZE2 100
#define TAU (PI*2)

/* Movement constants */
#define GRAVITY 32.f
#define MAX_SPEED 20.f
#define CROUCH_SPEED 5.f
#define JUMP_FORCE 12.f
#define MAX_ACCEL 150.f
#define FRICTION 0.86f
/* Increasing air drag, increases strafing speed */
#define AIR_DRAG 0.98f
#define CONTROL 15.f
#define CROUCH_HEIGHT 0.f
#define STAND_HEIGHT 1.f
#define BOTTOM_HEIGHT 0.5f

#define NORMALIZE_INPUT 0



typedef struct {
    Vector3 position;
    Vector3 velocity;
    bool is_grounded;
}Body;

Body CreateBody(
    Vector3 position,
    Vector3 velocity) {
    return (Body) { position, velocity, false };
}

float GetWindStrength(Vector3 velocity) {
    float magnitude = Vector3Length(velocity);
    float squared = magnitude * magnitude;
    return Remap(squared, 0.f, MAX_SPEED * 250.f, 0.f, 1.f);
}

void SetCameraAngle(Camera* camera, Vector2* rot, float bob_time, float walk_lerp, float lean) {
    // Rotation axis
    Vector3 up = (Vector3){0.f, 1.f, 0.f};

    /* Left & Right */
    Vector3 yaw = Vector3RotateByAxisAngle((Vector3) { 0.f, 0.f, -1.f }, up, rot->x);

    {
        // Clamp view up
        float maxAngleUp = Vector3Angle(up, yaw);
        maxAngleUp -= 0.001f; // avoid numerical errors
        if (-rot->y > maxAngleUp) { rot->y = -maxAngleUp; }

        // Clamp view down
        float maxAngleDown = Vector3Angle(Vector3Negate(up), yaw);
        maxAngleDown *= -1.0f; // downwards angle is negative
        maxAngleDown += 0.001f; // avoid numerical errors
        if (-rot->y < maxAngleDown) { rot->y = -maxAngleDown; }
    }

    Vector3 right = Vector3Normalize(Vector3CrossProduct(yaw, up));
    // Rotate view vector around right axis
    Vector3 pitch = Vector3RotateByAxisAngle(yaw, right, -rot->y - lean);

    // Rotate up direction around forward axis
    float _sin = sin(bob_time * PI);
    camera->up = Vector3RotateByAxisAngle(up, pitch, _sin * -0.01f);

    /* BOB */
    Vector3 bobbing = Vector3Scale(right, _sin * 0.1);
    bobbing.y = fabsf(cos(bob_time * PI)) * 0.15;
    camera->position = Vector3Add(camera->position, Vector3Scale(bobbing, walk_lerp));

    camera->target = Vector3Add(camera->position, pitch);
}


void UpdateBody(Body *body, float rot, char side, char forward, bool jumpPressed, bool crouchHold) {
    Vector2 input = (Vector2){ (float)side, (float)-forward };
#if defined(NORMALIZE_INPUT)
    if (side != 0 & forward != 0) {
        input = Vector2Normalize(input);
    }
#endif

    Vector3 front_vec = (Vector3){ sin(rot), 0.f, cos(rot) };
    Vector3 right_vec = (Vector3){ cos(-rot), 0.f, sin(-rot) };

    Vector3 dir = {
        input.x * right_vec.x + input.y * front_vec.x,
        0.f,
        input.x * right_vec.z + input.y * front_vec.z,
    };

    float delta = GetFrameTime();

    if (!body->is_grounded) {
        body->velocity.y -= GRAVITY * delta;
    }
    if (body->is_grounded && jumpPressed) {
        body->velocity.y = JUMP_FORCE;
        body->is_grounded = false; // <= Lost ground
    }

    float decel = body->is_grounded ? FRICTION : AIR_DRAG;
    Vector3 hvel = (Vector3){ 
        body->velocity.x * decel, 
        0.f, 
        body->velocity.z * decel
    };
    
    float hvel_length = Vector3Length(hvel); // a.k.a. magnitude
    if (hvel_length < MAX_SPEED * 0.01f) {
        hvel = (Vector3){ 0 };
    }

    /* This is what creates strafing */
    float speed = Vector3DotProduct(hvel, dir);

    /*
    Whenever the amount of acceleration to add is clamped by the maximum acceleration constant,
    a Player can make the speed faster by bringing the direction closer to horizontal velocity angle
    More info here: https://youtu.be/v3zT3Z5apaM?t=165
    */
    float max_speed = crouchHold ? CROUCH_SPEED : MAX_SPEED;
    float accel = Clamp(max_speed - speed, 0.f, MAX_ACCEL * delta);
    hvel.x += dir.x * accel;
    hvel.z += dir.z * accel;

    body->velocity.x = hvel.x;
    body->velocity.z = hvel.z;

    body->position.x += body->velocity.x * delta;
    body->position.y += body->velocity.y * delta;
    body->position.z += body->velocity.z * delta;

    /* Fancy collision system against "floor" */
    if (body->position.y <= 0.f) {
        body->position.y = 0.f;
        body->velocity.y = 0.f;
        body->is_grounded = true; // <= enable jumping
    }
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.fovy = 60.f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
    Body player = CreateBody((Vector3) { 0.f, 0.f, 4.f },(Vector3) { 0 }, false );
    camera.position = (Vector3){
            player.position.x,
            player.position.y + (BOTTOM_HEIGHT + STAND_HEIGHT),
            player.position.z,
    };

    Vector2 look_rotation = { 0 };
    Vector3 velocity = { 0 };
    Vector2 sensitivity = { 0.001f, 0.001f };
    float bob_timer = 0.f;
    float walk_lerp = 0.f;
    float lean = 0;

    SetCameraAngle(&camera, &look_rotation, bob_timer, walk_lerp, lean);


    // Generates some random columns
    float heights[MAX_COLUMNS] = { 0 };
    Vector3 positions[MAX_COLUMNS] = { 0 };
    Color colors[MAX_COLUMNS] = { 0 };

    for (int i = 0; i < MAX_COLUMNS; i++){
        heights[i] = (float)GetRandomValue(1, 12);
        positions[i] = (Vector3){ (float)GetRandomValue(-GROUND_SIZE2 + 1, GROUND_SIZE2 -1), heights[i] / 2.0f, (float)GetRandomValue(-GROUND_SIZE2 + 1, GROUND_SIZE2 - 1) };
        colors[i] = (Color){ GetRandomValue(20, 255), GetRandomValue(10, 55), 30, 255 };
    }

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        Vector2 mouse_delta = GetMouseDelta();
        look_rotation.x -= mouse_delta.x * sensitivity.x;
        look_rotation.y += mouse_delta.y * sensitivity.y;

        char sideway = (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
        char forward = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
        bool crouching = IsKeyDown(KEY_LEFT_CONTROL);
        UpdateBody(&player, look_rotation.x, sideway, forward, IsKeyPressed(KEY_SPACE), crouching);

        
        camera.position = (Vector3){
            player.position.x,
            player.position.y + (BOTTOM_HEIGHT + (crouching ? CROUCH_HEIGHT : STAND_HEIGHT)),
            player.position.z,
        };

        float delta = GetFrameTime();
        if (forward != 0 || sideway != 0) {
            bob_timer += delta * 3.f;
            walk_lerp = Lerp(walk_lerp, 1.f, 10.f * delta);
            lean = Lerp(lean, forward * 0.015f, 10.f * delta);
        }
        else {
            walk_lerp = Lerp(walk_lerp, 0.f, 10.f * delta);
            lean = Lerp(lean, 0.f, 10.f * delta);
        }
        SetCameraAngle(&camera, &look_rotation, bob_timer, walk_lerp, lean);


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawPlane((Vector3) { 0.0f, 0.0f, 0.0f }, (Vector2) { GROUND_SIZE, GROUND_SIZE }, LIGHTGRAY); // Draw ground
        DrawCube((Vector3) { -GROUND_SIZE2, 2.5f, 0.0f }, 1.0f, 5.0f, GROUND_SIZE, BLUE);     // Draw a blue wall
        DrawCube((Vector3) { GROUND_SIZE2, 2.5f, 0.0f }, 1.0f, 5.0f, GROUND_SIZE, LIME);      // Draw a green wall
        DrawCube((Vector3) { 0.0f, 2.5f, GROUND_SIZE2 }, GROUND_SIZE, 5.0f, 1.0f, GOLD);      // Draw a yellow wall

        // Draw some cubes around
        for (int i = 0; i < MAX_COLUMNS; i++)
        {
            DrawCube(positions[i], 2.0f, heights[i], 2.0f, colors[i]);
            DrawCubeWires(positions[i], 2.f, heights[i], 2.f, MAROON);
        }

        EndMode3D();

        // Draw info boxes
        DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(5, 5, 330, 100, BLUE);

        DrawText("Camera controls:", 15, 15, 10, BLACK);
        DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
        DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
        DrawText("- Camera mode keys: 1, 2, 3, 4", 15, 60, 10, BLACK);
        DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15, 75, 10, BLACK);
        DrawText("- Camera projection key: P", 15, 90, 10, BLACK);

        DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(600, 5, 195, 100, BLUE);

        DrawText(TextFormat("- Velocity Len: (%06.3f)", Vector2Length((Vector2) { player.velocity.x, player.velocity.z})), 610, 45, 10, BLACK);
        DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", player.position.x, player.position.y, player.position.z), 610, 60, 10, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
