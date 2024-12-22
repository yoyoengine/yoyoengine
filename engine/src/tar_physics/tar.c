/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdbool.h>

#include <yoyoengine/utils.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/export.h>

#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/collider.h>
#include <yoyoengine/ecs/transform.h>

#include <yoyoengine/tar_physics/rigidbody.h>

/*
    Thoughts: we technically have to respond to collisions differently based on whether or not the parent has a collider and the other has a rigidbody.

    Other no rigid: move parent
    Other rigid: solve based on mass, etc
*/

void ye_physics_tick(float dt) {
    struct ye_entity_node *node = rigidbody_list_head;
    while(node) {
        struct ye_entity *entity = node->entity;
        if(entity->rigidbody) {
            if(entity->rigidbody->active) {
                /*
                    Apply gravity
                */
                if(entity->rigidbody->velocity.x < YE_STATE.engine.tar.terminal_velocity)
                    entity->rigidbody->velocity.x += YE_STATE.engine.tar.gravity_x * dt;
                if(entity->rigidbody->velocity.y < YE_STATE.engine.tar.terminal_velocity)
                    entity->rigidbody->velocity.y += YE_STATE.engine.tar.gravity_y * dt;
                
                /*
                    Move based on velocity
                */
                entity->transform->x += entity->rigidbody->velocity.x * dt;
                entity->transform->y += entity->rigidbody->velocity.y * dt;
                entity->transform->rotation += entity->rigidbody->rotational_velocity * dt;

                /*
                    if we dont have an active collider, we dont
                    care at all about colliding with things!
                */
                if(!entity->collider || !entity->collider->active) {
                    node = node->next;
                    continue;
                }

                /*
                    Detect collisions
                */
                enum ye_collider_type collider_type = entity->collider->type;
                
                struct ye_entity_node *other = collider_list_head;
                enum ye_collider_type other_type;

                while(other) {
                    // if collider on same ent, skip
                    if(other->entity == entity) {
                        other = other->next;
                        continue;
                    }

                    // disabled, etc
                    if(!other->entity->active || !other->entity->collider || !other->entity->collider->active) {
                        other = other->next;
                        continue;
                    }

                    /*
                        Check for collisions
                    */
                    other_type = other->entity->collider->type;

                    if(collider_type == YE_COLLIDER_RECT && other_type == YE_COLLIDER_RECT) {
                        struct ye_point_rectf r1 = ye_get_position2(entity, YE_COMPONENT_COLLIDER);
                        struct ye_point_rectf r2 = ye_get_position2(other->entity, YE_COMPONENT_COLLIDER);

                        if(ye_detect_rect_rect_collision(r1, r2)) {
                            ye_resolve_rect_rect_collision();
                        }
                    }
                    // else if(collider_type == YE_COLLIDER_RECT && other_type == YE_COLLIDER_CIRCLE) {
                    //     if(ye_detect_rect_circle_collision())
                    //         ye_resolve_rect_circle_collision();
                    // }
                    // else if(collider_type == YE_COLLIDER_CIRCLE && other_type == YE_COLLIDER_CIRCLE) {
                    //     if(ye_detect_circle_circle_collision())
                    //         ye_resolve_circle_circle_collision();
                    // }
                    other = other->next;
                }
            }
        }
        node = node->next;
    }
}