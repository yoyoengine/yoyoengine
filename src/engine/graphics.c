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
#include <unistd.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <jansson.h>

#include <yoyoengine/yoyoengine.h>

// define globals for file
SDL_Window *pWindow = NULL;
SDL_Surface *pScreenSurface = NULL;
SDL_Renderer *pRenderer = NULL;
SDL_Texture* screen_buffer = NULL;

// int that increments each renderObject created, allowing new unique id's to be assigned
int lastChunkCount = 0;
int lastLinesWritten = 0;

// NOTE: negative global_id's are reserved for engine components, 
// and traversing the list to clear renderObjects will only clear 
// non engine renderObjects

// fps related counters
int fpsUpdateTime = 0;
int frameCounter = 0;
int fps = 0;
int startTime = 0;
int fpscap = 0;
int desiredFrameTime = 0;  

// initialize some variables in engine to track screen size
float targetAspectRatio = 16.0f / 9.0f;
int virtualWidth = 1920;
int virtualHeight = 1080;
int xOffset = 0;
int yOffset = 0;

int currentResolutionWidth = 1920;
int currentResolutionHeight = 1080;

/*
    Texture used for missing textures
*/
SDL_Texture *missing_texture = NULL;

// TODO: move most of engine runtime state into struct engine_data engine_state

char * render_scale_quality = "linear"; // also available: best (high def, high perf), nearest (sharp edges, pixel-y)

// create a cache to hold textures colors and fonts
VariantCollection* cache;

// converts a relative float to real screen pixel width
int convertToRealPixelWidth(float in){
    return (int)((float)in * (float)virtualWidth);
}

// converts a relative float to real screen pixel height
int convertToRealPixelHeight(float in){
    return (int)((float)in * (float)virtualHeight);
}

// creates a real screen pixel rect from relative float values
SDL_Rect createRealPixelRect(bool centered, float x, float y, float w, float h) {
    int realX = convertToRealPixelWidth(x);
    int realY = convertToRealPixelHeight(y);
    int realW = convertToRealPixelWidth(w);
    int realH = convertToRealPixelHeight(h);

    if (centered) {
        realX -= realW / 2;
        realY -= realH / 2;
    }

    SDL_Rect rect = {realX, realY, realW, realH};
    return rect;
}

// load a font into memory and return a pointer to it
// TODO: evaluate where this stands in relation to getFont functionality, does this just extend the backend of get Font?
TTF_Font *loadFont(char *pFontPath, int fontSize) {
    if(fontSize > 500){
        ye_logf(error, "ERROR: FONT SIZE TOO LARGE\n");
        return NULL;
    }
    char *fontpath = pFontPath;
    if(access(fontpath, F_OK) == -1){
        ye_logf(error, "Could not access file '%s'.\n", fontpath);
    }
    TTF_Font *pFont = TTF_OpenFont(fontpath, fontSize);
    if (pFont == NULL) {
        ye_logf(error, "Failed to load font: %s\n", TTF_GetError());
        return NULL;
    }
    ye_logf(debug, "Loaded font: %s\n", pFontPath);
    return pFont;
}

/*
    https://gamedev.stackexchange.com/questions/119642/how-to-use-sdl-ttfs-outlines
    
    NOTE: The actual width modifier depends on the resolution of the passed font.
    if we want to allow better consistency over the width between different res,
    we need to identify the font by its loaded width (can be stored with its data in cache)
    and then we could pass in a relative width modifier
*/
SDL_Texture *createTextTextureWithOutline(const char *pText, int width, TTF_Font *pFont, SDL_Color *pColor, SDL_Color *pOutlineColor) {
    int temp = TTF_GetFontOutline(pFont);

    SDL_Surface *fg_surface = TTF_RenderText_Blended(pFont, pText, *pColor); 

    TTF_SetFontOutline(pFont, width);
    
    SDL_Surface *bg_surface = TTF_RenderText_Blended(pFont, pText, *pOutlineColor); 
    
    SDL_Rect rect = {width, width, fg_surface->w, fg_surface->h}; 

    /* blit text onto its outline */ 
    SDL_SetSurfaceBlendMode(fg_surface, SDL_BLENDMODE_BLEND); 
    SDL_BlitSurface(fg_surface, NULL, bg_surface, &rect); 
    SDL_FreeSurface(fg_surface); 
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, bg_surface);
    SDL_FreeSurface(bg_surface);
    
    // error out if texture creation failed
    if (pTexture == NULL) {
        ye_logf(error, "Failed to create texture: %s\n", SDL_GetError());
        return NULL;
    }
    
    TTF_SetFontOutline(pFont, temp);

    return pTexture;
}

// Create a texture from text string with specified font and color, returns NULL for failure
SDL_Texture *createTextTexture(const char *pText, TTF_Font *pFont, SDL_Color *pColor) {
    // create surface from parameters
    SDL_Surface *pSurface = TTF_RenderUTF8_Blended(pFont, pText, *pColor); // MEMLEAK: valgrind says so but its not my fault, internal in TTF
    
    // error out if surface creation failed
    if (pSurface == NULL) {
        ye_logf(error, "Failed to render text: %s\n", TTF_GetError());
        return NULL;
    }

    // create texture from surface
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);

    // error out if texture creation failed
    if (pTexture == NULL) {
        ye_logf(error, "Failed to create texture: %s\n", SDL_GetError());
        return NULL;
    }

    // free the surface memory
    SDL_FreeSurface(pSurface);

    // return the created texture
    return pTexture;
}

// Create a texture from image path, returns its texture pointer and a flag on whether
// or not the texture is also cached
struct textureInfo createImageTexture(char *pPath, bool shouldCache) {
    // try to get it from cache TODO: should we wrap this in if shouldCache?
    Variant *pVariant = getVariant(cache, pPath);
    if (pVariant != NULL) { // found in cache
        // ye_logf(debug, "Found texture in cache\n");

        pVariant->refcount++; // increase refcount

        struct textureInfo ret = {pVariant->textureValue, true};
        // printf("refcount for %s: %d\n",pPath,pVariant->refcount);
        return ret;
    }
    else{ // not found in cache
        if(access(pPath, F_OK) == -1){
            ye_logf(error, "Could not access file '%s'.\n", pPath);
            return (struct textureInfo){missing_texture, NULL}; // return missing texture, error has been logged
        }

        // create surface from loading the image
        SDL_Surface *pImage_surface = IMG_Load(pPath);
        
        // error out if surface load failed
        if (!pImage_surface) {
            ye_logf(error, "Error loading image: %s\n", IMG_GetError());
            return (struct textureInfo){missing_texture, NULL}; // return missing texture, error has been logged
        }

        // create texture from surface
        SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pImage_surface);
        
        // error out if texture creation failed
        if (!pTexture) {
            ye_logf(error, "Error creating texture: %s\n", SDL_GetError());
            return (struct textureInfo){missing_texture, NULL}; // return missing texture, error has been logged
        }

        // release surface from memory
        SDL_FreeSurface(pImage_surface);
        if(shouldCache){ // we are going to cache it
            // cache the texture
            Variant variant;
            variant.type = VARIANT_TEXTURE;
            variant.textureValue = pTexture;
            variant.refcount = 1;

            addVariant(cache, pPath, variant);

            // char buffer[100];
            // snprintf(buffer, sizeof(buffer),  "Cached a texture. key: %s\n", pPath);
            // ye_logf(debug, buffer);

            if(getVariant(cache, pPath) == NULL){
                ye_logf(error, "ERROR CACHING TEXTURE\n");
            }

            struct textureInfo ret = {pTexture, true};
            
            // return the created texture
            // printf("refcount for %s: %d\n",pPath,variant.refcount);
            return ret;

        }
        else{ // we are not going to cache it
            struct textureInfo ret = {pTexture, false}; 

            return ret;
        }
    }
}

/*
    takes in a key (relative resource path to a font) and returns a 
    pointer to the loaded font (caches the font if it doesnt exist in cache)
*/
TTF_Font *getFont(char *key){
    Variant *v = getVariant(cache, key);
    if(v == NULL){
        Variant fontVariant;
        fontVariant.type = VARIANT_FONT;
        fontVariant.fontValue = loadFont(key,100);

        addVariant(cache, key, fontVariant);
        ye_logf(debug, "Cached a font. key: %s\n", key);
        v = getVariant(cache, key);
    }
    else{
        // ye_logf(debug, "Found cached font.\n");
    }
    return v->fontValue;
}

/*
    takes in a key (relative resource path to a font) and returns a 
    pointer to the loaded font (caches the font if it doesnt exist in cache)

    This is mainly here for the game, the engine cant utilize this super well as
    it doesnt have an understanding of named colors, it can just cache them for quick
    access by the game
*/
SDL_Color *getColor(char *key, SDL_Color color){
    Variant *v = getVariant(cache, key);
    if(v == NULL){
        Variant colorVariant;
        colorVariant.type = VARIANT_COLOR;
        colorVariant.colorValue = color;

        addVariant(cache, key, colorVariant);
        ye_logf(debug, "Cached a color. key: %s\n", key);
        v = getVariant(cache, key);
    }
    else{
        // ye_logf(debug, "Found cached color.\n");
    }
    SDL_Color *pColor = &v->colorValue;
    return pColor;
}

/*
    Massive function to handle the orientation of an object within a set of bounds
*/
void autoFitBounds(SDL_Rect* bounds, SDL_Rect* obj, Alignment alignment){
    // check if some loser wants to stretch something
    if(alignment == ALIGN_STRETCH){
        obj->w = bounds->w;
        obj->h = bounds->h;
        obj->x = bounds->x;
        obj->y = bounds->y;
        return;
    }

    // actual orientation handling

    float boundsAspectRatio = (float)bounds->w / (float)bounds->h;
    float objectAspectRatio = (float)obj->w / (float)obj->h;

    if (objectAspectRatio > boundsAspectRatio) {
        obj->h = bounds->w / objectAspectRatio;
        obj->w = bounds->w;
    } else {
        obj->w = bounds->h * objectAspectRatio;
        obj->h = bounds->h;
    }

    switch(alignment) {
        case ALIGN_TOP_LEFT:
            obj->x = bounds->x;
            obj->y = bounds->y;
            break;
        case ALIGN_TOP_CENTER:
            obj->x = bounds->x + (bounds->w - obj->w) / 2;
            obj->y = bounds->y;
            break;
        case ALIGN_TOP_RIGHT:
            obj->x = bounds->x + (bounds->w - obj->w);
            obj->y = bounds->y;
            break;
        case ALIGN_MID_LEFT:
            obj->x = bounds->x;
            obj->y = bounds->y + (bounds->h - obj->h) / 2;
            break;
        case ALIGN_MID_CENTER:
            obj->x = bounds->x + (bounds->w - obj->w) / 2;
            obj->y = bounds->y + (bounds->h - obj->h) / 2;
            break;
        case ALIGN_MID_RIGHT:
            obj->x = bounds->x + (bounds->w - obj->w);
            obj->y = bounds->y + (bounds->h - obj->h) / 2;
            break;
        case ALIGN_BOT_LEFT:
            obj->x = bounds->x;
            obj->y = bounds->y + (bounds->h - obj->h);
            break;
        case ALIGN_BOT_CENTER:
            obj->x = bounds->x + (bounds->w - obj->w) / 2;
            obj->y = bounds->y + (bounds->h - obj->h);
            break;
        case ALIGN_BOT_RIGHT:
            obj->x = bounds->x + (bounds->w - obj->w);
            obj->y = bounds->y + (bounds->h - obj->h);
            break;
        default:
            ye_logf(error, "Invalid alignment\n");
            break;
    }
}

/*
    Will attempt to grab a variant by name, and decrease its refcount, destroying it if it hits 0
*/
void imageDecref(char *name){
    Variant *v = getVariant(cache, name);
    if(v == NULL){
        ye_logf(error, "ERROR DECREFFING IMAGE: IMAGE NOT FOUND\n");
        return;
    }
    v->refcount--;
    if(v->refcount <= 0){
        removeVariant(cache,name); // this is responsible for destroying the texture
    }
}

// render everything in the scene
void renderAll() {
    int frameStart = SDL_GetTicks();

    // Set the render target to the screen
    SDL_SetRenderTarget(pRenderer, NULL);
    
    // Set background color to black
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);

    // Clear the window with the set background color
    SDL_RenderClear(pRenderer);

    // Set the render target to the screen buffer
    SDL_SetRenderTarget(pRenderer, screen_buffer);
    
    // Set background color to black
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);

    // Clear the window with the set background color
    SDL_RenderClear(pRenderer);

    // Get paint start timestamp
    Uint32 paintStartTime = SDL_GetTicks();

    // if we are showing metrics and need to update them (this does not really save that much perf)
    if(engine_state.metrics_visible){
        // increment the frame counter
        frameCounter++;

        // if we have waited long enough to update the display
        if (SDL_GetTicks() - fpsUpdateTime >= 250) {
            // get the elapsed time and scale it to our time factor to get fps
            fpsUpdateTime = SDL_GetTicks();
            fps = frameCounter * 4;
            frameCounter = 0; // reset counted frames
            engine_runtime_state.fps = fps;
        }
    }


    // INDEV TESTING: renderer system paint all entities with renderer
    ye_system_renderer(pRenderer);

    /*
        If we are in the editor, we want to paint the scene to a nuklear window viewport
        Then we paint the ui to the actual screen, not our intermediary buffer
    */
    if(engine_state.editor_mode){
        // Reset the render target to the default
        SDL_SetRenderTarget(pRenderer, NULL);

        // seems like we arent going to be able to paint the SDL_Texture into nuklear viewport (wihtout significant modification)
        // so for now, lets just paint it to the upper 25% of the screen
        SDL_Rect viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.w = currentResolutionWidth / 1.5;
        viewport.h = currentResolutionHeight / 1.5;

        // // draw some grid lines (offset by camera position) within the viweport bounds to serve as backdrop
        // SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
        // // SDL_RenderDrawLine(pRenderer, viewport.x, viewport.y, viewport.x + viewport.w, viewport.y);
        // // SDL_RenderDrawLine(pRenderer, viewport.x, viewport.y, viewport.x, viewport.y + viewport.h);
        // // SDL_RenderDrawLine(pRenderer, viewport.x + viewport.w, viewport.y, viewport.x + viewport.w, viewport.y + viewport.h);
        // // SDL_RenderDrawLine(pRenderer, viewport.x, viewport.y + viewport.h, viewport.x + viewport.w, viewport.y + viewport.h);
        // // we want a grid of 100x100 lines
        // int gridWidth = 100;
        // int gridHeight = 100;
        // int gridX = viewport.x + (int)engine_state.target_camera->transform->rect.x % gridWidth;
        // int gridY = viewport.y + (int)engine_state.target_camera->transform->rect.y % gridHeight;
        // for(int i = 0; i < viewport.w / gridWidth; i++){
        //     SDL_RenderDrawLine(pRenderer, gridX + (i * gridWidth), viewport.y, gridX + (i * gridWidth), viewport.y + viewport.h);
        // }

        SDL_RenderCopy(pRenderer, screen_buffer, NULL, &viewport);        

        // update ui (TODO: profile if this is an expensive op)
        ui_render();         
    }else{
        // update ui (TODO: profile if this is an expensive op)
        ui_render(); 
        
        // Reset the render target to the default
        SDL_SetRenderTarget(pRenderer, NULL);

        // copy the screen buffer to the renderer
        SDL_RenderCopy(pRenderer, screen_buffer, NULL, NULL);
    }

    // present our new changes to the renderer
    SDL_RenderPresent(pRenderer);

    // update the window to reflect the new renderer changes
    SDL_UpdateWindowSurface(pWindow);

    // Get paint end timestamp
    Uint32 paintEndTime = SDL_GetTicks();

    // Calculate paint time
    Uint32 paintTime = paintEndTime - paintStartTime;

    // set the end of the render frame
    int frameEnd = SDL_GetTicks();

    engine_runtime_state.frame_time = frameEnd - frameStart;

    // if we arent on vsync we need to preform some frame calculations to delay next frame
    if(fpscap != -1){
        // check the desired FPS cap and add delay if needed
        if (engine_runtime_state.frame_time < desiredFrameTime) {
            SDL_Delay(desiredFrameTime - engine_runtime_state.frame_time);
        }
    }

    engine_runtime_state.frame_time = SDL_GetTicks() - frameStart;
}

// method to ensure our game content is centered and scaled well
void setViewport(int screenWidth, int screenHeight){
    // get our current aspect ratio
    float currentAspectRatio = (float)screenWidth / (float)screenHeight;

    // if aspect ratio is too wide
    if (currentAspectRatio >= targetAspectRatio) {
        // set our width to be the max that can accomidate the height
        virtualWidth = (int)(screenHeight * targetAspectRatio);
        virtualHeight = screenHeight;
        xOffset = (screenWidth - virtualWidth) / 2;
    } 
    // if aspect ratio is too tall
    else {
        // set our width to be the max that can fit
        virtualWidth = screenWidth;
        virtualHeight = screenWidth / targetAspectRatio;
        yOffset = (screenHeight - virtualHeight) / 2;
    }

    // debug outputs
    ye_logf(debug, "Targeting aspect ratio: %f\n",targetAspectRatio);
    ye_logf(debug, "Virtual Resolution: %dx%d\n",virtualWidth,virtualHeight);
    ye_logf(debug, "(unused) offset: %dx%d\n",xOffset,yOffset);

    // setup viewport with our virtual resolutions
    SDL_Rect viewport;
    viewport.x = xOffset;
    viewport.y = yOffset;
    viewport.w = virtualWidth;
    viewport.h = virtualHeight;
    SDL_RenderSetViewport(pRenderer, &viewport);
}

// /*
//     method to change the window mode, flag passed in will set the window mode
//     method will also change the resolution to match the new window mode:
//     fullscreen - will auto detect screen resolution and set it
//     windowed - will set the resolution to 1920x1080
// */
void changeWindowMode(Uint32 flag)
{
    int success = SDL_SetWindowFullscreen(pWindow, flag);
    if(success < 0) 
    {
        ye_logf(error, "ERROR: COULD NOT CHANGE WINDOW MODE\n");
        return;
    }
    else
    {
        ye_logf(info, "Changed window mode.\n");
    }

    if(flag == 0){
        changeResolution(1920, 1080);
    }
    else{
        SDL_DisplayMode displayMode;
        if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
            ye_logf(error, "SDL_GetCurrentDisplayMode failed!\n");
            return;
        }
        int screenWidth = displayMode.w;
        int screenHeight = displayMode.h;
        
        ye_logf(debug,  "Inferred screen size: %dx%d\n", screenWidth, screenHeight);

        changeResolution(screenWidth, screenHeight);
    }
}

// /*
//     Shuts down current renderer, creates a new renderer with or withou
//     vsync according to passed bool
// */
void setVsync(bool enabled) {
    SDL_DestroyRenderer(pRenderer);
    ye_logf(debug, "Renderer destroyed to toggle vsync.\n");

    uint32_t flags = SDL_RENDERER_ACCELERATED;
    if (enabled) {
        flags |= SDL_RENDERER_PRESENTVSYNC;
    }

    // Reset the texture filtering hint
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, render_scale_quality);

    pRenderer = SDL_CreateRenderer(pWindow, -1, flags);
    ye_logf(debug, "Renderer re-created.\n");

    if (pRenderer == NULL) {
        ye_logf(warning,"ERROR RE-CREATING RENDERER\n");     
        exit(1);
    }

    // NOTE: in fullscreen, resetting the render means we need to reset our viewport
    setViewport(currentResolutionWidth,currentResolutionHeight);
}

// /*
//     Changes the game fps cap to the passed integer
//     TODO: add checks for if we need to change vsync to save performance
// */
void changeFPS(int cap){
    if(cap == -1){
        setVsync(true);
    }
    else{
        setVsync(false);
        fpscap = cap;
        desiredFrameTime = 1000 / fpscap;
    }
}

/*
    returns a ScreenSize struct of the current resolution w,h
*/
struct ScreenSize getCurrentResolution(){
    struct ScreenSize screensize = {currentResolutionWidth,currentResolutionHeight}; // TODO: this sucks
    return screensize;
}

/*
    Changes the game resolution to the passed width and height
*/
void changeResolution(int width, int height) {
    SDL_SetWindowSize(pWindow, width, height);
    setViewport(width, height);
    currentResolutionWidth = width;
    currentResolutionHeight = height;
}

// initialize graphics
void initGraphics(int screenWidth,int screenHeight, int windowMode, int framecap, char *title, char *icon_path){
    // test for video init, alarm if failed
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        ye_logf(debug, "SDL initialization failed: %s\n", SDL_GetError());
        exit(1);
    }

    ye_logf(info, "SDL initialized.\n");

    // Set the texture filtering hint
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, render_scale_quality);

    // test for window init, alarm if failed
    pWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | windowMode | SDL_WINDOW_ALLOW_HIGHDPI);
    if (pWindow == NULL) {
        ye_logf(debug, "Window creation failed: %s\n", SDL_GetError());
        exit(1);
    }
    
    ye_logf(info, "Window initialized.\n");
    
    // set our fps cap to the frame cap param
    // (-1) for vsync
    fpscap = framecap;
    desiredFrameTime = 1000 / fpscap;  

    // if vsync is on
    if(fpscap == -1) {
        ye_logf(info, "Starting renderer with vsync... \n");
        pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    }
    else {
        ye_logf(debug, "Starting renderer with maxfps %d... \n",framecap);
        pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
    }

    if (pRenderer == NULL) {
        ye_logf(error, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        exit(1);
    }

    // load our missing texture into memory
    SDL_Surface *missing_surface = IMG_Load(ye_get_engine_resource_static("missing.png"));
    missing_texture = SDL_CreateTextureFromSurface(pRenderer, missing_surface);
    SDL_FreeSurface(missing_surface);

    init_ui(pWindow,pRenderer);

    // TODO: only do this in debug mode, we need to check engine state for that
    // TODO: the refactor is that this debug overlay needs to move to this file from ui.c, and use the local vars from here. i dont care enough to do this now
    // ui_register_component(ui_paint_debug_overlay, "debug_overlay");

    // set our viewport to the screen size with neccessary computed offsets
    setViewport(screenWidth, screenHeight);

    /*
        Create our screen buffer

        This will be used as a paint target seperate from what we present to the screen
        This way we can paint our full output to either a texture/ui panel or the screen
    */
    screen_buffer = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, screenWidth, screenHeight);
    
    // test for TTF init, alarm if failed
    if (TTF_Init() == -1) {
        ye_logf(error, "SDL2_ttf could not initialize! SDL2_ttf Error: %s\n", TTF_GetError());
        exit(1);
    }
    ye_logf(info, "TTF initialized.\n");

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        ye_logf(error, "IMG_Init error: %s", IMG_GetError());
        exit(1);
    }
    ye_logf(info, "IMG initialized.\n");

    // initialize cache
    cache = createVariantCollection();

    // load icon to surface
    printf("icon path: %s\n", icon_path);
    SDL_Surface *pIconSurface = IMG_Load(icon_path);
    if (pIconSurface == NULL) {
        ye_logf(error, "IMG_Load error: %s", IMG_GetError());
        exit(1);
    }
    // set icon
    SDL_SetWindowIcon(pWindow, pIconSurface);
    
    // release surface
    SDL_FreeSurface(pIconSurface);

    ye_logf(info, "Window icon set.\n");

    // set a start time for counting fps
    startTime = SDL_GetTicks();
}

// shuts down all initialzied graphics systems
void shutdownGraphics(){
    // destroy cache
    destroyVariantCollection(cache);

    // shutdown TTF
    TTF_Quit();
    ye_logf(info, "Shut down TTF.\n");

    // shutdown IMG
    IMG_Quit();
    ye_logf(info, "Shut down IMG.\n");

    // free the missing texture
    SDL_DestroyTexture(missing_texture);

    shutdown_ui();

    // shutdown renderer
    SDL_DestroyRenderer(pRenderer);
    ye_logf(info, "Shut down renderer.\n");

    // shutdown window
    SDL_DestroyWindow(pWindow);
    ye_logf(info, "Shut down window.\n");
}