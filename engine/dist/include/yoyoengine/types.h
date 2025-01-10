/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_TYPES_H
#define YE_TYPES_H

/**
 * @brief A rectangle made up of floats.
 */
struct ye_rectf {
    float x, y, w, h;
};

/**
 * @brief A 2D vector made up of floats.
 */
struct ye_vec2f {
    float x, y;
};

/**
 * @brief A 2D point made up of floats.
 */
struct ye_pointf {
    float x, y;
};

/**
 * @brief A rectangle made up float points.
 */
struct ye_point_rectf {
    struct ye_pointf verticies[4];
};

#endif // YE_TYPES_H