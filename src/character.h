#ifndef CHARACTER_H
#define CHARACTER_H

#include "raylib.h"
#include "sounds.h"
#include "input.h"
#include "camera.h"
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

#define NORMALIZE_INPUT 0

typedef struct {
    Vector3 position;
    Vector3 velocity;
    Vector3 dir;
    Vector3 look_dir;
    Vector2 rotation;
    bool is_grounded;
    float head_lerp;
    Sound sound_jump;
    PhysicsCharacter phys;
}Character;


/* Generate initialized struct */
Character CreateCharacter(Vector3 position, Vector2 rotation, PhysicsCharacter phys);

/*
Quake-like movement
body: state struct
rot: horizontal rotation
side: (-1 to 1) walk direction sideways
forward: (-1 to 1) walk direction forward
*/
void UpdateCharacter(PhysicsInstance* instance, Character* body, float rot, PlayerInput* input, float delta);

void UpdateCharacterPlayer(PhysicsInstance* instance, Character* body, PlayerInput* input, CameraFPS* camera, float delta);

#endif // CHARACTER_H