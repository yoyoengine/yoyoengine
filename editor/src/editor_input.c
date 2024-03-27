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

#include <math.h>

#include <yoyoengine/yoyoengine.h>

#include "editor.h"
#include "editor_serialize.h"
#include "editor_build.h"
#include "editor_selection.h"

/*
    Getting this equation right was exponentially more difficult than you could possibly imagine
*/
void editor_update_mouse_world_pos(int x, int y){
    // y = y - 35; // account for the menu bar

    float scaleX = (float)YE_STATE.engine.screen_width / (float)YE_STATE.engine.target_camera->camera->view_field.w;
    float scaleY = (float)YE_STATE.engine.screen_height / (float)YE_STATE.engine.target_camera->camera->view_field.h;
    struct ye_rectf campos = ye_get_position(YE_STATE.engine.target_camera, YE_COMPONENT_CAMERA);
    mouse_world_x = ((x / scaleX) + campos.x);
    mouse_world_y = ((y / scaleY) + campos.y);
}

/*
    Checks if the mouse is within the editor viewport
*/
bool is_hovering_editor(int x, int y){
    return (x > 0 && x < screenWidth / 1.5 &&
            y > 35 && y < 35 + screenHeight / 1.5);
}

float original_pan_x, original_pan_y, original_cam_x, original_cam_y;
float camera_zoom = 1.0;
float camera_zoom_sens = 1.0;
void editor_input_panning(SDL_Event event){
    // get the current mouse position in window
    int _mx, _my; SDL_GetMouseState(&_mx, &_my);
    float mx = _mx;
    float my = _my;

    // use the camera size to calculate the world coordinates of the mouse position
    float scaleX = (float)YE_STATE.engine.screen_width / (float)YE_STATE.engine.target_camera->camera->view_field.w;
    float scaleY = (float)YE_STATE.engine.screen_height / (float)YE_STATE.engine.target_camera->camera->view_field.h;
    struct ye_rectf campos = ye_get_position(YE_STATE.engine.target_camera, YE_COMPONENT_CAMERA);
    mx = ((mx / scaleX) + campos.x);
    my = ((my / scaleY) + campos.y);

    // if middle mouse clicked down, initialize panning
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_MIDDLE) {
            if (!lock_viewport_interaction) {
                editor_panning = true;
                
                original_pan_x = mx;
                original_pan_y = my;
                original_cam_x = editor_camera->transform->x;
                original_cam_y = editor_camera->transform->y;

                pan_start = (SDL_Point){original_pan_x, original_pan_y};
                pan_end = (SDL_Point){original_pan_x, original_pan_y};

                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL));
            }
        }
    }
    // if middle mouse released, stop panning
    else if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_MIDDLE) {
            editor_panning = false;
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));

            // temp: pan to result now
            float dx = mx - original_pan_x;
            float dy = my - original_pan_y;
            editor_camera->transform->x = original_cam_x - dx;
            editor_camera->transform->y = original_cam_y - dy;
        }
    }
    // mouse movement, if we are panning move the camera
    else if (event.type == SDL_MOUSEMOTION) {
        if (editor_panning) {
            float dx = mx - original_pan_x;
            float dy = my - original_pan_y;

            pan_end = (SDL_Point){mx, my};
        }
    }
    else if (event.type == SDL_MOUSEWHEEL && is_hovering_editor(mx, my) && !lock_viewport_interaction)
    {
        float dt = ye_delta_time();
        float zoom_factor = 0.1f; // Adjust this value to control the zoom speed

        if (event.wheel.y > 0)
        {
            camera_zoom *= pow(200.0f, zoom_factor * dt);
            // Ensure camera_zoom doesn't go above 10x
            camera_zoom = fmin(camera_zoom, 10.0f);
        }
        else if (event.wheel.y < 0)
        {
            camera_zoom /= pow(200.0f, zoom_factor * dt);
            // Ensure camera_zoom doesn't go below 0.1x
            camera_zoom = fmax(camera_zoom, 0.1f);
        }

        // save the size before
        float old_w = editor_camera->camera->view_field.w;
        float old_h = editor_camera->camera->view_field.h;

        // update the camera zoom
        editor_camera->camera->view_field.w = screenWidth / camera_zoom;
        editor_camera->camera->view_field.h = screenHeight / camera_zoom;

        // offset position to keep center of the screen in the same place TODO: scale this with viewport size
        editor_camera->transform->x -= (editor_camera->camera->view_field.w - old_w) / 2;
        editor_camera->transform->y -= (editor_camera->camera->view_field.h - old_h) / 2;

        editor_update_mouse_world_pos(mx, my);
    }
}

/*
    Any Misc items that need to happen on editor input
*/
void editor_input_misc(SDL_Event event){
    // update the mouse world position
    if(event.type == SDL_MOUSEMOTION){
        editor_update_mouse_world_pos(event.motion.x, event.motion.y);
    }

    // window events //
    if(event.type == SDL_WINDOWEVENT){
        if(event.window.event == SDL_WINDOWEVENT_RESIZED){
            screenWidth = event.window.data1;
            screenHeight = event.window.data2;

            // if we have resized and we are in the editor, we need to update the editor camera
            editor_camera->camera->view_field.w = screenWidth / camera_zoom;
            editor_camera->camera->view_field.h = screenHeight / camera_zoom;
        }
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
                editor_write_scene_to_disk(ye_path_resources(YE_STATE.runtime.scene_file_path));
                editor_saved();
            }
            break;
        case SDLK_r:
            // CTRL + SHIFT + R ->= reload the current scene
            if (event.key.keysym.mod & KMOD_CTRL && event.key.keysym.mod & KMOD_SHIFT)
            {
                ye_logf(debug,"Editor Reloading Scene.\n");
                ye_reload_scene();
                editor_re_attach_ecs();
                editor_saved();
            }
            // CTRL + R ->= build and run the project
            else if (event.key.keysym.mod & KMOD_CTRL)
            {
                editor_build_and_run();
            }
            break;
        case SDLK_BACKQUOTE:
            // if we opened or closed the console, lock accordingly
            lock_viewport_interaction = ui_component_exists("console");
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
    editor_selection_handler(event); // editor_selection.c
    editor_input_misc(event);
    editor_input_shortcuts(event);
}