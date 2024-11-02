/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file scene.h
 * @brief Manages the active scene in the engine as well as loading in new scenes.
 */

#ifndef YE_SCENE_H
#define YE_SCENE_H

#include <yoyoengine/export.h>

#include <stdbool.h>

#include <jansson.h>

/**
 * @brief Initializes the scene manager
 */
YE_API void ye_init_scene_manager();

/**
 * @brief Loads a scene from file (.json, .yoyo, etc) and sets the current scene name,
 * creating all entities and components as described by the file.
 * 
 * @param scene_name The name of the scene to load
 */
YE_API void ye_load_scene(const char *scene_name);

/**
 * @brief Reloads the current scene from disk
 */
YE_API void ye_reload_scene();

/**
 * @brief Returns the current scene name
 * 
 * @return char* The name of the current scene
 */
YE_API char *ye_get_scene_name();

/**
 * @brief Tears down the scene manager context, freeing the current scene name
 */
YE_API void ye_shutdown_scene_manager();

/**
 * @brief Loads a scene from a json object
 * 
 * You are responsible for loading and freeing the json argument.
 * 
 * @param SCENE The json object representing the scene
 */
YE_API void ye_raw_scene_load(json_t *SCENE);

/**
 * @brief Defer loading a scene til next frame
 * 
 * @param scene_path The handle to the scene to load
 */
YE_API void ye_load_scene_deferred(const char *scene_path);

/**
 * @brief Runs once a frame, checks if a scene is deferred to be loaded and loads it
 *
 * @return bool True if a scene was loaded
 */
YE_API bool ye_scene_check_deferred_load();

#endif