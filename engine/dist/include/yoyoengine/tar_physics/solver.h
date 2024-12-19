/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_SOLVER_H
#define YE_SOLVER_H

#include <stdbool.h>

#include <yoyoengine/export.h>

#include <yoyoengine/ecs/ecs.h>


YE_API bool ye_detect_circle_circle_collision(struct ye_vec2f pos1, float radius1, struct ye_vec2f pos2, float radius2);

YE_API bool ye_detect_circle_rect_collision(struct ye_vec2f circle_pos, float circle_radius, struct ye_rectf rect, float rotation);

YE_API bool ye_detect_rect_rect_collision(struct ye_point_rectf rect1, struct ye_point_rectf rect2);

#endif // YE_SOLVER_H