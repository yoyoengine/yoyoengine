/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>

#include <yoyoengine/yep.h>
#include <yoyoengine/cache.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/tag.h>
#include <yoyoengine/ecs/camera.h>
#include <yoyoengine/ecs/button.h>
#include <yoyoengine/ecs/physics.h>
#include <yoyoengine/ecs/collider.h>
#include <yoyoengine/ecs/renderer.h>
#include <yoyoengine/ecs/transform.h>
#include <yoyoengine/ecs/lua_script.h>
#include <yoyoengine/ecs/audiosource.h>

// entity id counter (used to assign unique ids to entities)
int eid = 0;

//////////////////////// LINKED LIST //////////////////////////

struct ye_entity_node *ye_entity_list_create() {
    return NULL; // Initialize an empty list
}

void ye_entity_list_add(struct ye_entity_node **list, struct ye_entity *entity) {
    struct ye_entity_node *newNode = malloc(sizeof(struct ye_entity_node));
    newNode->entity = entity;
    newNode->next = *list;
    *list = newNode;
}

void ye_entity_list_add_sorted_renderer_z(struct ye_entity_node **list, struct ye_entity *entity){
    if(entity == NULL || entity->renderer == NULL){
        ye_logf(warning, "Error adding to render list sorted Z, something was null.\n");
        return;
    }
    
    struct ye_entity_node *newNode = malloc(sizeof(struct ye_entity_node));
    newNode->entity = entity;
    newNode->next = NULL;

    // if the list is empty, add to head
    if(*list == NULL){
        *list = newNode;
        return;
    }

    // if the new node is less than the head, add to head
    if(newNode->entity->renderer->z < (*list)->entity->renderer->z){
        newNode->next = *list;
        *list = newNode;
        return;
    }

    // otherwise, traverse the list and find the correct spot
    struct ye_entity_node *current = *list;
    while(current->next != NULL){
        if(newNode->entity->renderer->z < current->next->entity->renderer->z){
            newNode->next = current->next;
            current->next = newNode;
            return;
        }
        current = current->next;
    }

    // if we reach the end of the list, add to the end
    current->next = newNode;
}

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
struct ye_entity_node *collider_list_head;
struct ye_entity_node *lua_script_list_head;
struct ye_entity_node *audiosource_list_head;
struct ye_entity_node *button_list_head;

struct ye_entity_node * ye_get_entity_list_head(){
    return entity_list_head;
}

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
    entity->lua_script = NULL;
    entity->button = NULL;
    entity->physics = NULL;
    entity->collider = NULL;
    entity->tag = NULL;
    entity->audiosource = NULL;

    // add the entity to the entity list
    ye_entity_list_add(&entity_list_head, entity);
    // ye_logf(debug, "Created and added an entity\n");

    YE_STATE.runtime.entity_count++;

    return entity;
}

struct ye_entity * ye_create_entity_named(const char *name){
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
    entity->lua_script = NULL;
    entity->button = NULL;
    entity->physics = NULL;
    entity->collider = NULL;
    entity->tag = NULL;
    entity->audiosource = NULL;

    // add the entity to the entity list
    ye_entity_list_add(&entity_list_head, entity);
    // ye_logf(debug, "Created and added an entity\n");

    YE_STATE.runtime.entity_count++;

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
    char *suffix = " copy";
    int temp_len = strlen(entity->name) + strlen(suffix) + 1; // +1 for the null-terminating character
    char *temp = (char *)malloc(temp_len * sizeof(char));
    if (temp == NULL) {
        // handle error
    }
    strcpy(temp, entity->name);
    strcat(temp, suffix);

    struct ye_entity *new_entity = ye_create_entity_named(temp);

    free(temp);

    // copy all components
    if(entity->transform != NULL) ye_add_transform_component(new_entity, entity->transform->x, entity->transform->y);
    if(entity->renderer != NULL){
        if(entity->renderer->type == YE_RENDERER_TYPE_IMAGE){
            ye_add_image_renderer_component(new_entity, entity->renderer->z, entity->renderer->renderer_impl.image->src);
        }
        else if(entity->renderer->type == YE_RENDERER_TYPE_TEXT){
            ye_add_text_renderer_component(new_entity, entity->renderer->z, entity->renderer->renderer_impl.text->text, entity->renderer->renderer_impl.text->font_name, entity->renderer->renderer_impl.text->font_size, entity->renderer->renderer_impl.text->color_name, entity->renderer->renderer_impl.text->wrap_width);
        }
        else if(entity->renderer->type == YE_RENDERER_TYPE_TEXT_OUTLINED){
            ye_add_text_outlined_renderer_component(new_entity, entity->renderer->z, entity->renderer->renderer_impl.text_outlined->text, entity->renderer->renderer_impl.text_outlined->font_name, entity->renderer->renderer_impl.text_outlined->font_size, entity->renderer->renderer_impl.text_outlined->color_name, entity->renderer->renderer_impl.text_outlined->outline_color_name, entity->renderer->renderer_impl.text_outlined->outline_size, entity->renderer->renderer_impl.text_outlined->wrap_width);
        }
        else if(entity->renderer->type == YE_RENDERER_TYPE_ANIMATION){
            ye_add_animation_renderer_component(new_entity, entity->renderer->z, entity->renderer->renderer_impl.animation->meta_file);
        }

        /*
            Update any fields here that arent tracked by the constructor
        */
        new_entity->renderer->flipped_x = entity->renderer->flipped_x;
        new_entity->renderer->flipped_y = entity->renderer->flipped_y;
        new_entity->renderer->active = entity->renderer->active;
        new_entity->renderer->rect = entity->renderer->rect;
        new_entity->renderer->relative = entity->renderer->relative;
        new_entity->renderer->rotation = entity->renderer->rotation;
    }
    if(entity->camera != NULL){ 
        ye_add_camera_component(new_entity, entity->camera->z, entity->camera->view_field);
        new_entity->camera->active = entity->camera->active;
    }
    if(entity->button != NULL){
        ye_add_button_component(new_entity, entity->button->rect);
        new_entity->button->active = entity->button->active;
    }
    if(entity->physics != NULL){
        ye_add_physics_component(new_entity, entity->physics->velocity.x, entity->physics->velocity.y);
        new_entity->physics->active = entity->physics->active;
        new_entity->physics->rotational_velocity = entity->physics->rotational_velocity;
    }
    if(entity->collider != NULL){
        if(entity->collider->is_trigger){
            ye_add_trigger_collider_component(new_entity, entity->collider->rect);
        }
        else{
            ye_add_static_collider_component(new_entity, entity->collider->rect);
        }
        new_entity->collider->active = entity->collider->active;
        new_entity->collider->is_trigger = entity->collider->is_trigger;
    }    
    if(entity->tag != NULL){
        ye_add_tag_component(new_entity);
        for(int i = 0; i < YE_TAG_MAX_NUMBER; i++){
            if(entity->tag->tags[i] != NULL){
                ye_add_tag(new_entity, entity->tag->tags[i]);
            }
        }
        new_entity->tag->active = entity->tag->active;
    }
    if(entity->audiosource != NULL){
        ye_add_audiosource_component(new_entity, entity->audiosource->handle, entity->audiosource->volume, entity->audiosource->play_on_awake, entity->audiosource->loops, entity->audiosource->simulated, entity->audiosource->range);
        new_entity->audiosource->active = entity->audiosource->active;
    }
    if(entity->lua_script != NULL){
        ye_add_lua_script_component(new_entity, entity->lua_script->script_handle);
        new_entity->lua_script->active = entity->lua_script->active;
    }

    return new_entity;
}

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
    if(entity->lua_script != NULL) ye_remove_lua_script_component(entity);
    if(entity->button != NULL) ye_remove_button_component(entity);
    if(entity->collider != NULL) ye_remove_collider_component(entity);
    if(entity->audiosource != NULL) ye_remove_audiosource_component(entity);
    if(entity->lua_script != NULL) ye_remove_lua_script_component(entity);
    // free the entity name
    free(entity->name);

    // free the entity
    free(entity);

    entity = NULL;

    // ye_logf(debug, "Destroyed an entity\n");

    YE_STATE.runtime.entity_count--;
}

struct ye_entity * ye_get_entity_by_name(const char *name){
    struct ye_entity_node *current = entity_list_head;

    while(current != NULL){
        if(strcmp(current->entity->name, name) == 0){
            return current->entity;
        }
        current = current->next;
    }

    ye_logf(error, "COULD NOT LOCATE ENTITY BY THE NAME \"%s\"\n",name);
    return NULL;
}

struct ye_entity * ye_get_entity_by_tag(const char *tag){
    struct ye_entity_node *current = tag_list_head;

    while(current != NULL){
        for(int i = 0; i < YE_TAG_MAX_NUMBER; i++){
            if(strcmp(current->entity->tag->tags[i], tag) == 0){
                return current->entity;
            }
        }
        current = current->next;
    }

    ye_logf(error, "COULD NOT LOCATE ENTITY BY THE TAG \"%s\"\n",tag);
    return NULL;
}

struct ye_entity *ye_get_entity_by_id(int id){
    struct ye_entity_node *current = entity_list_head;

    while(current != NULL){
        if(current->entity->id == id){
            return current->entity;
        }
        current = current->next;
    }

    ye_logf(error, "COULD NOT LOCATE ENTITY BY THE ID \"%d\"\n",id);
    return NULL;
}

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
    collider_list_head = ye_entity_list_create();
    audiosource_list_head = ye_entity_list_create();
    lua_script_list_head = ye_entity_list_create();
    button_list_head = ye_entity_list_create();
    ye_logf(info, "Initialized ECS\n");
}

void ye_purge_ecs(){
    ye_shutdown_ecs();
    ye_init_ecs();
    ye_logf(info, "Purged ECS\n");
    if(YE_STATE.editor.editor_mode){
        struct ye_entity * editor_camera = ye_create_entity_named("editor_camera");
        ye_add_transform_component(editor_camera, 0, 0);
        ye_add_camera_component(editor_camera, 999, (SDL_Rect){0, 0, 2560, 1440});
        ye_set_camera(editor_camera);

        struct ye_entity * origin = ye_create_entity_named("origin");
        ye_add_transform_component(origin, -50, -50);

        // load the origin texture
        SDL_Texture *orgn_tex = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, yep_engine_resource_image("originwhite.png"));
        ye_cache_texture_manual(orgn_tex, "originwhite.png");
        ye_add_image_renderer_component_preloaded(origin, 0, orgn_tex);
        origin->renderer->rect = (struct ye_rectf){0, 0, 100, 100};
    }
    ye_logf(info, "Re-created editor entities.\n");
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
    ye_entity_list_destroy(&collider_list_head);
    ye_entity_list_destroy(&lua_script_list_head);
    ye_entity_list_destroy(&audiosource_list_head);
    ye_entity_list_destroy(&button_list_head);

    // take care of cleaning up any entity pointers that exist in global state
    YE_STATE.engine.target_camera = NULL;
    YE_STATE.editor.scene_default_camera = NULL;

    eid = 0;

    ye_logf(info, "Shut down ECS\n");
}

void ye_print_entities(){
    struct ye_entity_node *current = entity_list_head;
    int i = 0;
    while(current != NULL){
        char b[100];
        snprintf(b, sizeof(b), "\"%s\" -> ID:%d Trn:%d Rdr:%d Cam:%d Btn:%d Scr:%d Phy:%d Col:%d Tag:%d Aud:%d\n",
            current->entity->name, current->entity->id, 
            current->entity->transform != NULL, 
            current->entity->renderer != NULL, 
            current->entity->camera != NULL,
            current->entity->button != NULL,
            current->entity->lua_script != NULL,
            current->entity->physics != NULL,
            current->entity->collider != NULL,
            current->entity->tag != NULL,
            current->entity->audiosource != NULL
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