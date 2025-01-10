/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_RIGIDBODY_H
#define YE_RIGIDBODY_H

#include <yoyoengine/export.h>

#include <stdbool.h>
#include <yoyoengine/ecs/ecs.h>

#include <p2d/p2d.h>

enum ye_rb_collider_type {
    YE_RB_COLLIDER_RECT = 0,
    YE_RB_COLLIDER_CIRCLE = 1
};

/*
    ALWAYS relative to a transform
*/
struct ye_component_rigidbody {
    bool active;

    struct p2d_object p2d_object;

    enum ye_rb_collider_type type;
};

YE_API void ye_add_rigidbody_component(struct ye_entity *entity, struct p2d_object p2d_object);

YE_API void ye_remove_rigidbody_component(struct ye_entity *entity);

#endif