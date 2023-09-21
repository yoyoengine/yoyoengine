/*
    TODO: ENGINE
    - (maybe) seperate some prinf output feedbacks into debug only
    - run every file in gpt4 and ask for issues
    - update all header files with the comment descriptions of the functions
*/

#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <yoyoengine/yoyoengine.h>

// engine version to be displayed on splash screen
char * engine_version = "v0.0.1 dev";

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
static const char *resources_path = NULL;
static const char *engine_resources_path = NULL;
static const char *log_file_path = NULL;
char *executable_path = NULL;

char* ye_get_resource_static(const char *sub_path) {
    static char resource_buffer[256];  // Adjust the buffer size as per your requirement

    if (resources_path == NULL) {
        ye_logf(error, "Resource paths not set!\n");
        return NULL;
    }

    snprintf(resource_buffer, sizeof(resource_buffer), "%s/%s", resources_path, sub_path);
    return strdup(resource_buffer);
}

char* ye_get_engine_resource_static(const char *sub_path) {
    static char engine_reserved_buffer[256];  // Adjust the buffer size as per your requirement

    if (engine_resources_path == NULL) {
        ye_logf(error, "Engine reserved paths not set!\n");
        return NULL;
    }

    snprintf(engine_reserved_buffer, sizeof(engine_reserved_buffer), "%s/%s", engine_resources_path, sub_path);
    return strdup(engine_reserved_buffer);
}

// some functions to apply a value if its uninitialized /////////////////////

int applyDefaultInt(int value, int defaultValue) {
    if (value != 0) {
        return value;
    }
    return defaultValue;
}

bool applyDefaultBool(bool value, bool defaultValue) {
    if (value) {
        return value;
    }
    return defaultValue;
}

char* applyDefaultString(char* value, char* defaultValue) {
    if (value != NULL && strlen(value) > 0) {
        return value;
    }
    return (char*)defaultValue; // Cast away const for default value
}

/////////////////////////////////////////////////////////////////////////////

// combines two paths together (sprintf's to result buffer)
void constructPath(char *result, size_t result_size, const char *base_path, const char *supplied_path, const char *default_path) {
    if (supplied_path) {
        snprintf(result, result_size, "%s%s", base_path, supplied_path);
    } else {
        strncpy(result, default_path, result_size);
    }
}

/*
    Potentially clever thinking by me - just set defaults for non overridden values
    For booleans, we can completely skip this because they are initialized to false and need to be overriden to true
    (potential issue if we want it to default to true)
*/
void configute_defaults(struct engine_data *data){
    if(!data->override_screen_width)
        data->screen_width = 1920;
    if(!data->override_screen_height)
        data->screen_height = 1080;
    if(!data->override_volume)
        data->volume = 0;
    if(!data->override_window_mode)
        data->window_mode = 0;
    if(!data->override_framecap)
        data->framecap = -1;
    if(!data->override_log_level)
        data->log_level = 4;
    if(!data->override_window_title)
        data->window_title = "Yoyo Engine Window";
    // we already have a ternery checking for uninitialized icon path
    // game/engine resource paths are handed in init engine... move into here?
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

    // update physics
    ye_system_physics();

    // render frame
    renderAll();
}

float ye_get_delta_time(){
    return (float)engine_runtime_state.frame_time / 1000.0;
}

/*
    Pass in a engine_data struct, with cooresponding override flags to initialize the engine with non default values
*/
void ye_init_engine(struct engine_data data) {
    // Get the path to our executable
    executable_path = SDL_GetBasePath(); // Don't forget to free memory later

    // ----------------- Default Checks ----------------

    // Set default paths for engineResourcesPath and gameResourcesPath
    char engine_default_path[256], game_default_path[256], log_default_path[256];
    snprintf(engine_default_path, sizeof(engine_default_path), "%sengine_resources", executable_path);
    snprintf(game_default_path, sizeof(game_default_path), "%sresources", executable_path);
    snprintf(log_default_path, sizeof(log_default_path), "%sdebug.log", executable_path);

    // Construct paths
    char engine_supplied_path[256], game_supplied_path[256], log_supplied_path[256];
    constructPath(engine_supplied_path, sizeof(engine_supplied_path), executable_path, data.engine_resources_path, engine_default_path);
    constructPath(game_supplied_path, sizeof(game_supplied_path), executable_path, data.game_resources_path, game_default_path);
    constructPath(log_supplied_path, sizeof(log_supplied_path), executable_path, data.log_file_path, log_default_path);

    // check overrides to configure uninitialized fields to defaults
    configute_defaults(&data);

    // Update global locations for resources
    engine_resources_path = strdup(engine_supplied_path); // Remember to free this memory later
    resources_path = strdup(game_supplied_path); // Remember to free this memory later
    log_file_path = strdup(log_supplied_path); // Remember to free this memory later

    // Get the icon path
    char *iconPath = data.icon_path ? ye_get_resource_static(data.icon_path) : ye_get_engine_resource_static("enginelogo.png");

    // TODO: i know these first two should be fine but does iconpath go out of scope after this fn?
    data.engine_resources_path = engine_resources_path;
    data.game_resources_path = resources_path;
    data.log_file_path = log_file_path;
    data.icon_path = iconPath;

    // copy our final data struct into the global engine state
    engine_state = data;

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

    // load a font for use in engine (value of global in engine.h modified) TODO: this will break
    pEngineFont = loadFont(ye_get_engine_resource_static("RobotoMono-Light.ttf"), 500);

    // allocate memory for and create a pointer to our engineFontColor struct for use in graphics.c
    // TODO: check this later because i'm so tired and perplexed with this workaround to letting the fn go out of scope
    SDL_Color engineFontColor = {255, 255, 0, 255};
    pEngineFontColor = &engineFontColor;
    pEngineFontColor = malloc(sizeof(SDL_Color));
    pEngineFontColor->r = 255;
    pEngineFontColor->g = 255;
    pEngineFontColor->b = 0;
    pEngineFontColor->a = 255;

    // no matter what we will initialize log level with what it should be. default is nothing but dev can override
    ye_log_init(log_file_path);

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
        ye_add_transform_component(splash_cam, (struct ye_rectf){0,0,1920,1080}, 1, YE_ALIGN_MID_CENTER);
        ye_add_camera_component(splash_cam, (SDL_Rect){0,0,1920,1080});
        ye_set_camera(splash_cam);

        struct ye_entity * splash_img = ye_create_entity();
        ye_add_transform_component(splash_img, (struct ye_rectf){0,0,1920,1080}, 0, YE_ALIGN_MID_CENTER);
        ye_temp_add_image_renderer_component(splash_img, ye_get_engine_resource_static("splash.png"));

        // TODO: version numbers back please (awaiting text renderer)

        // render everything in engine queue
        renderAll(); 

        // pause on engine splash for 2550ms (TODO: consider alternatives)
        // SDL_Delay(3000); maybe a more reasonable time scale?
        SDL_Delay(2550);

        // we need to delete everything in the ECS and reset the camera
        ye_destroy_entity(splash_cam);
        ye_set_camera(NULL);
        ye_destroy_entity(splash_img);
    }

    // render everything in engine queue after splash asset removal
    // renderAll(); ACHSHUALLY - this needs fixed when ECS is finalized
    // its ok to render a blank frame here, and then destroy the engine startup camera
    // the user needs to make a camera after this though to display anything
    // we will reset the default camera to null

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

    // shutdown ECS
    ye_shutdown_ecs();

    // shutdown graphics
    shutdownGraphics();
    ye_logf(info, "Shut down graphics.\n");

    // shutdown audio
    shutdownAudio();
    ye_logf(info, "Shut down audio.\n");

    // shutdown logging
    // note: must happen before SDL because it relies on SDL path to open file
    ye_log_shutdown();
    SDL_free(base_path); // free base path after (used by logging)
    SDL_free(executable_path); // free base path after (used by logging)

    // quit SDL (should destroy anything else i forget)
    SDL_Quit();
}