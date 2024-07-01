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

/**
 * @brief The script component for lua. Will recieve callbacks from the engine.
 */
struct ye_component_lua_script {
    bool active;                    // controls whether system will act upon this component
    char *script_handle;            // the path to the script

    lua_State *state;               // the lua state for this script

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
 * @brief Add a lua script component to an entity
 * 
 * @param entity The target entity
 * @param script_path The path to the script
 */
bool ye_add_lua_script_component(struct ye_entity *entity, const char *handle);

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

#endif