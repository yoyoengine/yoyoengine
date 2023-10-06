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

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <yoyoengine/yoyoengine.h>

// buffer to hold filepath strings
// will be modified by getPath()
char path_buffer[1024];

// get the base path
char *base_path = NULL;

// initialize engine internal variable globals to NULL
SDL_Color *pEngineFontColor = NULL;
TTF_Font *pEngineFont = NULL;

// expose our engine state data to the whole engine
struct engine_data engine_state;

// runtime global information about the engine (used everywhere)
struct engine_runtime_data engine_runtime_state; // this technically initializes it to 0

// helper function to get the screen size
// TODO: consider moving graphics.c TODO: yes move to graphics.c
struct ScreenSize getScreenSize(){
    // initialize video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        ye_logf(error, "SDL could not initialize!\n");
        exit(1);
    }

    // use video to initialize display mode
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        ye_logf(error, "SDL_GetCurrentDisplayMode failed!\n");
        exit(1);
    }

    int screenWidth = displayMode.w;
    int screenHeight = displayMode.h;
    
    ye_logf(debug, "Inferred screen size: %dx%d\n", screenWidth, screenHeight);

    // return a ScreenSize struct with the screen width and height
    struct ScreenSize screenSize = {screenWidth, screenHeight};
    return screenSize;
}

// Global variables for resource paths
char *executable_path = NULL;

char* ye_get_resource_static(const char *sub_path) {
    static char resource_buffer[256];  // Adjust the buffer size as per your requirement

    if (engine_state.game_resources_path == NULL) {
        ye_logf(error, "Resource paths not set!\n");
        return NULL;
    }

    snprintf(resource_buffer, sizeof(resource_buffer), "%s/%s", engine_state.game_resources_path, sub_path);
    return resource_buffer;
}

char* ye_get_engine_resource_static(const char *sub_path) {
    static char engine_reserved_buffer[256];  // Adjust the buffer size as per your requirement

    if (engine_state.engine_resources_path == NULL) {
        ye_logf(error, "Engine reserved paths not set!\n");
        return NULL;
    }

    snprintf(engine_reserved_buffer, sizeof(engine_reserved_buffer), "%s/%s", engine_state.engine_resources_path, sub_path);
    return engine_reserved_buffer;
}

// event polled for per frame
SDL_Event e;

void ye_process_frame(){
    ui_begin_input_checks();
    while (SDL_PollEvent(&e)) {
        ui_handle_input(&e);

        // check for any reserved engine buttons (console, etc)
        if(e.type == SDL_KEYDOWN){
            switch(e.key.keysym.sym){
                case SDLK_BACKQUOTE:
                    if(engine_state.console_visible){
                        engine_state.console_visible = false;
                        remove_ui_component("console");
                    }
                    else{
                        engine_state.console_visible = true;
                        ui_register_component("console",ye_paint_console);
                    }
                    break;
                default:
                    break;
            }
            // if freecam is on (rare) TODO: allow changing of freecam scale
            if(engine_state.freecam_enabled){
                switch(e.key.keysym.sym){     
                    case SDLK_LEFT:
                        engine_state.target_camera->transform->rect.x -= 100.0;
                        break;
                    case SDLK_RIGHT:
                        engine_state.target_camera->transform->rect.x += 100.0;
                        break;
                    case SDLK_UP:
                        engine_state.target_camera->transform->rect.y -= 100.0;
                        break;
                    case SDLK_DOWN:
                        engine_state.target_camera->transform->rect.y += 100.0;
                        break;
                }
            }
        }

        // send event to callback specified by game (if needed)
        if(engine_state.handle_input != NULL){
            engine_state.handle_input(e);
        }
    }
    ui_end_input_checks();

    if(!engine_state.editor_mode){
        // update physics
        ye_system_physics(); // TODO: decouple from framerate
    }

    // render frame
    renderAll();
}

float ye_get_delta_time(){
    return (float)engine_runtime_state.frame_time / 1000.0;
}

void set_setting_string(const char* key, char** value, json_t* settings) {
    char * json_value;
    if (ye_json_string(settings, key, &json_value)) {
        *value = strdup(json_value);
    }
}

void set_setting_int(const char* key, int* value, json_t* settings) {
    int json_value;
    if (ye_json_int(settings, key, &json_value)) {
        *value = json_value;
    }
}

void set_setting_bool(const char* key, bool* value, json_t* settings) {
    bool json_value;
    if (ye_json_bool(settings, key, &json_value)) {
        *value = json_value;
    }
}

void set_setting_float(const char* key, float* value, json_t* settings) {
    float json_value;
    if (ye_json_float(settings, key, &json_value)) {
        *value = json_value;
    }
}

// update the resources path
void ye_update_resources(char *path){
    // update the engine state
    free(engine_state.game_resources_path);
    engine_state.game_resources_path = strdup(path);
}

/*
    Pass in a engine_data struct, with cooresponding override flags to initialize the engine with non default values
*/
void ye_init_engine() {
    // Get the path to our executable
    executable_path = SDL_GetBasePath(); // Don't forget to free memory later

    // Set default paths for engineResourcesPath and gameResourcesPath
    char engine_default_path[256], game_default_path[256], log_default_path[256];
    snprintf(engine_default_path, sizeof(engine_default_path), "%sengine_resources", executable_path);
    snprintf(game_default_path, sizeof(game_default_path), "%sresources", executable_path);
    snprintf(log_default_path, sizeof(log_default_path), "%sdebug.log", executable_path);

    // set defaults for engine state
    engine_state.framecap = -1;
    engine_state.screen_width = 1920;
    engine_state.screen_height = 1080;
    engine_state.window_mode = 0;
    engine_state.volume = 64;
    engine_state.window_title = "Yoyo Engine Window";

    // set default paths, if we have an override we can change them later
    engine_state.engine_resources_path = strdup(engine_default_path);
    engine_state.game_resources_path = strdup(game_default_path);
    engine_state.log_file_path = strdup(log_default_path);
    engine_state.icon_path = strdup(ye_get_engine_resource_static("enginelogo.png"));

    engine_state.log_level = 4;

    // check if ./settings.yoyo exists (if not, use defaults)
    json_t *SETTINGS = ye_json_read(ye_get_resource_static("../settings.yoyo"));
    if (SETTINGS == NULL) {
        ye_logf(warning, "No settings.yoyo file found, using defaults.\n");
    }
    else{
        ye_logf(info, "Found settings.yoyo file, using values from it.\n");

        set_setting_string("engine_resources_path", &engine_state.engine_resources_path, SETTINGS);
        set_setting_string("game_resources_path", &engine_state.game_resources_path, SETTINGS);
        set_setting_string("log_file_path", &engine_state.log_file_path, SETTINGS);
        set_setting_string("icon_path", &engine_state.icon_path, SETTINGS);
        set_setting_string("window_title", &engine_state.window_title, SETTINGS);

        set_setting_int("window_mode", &engine_state.window_mode, SETTINGS);
        set_setting_int("volume", &engine_state.volume, SETTINGS);
        set_setting_int("log_level", &engine_state.log_level, SETTINGS);
        set_setting_int("screen_width", &engine_state.screen_width, SETTINGS);
        set_setting_int("screen_height", &engine_state.screen_height, SETTINGS);
        set_setting_int("framecap", &engine_state.framecap, SETTINGS);

        set_setting_bool("debug_mode", &engine_state.debug_mode, SETTINGS);
        set_setting_bool("skip_intro", &engine_state.skipintro, SETTINGS);
        set_setting_bool("editor_mode", &engine_state.editor_mode, SETTINGS);

        json_decref(SETTINGS);
    }

    // initialize the runtime state
    engine_runtime_state.scene_default_camera = NULL;
    engine_runtime_state.selected_entity = NULL;

    // ----------------- Begin Setup -------------------

    // initialize graphics systems, creating window renderer, etc
    // TODO: should this just take in engine state struct? would make things a lot easier tbh
    initGraphics(
        engine_state.screen_width,
        engine_state.screen_height,
        engine_state.window_mode,
        engine_state.framecap,
        engine_state.window_title,
        engine_state.icon_path
    );

    // init timers
    ye_init_timers();

    // initialize the cache
    ye_init_cache();

    // load a font for use in engine (value of global in engine.h modified) this will be used to return working fonts if a user specified one cannot be loaded
    pEngineFont = ye_load_font(ye_get_engine_resource_static("RobotoMono-Light.ttf"), 64);

    // allocate memory for and create a pointer to our engineFontColor struct for use in graphics.c
    // this is also returned as a color if a user specified one cannot be loaded
    SDL_Color engineFontColor = {255, 255, 0, 255};
    pEngineFontColor = &engineFontColor;
    pEngineFontColor = malloc(sizeof(SDL_Color));
    pEngineFontColor->r = 255;
    pEngineFontColor->g = 255;
    pEngineFontColor->b = 0;
    pEngineFontColor->a = 255;

    // no matter what we will initialize log level with what it should be. default is nothing but dev can override
    ye_log_init(engine_state.log_file_path);

    if(engine_state.editor_mode){
        ye_logf(info, "Detected editor mode.\n");
    }

    // initialize entity component system
    ye_init_ecs();

    // if we are in debug mode
    if(engine_state.debug_mode){
        // we will open the metrics by default in debug mode
        engine_state.metrics_visible = true;

        // display in console
        ye_logf(debug, "Debug mode enabled.\n");
    }

    // startup audio systems
    initAudio();

    // before we play our loud ass startup sound, lets check what volume the game wants
    // the engine to be at initially
    setVolume(-1, engine_state.volume);

    /*
        Part of the engine startup which isnt configurable by the game is displaying
        a splash screen with the engine title and logo for 2550ms and playing a
        startup noise
    */
    if(engine_state.skipintro){
        ye_logf(info,"Skipping Intro.\n");
    }
    else{
        playSound(ye_get_engine_resource_static("startup.mp3"),0,0); // play startup sound

        // im not a particularly massive fan of using the unstable ECS just yet, but might as well
        struct ye_entity * splash_cam = ye_create_entity();
        ye_add_transform_component(splash_cam, (struct ye_rectf){0,0,1920,1080}, 99, YE_ALIGN_MID_CENTER);
        ye_add_camera_component(splash_cam, (SDL_Rect){0,0,1920,1080});
        ye_set_camera(splash_cam);

        // background for splash
        struct ye_entity * splash_bg = ye_create_entity();
        ye_add_transform_component(splash_bg, (struct ye_rectf){0,0,1920,1080}, 0, YE_ALIGN_MID_CENTER);
        ye_temp_add_image_renderer_component(splash_bg, ye_get_engine_resource_static("splash_bg.png"));

        // foreground logo for splash
        struct ye_entity * splash_y = ye_create_entity();
        ye_add_transform_component(splash_y, (struct ye_rectf){1920/2 - 350/2 - 100,1080/2 - 350/2,350,350}, 2, YE_ALIGN_MID_CENTER);
        ye_temp_add_image_renderer_component(splash_y, ye_get_engine_resource_static("splash_y.png"));

        // gear spinning below logo
        struct ye_entity * splash_gear = ye_create_entity();
        ye_add_transform_component(splash_gear, (struct ye_rectf){1920/2,1080/2 - 110,300,300}, 1, YE_ALIGN_MID_CENTER);
        ye_temp_add_image_renderer_component(splash_gear, ye_get_engine_resource_static("splash_gear.png"));
        ye_add_physics_component(splash_gear,0,0);
        splash_gear->physics->rotational_velocity = 90;

        // TODO: version numbers back please (awaiting text renderer)

        // pause on engine splash for 2550ms (TODO: consider alternatives)
        // SDL_Delay(3000); maybe a more reasonable time scale?
        // SDL_Delay(2550);

        // get current ticks
        int ticks = SDL_GetTicks();

        // until we are 2550 ticks in the future
        while(SDL_GetTicks() - ticks < 2550){
            // process frame
            ye_process_frame();
        }

        // we need to delete everything in the ECS and reset the camera
        ye_destroy_entity(splash_cam);
        ye_set_camera(NULL);
        ye_destroy_entity(splash_bg);
        ye_destroy_entity(splash_y);
        ye_destroy_entity(splash_gear);
    }

    lua_init(); // initialize lua
    ye_logf(info, "Initialized Lua.\n");

    // debug output
    ye_logf(info, "Engine Fully Initialized.\n");
} // control is now resumed by the game

// function that shuts down all engine subsystems and components ()
void ye_shutdown_engine(){
    ye_logf(info, "Shutting down engine...\n");

    // shutdown lua
    lua_shutdown();
    ye_logf(info, "Shut down lua.\n");

    // free the engine font color
    free(pEngineFontColor);
    pEngineFontColor = NULL;

    // free the engine font
    TTF_CloseFont(pEngineFont);

    // shutdown ECS
    ye_shutdown_ecs();

    // shutdown timers
    ye_shutdown_timers();

    // shutdown cache
    ye_shutdown_cache();

    // shutdown graphics
    shutdownGraphics();
    ye_logf(info, "Shut down graphics.\n");

    // shutdown audio
    shutdownAudio();
    ye_logf(info, "Shut down audio.\n");

    // shutdown logging
    // note: must happen before SDL because it relies on SDL path to open file
    ye_log_shutdown();
    free(engine_state.log_file_path);
    free(engine_state.engine_resources_path);
    free(engine_state.game_resources_path);
    free(engine_state.icon_path);
    SDL_free(base_path); // free base path after (used by logging)
    SDL_free(executable_path); // free base path after (used by logging)

    // quit SDL (should destroy anything else i forget)
    SDL_Quit();
}