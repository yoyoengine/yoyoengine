/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/**
 * @file button.h
 * @brief ECS Button component
 */

#ifndef YE_BUTTON_H
#define YE_BUTTON_H

#include <yoyoengine/yoyoengine.h>

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

void ye_add_button_component(struct ye_entity *entity, struct ye_rectf rect);

void ye_remove_button_component(struct ye_entity *entity);

/**
 * @brief Iterates over the button list and updates button internal state.
 * 
 * @param event The SDL event to be processed.
 */
void ye_system_button(SDL_Event event);

/*
    API FOR ACCESSING STATE:
*/

/**
 * @brief Checks whether an entity's button is hovered.
 * 
 * @param entity The entity to be checked.
 */
bool ye_button_hovered(struct ye_entity *entity);

/**
 * @brief Checks whether an entity's button is clicked.
 * 
 * @param entity The entity to be checked.
 */
bool ye_button_clicked(struct ye_entity *entity);

/**
 * @brief Checks whether an entity's button is pressed.
 * 
 * @param entity The entity to be checked.
 */
bool ye_button_pressed(struct ye_entity *entity);

#endif