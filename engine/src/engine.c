/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <Lilith.h>

#include <jansson.h>

#include <p2d/p2d.h>

#include <yoyoengine/ui/ui.h>
#include <yoyoengine/yep.h>
#include <yoyoengine/input.h>
#include <yoyoengine/scene.h>
#include <yoyoengine/json.h>
#include <yoyoengine/audio.h>
#include <yoyoengine/event.h>
#include <yoyoengine/timer.h>
#include <yoyoengine/cache.h>
#include <yoyoengine/config.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/tricks.h>
#include <yoyoengine/physics.h>
#include <yoyoengine/console.h>
#include <yoyoengine/version.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ui/overlays.h>
#include <yoyoengine/graphics.h>
#include <yoyoengine/networking.h>
#include <yoyoengine/ecs/camera.h>
#include <yoyoengine/ecs/button.h>
#include <yoyoengine/ecs/renderer.h>
#include <yoyoengine/ecs/transform.h>
#include <yoyoengine/debug_renderer.h>
#include <yoyoengine/ecs/lua_script.h>
#include <yoyoengine/ecs/audiosource.h>

// buffer to hold filepath strings
// will be modified by getPath()
char path_buffer[1024];

// get the base path
char *base_path = NULL;

// expose our engine state data to the whole engine
struct ye_engine_state YE_STATE = {0};

// Global variables for resource paths
char *executable_path = NULL;



// TODO: temp leaving because editor knows where its copy of engine resources is through this
char* ye_get_engine_resource_static(const char *sub_path) {
    static char engine_reserved_buffer[256];  // Adjust the buffer size as per your requirement

    if (YE_STATE.engine.engine_resources_path == NULL) {
        ye_logf(YE_LL_ERROR, "Engine reserved paths not set!\n");
        return NULL;
    }

    snprintf(engine_reserved_buffer, sizeof(engine_reserved_buffer), "%s/%s", YE_STATE.engine.engine_resources_path, sub_path);
    return engine_reserved_buffer;
}

/*
    New refreshed path accessor functions:

    ye_path_relative(char*) takes in a path relative to the game executable and returns the full path
*/

char * ye_path(const char * path){
    static char path_relative_buffer[512];

    // this is set at engine init so discourage calling before then
    if (executable_path == NULL) {
        ye_logf(YE_LL_ERROR, "Executable path not set!\n");
        return NULL;
    }

    snprintf(path_relative_buffer, sizeof(path_relative_buffer), "%s/%s", executable_path, path);
    return path_relative_buffer;
}

char * ye_path_resources(const char * path){
    static char path_relative_buffer_resources[512];
    
    // this is set at engine init so discourage calling before then
    if (executable_path == NULL) {
        ye_logf(YE_LL_ERROR, "Executable path not set!\n");
        return NULL;
    }
    
    snprintf(path_relative_buffer_resources, sizeof(path_relative_buffer_resources), "%s/resources/%s", executable_path, path);
    return path_relative_buffer_resources;
}

/* ============== end new paths ============== */

int last_frame_time = 0;
void ye_process_frame(){
    // update time delta
    YE_STATE.runtime.delta_time = (SDL_GetTicks64() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks64();

    // check if a scene is deferred to be loaded and load it
    if(ye_scene_check_deferred_load()){
        YE_STATE.runtime.delta_time = (SDL_GetTicks64() - last_frame_time) / 1000.0f;
        last_frame_time = SDL_GetTicks64();
    }

    // update timers
    ye_update_timers();

    // C pre frame callback
    ye_fire_event(YE_EVENT_PRE_FRAME, (union ye_event_args){NULL});

    int input_time = SDL_GetTicks64();
    
    /*
        Let the input system handle the following:
        - Send events to Nuklear
        - Handle engine input events (terminal, resizing)
        - Send callback to game C code
        - Lookup events in mapping table and inform C and Lua
    */
    ye_system_input();

    YE_STATE.runtime.input_time = SDL_GetTicks64() - input_time;


    int physics_time = SDL_GetTicks64();
    if(!YE_STATE.editor.editor_mode){
        // test: discard running if delta time is too high
        // if(YE_STATE.runtime.delta_time > 0.1f){
        //     ye_logf(YE_LL_WARNING, "Discarding frame due to high delta time: %f\n", YE_STATE.runtime.delta_time);
        //     return;
        // }

        // update physics
        p2d_step(YE_STATE.runtime.delta_time); // TODO: decouple from framerate

        // printf("delta time: %f\n", YE_STATE.runtime.delta_time);
        // printf("+---------------------+\n");
        // printf("|        STATE        |\n");
        // printf("+---------------------+\n");
        // printf("objects: %d\n", p2d_state.p2d_object_count);
        // printf("world nodes: %d\n", p2d_state.p2d_world_node_count);
        // printf("contact checks: %d\n", p2d_state.p2d_contact_checks);
        // printf("contacts found: %d\n", p2d_state.p2d_contacts_found);
        // printf("collision pairs: %d\n", p2d_state.p2d_collision_pairs);
    }
    YE_STATE.runtime.physics_time = SDL_GetTicks64() - physics_time;

    // if we are in runtime, run callbacks
    if(!YE_STATE.editor.editor_mode){
        // run all trick update callbacks
        ye_run_trick_updates();
    
        // run all scripting before the frame is rendered
        ye_system_lua_scripting();
    }

    // render frame
    ye_render_all();

    // recompute audio spatialization
    if(!YE_STATE.editor.editor_mode)
        ye_system_audiosource();

    YE_STATE.runtime.frame_time = SDL_GetTicks64() - last_frame_time;

    // C post frame callback
    ye_fire_event(YE_EVENT_POST_FRAME, (union ye_event_args){NULL});
}

float ye_delta_time(){
    return YE_STATE.runtime.delta_time;
}

void ye_update_base_path(const char *path){
    // update the engine state
    free(executable_path);
    executable_path = strdup(path);
}

void teardown_splash_screen(){
    // open SETTINGS again
    json_t *SETTINGS = ye_json_read(ye_path("settings.yoyo"));
    
    // retrieve and load the entry scene (editor mode handles this itself)
    if(!YE_STATE.editor.editor_mode){
        const char * entry_scene;
        if (ye_json_string(SETTINGS, "entry_scene", &entry_scene)) {
            ye_logf(YE_LL_INFO, "Detected entry: %s.\n", entry_scene);
            ye_load_scene(entry_scene);
        }
        else{
            if(!YE_STATE.editor.editor_mode)
                ye_logf(YE_LL_WARNING, "No entry_scene specified in settings.yoyo, if you do not load a custom scene the engine will crash.\n");
        }
    }

    // no need to free timer, timer system will do it

    // free the settings json as needed
    if(SETTINGS != NULL)
        json_decref(SETTINGS);
}

void setup_splash_screen(){
    /*
        Setup the engine logo screen

        Construct ECS, add a timer to callback into loading the
        default scene after 5000ms
    */

    YE_STATE.runtime.scene_name = strdup("engine_splash_screen");

    json_t *splash_scene = NULL; // the intro scene bytes

    // get the scene into a json from the engine pack
    splash_scene = yep_engine_resource_json("startup/intro.yoyo");

    if(!YE_STATE.editor.editor_mode){
        /*
            We need to prepopulate the cache with the resources
            the intro scene uses, since they are all loaded
            from the engine.yep pack.
        */

        TTF_Font *chakra = yep_engine_resource_font("startup/ChakraPetch-Regular.ttf");
        ye_cache_font_manual("chakra", chakra);

        SDL_Texture *splash_bg_tex = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, yep_engine_resource_image("startup/splash_bg.png"));
        ye_cache_texture_manual(splash_bg_tex, "startup/splash_bg.png");

        SDL_Texture *splash_y_tex = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, yep_engine_resource_image("startup/splash_y.png"));
        ye_cache_texture_manual(splash_y_tex, "startup/splash_y.png");

        SDL_Texture *splash_gear_tex = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, yep_engine_resource_image("startup/splash_gear.png"));
        ye_cache_texture_manual(splash_gear_tex, "startup/splash_gear.png");
    }

    // load the intro scene file out of the engine resources
    ye_raw_scene_load(splash_scene);
    json_decref(splash_scene);

    // hook into the build text entity and set the version string
    struct ye_entity *build_text = ye_get_entity_by_name("build text");
    if(build_text != NULL){
        free(build_text->renderer->renderer_impl.text->text);
        build_text->renderer->renderer_impl.text->text = strdup(YOYO_ENGINE_VERSION_STRING);
        ye_update_renderer_component(build_text);
    }

    // start playing the music, since we cant do it from the scene file (should refactor to be generic in the future)
    _ye_mixer_engine_cache("startup/startup.wav"); // done down here because load scene raw purges mixer cache
    ye_play_sound("startup/startup.wav", 0, 1);

    /*
        Create a timer 3000ms from now to load the default scene
    */
    struct ye_timer * splash_timer = malloc(sizeof(struct ye_timer));
    splash_timer->start_ticks = -1;
    splash_timer->loops = 0;
    splash_timer->length_ms = 5000;
    splash_timer->callback = teardown_splash_screen;
    ye_register_timer(splash_timer);
}

void ye_init_engine() {

    // pre init callback //
    ye_fire_event(YE_EVENT_PRE_INIT, (union ye_event_args){NULL});
    ///////////////////////

    /*
        We initialize the console here so we
        retain logs from startup.
    */
    ye_init_console(YE_DEFAULT_CONSOLE_BUFFER_SIZE);

    // Get the path to our executable
    executable_path = SDL_GetBasePath(); // Don't forget to free memory later
    // printf("Executable path: %s\n", executable_path);

    // #ifdef __EMSCRIPTEN__
    //     free(executable_path);
    //     executable_path = strdup("");
    // #endif

    // Set default paths for engineResourcesPath and gameResourcesPath
    char engine_default_path[256], game_default_path[256], log_default_path[256];
    snprintf(engine_default_path, sizeof(engine_default_path), "%sengine_resources", executable_path);
    snprintf(game_default_path, sizeof(game_default_path), "%sresources", executable_path);
    snprintf(log_default_path, sizeof(log_default_path), "%sdebug.log", executable_path);

    // check if ./settings.yoyo exists (if not, use defaults)
    json_t *SETTINGS = ye_json_read(ye_path("settings.yoyo"));
    if (SETTINGS == NULL) {
        ye_logf(YE_LL_WARNING, "No settings.yoyo file found, it will be created using default values.\n");
        SETTINGS = json_object();
    }

    // config strings all need freed later
    YE_STATE.engine.engine_resources_path   = ye_config_string(SETTINGS, "engine_resources_path", engine_default_path);
    YE_STATE.engine.game_resources_path     = ye_config_string(SETTINGS, "game_resources_path", game_default_path);
    YE_STATE.engine.log_file_path           = ye_config_string(SETTINGS, "log_file_path", log_default_path);
    YE_STATE.engine.icon_path               = ye_config_string(SETTINGS, "icon_path", "enginelogo.png");
    YE_STATE.engine.window_title            = ye_config_string(SETTINGS, "window_title", "Yoyo Engine Window");
    ye_config_string(SETTINGS, "engine_version", YOYO_ENGINE_VERSION_STRING); // overwrite the version with what we opened it with

    YE_STATE.engine.window_mode             = ye_config_int(SETTINGS, "window_mode", 0);
    YE_STATE.engine.volume                  = ye_config_int(SETTINGS, "volume", 64);
    YE_STATE.engine.log_level               = ye_config_int(SETTINGS, "log_level", 4);
    YE_STATE.engine.screen_width            = ye_config_int(SETTINGS, "screen_width", 1920);
    YE_STATE.engine.screen_height           = ye_config_int(SETTINGS, "screen_height", 1080);
    YE_STATE.engine.framecap                = ye_config_int(SETTINGS, "framecap", -1);
    YE_STATE.engine.sdl_quality_hint        = ye_config_int(SETTINGS, "sdl_quality_hint", 1); // linear

    YE_STATE.engine.debug_mode              = ye_config_bool(SETTINGS, "debug_mode", false);
    YE_STATE.engine.skipintro               = ye_config_bool(SETTINGS, "skip_intro", false);
    YE_STATE.editor.editor_mode             = ye_config_bool(SETTINGS, "editor_mode", false);
    YE_STATE.engine.stretch_resolution      = ye_config_bool(SETTINGS, "stretch_resolution", false);

    int p2d_grid_size = ye_config_int(SETTINGS, "p2d_grid_size", 250);
    float p2d_gravity_x = ye_config_float(SETTINGS, "p2d_gravity_x", 0.0f);
    float p2d_gravity_y = ye_config_float(SETTINGS, "p2d_gravity_y", 20.0f);


    // initialize some editor state
    YE_STATE.editor.scene_default_camera = NULL;

    // ----------------- Begin Setup -------------------

    // initialize overlays
    ye_init_overlays();
    ye_register_default_overlays();

    // initialize graphics systems, creating window renderer, etc
    ye_init_graphics();

    // init timers
    ye_init_timers();

    // initialize the cache
    ye_init_cache();

    // load a font for use in engine (value of global in engine.h modified) this will be used to return working fonts if a user specified one cannot be loaded
    if(YE_STATE.editor.editor_mode){
        YE_STATE.engine.pEngineFont = ye_load_font(ye_get_engine_resource_static("fonts/RobotoMono-Light.ttf"));
    }
    else{
        YE_STATE.engine.pEngineFont = yep_engine_resource_font("fonts/RobotoMono-Light.ttf");
    }
    // since we are bypassing the cache to do this, we need to customly resize this
    TTF_SetFontSize(YE_STATE.engine.pEngineFont, 128); // high enough to be readable where-ever (I think)

    // allocate memory for and create a pointer to our engineFontColor struct for use in graphics.c
    // this is also returned as a color if a user specified one cannot be loaded
    SDL_Color engineFontColor = {255, 255, 0, 255};
    YE_STATE.engine.pEngineFontColor = &engineFontColor;
    YE_STATE.engine.pEngineFontColor = malloc(sizeof(SDL_Color));
    YE_STATE.engine.pEngineFontColor->r = 255;
    YE_STATE.engine.pEngineFontColor->g = 255;
    YE_STATE.engine.pEngineFontColor->b = 0;
    YE_STATE.engine.pEngineFontColor->a = 255;

    // no matter what we will initialize log level with what it should be. default is nothing but dev can override
    ye_log_init(YE_STATE.engine.log_file_path);

    // init the input system (and all controllers)
    ye_init_input();

    if(YE_STATE.editor.editor_mode){
        ye_logf(YE_LL_INFO, "Detected editor mode.\n");
    }

    // initialize entity component system
    ye_init_ecs();

    // initialize physics
    p2d_init(p2d_grid_size, ye_physics_collision_callback, ye_physics_trigger_callback, ye_p2d_logf_wrapper);
    YE_STATE.engine.p2d_state = &p2d_state;
    YE_STATE.engine.p2d_state->p2d_gravity = (vec2_t){{p2d_gravity_x, p2d_gravity_y}};

    // if we are in debug mode
    if(YE_STATE.engine.debug_mode){
        // display in console
        ye_logf(YE_LL_DEBUG, "Debug mode enabled.\n");
    }

    // startup audio systems
    ye_init_audio();

    // the audio initialization accesses YE_STATE.engine.volume to cap each channel by default

    // initialize networking
    ye_init_networking();

    // initialize and load tricks (modules/plugins)
    ye_init_tricks();

    // set our last frame time now because we might play the intro
    last_frame_time = SDL_GetTicks64();

    /*
        Part of the engine startup which isnt configurable by the game is displaying
        a splash screen with the engine title and logo for 2550ms and playing a
        startup noise
    */
    if(YE_STATE.engine.skipintro){
        ye_logf(YE_LL_INFO,"Skipping Intro.\n");

        // load entry scene
        const char * entry_scene;
        if (ye_json_string(SETTINGS, "entry_scene", &entry_scene)) {
            ye_logf(YE_LL_INFO, "Detected entry: %s.\n", entry_scene);
            ye_load_scene(entry_scene);
        }
        else{
            if(!YE_STATE.editor.editor_mode)
                ye_logf(YE_LL_WARNING, "No entry_scene specified in settings.yoyo, if you do not load a custom scene the engine will crash.\n");
        }
    }
    else{
        setup_splash_screen();
    }

    // debug output
    ye_logf(YE_LL_INFO, "Engine Fully Initialized.\n");

    // dump settings in case we created defaults, then close
    if(SETTINGS != NULL){
        ye_json_write(ye_path("settings.yoyo"), SETTINGS);
        json_decref(SETTINGS);
    }

    // post init callback //
    ye_fire_event(YE_EVENT_POST_INIT, (union ye_event_args){NULL}); // TODO: does not work with none flag
    ///////////////////////

} // control is now resumed by the game

void ye_shutdown_engine(){

    // pre shutdown callback //
    ye_fire_event(YE_EVENT_PRE_SHUTDOWN, (union ye_event_args){NULL});
    ///////////////////////////

    ye_logf(YE_LL_INFO, "Shutting down engine...\n");

    // shut tricks down
    ye_shutdown_tricks();

    // shutdown networking
    ye_shutdown_networking();

    // purge debug renderer
    ye_debug_renderer_cleanup(true);

    // shutdown ECS
    ye_shutdown_ecs();

    // shutdown physics
    p2d_shutdown();

    // shutdown timers
    ye_shutdown_timers();

    // shutdown cache
    ye_shutdown_cache();

    // free the engine font color
    free(YE_STATE.engine.pEngineFontColor);
    YE_STATE.engine.pEngineFontColor = NULL;

    // free the engine font
    TTF_CloseFont(YE_STATE.engine.pEngineFont);

    // shutdown graphics
    ye_shutdown_graphics();
    ye_logf(YE_LL_INFO, "Shut down graphics.\n");

    // shutdown audio
    ye_shutdown_audio();
    ye_logf(YE_LL_INFO, "Shut down audio.\n");

    // shutdown input
    ye_shutdown_input();

    // Shutdown console
    ye_shutdown_console();

    // shutdown overlays
    ye_shutdown_overlays();

    // shutdown logging
    // note: must happen before SDL because it relies on SDL path to open file
    ye_log_shutdown();

    free(YE_STATE.engine.log_file_path);
    free(YE_STATE.engine.engine_resources_path);
    free(YE_STATE.engine.game_resources_path);
    free(YE_STATE.engine.icon_path);
    // free(YE_STATE.engine.window_title); copilot added this but i havent checked if this is freed elsewhere
    SDL_free(base_path); // free base path after (used by logging)
    SDL_free(executable_path); // free base path after (used by logging)

    // quit SDL (should destroy anything else i forget)
    SDL_Quit();

    // post shutdown callback //
    ye_fire_event(YE_EVENT_POST_SHUTDOWN, (union ye_event_args){NULL});
    ////////////////////////////

    // free all event memory
    ye_purge_events(true);
}