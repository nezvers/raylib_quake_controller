#include "models.h"
#include "scene.h"
#include "raylib.h"
#include "stb_ds.h"


int CreateModelPlane(Model** model_list, Vector2 size, Shader shadr, Texture texture) {
    Model model = LoadModelFromMesh(GenMeshPlane(size.x, size.y, 1.f, 1.f));
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    model.materials[0].shader = shadr;

    int i = arrlen(*model_list);
    arrput(*model_list, model);
    if (arrlen(*model_list) > i) {
        return i;
    }
    return -1;
}

int CreateModelBox(Model** model_list, Vector3 size, Shader shadr, Texture texture) {
    Model model = LoadModelFromMesh(GenMeshCube(size.x, size.y, size.z));
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    model.materials[0].shader = shadr;

    int i = arrlen(*model_list);
    arrput(*model_list, model);
    if (arrlen(*model_list) > i) {
        return i;
    }
    return -1;
}

int CreateModelSphere(Model** model_list, float radius, Shader shadr, Texture texture) {
    Model model = LoadModelFromMesh(GenMeshSphere(radius, 10, 10));
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    model.materials[0].shader = shadr;

    int i = arrlen(*model_list);
    arrput(*model_list, model);
    if (arrlen(*model_list) > i) {
        return i;
    }
    return -1;
}

int CreateAnimatedMasterModel(rlmModel** master_model_list, const char *file, Shader shader, Texture texture) {
    Model original_model = LoadModel(file);
    rlmModel master_model = rlmLoadFromModel(original_model);
    for (int i = 0; i < master_model.groupCount; i++) {
        rlmSetMaterialDefShader(&master_model.groups[i].material, shader);
        rlmSetMaterialChannelTexture(&master_model.groups[i].material.baseChannel, texture);
    }

    int i = arrlen(*master_model_list);
    arrput(*master_model_list, master_model);
    if (arrlen(*master_model_list) > i) {
        return i;
    }
    return -1;
}

int CreateAnimationSet(rlmModelAnimationSet** set_list, const char *file, rlmModel *master_model) {
    if (master_model->skeleton == NULL) {
        return -1;
    }
    rlmModelAnimationSet animation_set = (rlmModelAnimationSet){ 0 };
    ModelAnimation* animations = LoadModelAnimations(file, &animation_set.sequenceCount);
    animation_set.sequences = rlmLoadModelAnimations(master_model->skeleton, animations, animation_set.sequenceCount);

    int i = arrlen(*set_list);
    arrput(*set_list, animation_set);
    if (arrlen(*set_list) > i) {
        return i;
    }
    return -1;
}

int CreateMasterModelInstance(rlmModel** instance_list, rlmModel* master_model) {
    rlmModel instance_model = rlmCloneModel(*master_model);
    //instance_model.groups[1].material.baseChannel.color = BLUE; // instance coloring

    int i = arrlen(*instance_list);
    arrput(*instance_list, instance_model);
    if (arrlen(*instance_list) > i) {
        return i;
    }
    return -1;
}

int CreateAnimatedInstance(rlmAnimatedModelInstance** animated_instance_list, rlmModel* model_instance, rlmModelAnimationSet* set_list) {
    rlmAnimatedModelInstance animated_instance = (rlmAnimatedModelInstance){ 0 };
    animated_instance.model = model_instance;
    //animated_instance.transform = rlmPQSTranslation(-6.f, 3, 0); // Test position
    //animated_instance.transform.rotation = QuaternionFromAxisAngle((Vector3) { 0, 1, 0 }, 180 * DEG2RAD); // Test rotation

    if (model_instance->skeleton != NULL) {
        animated_instance.sequences = set_list;
        animated_instance.interpolate = true;
        animated_instance.currentPose = rlmLoadPoseFromModel(*model_instance);

        rlmSetAnimationInstanceSequence(&animated_instance, 0); // ?? sets animation
    }

    int i = arrlen(*animated_instance_list);
    arrput(*animated_instance_list, animated_instance);
    if (arrlen(*animated_instance_list) > i) {
        return i;
    }
    return -1;
}

int CreateAnimatedInstanceEx(rlmModel* master_model, rlmModelAnimationSet* animation_set, rlmModel** instance_list, rlmAnimatedModelInstance** animated_instance_list) {
    int master_instance_id = CreateMasterModelInstance(instance_list, master_model);
    if (master_instance_id == -1) { return -1; }

    rlmModel* inst_list = *instance_list;
    return CreateAnimatedInstance(animated_instance_list, &inst_list[master_instance_id], animation_set);
}
