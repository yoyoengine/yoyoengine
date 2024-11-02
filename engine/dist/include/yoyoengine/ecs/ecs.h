/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file ecs.h
 * @brief The engine ECS API
 */

#ifndef YE_ECS_H
#define YE_ECS_H

#include <yoyoengine/export.h>

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
extern struct ye_entity_node *tag_list_head;
extern struct ye_entity_node *collider_list_head;
extern struct ye_entity_node *lua_script_list_head;
extern struct ye_entity_node *audiosource_list_head;
extern struct ye_entity_node *button_list_head;

/**
 * @brief Linked list structure for storing entities
 */
struct ye_entity_node {
    struct ye_entity *entity;
    struct ye_entity_node *next;
};

/**
 * @brief Get the head of the entity list
 * 
 * @return struct ye_entity_node* 
 */
YE_API struct ye_entity_node * ye_get_entity_list_head();

/**
 * @brief Add an entity to a list
 * 
 * @param list The list to add the entity to
 * @param entity The entity to add
 */
YE_API void ye_entity_list_add(struct ye_entity_node **list, struct ye_entity *entity);

/**
 * @brief Add an entity to a list sorted by its Z transform value (used for rendering order)
 * 
 * @param list The list to add the entity to
 * @param entity The entity to add
 */
YE_API void ye_entity_list_add_sorted_renderer_z(struct ye_entity_node **list, struct ye_entity *entity);

/**
 * @brief Remove an entity from a list
 * 
 * @param list The list to remove the entity from
 * @param entity The entity to remove
 * 
 * @note THIS DOES NOT FREE THE ACTUAL ENTITY ITSELF
 * 
 * This is temporarialy ok because we dont delete any at runtime but TODO
 * we need a method of this that does call the destructor for the entity
 * probably ye_entity_list_remove_free?
 * Not sure how to differentiate when this needs called though... just for actual list of entities?
 */
YE_API void ye_entity_list_remove(struct ye_entity_node **list, struct ye_entity *entity);

/**
 * @brief Entity structure. An entity is a collection of components that make up a game object.
 */
struct ye_entity {
    bool active;        // controls whether system will act upon this entity and its components

    int id;             // unique id for this entity
    char *name;         // name that can also be used to access the entity

    struct ye_component_transform *transform;       // transform component
    struct ye_component_renderer *renderer;         // renderer component
    struct ye_component_lua_script *lua_script;     // lua script component
    struct ye_component_button *button;             // button component
    struct ye_component_camera *camera;             // camera component
    struct ye_component_physics *physics;           // physics component
    struct ye_component_collider *collider;         // collider component
    struct ye_component_tag *tag;                   // tag component
    struct ye_component_audiosource *audiosource;   // audiosource component
};

/**
 * @brief 2D vector structure
 */
struct ye_vec2f {
    float x;
    float y;
};

/*
    =============================================================
                        ENTITY MANIPULATION
    =============================================================
*/

/**
 * @brief Create a new entity and return a pointer to it
 * 
 * @return struct ye_entity* 
 */
YE_API struct ye_entity * ye_create_entity();

/**
 * @brief Create a new entity and return a pointer to it (named)
 * 
 * @param name The name of the entity
 * @return struct ye_entity* 
 */
YE_API struct ye_entity * ye_create_entity_named(const char *name);

/**
 * @brief Rename an entity by pointer
 * 
 * @param entity The entity to rename
 * @param name The new name
 */
YE_API void ye_rename_entity(struct ye_entity *entity, const char *new_name);

/**
 * @brief !!!DO NOT USE THIS RIGHT NOW. IT IS COMPLETELY BROKEN!!! Duplicate an entity by pointer. Will rename the entity to "entity_name (copy)"
 * 
 * @param entity The entity to duplicate
 * @return struct ye_entity* 
 */
YE_API struct ye_entity * ye_duplicate_entity(struct ye_entity *entity);

/**
 * @brief Destroy an entity by pointer
 * 
 * @param entity The entity to destroy
 */
YE_API void ye_destroy_entity(struct ye_entity * entity);

/**
 * @brief Find entity by name, returns pointer to first entity of specified name, NULL if not found
 * 
 * @param name The name of the entity to find
 * @return struct ye_entity* 
 */
YE_API struct ye_entity * ye_get_entity_by_name(const char *name);

/**
 * @brief Find an entity by tag (if there are more than one entity with this tag, it will return the first one, and NOT by distance)
 * 
 * @param tag The tag of the entity to find
 * @return struct ye_entity* 
 */
YE_API struct ye_entity * ye_get_entity_by_tag(const char *tag);

/**
 * @brief Find an entity by id, returns pointer to first entity of specified id, NULL if not found
 * 
 * @param id The id of the entity to find
 * @return struct ye_entity* 
 */
YE_API struct ye_entity *ye_get_entity_by_id(int id);

/**
 * @brief Initialize the ECS
 */
YE_API void ye_init_ecs();

/**
 * @brief Purge the ECS
 */
YE_API void ye_purge_ecs();

/**
 * @brief Shutdown the ECS
 */
YE_API void ye_shutdown_ecs();

/**
 * @brief Print all entities
 */
YE_API void ye_print_entities();

#endif