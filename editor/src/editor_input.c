/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

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

#include <yoyoengine/yoyoengine.h>
#include "editor.h"

bool is_hovering_editor(int x, int y){
    return (x > 0 && x < screenWidth / 1.5 &&
            y > 0 && y < screenHeight / 1.5);
}

float camera_zoom = 1.0;
float camera_zoom_sens = 1.0;
void editor_input_panning(SDL_Event event){
    // initial right click down, initialize panning
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_RIGHT) {
            if (is_hovering_editor(event.button.x, event.button.y) &&
                !lock_viewport_interaction)
            {
                dragging = true;
                last_x = event.button.x;
                last_y = event.button.y;
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL));
            }
        }
    }
    // release right click, stop panning TODO: add checks for if we are even panning here
    else if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_RIGHT)
        {
            dragging = false;
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        }
    }
    // mouse movement, if we are panning then move the camera
    else if (event.type == SDL_MOUSEMOTION) {
        if (dragging)
        {
            int dx = event.motion.x - last_x;
            int dy = event.motion.y - last_y;
            editor_camera->transform->rect.x -= dx;
            editor_camera->transform->rect.y -= dy;
            last_x = event.motion.x;
            last_y = event.motion.y;
        }
    }
    // camera zooming TODO: disabled. Im losing my mind.
    // else if (event.type == SDL_MOUSEWHEEL)
    // {
    //     float dt = (float)engine_runtime_state.frame_time / 1000.0;
    //     // float zoom_sensitive = 0.1;
    //     if (event.wheel.y > 0)
    //    {
    //         float new_zoom = camera_zoom + camera_zoom_sens * dt;
    //         printf("new zoom: %f\n", new_zoom);
    //         // 10x zoom is the max
    //         if(camera_zoom < 10){
    //             editor_camera->camera->view_field.w = screenWidth / new_zoom;
    //             editor_camera->camera->view_field.h = screenHeight / new_zoom;

    //             camera_zoom = new_zoom;
    //         }
    //     }
    //     else if (event.wheel.y < 0)
    //     {
    //         float new_zoom = camera_zoom - camera_zoom_sens * dt;
    //         printf("new zoom: %f\n", new_zoom);
    //         // 0.1x zoom is the min
    //         if(camera_zoom > 0.1){
    //             editor_camera->camera->view_field.w = screenWidth / new_zoom;
    //             editor_camera->camera->view_field.h = screenHeight / new_zoom;

    //             camera_zoom = new_zoom;
    //         }
    //     }
    // }
}

/*
    Selecting entities and manipulating them
*/
void editor_input_selection(SDL_Event event){
    if (event.button.button == SDL_BUTTON_LEFT) {   
        // dont do anything if we are outside the viewport
        if (is_hovering_editor(event.button.x,event.button.y) &&
            !lock_viewport_interaction){

            bool nothing = true;
            // check what entity we clicked over, we clicked on an entity (thats ignoring our current selection) we should change the current selection, else we deselect
            struct ye_entity_node *clicked_entity = entity_list_head;
            while (clicked_entity != NULL)
            {
                if(clicked_entity->entity == engine_runtime_state.selected_entity)
                {
                    clicked_entity = clicked_entity->next;
                    continue;
                }

                if (ye_point_in_rect(mouse_world_x, mouse_world_y, ye_convert_rectf_rect(clicked_entity->entity->transform->bounds))) // TODO: bounds vs rect here
                {
                    // we clicked on this entity
                    if (clicked_entity != engine_runtime_state.selected_entity)
                    {
                        engine_runtime_state.selected_entity = clicked_entity->entity;
                    }
                    nothing = false;
                    break;
                }
                clicked_entity = clicked_entity->next; // TODO: solve world mouse point
            }
            if(nothing){
                engine_runtime_state.selected_entity = NULL;
            }
        }
    }
}

/*
    Any Misc items that need to happen on editor input
*/
void editor_input_misc(SDL_Event event){
    // update the mouse world position
    if(event.type == SDL_MOUSEMOTION){
        float scaleX = (float)engine_state.screen_width / (float)engine_state.target_camera->camera->view_field.w;
        float scaleY = (float)engine_state.screen_height / (float)engine_state.target_camera->camera->view_field.h;

        mouse_world_x = ((event.motion.x + editor_camera->transform->rect.x) / scaleX);
        mouse_world_y = ((event.motion.y + editor_camera->transform->rect.y) / scaleY);
    }
}

/*
    Any keyboard shortcuts for the editor
*/
void editor_input_shortcuts(SDL_Event event){
    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_s:
            // CTRL + S ->= save the scene
            if (event.key.keysym.mod & KMOD_CTRL)
            {
                // TODO: save the scene
                printf("TODO: IMPLEMENT SAVING SCENE. -> Ctrl+S pressed\n");
            }
            break;
        default:
            break;
        }
    }
}

/*
    Registered input handler

    TODO:
    - zoom in should center on the mouse or the center of the screen
*/
void editor_handle_input(SDL_Event event) {
    if (event.type == SDL_QUIT)
        quit = true;

    // misc input handling
    editor_input_panning(event);
    editor_input_selection(event);
    editor_input_misc(event);
    editor_input_shortcuts(event);
}