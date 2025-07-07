#include "debug_draw.h"
#include "raylib.h"
#include "raymath.h"
#include "stb_ds.h"

typedef struct {
    Vector3 start;
    Vector3 end;
    Color color;
}DebugLine3D;

typedef struct {
    Vector3 position;
    float radius;
    Color color;
}DebugSphere;

typedef union {
    DebugLine3D line3d;
    DebugSphere sphere;
} DebugDrawArgs;

enum DEBUG_DRAW {
    DEBUG_DRAW_LINE_3D,
    DEBUG_DRAW_SPHERE,
    DEBUG_DRAW_COUNT,
};

typedef struct {
    unsigned long type;
    float time;
    DebugDrawArgs args;
}DebugDrawInstance;

DebugDrawInstance* instance_list = NULL;

void UpdateDebugDraw(float delta) {
    int count = arrlen(instance_list);
    for (int i = 0; i < count; i++) {
        instance_list[i].time -= delta;
        if (instance_list[i].time < 0.f) {
            arrdelswap(instance_list, i);
        }
    }
}

void AppendDebugDrawLine3D(Vector3 start, Vector3 end, Color color, float time) {
    DebugDrawInstance inst = { 0 };
    inst.type = DEBUG_DRAW_LINE_3D;
    inst.time = time;
    inst.args.line3d.start = start;
    inst.args.line3d.end = end;
    inst.args.line3d.color = color;
    arrput(instance_list, inst);
}

void AppendDebugDrawSphere(Vector3 position, float radius, Color color, float time) {
    DebugDrawInstance inst = { 0 };
    inst.type = DEBUG_DRAW_SPHERE;
    inst.time = time;
    inst.args.sphere.position = position;
    inst.args.sphere.radius = radius;
    inst.args.sphere.color = color;
    arrput(instance_list, inst);
}

void DrawDebugDrawInstances(DebugDrawInstance* inst) {
    switch (inst->type) {
        case (DEBUG_DRAW_LINE_3D): {
            DrawLine3D(inst->args.line3d.start, inst->args.line3d.end, inst->args.line3d.color);
            break;
        }
        case (DEBUG_DRAW_SPHERE): {
            DrawSphere(inst->args.sphere.position, inst->args.sphere.radius, inst->args.sphere.color);
            break;
        }
    }
}

void DrawDebugDraw() {
    int count = arrlen(instance_list);
    for (int i = 0; i < count; i++) {
        DrawDebugDrawInstances(&instance_list[i]);
    }
}