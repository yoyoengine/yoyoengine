/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file button.h
 * @brief ECS Button component
 */

#ifndef YE_BUTTON_H
#define YE_BUTTON_H

#include <yoyoengine/export.h>

#include <stdbool.h>

#include <SDL.h>

#include <yoyoengine/utils.h>
#include <yoyoengine/ecs/ecs.h>

struct ye_component_button {
    bool active;
    bool relative;
    struct ye_rectf rect;

    // this is state that the system will track
    bool is_hovered;    // tracks mouse over
    bool is_pressed;    // tracks mouse down and holding
    bool is_clicked;    // tracks mouse up "clicked"

    // private state
    bool _was_pressed;  // tracks mouse down across event loop
};

YE_API void ye_add_button_component(struct ye_entity *entity, struct ye_rectf rect);

YE_API void ye_remove_button_component(struct ye_entity *entity);

/**
 * @brief Iterates over the button list and updates button internal state.
 * 
 * @param event The SDL event to be processed.
 */
YE_API void ye_system_button(SDL_Event event);

/*
    API FOR ACCESSING STATE:
*/

/**
 * @brief Checks whether an entity's button is hovered.
 * 
 * @param entity The entity to be checked.
 */
YE_API bool ye_button_hovered(struct ye_entity *entity);

/**
 * @brief Checks whether an entity's button is clicked.
 * 
 * @param entity The entity to be checked.
 */
YE_API bool ye_button_clicked(struct ye_entity *entity);

/**
 * @brief Checks whether an entity's button is pressed.
 * 
 * @param entity The entity to be checked.
 */
YE_API bool ye_button_pressed(struct ye_entity *entity);

#endif