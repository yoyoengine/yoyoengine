/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file scene.h
 * @brief Manages the active scene in the engine as well as loading in new scenes.
 */

#ifndef YE_SCENE_H
#define YE_SCENE_H

#include <stdbool.h>

#include <jansson.h>

/**
 * @brief Initializes the scene manager
 */
void ye_init_scene_manager();

/**
 * @brief Loads a scene from file (.json, .yoyo, etc) and sets the current scene name,
 * creating all entities and components as described by the file.
 * 
 * @param scene_name The name of the scene to load
 */
void ye_load_scene(const char *scene_name);

/**
 * @brief Reloads the current scene from disk
 */
void ye_reload_scene();

/**
 * @brief Returns the current scene name
 * 
 * @return char* The name of the current scene
 */
char *ye_get_scene_name();

/**
 * @brief Tears down the scene manager context, freeing the current scene name
 */
void ye_shutdown_scene_manager();

/**
 * @brief Loads a scene from a json object
 * 
 * You are responsible for loading and freeing the json argument.
 * 
 * @param SCENE The json object representing the scene
 */
void ye_raw_scene_load(json_t *SCENE);

/**
 * @brief Defer loading a scene til next frame
 * 
 * @param scene_path The handle to the scene to load
 */
void ye_load_scene_deferred(const char *scene_path);

/**
 * @brief Runs once a frame, checks if a scene is deferred to be loaded and loads it
 *
 * @return bool True if a scene was loaded
 */
bool ye_scene_check_deferred_load();

#endif