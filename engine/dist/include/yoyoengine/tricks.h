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

/**
    @file tricks.c
    @brief Functions for loading and unloading dynamic libraries.
*/

/*
    Maybe Cmake build script should generate a tricks.yoyo file that contain each trick... blah blah blah
*/

#ifndef YE_TRICKS_H
#define YE_TRICKS_H

#include <yoyoengine/yoyoengine.h>

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
    struct ye_trick_node_v2 * next;
};

/**
 * @brief Register a trick with the engine.
 * 
 * @param trick Data on the trick to register
 * 
 * Should be invoked in yoyo_trick_init to make the engine aware of loaded tricks as well as any callbacks they desire to subscribe to.
 */
void ye_register_trick(struct ye_trick_node trick);

/**
 * @brief Will unload all loaded tricks and cleanup.
 */
void ye_shutdown_tricks();

/**
 * @brief Iterates over all loaded tricks and runs their updates
 */
void ye_run_trick_updates();

/**
 * @brief Registers any lua bindings tricks wish to expose
 * 
 * @param state The lua state to register the bindings to
 */
void ye_register_trick_lua_bindings(lua_State *state);

#endif