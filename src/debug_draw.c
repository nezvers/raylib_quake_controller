#include "debug_draw.h"
#include "raylib.h"
#include "raymath.h"
#include "stb_ds.h"

typedef struct {
    Vector3 start;
    Vector3 end;
    Color color;
}DebugLine3D;

typedef union {
    DebugLine3D line3d;
} DebugDrawArgs;

enum DEBUG_DRAW {
    DEBUG_DRAW_LINE_3D,
    DEBUG_DRAW_COUNT,
};

typedef struct {
    unsigned long type;
    float time;
    DebugDrawArgs args;
}DebugDrawInstance;

DebugDrawInstance* instance_list = NULL;

void UpdateDebugDraw(float delta) {
    for (int i = 0; i < arrlen(instance_list); i++) {
        instance_list[i].time -= delta;
        if (instance_list[i].time < 0.f) {
            arrdelswap(instance_list, i);
        }
    }
}

void AppendDebugDrawLine3D(Vector3 start, Vector3 end, Color color, float time) {
    DebugDrawInstancea inst = { 0 };
    inst.type = DEBUG_DRAW_LINE_3D;
    inst.time = time;
    inst.args.line.start = start;
    inst.args.line.end = end;
    inst.args.line.color = color;
    arrput(instancel_list);
}

void DrawDebugDrawInstances(DebugDrawInstance* inst) {
    switch (inst->type) {
        case (DEBUG_DRAW_LINE_3D): {
            DrawLine3D(inst->args.line3d.start, inst->args.line3d.end, inst->args.line3d.color);
            break;
        }
    }
}

void DrawDebugDraw() {
    for (int i = 0; i < arrlen(instance_list); i++) {
        DrawDebugDrawInstances(&instance_list[i]);
    }
}