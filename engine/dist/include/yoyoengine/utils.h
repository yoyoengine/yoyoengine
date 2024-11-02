/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
    @file utils.h
    @brief Provides some utility functions for the engine.
*/

#ifndef UTILS_H
#define UTILS_H

#include <yoyoengine/export.h>

#include <stdbool.h>

#include <SDL.h>

#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/logging.h>

/**
 * @brief An enum that defines the alignment of an entity within its bounds.
*/
enum ye_alignment {
    YE_ALIGN_TOP_LEFT,  YE_ALIGN_TOP_CENTER,    YE_ALIGN_TOP_RIGHT,
    YE_ALIGN_MID_LEFT,  YE_ALIGN_MID_CENTER,    YE_ALIGN_MID_RIGHT,
    YE_ALIGN_BOT_LEFT,  YE_ALIGN_BOT_CENTER,    YE_ALIGN_BOT_RIGHT,
    YE_ALIGN_STRETCH
};

struct ye_rectf; // this forward declaration ignores a bunch of compiler warnings, and i have no clue why

/**
 * @brief Clamps a value between a minimum and maximum.
 * 
 * @param value The value to clamp.
 * @param min The minimum value.
 * @param max The maximum value.
 * @return int The clamped value.
 */
YE_API int ye_clamp(int value, int min, int max);

/**
 * @brief Aligns a rectangle within another rectangle (by modifying the passed values).
 * 
 * @param bounds_f The bounds to align within.
 * @param obj_f The object to align.
 * @param alignment The alignment to use.
 * @param center The center point of the object (to be written to).
 */
YE_API void ye_auto_fit_bounds(struct ye_rectf* bounds_f, struct ye_rectf* obj_f, enum ye_alignment alignment, SDL_Point* center, bool should_grow_to_fit);

/**
 * @brief Returns the size of a texture as an SDL_Rect
 * 
 * @param pTexture The texture to query for its actual size
 * @return SDL_Rect The returned size as an SDL_Rect
 */
YE_API SDL_Rect ye_get_real_texture_size_rect(SDL_Texture *pTexture);

/**
 * @brief A rectangle made up of floats.
 */
struct ye_rectf {
    float x, y, w, h;
};

/**
 * @brief Convert a floating rectangle to an integer rectangle.
 * 
 * @param rect The floating rectangle to convert.
 * @return SDL_Rect The converted rectangle.
 */
YE_API SDL_Rect ye_convert_rectf_rect(struct ye_rectf rect);

/**
 * @brief Convert an integer rectangle to a floating rectangle.
 * 
 * @param rect The integer rectangle to convert.
 * @return struct ye_rectf The converted rectangle.
 */
YE_API struct ye_rectf ye_convert_rect_rectf(SDL_Rect rect);

/**
 * @brief A collection of enums that define the different types of components.
 */
enum ye_component_type {
    YE_COMPONENT_TRANSFORM,
    YE_COMPONENT_RENDERER,
    YE_COMPONENT_PHYSICS,
    YE_COMPONENT_COLLIDER,
    YE_COMPONENT_LUA_SCRIPT,
    YE_COMPONENT_AUDIOSOURCE,
    YE_COMPONENT_CAMERA,
    YE_COMPONENT_TAG,
    YE_COMPONENT_BUTTON
};

/**
 * @brief Returns the angle between two points.
 * 
 * @param x1 The x position of the first point.
 * @param y1 The y position of the first point.
 * @param x2 The x position of the second point.
 * @param y2 The y position of the second point.
 * @return float The angle between the two points.
*/
YE_API float ye_angle(float x1, float y1, float x2, float y2);

/**
 * @brief Returns the distance between two points.
 * 
 * @param x1 The x position of the first point.
 * @param y1 The y position of the first point.
 * @param x2 The x position of the second point.
 * @param y2 The y position of the second point.
 * @return float The distance between the two points.
*/
YE_API float ye_distance(float x1, float y1, float x2, float y2);

/**
 * @brief Returns the position of a component on an entity.
 * 
 * Passing YE_COMPONENT_TRANSFORM will return the position of the entity transform, whereas passing
 * a component type will take into account the position of that component as a relative or absolute
 * position.
 * 
 * Ex: if you have a renderer on a entity, but the renderer is absolute at (100, 100), and the entity
 * transform is at (200, 200), then passing YE_COMPONENT_RENDERER will return (200, 200), whereas
 * passing YE_COMPONENT_TRANSFORM will return (100, 100).
 * If that same renderer was relative, then passing YE_COMPONENT_RENDERER would return (200, 200).
 * 
 * @param entity The entity to get the position of.
 * @param type The type of component to get the position of.
 * @return struct ye_rectf The position of the component.
 */
YE_API struct ye_rectf ye_get_position(struct ye_entity *entity, enum ye_component_type type);

/**
 * @brief Wrapper for @ref ye_get_position that returns an SDL_Rect instead of a ye_rectf.
 * 
 * @param entity The entity to get the position of.
 * @param type The type of component to get the position of.
 * @return SDL_Rect The position of the component.
 */
YE_API SDL_Rect ye_get_position_rect(struct ye_entity *entity, enum ye_component_type type);

/**
 * Draws a "thick point" (a filled square) centered on the given coordinates.
 * 
 * @param renderer The SDL_Renderer to use for drawing.
 * @param x The x-coordinate of the center of the thick point.
 * @param y The y-coordinate of the center of the thick point.
 * @param thickness The side length of the square, which determines the "thickness" of the point.
 */
YE_API void ye_draw_thick_point(SDL_Renderer *renderer, int x, int y, int thickness);

/**
 * @brief Draws a circle using SDL_RenderDrawPoint.
 * 
 * @param renderer The renderer to draw the circle on.
 * @param center_x The x position of the center of the circle.
 * @param center_y The y position of the center of the circle.
 * @param radius The radius of the circle.
 * @param thickness The thickness of the circle.
 */
YE_API void ye_draw_circle(SDL_Renderer * renderer, int32_t center_x, int32_t center_y, int32_t radius, int thickness);

/**
 * @brief Returns the world coordinates of a click on the window.
 * 
 * @param x A pointer to an int holding the SDL event x position.
 * @param y A pointer to an int holding the SDL event y position.
 * 
 * @returns Will modify the int pointers passed in to reflect the new world position.
 * 
 * @note offsets based on letterboxing, target scaling, and camera positioning
 */
YE_API void ye_get_mouse_world_position(int *x, int *y);

/**
 * @brief Checks if a specified component exists on an entity
 * 
 * @param entity The entity to check
 * @param type The type of component to check for
 * @return bool Sucess or failure
*/
YE_API bool ye_component_exists(struct ye_entity *entity, enum ye_component_type type);

/**
 * @brief Draws a line with a specified thickness.
 * 
 * @param renderer The renderer to draw the line on.
 * @param x1 The x position of the start of the line.
 * @param y1 The y position of the start of the line.
 * @param x2 The x position of the end of the line.
 * @param y2 The y position of the end of the line.
 * @param thickness The thickness of the line.
 * @param color The color of the line.
 * @return bool Success or failure.
*/
YE_API bool ye_draw_thick_line(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, int thickness, SDL_Color color);

/**
 * @brief Draws a rectangle with a specified thickness.
 * 
 * @param renderer The renderer to draw the rectangle on.
 * @param x The x position of the rectangle.
 * @param y The y position of the rectangle.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param thickness The thickness of the rectangle.
 * @param color The color of the rectangle.
*/
YE_API void ye_draw_thick_rect(SDL_Renderer *renderer, float x, float y, float w, float h, int thickness, SDL_Color color);

#endif