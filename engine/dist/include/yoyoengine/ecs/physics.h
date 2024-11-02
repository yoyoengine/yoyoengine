/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file physics.h
 * @brief ECS Physics component
 */

#ifndef YE_PHYSICS_H
#define YE_PHYSICS_H

#include <yoyoengine/export.h>

/*
    Set a default of 10 substeps if the developer does not override it
*/
#ifndef YE_PHYSICS_SUBSTEPS
    #define YE_PHYSICS_SUBSTEPS 10
#endif

#include <stdbool.h>
#include <yoyoengine/ecs/ecs.h>

/**
 * @brief Physics component structure
 *
 * This structure holds the physics properties of an entity.
 */
struct ye_component_physics {
    bool active;                        /**< Controls whether system will act upon this component */

    struct ye_vec2f velocity;           /**< Velocity of entity */
    float rotational_velocity;          /**< Rotational velocity of entity */
};

/**
 * @brief Adds a physics component to an entity
 *
 * @param entity The entity to add the component to
 * @param velocity_x The x component of the velocity
 * @param velocity_y The y component of the velocity
 */
YE_API void ye_add_physics_component(struct ye_entity *entity, float velocity_x, float velocity_y);

/**
 * @brief Removes the physics component from an entity
 *
 * @param entity The entity to remove the component from
 */
YE_API void ye_remove_physics_component(struct ye_entity *entity);

/**
 * @brief Physics system function
 *
 * This function is responsible for updating the physics components of all entities.
 */
YE_API void ye_system_physics();

#endif