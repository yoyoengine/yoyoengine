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
extern TTF_Font *pEngineFont2;

// struct to hold screen points
struct ScreenSize {
    int width;
    int height;
};

// helper function to return ScreenSize struct from getting the screensize with SDL2
struct ScreenSize getScreenSize();

// returns the char * path to a resource specified in relation to root dir
char *getPathStatic(const char *path);

char *getPathDynamic(const char *path);

void toggleConsole();

void toggleOverlay();

// entry point to the engine, initializes all subsystems
void initEngine(int screenWidth, int screenHeight, bool debug, int volume, int windowMode, int framecap, bool skipintro);

// shutdown point for engine, shuts down all subsystems
void shutdownEngine();

#endif