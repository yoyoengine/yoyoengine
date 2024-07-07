/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdbool.h>

#include <yoyoengine/engine.h>
#include <yoyoengine/ecs/camera.h>

void ye_set_camera(struct ye_entity *entity){
    YE_STATE.engine.target_camera = entity;
}

void ye_add_camera_component(struct ye_entity *entity, int z, struct ye_rectf view_field){
    entity->camera = malloc(sizeof(struct ye_component_camera));
    entity->camera->active = true;
    entity->camera->view_field = view_field; // x and y are used as an offset from the transform on the camera
    entity->camera->z = z;
    entity->camera->relative = true;

    // log that we added a transform and to what ID
    // ye_logf(debug, "Added camera to entity %d\n", entity->id);

    // add this entity to the camera component list
    ye_entity_list_add(&camera_list_head, entity);
}

void ye_remove_camera_component(struct ye_entity *entity){
    free(entity->camera);
    entity->camera = NULL;

    // remove the entity from the camera component list
    ye_entity_list_remove(&camera_list_head, entity);
}