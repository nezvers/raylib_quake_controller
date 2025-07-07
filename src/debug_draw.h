#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include "raylib.h"

void UpdateDebugDraw(float delta);

void DrawDebugDraw();

void AppendDebugDrawLine3D(Vector3 start, Vector3 end, Color color, float time);

void AppendDebugDrawSphere(Vector3 position, float radius, Color color, float time);

#endif