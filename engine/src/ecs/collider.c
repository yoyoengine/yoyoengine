/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <yoyoengine/utils.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/collider.h>

void ye_add_static_collider_component(struct ye_entity *entity, struct ye_rectf rect){
    struct ye_component_collider *collider = malloc(sizeof(struct ye_component_collider));
    collider->active = true;
    collider->rect = rect;
    collider->is_trigger = false;
    entity->collider = collider;
    ye_entity_list_add(&collider_list_head, entity);
}

void ye_add_trigger_collider_component(struct ye_entity *entity, struct ye_rectf rect){
    ye_add_static_collider_component(entity, rect);
    entity->collider->is_trigger = true;
}

void ye_remove_collider_component(struct ye_entity *entity){
    free(entity->collider);
    entity->collider = NULL;
    ye_entity_list_remove(&collider_list_head, entity);
}