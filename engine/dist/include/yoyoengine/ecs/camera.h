/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file camera.h
 * @brief ECS Camera component
 */

#ifndef YE_CAMERA_H
#define YE_CAMERA_H

#include <yoyoengine/export.h>

#include <stdbool.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/utils.h>
#include <SDL.h>

/**
 * @brief The camera component
 * 
 * Holds some information about a camera and its view field (the x and y of the view field are unused, as its location is inferred from its transform)
*/
struct ye_component_camera {
    bool active;    // controls whether system will act upon this component

    bool relative;  // whether or not this comp is relative to a parent transform
    int z; // the layer the camera sits on

    struct ye_rectf view_field;    // view field of camera

    bool lock_aspect_ratio; // whether or not to lock the aspect ratio of the view field
};

/**
 * @brief Sets the camera to be used by the renderer
 * 
 * @param entity The entity to set as the camera
 */
YE_API void ye_set_camera(struct ye_entity *entity);

/**
 * @brief Adds a camera component to an entity
 * 
 * @param entity The entity to add the component to
 * @param z The layer the camera sits on
 * @param view_field The view field of the camera
 */
YE_API void ye_add_camera_component(struct ye_entity *entity, int z, struct ye_rectf view_field);

/**
 * @brief Removes a camera component from an entity
 * 
 * @param entity The entity to remove the component from
 */
YE_API void ye_remove_camera_component(struct ye_entity *entity);

#endif