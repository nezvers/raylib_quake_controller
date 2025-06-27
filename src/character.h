#ifndef CHARACTER_H
#define CHARACTER_H

#include "raylib.h"
#include "sounds.h"
#include "physics_ode.h"


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
    Body phys;
}Character;


/* Generate initialized struct */
Character CreateBody(Vector3 position);

/* Sets camera rotation and animation */
void UpdateCameraAngle(Camera* camera, Vector2* rot, float head_time, float walk_lerp, Vector2 lean);

/*
Quake-like movement
body: state struct
rot: horizontal rotation
side: (-1 to 1) walk direction sideways
forward: (-1 to 1) walk direction forward
*/
void UpdateBody(Character* body, float rot, char side, char forward, bool jumpPressed, bool crouchHold);

#endif // CHARACTER_H