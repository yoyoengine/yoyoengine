/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
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

#include <yoyoengine/yoyoengine.h>

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
    ye_logf(debug, "Removed physics component from entity %d\n", entity->id);
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

    TODO/Considerations:
    - maybe we want to check for hitting multiple overlapping triggers?
    - trigger colliders are needed

    Physics entities need a transform component to work, we apply these forces to the transform not the component position.
*/
void ye_system_physics(){
    float delta = ye_delta_time();
    // iterate over all entities with physics
    struct ye_entity_node *current = physics_list_head;
    while (current != NULL) {
        // if we have a physics component and a transform component
        if (current->entity->physics->active && current->entity->transform != NULL) {
            // if we have velocity proceed with checks
            if(current->entity->physics->velocity.x != 0 || current->entity->physics->velocity.y != 0){
                // get the current collider position
                struct ye_rectf new_position = ye_get_position(current->entity,YE_COMPONENT_COLLIDER);
                
                // calculate where this entity will be after this frame
                float dx = current->entity->physics->velocity.x * delta;
                float dy = current->entity->physics->velocity.y * delta;
                new_position.x += dx;
                new_position.y += dy;

                // if this entity has a static collider, we need to check if we are colliding with any other static colliders
                if(current->entity->collider && !current->entity->collider->is_trigger && current->entity->collider->active){
                    // check for collisions by comparing this entity to all entities with colliders
                    struct ye_entity_node *current_collider = collider_list_head;
                    while (current_collider != NULL) {
                        // if a collider is on a different entity and is active
                        if (current_collider->entity->id != current->entity->id && current_collider->entity->collider != NULL && current_collider->entity->collider->active) {
                            // check if we collide with it
                            if(ye_rectf_collision(new_position, ye_get_position(current_collider->entity,YE_COMPONENT_COLLIDER))){
                                break;
                            }
                        }
                        current_collider = current_collider->next;
                    }

                    // if we actually hit a collider, current_collider will not be NULL
                    if(current_collider != NULL){
                        // ye_logf(debug, "Hit collider on entity %d\n", current_collider->entity->id);
                        // if collider we touched is static
                        if(!current_collider->entity->collider->is_trigger){
                            current->entity->physics->velocity.x = 0;
                            current->entity->physics->velocity.y = 0;

                            // // START DUMB CHATGPT COLLISION HACK

                            //     // calculate the distance between the entity and the collider in both directions
                            //     int dx = current_collider->entity->collider->rect.x - (current->entity->transform->rect.w + current->entity->collider->rect.w) - current->entity->transform->rect.x;
                            //     int dy = current_collider->entity->collider->rect.y - (current->entity->transform->rect.h + current->entity->collider->rect.h) - current->entity->transform->rect.y;

                            //     // move the entity in the direction with the smaller distance, unless already overlapping
                            //     if (abs(dx) < abs(dy) && dx != 0) {
                            //         current->entity->transform->rect.x += dx + current->entity->collider->rect.w;
                            //     } else if (abs(dy) < abs(dx) && dy != 0) {
                            //         current->entity->transform->rect.y += dy;
                            //     }

                            // // END DUMB CHATGPT COLLISION HACK

                        }
                    } // TODO: do we want to cancel rotational velocity here too?
                    else{
                        current->entity->transform->x += dx;
                        current->entity->transform->y += dy;

                        // TODO: FIXME THIS IS A TEMPORARY HACK. ALL COMPS SHOULD HAVE A POSITION THAT IS OPTIONAL AND IF NOT IS RELATIVE TO PARENT
                        // move the entities collider forwards too, add the difference in x increase and y increase to the colliders rect
                        // current->entity->collider->rect.x += dx;
                        // current->entity->collider->rect.y += dy; // TODO: REMOVEME. leaving here now for context
                    }
                } 
                else { // if there is no static collider on this entity, nothing else is able to stop it, so we can just move it to its new pos
                    current->entity->transform->x += dx;
                    current->entity->transform->y += dy;
                }
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
}