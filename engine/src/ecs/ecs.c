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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// entity id counter (used to assign unique ids to entities)
int eid = 0;

//////////////////////// LINKED LIST //////////////////////////

// Create a linked list for entities of a specific type (e.g., renderable entities)
struct ye_entity_node *ye_entity_list_create() {
    return NULL; // Initialize an empty list
}

// Add an entity to the linked list
void ye_entity_list_add(struct ye_entity_node **list, struct ye_entity *entity) {
    struct ye_entity_node *newNode = malloc(sizeof(struct ye_entity_node));
    newNode->entity = entity;
    newNode->next = *list;
    *list = newNode;
}

/*
    Adds to the renderer list sorted by z value, lowest at head
*/
void ye_entity_list_add_sorted_z(struct ye_entity_node **list, struct ye_entity *entity){
    struct ye_entity_node *newNode = malloc(sizeof(struct ye_entity_node));
    newNode->entity = entity;
    newNode->next = NULL;

    // if the list is empty, add to head
    if(*list == NULL){
        *list = newNode;
        return;
    }

    // if the new node is less than the head, add to head
    if(newNode->entity->transform->z < (*list)->entity->transform->z){
        newNode->next = *list;
        *list = newNode;
        return;
    }

    // otherwise, traverse the list and find the correct spot
    struct ye_entity_node *current = *list;
    while(current->next != NULL){
        if(newNode->entity->transform->z < current->next->entity->transform->z){
            newNode->next = current->next;
            current->next = newNode;
            return;
        }
        current = current->next;
    }

    // if we reach the end of the list, add to the end
    current->next = newNode;
}

/*
    Remove an entity from the linked list

    NOTE: THIS DOES NOT FREE THE ACTUAL ENTITY ITSELF
    
    This is temporarialy ok because we dont delete any at runtime but TODO
    we need a method of this that does call the destructor for the entity
    probably ye_entity_list_remove_free?
    Not sure how to differentiate when this needs called though... just for actual list
    of entities?
*/ 
void ye_entity_list_remove(struct ye_entity_node **list, struct ye_entity *entity) {
    struct ye_entity_node *current = *list;
    struct ye_entity_node *prev = NULL;

    while (current != NULL) {
        if (current->entity == entity) {
            if (prev == NULL) {
                *list = current->next; // Update the head of the list
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Clean up the entire linked list
void ye_entity_list_destroy(struct ye_entity_node **list) {
    while (*list != NULL) {
        struct ye_entity_node *temp = *list;
        *list = (*list)->next;
        ye_destroy_entity(temp->entity);
        free(temp);
    }
    *list = NULL;
}

///////////////////////////////////////////////////////////////

struct ye_entity_node *entity_list_head;
struct ye_entity_node *transform_list_head;
struct ye_entity_node *renderer_list_head;
struct ye_entity_node *camera_list_head;
struct ye_entity_node *physics_list_head;
struct ye_entity_node *tag_list_head;

// struct ye_entity_node *script_list_head;
// struct ye_entity_node *interactible_list_head;

/*
    Get the entity list head pointer
    (For use outside this file, ex: the editor)
*/
struct ye_entity_node * ye_get_entity_list_head(){
    return entity_list_head;
}

/*
    Create a new entity and return a pointer to it
*/
struct ye_entity * ye_create_entity(){
    struct ye_entity *entity = malloc(sizeof(struct ye_entity));
    entity->id = eid++; // assign unique id to entity
    entity->active = true;

    //name the entity "entity id"
    char *name = malloc(sizeof(char) * 100);
    snprintf(name, 100, "entity %d", entity->id);
    entity->name = name;

    // assign all copmponents to null
    entity->transform = NULL;
    entity->renderer = NULL;
    entity->camera = NULL;
    entity->script = NULL;
    entity->interactible = NULL;
    entity->physics = NULL;
    entity->collider = NULL;
    entity->tag = NULL;

    // add the entity to the entity list
    ye_entity_list_add(&entity_list_head, entity);
    // ye_logf(debug, "Created and added an entity\n");

    engine_runtime_state.entity_count++;

    return entity;
}

/*
    Create a new entity and return a pointer to it (named)

    we must allocate space for the name and copy it
*/
struct ye_entity * ye_create_entity_named(char *name){
    struct ye_entity *entity = malloc(sizeof(struct ye_entity));
    entity->id = eid++; // assign unique id to entity
    entity->active = true;

    // name the entity by its passed name
    entity->name = malloc(strlen(name) + 1);
    strcpy(entity->name, name);
    
    // assign all copmponents to null
    entity->transform = NULL;
    entity->renderer = NULL;
    entity->camera = NULL;
    entity->script = NULL;
    entity->interactible = NULL;
    entity->physics = NULL;
    entity->collider = NULL;
    entity->tag = NULL;

    // add the entity to the entity list
    ye_entity_list_add(&entity_list_head, entity);
    // ye_logf(debug, "Created and added an entity\n");

    engine_runtime_state.entity_count++;

    return entity;
}

void ye_rename_entity(struct ye_entity *entity, char *new_name){
    // free the old name
    free(entity->name);

    // name the entity by its passed name
    entity->name = malloc(strlen(new_name) + 1);
    strcpy(entity->name, new_name);
}

struct ye_entity * ye_duplicate_entity(struct ye_entity *entity){
    // create a new entity named "(old name) (copy)"
    struct ye_entity *new_entity = ye_create_entity_named(strcat(strcat(entity->name, " "), "copy"));

    // copy all components
    if(entity->transform != NULL) ye_add_transform_component(new_entity, entity->transform->bounds, entity->transform->z, entity->transform->alignment);
    if(entity->renderer != NULL){
        if(entity->renderer->type == YE_RENDERER_TYPE_IMAGE){
            ye_temp_add_image_renderer_component(new_entity, entity->renderer->renderer_impl.image->src);
        }
        else if(entity->renderer->type == YE_RENDERER_TYPE_TEXT){
            ye_temp_add_text_renderer_component(new_entity, entity->renderer->renderer_impl.text->text, entity->renderer->renderer_impl.text->font, entity->renderer->renderer_impl.text->color);
        }
        else if(entity->renderer->type == YE_RENDERER_TYPE_ANIMATION){
            ye_temp_add_animation_renderer_component(new_entity, entity->renderer->renderer_impl.animation->animation_path, entity->renderer->renderer_impl.animation->image_format, entity->renderer->renderer_impl.animation->frame_count, entity->renderer->renderer_impl.animation->frame_delay, entity->renderer->renderer_impl.animation->loops);
        }
    }
    if(entity->camera != NULL) ye_add_camera_component(new_entity, entity->camera->view_field);
    // if(entity->script != NULL) ye_add_script_component(new_entity, entity->script->script_path);
    // if(entity->interactible != NULL) ye_add_interactible_component(new_entity, entity->interactible->interactible_type);
    if(entity->physics != NULL) ye_add_physics_component(new_entity, entity->physics->velocity.x, entity->physics->velocity.y);
    // if(entity->collider != NULL) ye_add_collider_component(new_entity, entity->collider->collider_type, entity->collider->collider_impl);
    
    // create new tag comp
    // loop through tag component items, adding them one by one to the new entity tag component

    return new_entity;
}

/*
    Destroy an entity by pointer
*/
void ye_destroy_entity(struct ye_entity * entity){
    if(entity == NULL){
        ye_logf(warning, "Attempted to destroy a null entity\n");
        return;
    }

    // remove from the entity list (frees its node)
    ye_entity_list_remove(&entity_list_head, entity);

    // check for non null components and free them
    if(entity->transform != NULL) ye_remove_transform_component(entity);
    if(entity->renderer != NULL) ye_remove_renderer_component(entity);
    if(entity->camera != NULL) ye_remove_camera_component(entity);
    if(entity->physics != NULL) ye_remove_physics_component(entity);
    if(entity->tag != NULL) ye_remove_tag_component(entity);
    // if(entity->script != NULL) ye_remove_script_component(entity);
    // if(entity->interactible != NULL) ye_remove_interactible_component(entity);

    // free the entity name
    free(entity->name);

    // free all tags
    // for(int i = 0; i < 10; i++){
    //     if(entity->tags[i] != NULL) free(entity->tags[i]);
    // }

    // free the entity
    free(entity);

    entity = NULL;

    // ye_logf(debug, "Destroyed an entity\n");

    engine_runtime_state.entity_count--;
}

struct ye_entity * ye_get_entity_by_name(char *name){
    struct ye_entity_node *current = entity_list_head;

    while(current != NULL){
        if(strcmp(current->entity->name, name) == 0){
            return current->entity;
        }
        current = current->next;
    }

    return NULL;
}

struct ye_entity * ye_get_entity_by_tag(char *tag){
    struct ye_entity_node *current = tag_list_head;

    while(current != NULL){
        for(int i = 0; i < YE_TAG_MAX_NUMBER; i++){
            if(strcmp(current->entity->tag->tags[i], tag) == 0){
                return current->entity;
            }
        }
        current = current->next;
    }

    return NULL;
}

// struct ye_entity *ye_get_entity_by_id(int id){}

// struct ye_entity *ye_get_entity_by_name(char *name){}

// struct ye_entity *ye_get_entity_by_tag(char *tag){}

/////////////////////////  SYSTEMS  ////////////////////////////

/////////////////////////   ECS    ////////////////////////////
/*
    If you wanted to do something equivalent to the legacy clearAll()
    you would instead ye_shutdown_ecs and then ye_init_ecs
*/


void ye_init_ecs(){
    entity_list_head = ye_entity_list_create();
    transform_list_head = ye_entity_list_create();
    renderer_list_head = ye_entity_list_create();
    camera_list_head = ye_entity_list_create();
    physics_list_head = ye_entity_list_create();
    tag_list_head = ye_entity_list_create();
    ye_logf(info, "Initialized ECS\n");
}

void ye_shutdown_ecs(){
    ye_entity_list_destroy(&entity_list_head);
    
    /* 
        destroy the other, now empty lists (all items are null)

        this mainly consists of freeing the malloced nodes (2 pointers to ent and nxt)
    */

    ye_entity_list_destroy(&transform_list_head);
    ye_entity_list_destroy(&renderer_list_head);
    ye_entity_list_destroy(&camera_list_head);
    ye_entity_list_destroy(&physics_list_head);
    ye_entity_list_destroy(&tag_list_head);

    ye_logf(info, "Shut down ECS\n");
}

/*
    Function that iterates through entity list and logs all entities with their ids
*/
void ye_print_entities(){
    struct ye_entity_node *current = entity_list_head;
    int i = 0;
    while(current != NULL){
        char b[100];
        snprintf(b, sizeof(b), "\"%s\" -> ID:%d Transform:%d Renderer:%d Camera:%d Interactible:%d Script:%d Physics:%d Collider:%d Tag:%d\n",
            current->entity->name, current->entity->id, 
            current->entity->transform != NULL, 
            current->entity->renderer != NULL, 
            current->entity->camera != NULL,
            current->entity->interactible != NULL,
            current->entity->script != NULL,
            current->entity->physics != NULL,
            current->entity->collider != NULL,
            current->entity->tag != NULL
        );
        ye_logf(debug, b);
        current = current->next;
        i++;
    }
    ye_logf(debug, "Total entities: %d\n", i);
}

/*
    TODO: we need to deny adding multiple of the same component - or do we?
    right now nothing is stopping from dropping without freeing, we need non null
    check
*/