#include "models.h"
#include "scene.h"
#include "raylib.h"
#include "stb_ds.h"

int CreateModelPlane(Scene* scene, Vector2 size, Shader shadr, Texture texture) {
    Model model = LoadModelFromMesh(GenMeshPlane(size.x, size.y, 1.f, 1.f));
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    model.materials[0].shader = shadr;

    int i = arrlen(scene->model_list);
    arrput(scene->model_list, model);
    if (arrlen(scene->model_list) > i) {
        return i;
    }
    return -1;
}

int CreateModelBox(Scene* scene, Vector3 size, Shader shadr, Texture texture) {
    Model model = LoadModelFromMesh(GenMeshCube(size.x, size.y, size.z));
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    model.materials[0].shader = shadr;

    int i = arrlen(scene->model_list);
    arrput(scene->model_list, model);
    if (arrlen(scene->model_list) > i) {
        return i;
    }
    return -1;
}

int CreateModelSphere(Scene* scene, float radius, Shader shadr, Texture texture) {
    Model model = LoadModelFromMesh(GenMeshSphere(radius, 10, 10));
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    model.materials[0].shader = shadr;

    int i = arrlen(scene->model_list);
    arrput(scene->model_list, model);
    if (arrlen(scene->model_list) > i) {
        return i;
    }
    return -1;
}