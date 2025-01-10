/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stddef.h>
#include <stdlib.h>

#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/transform.h>

void ye_add_transform_component(struct ye_entity *entity, int x,int y){
    entity->transform = malloc(sizeof(struct ye_component_transform));
    // entity->transform->active = true; transform doesnt need active
    entity->transform->x = x;
    entity->transform->y = y;

    // add this entity to the transform component list
    ye_entity_list_add(&transform_list_head, entity);

    // log that we added a transform and to what ID
    // ye_logf(debug, "Added transform to entity %d\n", entity->id);
}

void ye_remove_transform_component(struct ye_entity *entity){
    free(entity->transform);
    entity->transform = NULL;

    // remove the entity from the transform component list
    ye_entity_list_remove(&transform_list_head, entity);
}