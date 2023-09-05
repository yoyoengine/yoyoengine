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

// struct to hold screen points
struct ScreenSize {
    int width;
    int height;
};

// helper function to return ScreenSize struct from getting the screensize with SDL2
struct ScreenSize getScreenSize();

char *getPathDynamic(const char *path);

char *getResourceStatic(const char *sub_path);

char* getEngineResourceStatic(const char *sub_path);

void toggleConsole();

// constructor for engine (any fields can be left blank for defaults)
struct engine_data {
    int screen_width;
    int screen_height;
    int volume;
    int window_mode;
    int framecap;
    int log_level;
    
    bool debug_mode; // does not need override
    bool skipintro; // does not need override

    char *window_title;
    char *icon_path;
    char *engine_resources_path;
    char *game_resources_path;

    // overrides - this is confusing with what needs overridden and what doesnt TODO:
    bool override_screen_width;
    bool override_screen_height;
    bool override_volume;
    bool override_window_mode;
    bool override_framecap;
    bool override_log_level;
    // skip boolean overrides...
    bool override_window_title;
    // skip icon path overrides...
    bool override_engine_resources_path;
    bool override_game_resources_path;
};

// entry point to the engine, initializes all subsystems
void initEngine(struct engine_data data);

// shutdown point for engine, shuts down all subsystems
void shutdownEngine();

#endif