/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <yoyoengine/utils.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/collider.h>

/////////////////////////
//         v2          //
/////////////////////////

void ye_add_static_rect_collider_component(struct ye_entity *entity, float x, float y, float w, float h){
    struct ye_component_collider *collider = malloc(sizeof(struct ye_component_collider));
    collider->active = true;

    collider->is_trigger = false;
    collider->type = YE_COLLIDER_RECT;
    collider->x = x;
    collider->y = y;
    collider->width = w;
    collider->height = h;

    entity->collider = collider;
    ye_entity_list_add(&collider_list_head, entity);
}

void ye_add_trigger_rect_collider_component(struct ye_entity *entity, float x, float y, float w, float h){
    ye_add_static_rect_collider_component(entity, x, y, w, h);
    entity->collider->is_trigger = true;
}

void ye_add_static_circle_collider_component(struct ye_entity *entity, float x, float y, float radius){
    struct ye_component_collider *collider = malloc(sizeof(struct ye_component_collider));
    collider->active = true;

    collider->is_trigger = false;
    collider->type = YE_COLLIDER_CIRCLE;
    collider->x = x;
    collider->y = y;
    collider->radius = radius;

    entity->collider = collider;
    ye_entity_list_add(&collider_list_head, entity);
}

void ye_add_trigger_circle_collider_component(struct ye_entity *entity, float x, float y, float radius){
    ye_add_static_circle_collider_component(entity, x, y, radius);
    entity->collider->is_trigger = true;
}

/////////////////////////

void ye_remove_collider_component(struct ye_entity *entity){
    free(entity->collider);
    entity->collider = NULL;
    ye_entity_list_remove(&collider_list_head, entity);
}