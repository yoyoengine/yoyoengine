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

/**
 * @brief A structure representing a collider component.
 *
 * This structure might need to be broken out into two different colliders, trigger and static to enable
 * multiple colliders on a single entity. Also, consider giving this a z or a layer, colliders do not care about the layer right now.
 */
struct ye_component_collider {
    bool active;            /**< Controls whether system will act upon this component. */
    
    bool relative;          /**< Specifies whether this component is relative to a parent transform. */
    
    struct ye_rectf rect;   /**< The collider rectangle. */

    bool is_trigger;        /**< Specifies whether this collider is a trigger. If false, it is a static collider. */

    /*
        Meta tracking trigger states
    */
    // bool _trigger_entered;
};

/**
 * @brief Adds a static collider to an entity.
 *
 * @param entity The entity to which the collider is to be added.
 * @param rect The rectangle defining the collider.
 */
YE_API void ye_add_static_collider_component(struct ye_entity *entity, struct ye_rectf rect);

/**
 * @brief Adds a trigger collider to an entity.
 *
 * @param entity The entity to which the collider is to be added.
 * @param rect The rectangle defining the collider.
 */
YE_API void ye_add_trigger_collider_component(struct ye_entity *entity, struct ye_rectf rect);

/**
 * @brief Removes an entity's collider component.
 *
 * @param entity The entity from which the collider is to be removed.
 */
YE_API void ye_remove_collider_component(struct ye_entity *entity);

#endif