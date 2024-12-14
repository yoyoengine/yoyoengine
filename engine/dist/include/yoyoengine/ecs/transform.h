/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file transform.h
 * @brief ECS Transform component
 */

#ifndef YE_TRANSFORM_H
#define YE_TRANSFORM_H

#include <Lilith.h>

#include <yoyoengine/export.h>

#include <yoyoengine/ecs/ecs.h>

/**
 * @brief The transform component
 * 
 * Describes where the entity sits in the world.
*/
struct ye_component_transform {
    // bool active;    // controls whether system will act upon this component

    float x;        // the transform x position
    float y;        // the transform y position

    // physics2
    float rotation; // clockwise rotation in degrees
};

/**
 * @brief Adds a transform component to an entity
 * 
 * @param entity The entity to add the component to
 * @param x The x position of the transform
 * @param y The y position of the transform
 */
YE_API void ye_add_transform_component(struct ye_entity *entity, int x, int y);

/**
 * @brief Removes a transform component from an entity
 * 
 * @param entity The entity to remove the component from
 */
YE_API void ye_remove_transform_component(struct ye_entity *entity);

#endif