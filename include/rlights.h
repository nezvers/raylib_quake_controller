/**********************************************************************************************
*
*   raylib.lights - Some useful functions to deal with lights data
*
*   CONFIGURATION:
*
*   #define RLIGHTS_IMPLEMENTATION
*       Generates the implementation of the library into the included file.
*       If not defined, the library is in header only mode and can be included in other headers 
*       or source files without problems. But only ONE file should hold the implementation.
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2017-2024 Victor Fisac (@victorfisac) and Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef RLIGHTS_H
#define RLIGHTS_H

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define MAX_LIGHTS  4         // Max dynamic lights supported by shader

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Light data
typedef struct {   
    int type;
    bool enabled;
    Vector3 position;
    Vector3 target;
    Color color;
    float strength;
    
    // Shader locations
    int enabledLoc;
    int typeLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
    int strengthLoc;

    unsigned char dirty; // bit mask flags
} Light;

enum {
    LIGHT_DIRTY_TYPE        = 1 << 0,
    LIGHT_DIRTY_ENABLED     = 1 << 1,
    LIGHT_DIRTY_POSITION    = 1 << 2,
    LIGHT_DIRTY_TARGET      = 1 << 3,
    LIGHT_DIRTY_COLOR       = 1 << 4,
    LIGHT_DIRTY_STRENGTH    = 1 << 5,
    LIGHT_DIRTY_ALL         = ~0L,
};

// Light type
typedef enum {
    LIGHT_DIRECTIONAL = 0,
    LIGHT_POINT
} LightType;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
Light CreateLight(int type, Vector3 position, Vector3 target, Color color, float strength, int i, Shader shader);   // Create a light and get shader locations
void UpdateLightValues(Shader shader, Light light);         // Send light properties to shader

#ifdef __cplusplus
}
#endif

#endif // RLIGHTS_H


/***********************************************************************************
*
*   RLIGHTS IMPLEMENTATION
*
************************************************************************************/

#if defined(RLIGHTS_IMPLEMENTATION)

#include "raylib.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static int lightsCount = 0;    // Current amount of created lights

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Create a light and get shader locations
// TODO: optimize with ditry flags to skip unnecessary updates
Light CreateLight(int type, Vector3 position, Vector3 target, Color color, float strength, int i, Shader shader)
{
    Light light = { 0 };

    light.enabled = true;
    light.type = type;
    light.position = position;
    light.target = target;
    light.color = color;
    light.strength = strength;

    // NOTE: Lighting shader naming must be the provided ones
    light.enabledLoc = GetShaderLocation(shader, TextFormat("lights[%i].enabled", i));
    light.typeLoc = GetShaderLocation(shader, TextFormat("lights[%i].type", i));
    light.positionLoc = GetShaderLocation(shader, TextFormat("lights[%i].position", i));
    light.targetLoc = GetShaderLocation(shader, TextFormat("lights[%i].target", i));
    light.colorLoc = GetShaderLocation(shader, TextFormat("lights[%i].color", i));
    light.strengthLoc = GetShaderLocation(shader, TextFormat("lights[%i].strength", i));
    light.dirty = LIGHT_DIRTY_ALL;
    UpdateLightValues(shader, light);
        
    lightsCount++;

    return light;
}

// Send light properties to shader
// NOTE: Light shader locations should be available 
void UpdateLightValues(Shader shader, Light light)
{
    if (LIGHT_DIRTY_ENABLED & light.dirty) {
        SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
    }
    if (LIGHT_DIRTY_TYPE & light.dirty) {
        SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);
    }
    if (LIGHT_DIRTY_STRENGTH & light.dirty) {
        SetShaderValue(shader, light.strengthLoc, &light.strength, SHADER_UNIFORM_FLOAT);
    }
    if (LIGHT_DIRTY_POSITION & light.dirty) {
        // Send to shader light position values
        float position[3] = { light.position.x, light.position.y, light.position.z };
        SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);
    }
    if (LIGHT_DIRTY_TARGET & light.dirty) {
        // Send to shader light target position values
        float target[3] = { light.target.x, light.target.y, light.target.z };
        SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);
    }
    if (LIGHT_DIRTY_COLOR & light.dirty) {
        // Send to shader light color values
        float color[4] = { (float)light.color.r / (float)255, (float)light.color.g / (float)255,
                           (float)light.color.b / (float)255, (float)light.color.a / (float)255 };
        SetShaderValue(shader, light.colorLoc, color, SHADER_UNIFORM_VEC4);
    }
}

#endif // RLIGHTS_IMPLEMENTATION