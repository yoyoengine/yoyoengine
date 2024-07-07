/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef EDITOR_SELECTION_H
#define EDITOR_SELECTION_H

/*
    The purpose of this header is to provide functionality
    involving the editor selection datastructures and algorithms
    ex:
    - multi select
    - click and drag
    - rectangle selection
*/

#include <stdbool.h>

#include <yoyoengine/yoyoengine.h>

extern bool editor_draw_drag_rect;

struct editor_selection_node{
    struct ye_entity * ent;
    struct editor_selection_node * next;
};

extern struct editor_selection_node * editor_selections;
extern int num_editor_selections;

#define editor_current_selection (editor_selections ? editor_selections->ent : NULL)

/**
 * @brief The entry point for all selection event parsing
 * 
 * @param event An SDL event that just occurred
 */
void editor_selection_handler(SDL_Event event);

/**
 * @brief Renders the selection rectangles for all selected entities
*/
void editor_render_selection_rects();

/**
 * @brief Deselects all selected entities in the editor
 */
void editor_deselect_all();

/**
 * @brief Checks if an entity is selected in the editor
 * 
 * @param ent The entity to check if it is selected
 * @return true if the entity is selected, false otherwise
 */
bool editor_is_selected(struct ye_entity * ent);

/**
 * @brief Removes an entity from the editor selection
 * 
 * @param ent The entity to remove from the selection
 */
void editor_deselect(struct ye_entity * ent);

/**
 * @brief Adds an entity to the editor selection
 * 
 * @param ent The entity to add to the selection
 */
void editor_select(struct ye_entity * ent);

#endif