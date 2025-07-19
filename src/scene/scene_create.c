#include "scene_create.h"
#include "stb_ds.h"

int SceneAddPlaneStatic(Scene* scene, Vector3 position, int model_id, dGeomID geom_id) {
    StaticMesh static_mesh = { 0 };
    static_mesh.model = model_id;
    static_mesh.position = position;
    static_mesh.geom = geom_id;

    int i = arrlen(scene->static_list);
    arrput(scene->static_list, static_mesh);
    if (arrlen(scene->static_list) > i) {
        return i;
    }
    return -1;
}

int SceneAddCubeStatic(Scene* scene, Vector3 position, int model_id, dGeomID geom_id) {
    StaticMesh static_mesh = { 0 };
    static_mesh.model = model_id;
    static_mesh.position = position;
    static_mesh.geom = geom_id;

    int i = arrlen(scene->static_list);
    arrput(scene->static_list, static_mesh);
    if (arrlen(scene->static_list) > i) {
        return i;
    }
    return -1;
}

int SceneAddBoxDynamic(Scene* scene, int model_id, dBodyID geom_id) {
    DynamicMesh dynamic_mesh = { 0 };
    dynamic_mesh.model = model_id;
    dynamic_mesh.body = geom_id;

    int i = arrlen(scene->dynamic_list);
    arrput(scene->dynamic_list, dynamic_mesh);
    if (arrlen(scene->dynamic_list) > i) {
        return i;
    }
    return -1;
}

int SceneAddPlatform(Scene* scene, int model_id, dBodyID geom_id, PlatformMovementAnimation anim) {
    DynamicMesh dynamic_mesh = { 0 };
    dynamic_mesh.model = model_id;
    dynamic_mesh.body = geom_id;

    int anim_i = arrlen(scene->platform_animation_list);
    arrput(scene->platform_animation_list, anim);
    if (arrlen(scene->platform_animation_list) > anim_i) {
        dBodySetData(dynamic_mesh.body, (void*)&scene->platform_animation_list[anim_i]);
    }

    int i = arrlen(scene->platform_list);
    arrput(scene->platform_list, dynamic_mesh);
    if (arrlen(scene->platform_list) > i) {
        return i;
    }
    return -1;
}

int SceneAddSphereDynamic(Scene* scene, int model_id, dBodyID geom_id) {
    DynamicMesh dynamic_mesh = { 0 };
    dynamic_mesh.model = model_id;
    dynamic_mesh.body = geom_id;

    int i = arrlen(scene->dynamic_list);
    arrput(scene->dynamic_list, dynamic_mesh);
    if (arrlen(scene->dynamic_list) > i) {
        return i;
    }
    return -1;
}