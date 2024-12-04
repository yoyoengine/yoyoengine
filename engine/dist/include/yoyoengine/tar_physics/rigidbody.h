/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_RIGIDBODY_H
#define YE_RIGIDBODY_H

#include <stdbool.h>

#include <yoyoengine/export.h>

#include <yoyoengine/ecs/ecs.h>

// TODO: gravity as a vec2 for flexibility (in physics main, not here)
// note: will be applying impulses directly to velocity

struct ye_component_rigidbody {
    bool active;                            /**< Controls whether system will act upon this component. */
    
    // TODO: inverse mass for performance
    float mass;                             /**< The mass of the rigidbody. */

    float restitution;                      /**< The "bounciness" of the rigidbody. */
    
    float kinematic_friction;               /**< The friction of the rigidbody. */
    float rotational_kinematic_friction;    /**< The rotational friction of the rigidbody. */

    struct ye_vec2f velocity;               /**< Velocity of entity */
    float rotational_velocity;              /**< Rotational velocity of entity */
};

// TODO: freeze axes?

YE_API void ye_add_rigidbody_component(struct ye_entity *ent, float mass, float restitution, float kinematic_friction, float rotational_kinematic_friction);
YE_API void ye_remove_rigidbody_component(struct ye_entity *ent);

#endif // YE_RIGIDBODY_H