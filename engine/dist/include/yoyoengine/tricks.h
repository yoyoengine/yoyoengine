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

/**
 * @brief Allows a trick to externally register its callbacks after it has been loaded from dllload.
 * 
 * @param name The name of the trick
 * @param on_load The function to call when the trick is loaded
 * @param on_unload The function to call when the trick is unloaded
 * @param on_update The function to call when the trick is updated
 */
void ye_trick_register_self(const char * name, void (*on_load)(), void (*on_unload)(), void (*on_update)());

/**
 * @brief Will look through resources/../tricks/ASTERISK.so and load them.
 */
void ye_init_tricks();

// void ye_load_trick(const char * trick_name);

/**
 * @brief Will unload all loaded tricks and cleanup.
 */
void ye_shutdown_tricks();

/**
 * @brief Iterates over all loaded tricks and runs their updates
 */
void ye_run_trick_updates();

#endif