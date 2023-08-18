/*
    TODO: ENGINE
    - engine resources (icon, fonts) need to be seperated in a sensical way from game code
      and resources. Engine should feel more seperate and able to be converted to other projects
    - (maybe) seperate some prinf output feedbacks into debug only
    - run every file in gpt4 and ask for issues
    - update all header files with the comment descriptions of the functions
*/

#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "lib/audio.h"
#include "lib/graphics.h"
#include "lib/engine.h"
#include "lib/logging.h"

// buffer to hold filepath strings
// will be modified by getPath()
char path_buffer[1024];

// get the base path
char *base_path = NULL;

// initialize engine internal variable globals to NULL
SDL_Color *pEngineFontColor = NULL;
TTF_Font *pEngineFont = NULL;
TTF_Font *pEngineFont2 = NULL;

// debug overlay state controller
bool debugOverlay = false;

// console overlay state controller
bool consoleOverlay = false;

// helper function to get the screen size
// TODO: consider moving graphics.c
struct ScreenSize getScreenSize(){
    // initialize video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        logMessage(error, "SDL could not initialize!\n");
        exit(1);
    }

    // use video to initialize display mode
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        logMessage(error, "SDL_GetCurrentDisplayMode failed!\n");
        exit(1);
    }

    int screenWidth = displayMode.w;
    int screenHeight = displayMode.h;
    
    char buffer[100];
    snprintf(buffer, sizeof(buffer),  "Inferred screen size: %dx%d\n", screenWidth, screenHeight);
    logMessage(debug, buffer);

    // return a ScreenSize struct with the screen width and height
    struct ScreenSize screenSize = {screenWidth, screenHeight};
    return screenSize;
}

// get path for one time use
char *getPathStatic(const char *path) {
    static char path_buffer[256];  // Adjust the buffer size as per your requirement

    if (base_path == NULL) {
        base_path = SDL_GetBasePath();
        if (base_path == NULL) {
            logMessage(error, "Error getting base path!\n");
            return NULL;
        }
    }

    snprintf(path_buffer, sizeof(path_buffer), "%s../../resources/%s", base_path, path);
    return path_buffer;
}

// get dynamically allocated path which must be freed
char *getPathDynamic(const char *path) {
    if (base_path == NULL) {
        base_path = SDL_GetBasePath();
        if (base_path == NULL) {
            logMessage(error, "Error getting base path!\n");
            return NULL;
        }
    }

    size_t buffer_size = strlen(base_path) + strlen("../../resources/") + strlen(path) + 1;
    char *path_buffer = malloc(buffer_size);
    if (path_buffer == NULL) {
        logMessage(error, "Error allocating memory for path!\n");
        return NULL;
    }

    snprintf(path_buffer, buffer_size, "%s../../resources/%s", base_path, path);
    return path_buffer;
}

void toggleConsole(){
    if(consoleOverlay){
        logMessage(debug, "Toggled Console Overlay Off.\n");
        removeRenderObject(-901);
        removeRenderObject(-902);
    }
    else{
        logMessage(debug, "Toggled Console Overlay On.\n");

        // add back panel to debug overlay
        struct textureInfo info = createImageTexture("images/ui/dimpanel.png",false);

        renderObject panel = {
            -901, // we set ID each time
            900,
            renderType_Image,
            info.pTexture,
            createRealPixelRect(false,0,.9,.4,.1), // dummy rect
            NULL, 
            true, // cache this (eventually between scene loads we will want to keep it, actually we might already)
            createRealPixelRect(false,0,.9,.4,.1),
            false, // not centered
            .ImageData = {
                "images/ui/dimpanel.png"
            }
        };
        
        addRenderObject(panel);

        renderObject text = {
            -902, // we set ID each time
            901,
            renderType_Text,
            createTextTexture("console init...",pEngineFont,pEngineFontColor), // we set texture each time
            createRealPixelRect(false,0,.9,.4,.1), // dummy rect
            NULL, 
            false, // no cache
            createRealPixelRect(false,0,.9,.4,.1),
            false, // not centered
            ALIGN_MID_LEFT,
            .TextData = {
                pEngineFont2,
                0,
                pEngineFontColor,
                NULL, // no outline color
                ">",
            }
        };
        
        addRenderObject(text);
        updateText(-902,">");
    }
    consoleOverlay = !consoleOverlay;
}

void toggleOverlay(){
    if(debugOverlay){
        logMessage(debug, "Toggled Debug Overlay Off.\n");
        // remove all items in debug overlay
        removeRenderObject(-1);
        removeRenderObject(-2);
        removeRenderObject(-3);
        removeRenderObject(-4);
        removeRenderObject(-5);
        removeRenderObject(-900);
    }
    else{ // NOTE: lots of hard coded falses for texture caching
        logMessage(debug, "Toggled Debug Overlay On.\n");
        
        // construct a prototype renderObject to pass to addRenderObject() which we modify each time
        renderObject staging = {
            -1, // we set ID each time
            999,
            renderType_Text,
            createTextTexture("fps: 0",pEngineFont,pEngineFontColor), // we set texture each time
            createRealPixelRect(false,0,0,.12f,.08f), // dummy rect
            NULL, 
            false, // no cache
            createRealPixelRect(false,0,0,.12f,.08f),
            false, // not centered
            .TextData = {
                pEngineFont2,
                0,
                pEngineFontColor,
                NULL, // no outline color
                "fps: 0",
            }
        };
        
        // add fps counter manually to render stack with a custom id
        addRenderObject(staging);

        // add object counter (only updates when changed)
        staging.identifier = -2;
        staging.rect.y += 75;
        staging.bounds.y += 75;
        staging.pTexture = createTextTexture("renderObjects: 0",pEngineFont2,pEngineFontColor);
        addRenderObject(staging);

        // add audio chunk counter (only updates when changed)
        staging.identifier = -3;
        staging.rect.y += 50;
        staging.bounds.y += 50;
        staging.pTexture = createTextTexture("audio chunks: 0",pEngineFont2,pEngineFontColor);
        addRenderObject(staging);
        
        // add audio chunk counter (only updates when changed)
        staging.identifier = -4;
        staging.rect.y += 50;
        staging.bounds.y += 50;
        staging.pTexture = createTextTexture("log lines: 0",pEngineFont2,pEngineFontColor);
        addRenderObject(staging);

        // add audio chunk counter (only updates when changed)
        staging.identifier = -5;
        staging.rect.y += 50;
        staging.bounds.y += 50;
        staging.pTexture = createTextTexture("paint time: 0ms",pEngineFont2,pEngineFontColor);
        addRenderObject(staging);
    
        // add back panel to debug overlay
        struct textureInfo info = createImageTexture("images/ui/dimpanel.png",false);
        
        renderObject panel = {
            -900, // we set ID each time
            900,
            renderType_Image,
            info.pTexture,
            (SDL_Rect){0,0,250,400},
            NULL, 
            false, // cache this (eventually between scene loads we will want to keep it, actually we might already)
            (SDL_Rect){0,0,250,400},
            false, // not centered
            .ImageData = {
                "images/ui/dimpanel.png"
            }
        };
        
        addRenderObject(panel);

        // force overlay refresh or text will be default
        debugForceRefresh();
    }
    debugOverlay = !debugOverlay;
}

// engine entry point, takes in the screenWidth, screenHeight and a bool flag for
// starting in debug mode
void initEngine(int screenWidth, int screenHeight, bool debugMode, int volume, int windowMode, int framecap, bool skipintro){
    // initialize graphics systems, creating window renderer, etc
    initGraphics(screenWidth,screenHeight,windowMode,framecap);

    // load a font for use in engine (value of global in engine.h modified)
    pEngineFont = loadFont("engine_reserved/RobotoMono-Light.ttf", 500);
    pEngineFont2 = loadFont("engine_reserved/RobotoMono-Light.ttf", 500);

    // allocate memory for and create a pointer to our engineFontColor struct for use in graphics.c
    // TODO: check this later because i'm so tired and perplexed with this workaround to letting the fn go out of scope
    SDL_Color engineFontColor = {255, 255, 0, 255};
    pEngineFontColor = &engineFontColor;
    pEngineFontColor = malloc(sizeof(SDL_Color));
    pEngineFontColor->r = 255;
    pEngineFontColor->g = 255;
    pEngineFontColor->b = 0;
    pEngineFontColor->a = 255;

    // if we are in debug mode
    // BUG/INFO: FPS COUNTER MUST ABSOLUTELY BE THE HIGHEST DEPTH OR THE RENDER ORDER FUDGES THE NUMBERS 
    // (we need to count from the first item which is the counter to get accurate numbers (i think))
    if(debugMode){
        // initialize logging
        log_init(debug);

        // display in console
        logMessage(debug, "Debug mode enabled.\n");

        // turn on debug overlay at launch in debug mode
        toggleOverlay();
    }
    else{
        log_init(warning); // if we launch not in debug mode, only log warnings and errors
    }

    // startup audio systems
    initAudio();

    // before we play our loud ass startup sound, lets check what volume the game wants
    // the engine to be at initially
    setVolume(-1, volume);

    /*
        Part of the engine startup which isnt configurable by the game is displaying
        a splash screen with the engine title and logo for 2550ms and playing a
        startup noise
    */
    if(skipintro){
        logMessage(info,"Skipping Intro.\n");
    }
    else{
        playSound("sfx/startup.mp3",0,0); // play startup sound

        // add startup splash image
        createImage(0,.5f,.5f,1.0f,1.0f,"engine_reserved/splash.png",true,ALIGN_STRETCH);

        // render everything in engine queue
        renderAll(); 

        // pause on engine splash for 2550ms (TODO: consider alternatives)
        SDL_Delay(2550); 
        
        // remove startup objects
        clearAll(false);
    }

    // render everything in engine queue after splash asset removal
    renderAll();

    // debug output
    logMessage(info, "Engine Fully Initialized.\n");
} // control is now resumed by the game

// function that shuts down all engine subsystems and components ()
void shutdownEngine(){
    logMessage(info, "Shutting down engine...\n");

    /*
        TECHNICALLY this might not have been called unless getPath() 
        was called at least once, however: i dont feel like fixing 
        this as the engine will always call this internally for the 
        splash screen (unless the argv for skipping is passed) in 
        which case it doesnt matter because its called in the game 
        and nobody will ever use this engine :P
    */

    // free the engine font color
    free(pEngineFontColor);
    pEngineFontColor = NULL;

    // shutdown graphics
    shutdownGraphics();
    logMessage(info, "Shut down graphics.\n");

    // shutdown audio
    shutdownAudio();
    logMessage(info, "Shut down audio.\n");

    // shutdown logging
    // note: must happen before SDL because it relies on SDL path to open file
    log_shutdown();
    SDL_free(base_path); // free base path after (used by logging)

    // quit SDL (should destroy anything else i forget)
    SDL_Quit();
}