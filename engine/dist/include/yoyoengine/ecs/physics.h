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
 * @file physics.h
 * @brief ECS Physics component
 */

#ifndef YE_PHYSICS_H
#define YE_PHYSICS_H

#include <yoyoengine/yoyoengine.h>

/*
    Set a default of 10 substeps if the developer does not override it
*/
#ifndef YE_PHYSICS_SUBSTEPS
    #define YE_PHYSICS_SUBSTEPS 10
#endif

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
void ye_add_physics_component(struct ye_entity *entity, float velocity_x, float velocity_y);

/**
 * @brief Removes the physics component from an entity
 *
 * @param entity The entity to remove the component from
 */
void ye_remove_physics_component(struct ye_entity *entity);

/**
 * @brief Physics system function
 *
 * This function is responsible for updating the physics components of all entities.
 */
void ye_system_physics();

#endif