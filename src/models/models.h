#ifndef MODELS_H
#define MODELS_H

#include "raylib.h"
#include "rlModels.h"
#include "rlModels_IO.h"

int CreateModelPlane(Model** model_list, Vector2 size, Shader shadr, Texture texture);

int CreateModelBox(Model** model_list, Vector3 size, Shader shadr, Texture texture);

int CreateModelSphere(Model** model_list, float radius, Shader shadr, Texture texture);

int CreateAnimatedMasterModel(rlmModel** master_model_list, const char* file, Shader shader, Texture texture);

int CreateAnimationSet(rlmModelAnimationSet** set_list, const char* file, rlmModel* master_model);

int CreateMasterModelInstance(rlmModel** instance_list, rlmModel* master_model);

int CreateAnimatedInstance(rlmAnimatedModelInstance** animated_instance_list, rlmModel* model_instance, rlmModelAnimationSet* set_list);

int CreateAnimatedInstanceEx(rlmModel* master_model, rlmModelAnimationSet* animation_set, rlmModel** instance_list, rlmAnimatedModelInstance** animated_instance_list);

#endif // MODELS_H