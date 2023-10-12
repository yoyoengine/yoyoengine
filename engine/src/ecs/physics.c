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

/*
    Physics system

    Acts upon the list of tracked entities with physics components and updates their
    position based on their velocity and acceleration. Multiply by delta time to get
    the actual change in position. (engine_runtime_state.frame_time).
*/
void ye_system_physics(){
    // Traverse tracked entities with physics components
    struct ye_entity_node *current = physics_list_head;

    float offset = (float)engine_runtime_state.frame_time / 1000.0;

    while (current != NULL) {
        if (current->entity->physics->active) {
            // log the frame time
            // ye_logf(debug, "Frame time: %f\n", engine_runtime_state.frame_time);
            // update the entity's position based on its velocity and acceleration
            current->entity->transform->rect.x += current->entity->physics->velocity.x * offset;
            current->entity->transform->rect.y += current->entity->physics->velocity.y * offset;
            current->entity->transform->bounds.x += current->entity->physics->velocity.x * offset;
            current->entity->transform->bounds.y += current->entity->physics->velocity.y * offset;
            
            // update the entity's rotation based on its rotational velocity
            current->entity->transform->rotation += current->entity->physics->rotational_velocity * offset;
            if(current->entity->transform->rotation > 360) current->entity->transform->rotation -= 360;
            if(current->entity->transform->rotation < 0) current->entity->transform->rotation += 360;

            // current->entity->physics->velocity.x += current->entity->physics->acceleration.x * engine_runtime_state.frame_time;
            // current->entity->physics->velocity.y += current->entity->physics->acceleration.y * engine_runtime_state.frame_time;
        }
        current = current->next;
    }
}