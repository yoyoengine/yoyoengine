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

#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "graphics.h"

/*
    Define some important constants to the engine
*/
#define YE_ENGINE_VERSION "v0.0.1 dev"
#define YE_ENGINE_SCENE_VERSION 0 // version 0 of scene files

#ifdef YE_BUILD_MODE
    /*
        Do things like limit the debug log output unless override is set
    */
#endif
/*
    Reserve an internal font and color for the engine to use rendering
    overlays such as the fpsCounter. Font NULL until initialized in init()
*/
extern SDL_Color *pEngineFontColor;
extern TTF_Font *pEngineFont;

extern struct engine_data engine_state;

extern struct engine_runtime_data engine_runtime_state;

// struct to hold screen points
struct ScreenSize {
    int width;
    int height;
};

// helper function to return ScreenSize struct from getting the screensize with SDL2
// TODO: remove me?
struct ScreenSize getScreenSize();

char *ye_get_resource_static(const char *sub_path);

char* ye_get_engine_resource_static(const char *sub_path);

/*
    constructor for engine (any fields can be left blank for defaults)

    There are some private fields at the bottom that can TECHNICALLY be overridden
    by the game for some extra control (hence why they are exposed here)
*/
struct engine_data {
    int screen_width;
    int screen_height;
    int volume;
    int window_mode;
    int framecap;

    /*
        0 - debug and higher
        1 - info and higher
        2 - warning and higher
        3 - error and higher
        4 - nothing
    */
    int log_level;
    
    bool debug_mode; // does not need override
    bool skipintro; // does not need override
    bool editor_mode; // does not need override

    char *window_title;
    char *icon_path;
    char *engine_resources_path;
    char *game_resources_path;
    char *log_file_path;

    // added the above for editor: allow overriding the path entirely, not assuming it starts from the executable

    // void pointer to a function that will be called to handle game input (takes in a SDL key)
    void (*handle_input)(SDL_Event event);

    /*
        State that is reserved to the engine, but 
        could technically be mutated by the game
    */
    bool paintbounds_visible;
    bool colliders_visible;
    bool metrics_visible;
    bool console_visible;
    bool scene_camera_bounds_visible;

    bool freecam_enabled;

    struct ye_entity *target_camera;

    /*
        By default, the viewport will render from its actual perspective,
        setting this to true will disable this behavior and simply paint any object in the
        cameras view cone to the viewport actual position
    */
    bool stretch_viewport;
};

/*
    Struct to hold and persist globals to the engine (for ease documentation and use)
    This will be instantiated to one global variable in engine.c and used throughout the engine
*/
struct engine_runtime_data {
    int fps;
    int frame_time;
    int entity_count;
    int painted_entity_count;
    int log_line_count;
    int audio_chunk_count;

    char *scene_name;

    // EDITOR SPECIFIC FIELDS BELOW

    struct ye_entity *scene_default_camera; // this is set when the scene has its own declared camera, which cannot override the editor camera. we can draw a viewport for this cam

    bool display_names;

    bool editor_display_viewport_lines;

    struct ye_entity *selected_entity;
};

void ye_process_frame();

float ye_get_delta_time();

void ye_update_resources(char *path);

/*
    entry point to the engine, initializes all subsystems
    Will look at ./settings.yoyo for initialization parameters (if empty or nonexistant will use defaults)
*/
void ye_init_engine();

// shutdown point for engine, shuts down all subsystems
void ye_shutdown_engine();

#endif