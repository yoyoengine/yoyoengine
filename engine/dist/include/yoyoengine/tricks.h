/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
    @file tricks.c
    @brief Functions for loading and unloading dynamic libraries.
*/

/*
    Maybe Cmake build script should generate a tricks.yoyo file that contain each trick... blah blah blah
*/

#ifndef YE_TRICKS_H
#define YE_TRICKS_H

#include <yoyoengine/export.h>

#include <lua.h>

extern struct ye_trick_node * ye_tricks_head;

struct ye_trick_node{
    // metadata
    char * name;
    char * author;
    char * description;
    char * version;

    // regularly invoked callbacks
    void (*on_load)();
    void (*on_unload)();
    void (*on_update)();
    void (*lua_bind)(lua_State *);

    // LL
    struct ye_trick_node * next;
};

/**
 * @brief Initializes the trick system
 * @note Currently does nothing
 */
YE_API void ye_init_tricks();

/**
 * @brief Register a trick with the engine.
 * 
 * @param trick Data on the trick to register
 * 
 * Should be invoked in yoyo_trick_init to make the engine aware of loaded tricks as well as any callbacks they desire to subscribe to.
 */
YE_API void ye_register_trick(struct ye_trick_node trick);

/**
 * @brief Will unload all loaded tricks and cleanup.
 */
YE_API void ye_shutdown_tricks();

/**
 * @brief Iterates over all loaded tricks and runs their updates
 */
YE_API void ye_run_trick_updates();

/**
 * @brief Registers any lua bindings tricks wish to expose
 * 
 * @param state The lua state to register the bindings to
 */
YE_API void ye_register_trick_lua_bindings(lua_State *state);

#endif