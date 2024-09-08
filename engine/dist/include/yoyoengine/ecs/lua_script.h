/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file lua_script.h
 * @brief lua script component
 */

#ifndef YE_LUA_SCRIPT_H
#define YE_LUA_SCRIPT_H

#include <stdbool.h>
#include <yoyoengine/ecs/ecs.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

/*
    Types of lua globals we can pass through editor

    TODO: in the future we could implicitly cast to function sigs or tables,
    via casting in lua_script.c when we create the lua state.
*/
enum ye_lua_script_global_t {
    YE_LSG_NUMBER,
    YE_LSG_STRING,
    YE_LSG_BOOL
};

#define YE_LUA_SCRIPT_GLOBAL_NAME_MAX_CHARACTERS 64
#define YE_LUA_SCRIPT_GLOBAL_VALUE_STRING_MAX_CHARACTERS 512

/*
    Linked list holding lua script editor defined globals
*/
struct ye_lua_script_global {
    enum ye_lua_script_global_t type;
    char name[YE_LUA_SCRIPT_GLOBAL_NAME_MAX_CHARACTERS];

    union value_actual {
        double number;
        char string[YE_LUA_SCRIPT_GLOBAL_VALUE_STRING_MAX_CHARACTERS]; // TODO: should never need more than this, also Nuklear text area for large strings?
        bool boolean;
    } value;

    struct ye_lua_script_global *next;
};

/**
 * @brief The script component for lua. Will recieve callbacks from the engine.
 */
struct ye_component_lua_script {
    bool active;                    // controls whether system will act upon this component
    char *script_handle;            // the path to the script

    lua_State *state;               // the lua state for this script

    /*
        List of globals we pass to script.

        NOTE: this is not mutated at runtime, this is
        just a pure declaration of variables to init
        when the engine creates this component.

        DO NOT USE THIS TO ACCESS OR MUTATE VALUES AT RUNTIME!
    */
    struct ye_lua_script_global *globals;

    /*
        Once the script is boostrapped, we parse references so we know
        not to look for invalid references.
    */
    bool has_on_mount;
    bool has_on_unmount;
    bool has_on_update;
    bool has_on_collision;
    bool has_on_trigger_enter;
    // ... etc
};

/**
 * @brief Internal engine function to specify adding a global to a custom initialized struct ye_lua_script_global
 */
void ye_lua_script_add_manual_global(struct ye_lua_script_global **target, enum ye_lua_script_global_t type, const char *name, void *value);

/**
 * @brief This exists primarily for the editor to add new globals to entity scripts to serialize them properly. If you don't already know how this works, you aren't meant to use it!
 */
void ye_lua_script_add_global(struct ye_entity *ent, enum ye_lua_script_global_t type, const char *name, void *value);

/**
 * @brief This exists primarily for the editor to remove globals from entity scripts and serialize them properly. If you don't already know how this works, you aren't meant to use it!
 */
void ye_lua_script_remove_global(struct ye_entity *ent, const char *name);

/**
 * @brief Add a lua script component to an entity
 * 
 * @param entity The target entity
 * @param script_path The path to the script
 * @param globals A constructed list of globals to feed into the script. NULL for none.
 */
bool ye_add_lua_script_component(struct ye_entity *entity, const char *handle, struct ye_lua_script_global *globals);

/**
 * @brief Remove a lua script component from an entity
 * 
 * @param entity The target entity
 */
void ye_remove_lua_script_component(struct ye_entity *entity);

/**
 * @brief The system that controls the behavior of lua scripts
 */
void ye_system_lua_scripting();

/**
 * @brief Send onCollision to all scripts that observe it
 */
void ye_lua_signal_collisions(struct ye_entity *entity1, struct ye_entity *entity2);

/**
 * @brief Send onTriggerEnter to all scripts that observe it
 */
void ye_lua_signal_trigger_enter(struct ye_entity *entity1, struct ye_entity *entity2);

/*
    +-----------------+
    |      C API      |
    +-----------------+

    This is a simple API to allow setting globals at runtime
*/

ye_set_lua_script_global_bool(struct ye_entity *ent, const char *name, bool value);

ye_set_lua_script_global_number(struct ye_entity *ent, const char *name, double value);

ye_set_lua_script_global_string(struct ye_entity *ent, const char *name, const char *value);

#endif