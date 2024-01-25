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

/**
    @file engine.h
    @brief The header for the engine entry point, contains state definitions.
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
#define YE_ENGINE_STYLES_VERSION 0 // version 0 of style files

#ifdef YE_BUILD_MODE
    /*
        Do things like limit the debug log output unless override is set
    */
#endif


extern struct ye_engine_state YE_STATE;

/**
 * @brief A struct that defines a screen size by width and height.
 */
struct ScreenSize {
    int width;
    int height;
};

/**
 * @brief !!!THIS IS NOT FOR USE AT RUNTIME!!! Returns a string of the full path (os specific) to the engine resource you have referenced.
 * 
 * Behaves the same as @ref ye_path_resource, but is meant for accessing engine resources.
 * 
 * @param sub_path The relative path (or just name with extension) of the engine resource you wish to access.
 * @return char* The absolute path to the engine resource.
 */
char* ye_get_engine_resource_static(const char *sub_path);

/**
 * @brief Returns a pointer to a string in engine memory that contains the absolute path relative to the executable of the item you have specified.
 * 
 * @param path The path relative to the executable you want to path to
 * @return char* A reference to the built string
 * 
 * NOTE: this only persists until the next call to this function. You must make your own copies if you need it to stick around
 */
char * ye_path(char * path);

/**
 * @brief THIS IS NOT FOR USE WITH RUNTIME RESOURCES. This is for editor resources only.
 * 
 * @param path The path relative to the project resources folder
 * @return char* The constructed path
 */
char * ye_path_resources(char * path);

/**
 * @brief This struct holds references to callbacks declared and assigned through C scripting.
 */
struct ye_engine_callbacks {
    void (*input_handler)(SDL_Event event);
    void (*pre_frame)();
    void (*post_frame)();
};

/**
 * @brief The struct that defines the configuration of the engine core specifically.
*/
struct ye_engine_config {
    /*
        Window Properties
        Do not update in realtime, only on init OR recompute functions
    */
    int screen_width;
    int screen_height;
    int volume;
    int window_mode;
    int framecap;
    char *window_title;
    char *icon_path;
    
    /*
        Some fields that handle pillarboxing and letterboxing
    */
    bool stretch_resolution;    // i dont even remember what stretch viewport does anymore... editor only? || this controls whether we even want to stretch
    bool need_boxing;           // if we need to pillarbox or letterbox
    SDL_Rect letterbox;        // the letterbox rect

    /*
        0 - debug and higher
        1 - info and higher
        2 - warning and higher
        3 - error and higher
        4 - nothing
    */
    int log_level;

    /*
        Provides some internal override behavior over defaults.
        Ex: logging will print to stdout before init.
    */
    bool debug_mode;

    /*
        TODO: remove me?
    */
    bool skipintro;

    /*
        Allocated strings for resource accessing paths.
    */
    char *engine_resources_path;
    char *game_resources_path;
    char *log_file_path;

    /*
        Controls which camera the scene is rendered from the perspective of.
    */
    struct ye_entity *target_camera;

    /*
        By default, the viewport will render from its actual perspective,
        setting this to true will disable this behavior and simply paint any object in the
        cameras view cone to the viewport actual position
    */
    bool stretch_viewport;

    /*
        The font and color used for when the engine needs to render text
        but is missing a font or color from. This will be automatically freed
        at the end of the engine's lifecycle, so if you replace these manually please
        free the old pointers.
    */
    SDL_Color *pEngineFontColor;
    TTF_Font *pEngineFont;

    // the nuklear context
    struct nk_context *ctx; // TODO: should maybe be moved to runtime but idgaf rn

    /*
        The callbacks struct that holds all the callbacks
        that can be assigned through C scripting
    */
    struct ye_engine_callbacks callbacks;
};

/**
 * @brief The struct that defines the configuration of the editor specifically.
 */
struct ye_editor_config {
    /*
        Controls whether the engine is in "editor mode"
        This has implications for how rendering is handled and which systems are enabled.
    */
    bool editor_mode;

    /*
        A whole bunch of editor flags
        that can also be changed at runtime
        to debug or potentially for visual effect
    */
    bool paintbounds_visible;
    bool colliders_visible;
    bool display_names;
    bool freecam_enabled;
    /*
        Only work with editor_mode enabled:
    */
    bool editor_display_viewport_lines;
    bool scene_camera_bounds_visible;

    /*
        Used to track the selected editor entity
    */
    struct ye_entity *selected_entity;

    /*
        Only used in editor mode to designate the camera the scene file has marked
        as default, so when we replace the rendering camera with the editor camera
        we still know which camera the scene file has marked as default

        ex: Draw camera viewport outline
    */
    struct ye_entity *scene_default_camera;
};

/**
 * @brief The struct that defines the runtime data of the engine.
 */
struct ye_runtime_data {
    /*
        Some variables tracking things we might
        be interested in at any given time:
    */
    int entity_count;           // scene entities
    int painted_entity_count;   // scene entities actually painted
    int fps;                    // our current fps (updated every frame)
    
    int paint_time;             // time in ms it took to paint the last frame
    int frame_time;             // overall time in ms it took to process the last frame (the delay included)
    int input_time;             // time in ms it took to process the input for the last frame
    int physics_time;           // time in ms it took to process the physics for the last frame
    float delta_time;           // the delta time in SECONDS between the last frame and the current frame
    
    int log_line_count;         // the number of lines in the log file
    int audio_chunk_count;      // the number of audio chunks currently allocated and playing
    
    char *scene_name;           // TODO: store current scene path for reloading in editor?
    char *scene_file_path;      // the path to the open scene file

    int error_count;            // tracks the number of error level logs that have occurred
    int warning_count;          // same but for warnings

    /*
        References to the current SDL window and renderer
    */
    SDL_Window *window;
    SDL_Renderer *renderer;
}; // TODO: move a bunch more information here, audio capacity comes to mind. expose some stuff here just for usage.

/*
    Struct that defines the state for the entire core engine, as well as some important editor state
    that is not necessarily part of the engine, but is important to the editor injecting its
    own behavior into the core and renderer
*/
/**
 * @brief The struct that defines the state of the entire engine, editor and runtime.
 * 
 * This contains references to @ref ye_engine_config, @ref ye_runtime_data, and @ref ye_editor_config.
*/
struct ye_engine_state {
    struct ye_engine_config engine;
    struct ye_runtime_data runtime;
    struct ye_editor_config editor;
};

/**
 * @brief The master function that returns control to the engine to process the next frame.
 * 
 * Will invoke many functions to process the next frame, including:
 * - @ref ui_handle_input
 * - @ref ye_system_physics
 * - @ref ye_render_all
 * 
 * As well as many other misc operations, such as updating the current delta in @ref ye_runtime_data
 */
void ye_process_frame();

/**
 * @brief Returns the current delta time in milliseconds.
 * 
 * Will pull the current delta time from @ref ye_runtime_data.
 * This exists just for ease of use and potentially wrapping later on.
 * 
 * @return float 
 */
float ye_delta_time();

/**
 * @brief Updates the engines resources path to the new path provided.
 * 
 * The only use of this at the moment is for the editor to switch into the game its
 * trying to edit after it loads all its necessary editor specific resources.
 * 
 * @param path The (absolute) path to the new resources folder.
 */
void ye_update_base_path(char *path);

/*
    entry point to the engine, initializes all subsystems
    Will look at ./settings.yoyo for initialization parameters (if empty or nonexistant will use defaults)
*/

/**
 * @brief The entry point to the engine, initializes all subsystems.
 * 
 * You will need to have a 'settings.yoyo' file at the root of your project to initialize the engine. 
 */
void ye_init_engine();

/**
 * @brief Shuts down the engine and all subsystems.
 * 
 * Does not shut down your own game logic, you will need a game loop to do that.
 * If you are using the editor and the default entry point, both will be handled for you.
 */
void ye_shutdown_engine();

#endif