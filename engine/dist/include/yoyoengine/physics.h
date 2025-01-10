/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_PHYSICS_H
#define YE_PHYSICS_H

#include <p2d/p2d.h>

#include <yoyoengine/physics.h>

YE_API void ye_physics_collision_callback(struct p2d_cb_data* data);

YE_API void ye_physics_trigger_callback(struct p2d_cb_data* data);

#endif // YE_PHYSICS_H