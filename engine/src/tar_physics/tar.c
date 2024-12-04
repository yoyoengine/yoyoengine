/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdbool.h>

#include <yoyoengine/export.h>

#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/transform.h>

#include <yoyoengine/tar_physics/rigidbody.h>

void ye_physics_tick(float dt) {
    struct ye_entity_node *node = rigidbody_list_head;
    while(node) {
        struct ye_entity *entity = node->entity;
        if(entity->rigidbody) {
            if(entity->rigidbody->active) {
                // TODO: apply gravity

                entity->transform->x += entity->rigidbody->velocity.x * dt;
                entity->transform->y += entity->rigidbody->velocity.y * dt;
                entity->transform->rotation += entity->rigidbody->rotational_velocity * dt;

                // TODO: collision detection

                // TODO: collision solver
            }
        }
        node = node->next;
    }
}