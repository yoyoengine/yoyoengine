/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_DEBUG_RENDERER_H
#define YE_DEBUG_RENDERER_H

#include <yoyoengine/export.h>

#include <stdbool.h>

#include <SDL.h>

enum ye_debug_render_type {
    YE_DEBUG_RENDER_FUNCTION,
    YE_DEBUG_RENDER_LINE,
    YE_DEBUG_RENDER_CIRCLE,
    YE_DEBUG_RENDER_RECT,
    YE_DEBUG_RENDER_POINT
};

// debug render immediate node
struct ye_debug_render_immediate_node {
    enum ye_debug_render_type type;
    SDL_Color color;
    union {

        SDL_Rect rect;
        
        SDL_Point point;
        
        struct {
            SDL_Point start;
            SDL_Point end;
        } line;
        
        struct {
            SDL_Point center;
            int radius;
        } circle;
    } data;
    int width;
    struct ye_debug_render_immediate_node * next;
};

// additional render callback node
struct ye_additional_render_callback_node {
    bool persistent;
    void (*fn)(void);
    struct ye_additional_render_callback_node * next;
};

/*
    immediate API
*/

/**
 * @brief Renders (THIS FRAME) a line from (x1, y1) to (x2, y2) with the specified color
 * 
 * @param x1 The x coordinate of the start of the line
 * @param y1 The y coordinate of the start of the line
 * @param x2 The x coordinate of the end of the line
 * @param y2 The y coordinate of the end of the line
 * @param color The color of the line
 * @param width The width of the line
*/
YE_API void ye_debug_render_line(int x1, int y1, int x2, int y2, SDL_Color color, int width);

/**
 * @brief Renders (THIS FRAME) a rectangle at (x, y) with width w and height h with the specified color
 * 
 * @param x The x coordinate of the top left corner of the rectangle
 * @param y The y coordinate of the top left corner of the rectangle
 * @param w The width of the rectangle
 * @param h The height of the rectangle
 * @param color The color of the rectangle
 * @param width The width of the line
*/
YE_API void ye_debug_render_rect(int x, int y, int w, int h, SDL_Color color, int width);

/**
 * @brief Renders (THIS FRAME) a circle at (x, y) with the specified radius and color
 * 
 * @param x The x coordinate of the center of the circle
 * @param y The y coordinate of the center of the circle
 * @param radius The radius of the circle
 * @param color The color of the circle
 * @param width The width of the line
*/
YE_API void ye_debug_render_circle(int x, int y, int radius, SDL_Color color, int width);

/**
 * @brief Renders (THIS FRAME) a point at (x, y) with the specified color
 * 
 * @param x The x coordinate of the point
 * @param y The y coordinate of the point
 * @param color The color of the point
 * @param width The width of the line
*/
YE_API void ye_debug_render_point(int x, int y, SDL_Color color, int width);

/*
    additional render callback API
*/

/**
 * @brief Pushes a rendering function to be called every frame
 * 
 * @param fn The function to be called
 * @param persistent If the function should be unregistered after a scene load
*/
YE_API void ye_additional_render_fn_push(void (*fn)(void), bool persistent);

/*
    Engine impl
*/

// renders all immediate and additional render calls
YE_API void ye_debug_renderer_render();

// cleans up all immediate and additional render calls
YE_API void ye_debug_renderer_cleanup(bool remove_persistant);

#endif // YE_DEBUG_RENDERER_H