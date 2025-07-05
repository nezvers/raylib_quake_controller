#include "input.h"
#include "raylib.h"
#include "raymath.h"



Vector2 sensitivity = { 0.001f, 0.001f };

PlayerInput UpdateInput() {
    PlayerInput player_input = { 0 };
    player_input.mouse = Vector2Multiply(GetMouseDelta(), sensitivity);
    player_input.x = (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    player_input.y = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
    player_input.jump = IsKeyPressed(KEY_SPACE);
    player_input.crouch = IsKeyDown(KEY_LEFT_CONTROL);
    player_input.shoot = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    return player_input;
}