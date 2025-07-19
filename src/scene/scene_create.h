#ifndef SCENE_CREATE_H
#define SCENE_CREATE_H

#include "scene.h"

int SceneAddPlaneStatic(Scene* scene, Vector3 position, int model_id, dGeomID geom_id);

int SceneAddCubeStatic(Scene* scene, Vector3 position, int model_id, dGeomID geom_id);

int SceneAddBoxDynamic(Scene* scene, int model_id, dBodyID geom_id);

int SceneAddPlatform(Scene* scene, int model_id, dBodyID geom_id, PlatformMovementAnimation anim);

int SceneAddSphereDynamic(Scene* scene, int model_id, dBodyID geom_id);

#endif // SCENE_CREATE_H