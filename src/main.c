#include "raylib.h"

#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

//#define PLATFORM_WEB

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

#define MODEL_COUNT 5

/* Movement constants */
#define GRAVITY 32.f
#define MAX_SPEED 20.f
#define CROUCH_SPEED 5.f
#define JUMP_FORCE 12.f
#define MAX_ACCEL 150.f
/* Grounded drag */
#define FRICTION 0.86f
/* Increasing air drag, increases strafing speed */
#define AIR_DRAG 0.98f
/* Responsiveness for turning movement direction to looked direction */
#define CONTROL 15.f
#define CROUCH_HEIGHT 0.f
#define STAND_HEIGHT 1.f
#define BOTTOM_HEIGHT 0.5f

#define NORMALIZE_INPUT 0

typedef struct {
    Vector3 position;
    Vector3 velocity;
    Vector3 dir;
    bool is_grounded;
    Sound sound_jump;
}Body;

typedef struct {
    int ambientLoc;
    int fogDensityLoc;
    float fogDensity;
    Light light;
}ShaderAttributes;

void UpdateDrawFrame(void);     // Update and Draw one frame

/* Generate initialized struct */
Body CreateBody(Vector3 position, Vector3 velocity);

/*
Shader setup and adds one point light.
attrib - initiates values and holds values for a shader
*/
Shader CreateShader(ShaderAttributes* attrib);

/* Updates uniforms */
void UpdateShader(Shader* shader, Camera* camera, ShaderAttributes* attrib);

/* Setups 3D models */
void CreateModels(Model* model_list, Shader* shader, Texture* texture, Vector3* position_list);

/*
Sets camera rotation and animation
rot: mouse rotation
bob_time: time in animation
walk_lerp: (0 -> 1) blends walking animation
lean: (-1 -> 1)slight additional look up/down when walking
*/
void UpdateCameraAngle(Camera* camera, Vector2* rot, float bob_time, float walk_lerp, float lean);

/*
Quake-like movement
body: state struct
rot: horizontal rotation
side: (-1 to 1) walk direction sideways
forward: (-1 to 1) walk direction forward
*/
void UpdateBody(Body* body, float rot, char side, char forward, bool jumpPressed, bool crouchHold);

const int screenWidth = 1280;
const int screenHeight = 720;
Vector2 sensitivity = { 0.001f, 0.001f };

Body player;
Camera camera;
Vector2 look_rotation = { 0 };
Vector3 velocity = { 0 };
float bob_timer;
float walk_lerp;
float lean;
ShaderAttributes shader_attrib;
Shader shader;


Model model_list[MODEL_COUNT];
Vector3 position_list[MODEL_COUNT];
Texture texture;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Raylib Quake-like controller");
    InitAudioDevice();

    player = CreateBody((Vector3) { 0.f, 0.f, 4.f },(Vector3) { 0 }, false );
    camera = (Camera){ 0 };
    camera.fovy = 60.f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
    camera.position = (Vector3){
            player.position.x,
            player.position.y + (BOTTOM_HEIGHT + STAND_HEIGHT),
            player.position.z,
    };

    look_rotation = (Vector2){ 0 };
    velocity = (Vector3){ 0 };
    bob_timer = 0.f;
    walk_lerp = 0.f;
    lean = 0;
    UpdateCameraAngle(&camera, &look_rotation, bob_timer, walk_lerp, lean);

    shader = CreateShader(&shader_attrib);

    texture = LoadTexture(RESOURCES_PATH"texel_checker.png");
    CreateModels(model_list, &shader, &texture, position_list);
    

    DisableCursor();  // Limit cursor to relative movement inside the window

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(shader);
    UnloadTexture(texture);
    UnloadSound(player.sound_jump);
    CloseAudioDevice();
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdateDrawFrame(void) {
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
    if (player.is_grounded && (forward != 0 || sideway != 0)) {
        bob_timer += delta * 3.f;
        walk_lerp = Lerp(walk_lerp, 1.f, 10.f * delta);
        lean = Lerp(lean, forward * 0.015f, 10.f * delta);
        camera.fovy = Lerp(camera.fovy, 55.f, 5.f * delta);
    }
    else {
        walk_lerp = Lerp(walk_lerp, 0.f, 10.f * delta);
        lean = Lerp(lean, 0.f, 10.f * delta);
        camera.fovy = Lerp(camera.fovy, 60.f, 5.f * delta);
    }
    UpdateCameraAngle(&camera, &look_rotation, bob_timer, walk_lerp, lean);
    UpdateShader(&shader, &camera, &shader_attrib);

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    for (int i = 0; i < MODEL_COUNT; i++) {
        DrawModel(model_list[i], position_list[i], 1.0f, WHITE);
    }

    EndMode3D();

    // Draw info box
    DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(5, 5, 330, 100, BLUE);

    DrawText("Camera controls:", 15, 15, 10, BLACK);
    DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
    DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
    DrawText(TextFormat("- Velocity Len: (%06.3f)", Vector2Length((Vector2) { player.velocity.x, player.velocity.z })), 15, 60, 10, BLACK);


    EndDrawing();
    //----------------------------------------------------------------------------------
}

void UpdateBody(Body* body, float rot, char side, char forward, bool jumpPressed, bool crouchHold) {
    Vector2 input = (Vector2){ (float)side, (float)-forward };
#if defined(NORMALIZE_INPUT)
    // Slow down diagonal movement
    if (side != 0 & forward != 0) {
        input = Vector2Normalize(input);
    }
#endif

    float delta = GetFrameTime();

    if (!body->is_grounded) {
        body->velocity.y -= GRAVITY * delta;
    }
    if (body->is_grounded && jumpPressed) {
        body->velocity.y = JUMP_FORCE;
        body->is_grounded = false;
        SetSoundPitch(body->sound_jump, 1.f + (GetRandomValue(-100, 100) * 0.001));
        PlaySound(body->sound_jump);
    }

    Vector3 front_vec = (Vector3){ sin(rot), 0.f, cos(rot) };
    Vector3 right_vec = (Vector3){ cos(-rot), 0.f, sin(-rot) };

    Vector3 desired_dir = {
        input.x * right_vec.x + input.y * front_vec.x,
        0.f,
        input.x * right_vec.z + input.y * front_vec.z,
    };

    body->dir = Vector3Lerp(body->dir, desired_dir, CONTROL * delta);

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
    float speed = Vector3DotProduct(hvel, body->dir);

    /*
    Whenever the amount of acceleration to add is clamped by the maximum acceleration constant,
    a Player can make the speed faster by bringing the direction closer to horizontal velocity angle
    More info here: https://youtu.be/v3zT3Z5apaM?t=165
    */
    float max_speed = crouchHold ? CROUCH_SPEED : MAX_SPEED;
    float accel = Clamp(max_speed - speed, 0.f, MAX_ACCEL * delta);
    hvel.x += body->dir.x * accel;
    hvel.z += body->dir.z * accel;

    body->velocity.x = hvel.x;
    body->velocity.z = hvel.z;

    body->position.x += body->velocity.x * delta;
    body->position.y += body->velocity.y * delta;
    body->position.z += body->velocity.z * delta;

    /* Fancy collision system against "THE FLOOR" */
    if (body->position.y <= 0.f) {
        body->position.y = 0.f;
        body->velocity.y = 0.f;
        body->is_grounded = true; // <= enables jumping
    }
}

void UpdateCameraAngle(Camera* camera, Vector2* rot, float bob_time, float walk_lerp, float lean) {
    const Vector3 up = (Vector3){ 0.f, 1.f, 0.f };
    const Vector3 target_offset = (Vector3){ 0.f, 0.f, -1.f };

    /* Left & Right */
    Vector3 yaw = Vector3RotateByAxisAngle(target_offset, up, rot->x);

    {
        // Clamp view up
        float maxAngleUp = Vector3Angle(up, yaw);
        maxAngleUp -= 0.001f; // avoid numerical errors
        if ( -(rot->y) > maxAngleUp) { rot->y = -maxAngleUp; }

        // Clamp view down
        float maxAngleDown = Vector3Angle(Vector3Negate(up), yaw);
        maxAngleDown *= -1.0f; // downwards angle is negative
        maxAngleDown += 0.001f; // avoid numerical errors
        if ( -(rot->y) < maxAngleDown) { rot->y = -maxAngleDown; }
    }

    /* Up & Down */
    Vector3 right = Vector3Normalize(Vector3CrossProduct(yaw, up));

    // Rotate view vector around right axis
    Vector3 pitch = Vector3RotateByAxisAngle(yaw, right, -rot->y - lean);

    // Head animation
    // Rotate up direction around forward axis
    float _sin = sin(bob_time * PI);
    float _cos = cos(bob_time * PI);
    const float BOB_ROTATION = 0.01f;
    camera->up = Vector3RotateByAxisAngle(up, pitch, _sin * BOB_ROTATION);

    /* BOB */
    const float BOB_SIDE = 0.1f;
    const float BOB_UP = 0.15f;
    Vector3 bobbing = Vector3Scale(right, _sin * BOB_SIDE);
    bobbing.y = fabsf(_cos * BOB_UP);
    camera->position = Vector3Add(camera->position, Vector3Scale(bobbing, walk_lerp));

    camera->target = Vector3Add(camera->position, pitch);
}

Body CreateBody(
    Vector3 position,
    Vector3 velocity) {
    Sound jump_sound = LoadSound(RESOURCES_PATH"huh_jump.wav");
    SetSoundVolume(jump_sound, 0.2f);
    return (Body) { position, velocity, (Vector3) { 0 }, false, jump_sound };
}

void CreateModels(Model* model_list, Shader* shader, Texture* texture, Vector3* position_list) {
    // Ground
    model_list[0] = LoadModelFromMesh(GenMeshPlane(100.f, 100.f, 1.f, 1.f));
    position_list[0] = (Vector3){ 0.f, 0.f, 0.f };

    // Boxes
    model_list[1] = LoadModelFromMesh(GenMeshCube(16.f, 32.f, 16.f));
    position_list[1] = (Vector3){ 16.f, 16.f, 16.f };

    model_list[2] = LoadModelFromMesh(GenMeshCube(16.f, 32.f, 16.f));
    position_list[2] = (Vector3){ -16.f, 16.f, 16.f };

    model_list[3] = LoadModelFromMesh(GenMeshCube(16.f, 32.f, 16.f));
    position_list[3] = (Vector3){ -16.f, 16.f, -16.f };

    model_list[4] = LoadModelFromMesh(GenMeshCube(16.f, 32.f, 16.f));
    position_list[4] = (Vector3){ 16.f, 16.f, -16.f };

    // Assign texture and shader
    for (int i = 0; i < MODEL_COUNT; i++) {
        model_list[i].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = *texture;
        model_list[i].materials[0].shader = *shader;
    }
}

Shader CreateShader(ShaderAttributes* attrib) {
    // Load shader and set up some uniforms
    Shader shader = LoadShader(RESOURCES_PATH"lighting.vs", RESOURCES_PATH"fog.fs");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    // Ambient light level
    attrib->ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, attrib->ambientLoc, (float[4]) { 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);

    attrib->fogDensity = 0.025f;
    attrib->fogDensityLoc = GetShaderLocation(shader, "fogDensity");
    SetShaderValue(shader, attrib->fogDensityLoc, &attrib->fogDensity, SHADER_UNIFORM_FLOAT);

    // Using just 1 point lights
    attrib->light = CreateLight(LIGHT_POINT, (Vector3) { 0, 4, 0 }, Vector3Zero(), WHITE, shader);

    return shader;
}

void UpdateShader(Shader* shader, Camera* camera, ShaderAttributes* attrib) {
    if (IsKeyDown(KEY_UP)) {
        attrib->fogDensity += 0.001f;
        if (attrib->fogDensity > 1.0f) { attrib->fogDensity = 1.0f; }
    }

    if (IsKeyDown(KEY_DOWN)) {
        attrib->fogDensity -= 0.001f;
        if (attrib->fogDensity < 0.0f) { attrib->fogDensity = 0.0f; }
    }

    SetShaderValue(*shader, attrib->fogDensityLoc, &attrib->fogDensity, SHADER_UNIFORM_FLOAT);

    // Update the light shader with the camera view position
    SetShaderValue(*shader, shader->locs[SHADER_LOC_VECTOR_VIEW], &camera->position.x, SHADER_UNIFORM_VEC3);
}

