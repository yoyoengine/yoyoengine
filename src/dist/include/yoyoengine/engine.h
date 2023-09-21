#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "graphics.h"

#define intFail -666 // define global engine int fail num

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

    char *window_title;
    char *icon_path;
    char *engine_resources_path;
    char *game_resources_path;
    char *log_file_path;

    // overrides - this is confusing with what needs overridden and what doesnt TODO:
    bool override_screen_width;
    bool override_screen_height;
    bool override_volume;
    bool override_window_mode;
    bool override_framecap;
    bool override_log_level;
    // skip boolean overrides...
    bool override_window_title;
    // skip path overrides... (this is implicitly deducted by if the fields are NULL or not)

    // void pointer to a function that will be called to handle game input (takes in a SDL key)
    void (*handle_input)(SDL_Event event);

    /*
        State that is reserved to the engine, but 
        could technically be mutated by the game
    */
    bool paintbounds_visible;
    bool metrics_visible;
    bool console_visible;

    bool freecam_enabled;

    struct ye_entity *target_camera;
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
};

void ye_process_frame();

float ye_get_delta_time();

// entry point to the engine, initializes all subsystems
void ye_init_engine(struct engine_data data);

// shutdown point for engine, shuts down all subsystems
void ye_shutdown_engine();

#endif