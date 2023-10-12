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

#ifndef YE_ECS_H
#define YE_ECS_H

#include <yoyoengine/yoyoengine.h>
#include <yoyoengine/utils.h>
#include <stdbool.h>

/*
    =============================================================
                        ENTITY LISTS
    =============================================================
*/

// lists the ECS acts upon
extern struct ye_entity_node *entity_list_head;
extern struct ye_entity_node *transform_list_head;
extern struct ye_entity_node *renderer_list_head;
extern struct ye_entity_node *camera_list_head;
extern struct ye_entity_node *physics_list_head;

// Define a linked list structure for storing entities
struct ye_entity_node {
    struct ye_entity *entity;
    struct ye_entity_node *next;
};

struct ye_entity_node * ye_get_entity_list_head();

/*
    =============================================================
                        ENTITY DEFINITIONS
    =============================================================
*/

/*
    Entity
    
    An entity is a collection of components that make up a game object.
*/
struct ye_entity {
    bool active;        // controls whether system will act upon this entity and its components

    int id;             // unique id for this entity
    char *name;         // name that can also be used to access the entity

    /*
        TODO: tag should become its own component, we dont want to check EVERY entity
        for posessing a tag when only 1/100 entities have a tag component
    */
    // char *tags[10];     // up to 10 tags that can also be used to access the entity

    struct ye_component_transform *transform;       // transform component
    struct ye_component_renderer *renderer;         // renderer component
    struct ye_component_script *script;             // script component
    struct ye_component_interactible *interactible; // interactible component
    struct ye_component_camera *camera;             // camera component
    struct ye_component_physics *physics;           // physics component
    struct ye_component_collider *collider;         // collider component

    /* NOTE/TODO:
        We can have arrays in the future malloced to the
        size of struct ye_component_* and then we can have
        multiple components of the same type on an entity.

        This would be good for tags, but for all else we would actually need
        to explicitely link which transform and which components are
        used for other components which have been assuming them by default
    */
};

// 2d vector
struct ye_vec2f {
    float x;
    float y;
};

enum ye_collider_type {
    YE_COLLIDER_TYPE_RECT,
    YE_COLLIDER_TYPE_CIRCLE
};

/*
    Collider component
    
    Holds information on how an entity collides with other entities.
*/
struct ye_component_collider {
    bool active;    // controls whether system will act upon this component

    enum ye_collider_type type; // type of collider
    bool is_trigger;            // whether collider is a trigger or not
    SDL_Rect rect;              // collider bounds
};

/*
    Script component
    
    Holds information on a script that is attatched to an entity.
    This script will have its main loop run once per frame.
*/
struct ye_component_script {
    bool active;    // controls whether system will act upon this component

    char *script_path;
};

/*
    Interactible component
    
    Holds information on how an entity can be interacted with.
*/
struct ye_component_interactible {
    bool active;    // controls whether system will act upon this component

    void *data;                     // data to communicate when callback finishes
    void (*callback)(void *data);   // callback to run when entity is interacted with

    /*
        Currently, we will get the transform of an object with a
        interactible component to check bounds for clicks.

        In the future, we would want a collider_2d component with
        an assosciated type and relative position so we can more
        presicely check for interactions. I dont forsee the need
        for this for a while though. Maybe future me in a year reading
        this wishes I did it now xD
    */
};

/*
    =============================================================
                        ENTITY MANIPULATION
    =============================================================
*/

/*
    Create a new entity and return a pointer to it
*/
struct ye_entity * ye_create_entity();

/*
    Create a new entity and return a pointer to it (named)

    we must allocate space for the name and copy it
*/
struct ye_entity * ye_create_entity_named(char *name);

/*
    Rename an entity by pointer
*/
void ye_rename_entity(struct ye_entity *entity, char *name);

/*
    Duplicate an entity by pointer. Will rename the entitity to "entity_name (copy)"
*/
struct ye_entity * ye_duplicate_entity(struct ye_entity *entity);

/*
    Destroy an entity by pointer
*/
void ye_destroy_entity(struct ye_entity * entity);

/*
    Find entity by name, returns pointer to first entity of specified name, NULL if not found
*/
struct ye_entity * ye_find_entity_named(char *name);

/*
    =============================================================
                        ECS CONTENXT & HELPERS
    =============================================================
*/

void ye_init_ecs();

void ye_shutdown_ecs();

void ye_print_entities();

#endif