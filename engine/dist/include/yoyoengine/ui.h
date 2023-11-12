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
 * @file ui.h
 * @brief Provides a simple UI system for the engine, built on top of Nuklear.
 */

#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include <SDL2/SDL.h>

/**
 * @brief Initializes the UI system.
 * 
 * @param win The window to use.
 * @param renderer The renderer to use.
 */
void init_ui(SDL_Window *win, SDL_Renderer *renderer);

/**
 * @brief Handles input for the UI system for that frame.
 * 
 * @param evt The event to handle (from SDL input).
 */
void ui_handle_input(SDL_Event *evt);

/**
 * @brief Starts the input checks for the UI system.
 */
void ui_begin_input_checks();

/**
 * @brief Ends the input checks for the UI system.
 */
void ui_end_input_checks();

/**
 * @brief Paints a debug overlay.
 */
void ui_paint_debug_overlay();

/**
 * @brief Renders all registered windows in UI system onto the SDL frame buffer.
 */
void ui_render();

/**
 * @brief Shuts down the UI system.
 */
void shutdown_ui();

/**
 * @brief Registers a component with the UI system.
 * 
 * @param key The key to register the component under.
 * @param render_function The function to call to render the component.
 */
void ui_register_component(const char* key, void (*render_function)());

/**
 * @brief Remove a component from the UI system.
 * 
 * @param key The key to remove the component under.
 */
void remove_ui_component(const char* key);

/**
 * @brief Checks if a component exists in the UI system.
 * 
 * @param key The key to check for.
 * @return true The component exists.
 * @return false The component does not exist.
 */
bool ui_component_exists(char *key);

#endif