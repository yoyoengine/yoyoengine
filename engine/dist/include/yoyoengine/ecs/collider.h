/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file collider.h
 * @brief ECS Collider component\
 * 
 * TODO:
 * - the flexibility of having "round" colliders or triange colliders
 * - the ability to have multiple colliders on a single entity
 * - on enter, on exit on stay
 * - trigger colliders
 */

#ifndef YE_COLLIDER_H
#define YE_COLLIDER_H

#include <yoyoengine/export.h>

#include <stdbool.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/utils.h>

enum ye_collider_type {
    YE_COLLIDER_RECT = 0,
    YE_COLLIDER_CIRCLE = 1
};

// v2, meh
// enum ye_collider_flags {
//     YE_COLLIDER_TRIGGER = 1 << 0,
//     YE_COLLIDER_STATIC = 1 << 1,
//     YE_COLLIDER_RECT = 1 << 2,
//     YE_COLLIDER_CIRCLE = 1 << 3
// };

/**
 * @brief A structure representing a collider component.
 *
 * This structure might need to be broken out into two different colliders, trigger and static to enable
 * multiple colliders on a single entity. Also, consider giving this a z or a layer, colliders do not care about the layer right now.
 */
struct ye_component_collider {
    bool active;            /**< Controls whether system will act upon this component. */
    
    bool relative;          /**< Specifies whether this component is relative to a parent transform. */

    bool is_trigger;        /**< Specifies whether this collider is a trigger. If false, it is a static collider. */

    enum ye_collider_type type; /**< The type of collider. */
    float x, y;
    // TODO: not sure how i feel about anonymous structs in the union...
    union {
        // YE_COLLIDER_RECT
        struct { float width, height; };

        // YE_COLLIDER_CIRCLE
        struct { float radius; };
    };
};

///////////////////////////////////////////////////

YE_API void ye_add_static_rect_collider_component(struct ye_entity *entity, float x, float y, float w, float h); 
YE_API void ye_add_trigger_rect_collider_component(struct ye_entity *entity, float x, float y, float w, float h); 

YE_API void ye_add_static_circle_collider_component(struct ye_entity *entity, float x, float y, float radius);
YE_API void ye_add_trigger_circle_collider_component(struct ye_entity *entity, float x, float y, float radius);

///////////////////////////////////////////////////

/**
 * @brief Removes an entity's collider component.
 *
 * @param entity The entity from which the collider is to be removed.
 */
YE_API void ye_remove_collider_component(struct ye_entity *entity);

#endif