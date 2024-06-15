/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <yoyoengine/utils.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/physics.h>
#include <yoyoengine/ecs/renderer.h>
#include <yoyoengine/ecs/collider.h>
#include <yoyoengine/ecs/transform.h>
#include <yoyoengine/ecs/lua_script.h>

/*
    Physics component

    Holds some information about an entity's physics

    Velocity is in pixels per second
*/
void ye_add_physics_component(struct ye_entity *entity, float velocity_x, float velocity_y){
    entity->physics = malloc(sizeof(struct ye_component_physics));
    entity->physics->active = true;
    // entity->physics->mass = mass;
    // entity->physics->drag = drag;
    entity->physics->velocity.x = velocity_x;
    entity->physics->velocity.y = velocity_y;
    entity->physics->rotational_velocity = 0; // directly modified by pointer because not often used
    // entity->physics->acceleration.x = acceleration_x;
    // entity->physics->acceleration.y = acceleration_y;

    // add this entity to the physics component list
    ye_entity_list_add(&physics_list_head, entity);

    // log that we added a physics and to what ID
    // ye_logf(debug, "Added physics component to entity %d\n", entity->id);
}

void ye_remove_physics_component(struct ye_entity *entity){
    free(entity->physics);
    entity->physics = NULL;

    // remove the entity from the physics component list
    ye_entity_list_remove(&physics_list_head, entity);

    // log that we removed a physics and to what ID
    // ye_logf(debug, "Removed physics component from entity %d\n", entity->id);
}

bool ye_rectf_collision(struct ye_rectf rect1, struct ye_rectf rect2){
    // ye_logf(debug, "rect1: %f, %f, %f, %f\n", rect1.x, rect1.y, rect1.w, rect1.h);
    if (rect1.x < rect2.x + rect2.w &&
        rect1.x + rect1.w > rect2.x &&
        rect1.y < rect2.y + rect2.h &&
        rect1.y + rect1.h > rect2.y) {
        return true;
    }
    return false;
}

/*
    Physics system

    Acts upon the list of tracked entities with physics components and updates their
    position based on their velocity and acceleration. Multiply by delta time to get
    the actual change in position. (engine_runtime_state.frame_time).

    Currently, we will always do CCD (continuous collision detection) for all entities with physics.
    In the future, a great optimization would be to only perform CCD for entities meeting certain velocity
    thresholds, or if we are below a certain framerate. We could also expose a bool for CCD on the physics
    component to allow more fine grained control, as well as an integer for specifying the number of steps.

    TODO/Considerations:
    - maybe we want to check for hitting multiple overlapping triggers?
    - trigger colliders are needed

    Physics entities need a transform component to work, we apply these forces to the transform not the component position.
*/
void ye_system_physics(){
    // current time - left for debugging
    // unsigned long start = SDL_GetTicks64();

    float delta = ye_delta_time();
    // iterate over all entities with physics
    struct ye_entity_node *current = physics_list_head;
    while (current != NULL) {
        // if the current entity is inactive, dont run physics on it
        if(!current->entity->active){
            current = current->next;
            continue;
        }

        // if we have a physics component and a transform component
        if (current->entity->physics->active && current->entity->transform != NULL) {
            // if we have velocity proceed with checks
            if(current->entity->physics->velocity.x != 0 || current->entity->physics->velocity.y != 0){
                
                // printf("started looking at entity with vx:%f vy:%f\n",current->entity->physics->velocity.x, current->entity->physics->velocity.y);

                /*
                    CASE THAT ENTITY HAS NO COLLIDER (or inactive one)
                    We just apply its velocity to its transform (also account for rotational)
                */
                if(current->entity->collider == NULL || !current->entity->collider->active){
                    current->entity->transform->x += current->entity->physics->velocity.x * delta;
                    current->entity->transform->y += current->entity->physics->velocity.y * delta;
                    
                    // do the rotation as well
                    if(current->entity->physics->rotational_velocity != 0 && current->entity->renderer != NULL){
                        // update the entity's rotation based on its rotational velocity
                        current->entity->renderer->rotation += current->entity->physics->rotational_velocity * delta;
                        if(current->entity->renderer->rotation > 360) current->entity->renderer->rotation -= 360;
                        if(current->entity->renderer->rotation < 0) current->entity->renderer->rotation += 360;
                    }

                    current = current->next;
                    continue;
                }

                /*
                    CASE THAT ENTITY HAS A COLLIDER
                    We need to check if we are colliding with any other colliders (CCD)
                */
                // get the current collider position
                struct ye_rectf old_position = ye_get_position(current->entity,YE_COMPONENT_COLLIDER);

                // // if relative, mitigate the offset bias
                // if(current->entity->collider->relative){
                //     old_position.x += current->entity->collider->rect.x;
                //     old_position.y += current->entity->collider->rect.y;
                // }

                struct ye_rectf new_position = old_position;

                // printf("original pos: %f,%f\n",old_position.x,old_position.y);

                // calculate the change in position based on the velocity
                float dx = current->entity->physics->velocity.x * delta;
                float dy = current->entity->physics->velocity.y * delta;

                // printf("calculated dx:%f dy:%f\n",dx,dy);

                // if this entity has a static collider, we need to check if we are colliding with any other static colliders
                if(current->entity->collider && !current->entity->collider->is_trigger && current->entity->collider->active){
                    for(int i = 0; i < YE_PHYSICS_SUBSTEPS; i++){
                        float substep = (i + 1) / (float)YE_PHYSICS_SUBSTEPS;  // Calculate sub-step factor

                        // Calculate the interpolated position based on the sub-step
                        new_position.x = old_position.x + substep * dx;
                        new_position.y = old_position.y + substep * dy;

                        // printf("tested substep at x:%f y:%f\n",new_position.x,new_position.y);
                        
                        // check for collisions by comparing this interpolated position with all other colliders
                        struct ye_entity_node *current_collider = collider_list_head;
                        while (current_collider != NULL) {
                            // if a collider is on a different entity and is active
                            if (current_collider->entity->id != current->entity->id && current_collider->entity->collider != NULL && current_collider->entity->collider->active) {
                                // check if we collide with it
                                if(ye_rectf_collision(new_position, ye_get_position(current_collider->entity,YE_COMPONENT_COLLIDER))){

                                    /*
                                        TODO: FIXME: PATCH

                                        This should actually reset to the smallest fitting substep we took
                                    */
                                    if(!current_collider->entity->collider->is_trigger){
                                        new_position.x = old_position.x;
                                        new_position.y = old_position.y;
                                    }
                                    
                                    break;
                                }
                            }
                            current_collider = current_collider->next;
                        }

                        // if we actually hit a collider this step, current_collider will not be NULL
                        if(current_collider != NULL && current_collider->entity->active && current_collider->entity->collider->active){
                            // ye_logf(debug, "Hit collider on entity %d\n", current_collider->entity->id);
                            // if collider we touched is static
                            if(!current_collider->entity->collider->is_trigger){
                                current->entity->physics->velocity.x = 0;
                                current->entity->physics->velocity.y = 0;

                                if(YE_STATE.engine.callbacks.collision != NULL)
                                    YE_STATE.engine.callbacks.collision(current->entity,current_collider->entity);
                                
                                ye_lua_signal_collisions(current->entity,current_collider->entity);

                                /*
                                    Saving for later as it may be relevant to the future:

                                    // calculate the distance between the entity and the collider in both directions
                                    int dx = current_collider->entity->collider->rect.x - (current->entity->transform->rect.w + current->entity->collider->rect.w) - current->entity->transform->rect.x;
                                    int dy = current_collider->entity->collider->rect.y - (current->entity->transform->rect.h + current->entity->collider->rect.h) - current->entity->transform->rect.y;

                                    // move the entity in the direction with the smaller distance, unless already overlapping
                                    if (abs(dx) < abs(dy) && dx != 0) {
                                        current->entity->transform->rect.x += dx + current->entity->collider->rect.w;
                                    } else if (abs(dy) < abs(dx) && dy != 0) {
                                        current->entity->transform->rect.y += dy;
                                    }
                                */
                                
                                break; // break out of the substep loop
                            }
                            else{
                                /*
                                    If we hit a trigger collider, broadcast the two collision entities into the
                                    event callback
                                */
                                if(YE_STATE.engine.callbacks.trigger_enter != NULL)
                                    YE_STATE.engine.callbacks.trigger_enter(current->entity,current_collider->entity);
                                
                                ye_lua_signal_trigger_enter(current->entity,current_collider->entity);
                            }
                        } // TODO: do we want to cancel rotational velocity here too?
                    }
                }
                /*
                    even if we havent changed our new position at all from the old, this line is still true.
                    We are changing whatever position this entity needs to be based on whatever substep max it hit or change it needs to be.
                */
                current->entity->transform->x = new_position.x; // bug? relativity makes it so we need to do something diff because this is offset with relative from root transform?
                current->entity->transform->y = new_position.y;
            }
            // if we have rotational velocity apply it (if we have a renderer)
            if(current->entity->physics->rotational_velocity != 0 && current->entity->renderer != NULL){
                // update the entity's rotation based on its rotational velocity
                current->entity->renderer->rotation += current->entity->physics->rotational_velocity * delta;
                if(current->entity->renderer->rotation > 360) current->entity->renderer->rotation -= 360;
                if(current->entity->renderer->rotation < 0) current->entity->renderer->rotation += 360;
            }
        }
        current = current->next;
    }
    // printf("Physics system took %lu ms\n", SDL_GetTicks64() - start);
}