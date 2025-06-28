#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include "raylib.h"

typedef struct {
    char x;
    char y;
    bool jump;
    bool crouch;
    Vector2 mouse;
}PlayerInput;

PlayerInput UpdateInput();

#endif // INPUT_H