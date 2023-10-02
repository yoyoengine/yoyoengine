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

#include <yoyoengine/yoyoengine.h>

// define globals for file
SDL_Window *pWindow = NULL;
SDL_Surface *pScreenSurface = NULL;
SDL_Renderer *pRenderer = NULL;
SDL_Texture* screen_buffer = NULL;

// fps related counters
int fpsUpdateTime = 0;
int frameCounter = 0;
int fps = 0;
int startTime = 0;
int fpscap = 0;
int desiredFrameTime = 0;  

/*
    Texture used for missing textures
*/
SDL_Texture *missing_texture = NULL;

// TODO: move most of engine runtime state into struct engine_data engine_state

char * render_scale_quality = "linear"; // also available: best (high def, high perf), nearest (sharp edges, pixel-y)

/*
    Loads a font from a path and size, returns a pointer to the font, or NULL if failed
*/
TTF_Font * ye_load_font(char *pFontPath, int fontSize) {
    if(fontSize > 500){
        ye_logf(error, "ERROR: FONT SIZE TOO LARGE\n");
        return pEngineFont;
    }
    char *fontpath = pFontPath;
    if(access(fontpath, F_OK) == -1){
        ye_logf(error, "Could not access file '%s'.\n", fontpath);
        return pEngineFont;
    }
    TTF_Font *pFont = TTF_OpenFont(fontpath, fontSize);
    if (pFont == NULL) {
        ye_logf(error, "Failed to load font: %s\n", TTF_GetError());
        return pEngineFont;
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

/*
    Create a text texture from a string, font, and color. Returns a pointer to the texture, or the default missing texture if failed
*/
SDL_Texture *createTextTexture(const char *pText, TTF_Font *pFont, SDL_Color *pColor) {
    // create surface from parameters
    SDL_Surface *pSurface = TTF_RenderUTF8_Blended(pFont, pText, *pColor); // MEMLEAK: valgrind says so but its not my fault, internal in TTF
    
    // error out if surface creation failed
    if (pSurface == NULL) {
        ye_logf(error, "Failed to render text: %s\n", TTF_GetError());
        return missing_texture; // return missing texture, error has been logged
    }

    // create texture from surface
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);

    // error out if texture creation failed
    if (pTexture == NULL) {
        ye_logf(error, "Failed to create texture: %s\n", SDL_GetError());
        return missing_texture; // return missing texture, error has been logged
    }

    // free the surface memory
    SDL_FreeSurface(pSurface);

    // return the created texture
    return pTexture;
}

/*
    Create a texture from a local image path, returns a pointer to the texture (and to the missing texture texture if it fails)
*/
SDL_Texture * ye_create_image_texture(char *pPath) {
    // check the file exists
    if(access(pPath, F_OK) == -1){
        ye_logf(error, "Could not access file '%s'.\n", pPath);
        return missing_texture; // return missing texture, error has been logged
    }

    // create surface from loading the image
    SDL_Surface *pImage_surface = IMG_Load(pPath);
    
    // error out if surface load failed
    if (!pImage_surface) {
        ye_logf(error, "Error loading image: %s\n", IMG_GetError());
        return missing_texture; // return missing texture, error has been logged
    }

    // create texture from surface
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pImage_surface);
    
    // error out if texture creation failed
    if (!pTexture) {
        ye_logf(error, "Error creating texture: %s\n", SDL_GetError());
        return missing_texture; // return missing texture, error has been logged
    }

    // release surface from memory
    SDL_FreeSurface(pImage_surface);

    // return the created texture
    return pTexture;
}

// render everything in the scene
void renderAll() {
    int frameStart = SDL_GetTicks();

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

    // Set the render target to the screen
    SDL_SetRenderTarget(pRenderer, NULL);
    
    // Set background color to black
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);

    // Clear the window with the set background color
    SDL_RenderClear(pRenderer);

    /*
        Do anything special in editor mode
    */
    if(engine_state.editor_mode){
        // render all entities
        ye_system_renderer(pRenderer);

        // seems like we arent going to be able to paint the SDL_Texture into nuklear viewport (wihtout significant modification)
        // so for now, lets just paint it to the upper 25% of the screen
        SDL_Rect viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.w = engine_state.screen_width / 1.5;
        viewport.h = engine_state.screen_height / 1.5;
        SDL_RenderCopy(pRenderer, screen_buffer, NULL, &viewport);
    }
    else{
        // Set the render target to the screen buffer
        SDL_SetRenderTarget(pRenderer, screen_buffer);

        // Set background color to black
        SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);

        // Clear the window with the set background color
        SDL_RenderClear(pRenderer);

        // render all entities
        ye_system_renderer(pRenderer);

        // Reset the render target to the default
        SDL_SetRenderTarget(pRenderer, NULL);

        // render the screen buffer to the screen
        SDL_RenderCopy(pRenderer, screen_buffer, NULL, NULL);
    }

    // update ui (TODO: profile if this is an expensive op)
    ui_render(); 

    // present our new changes to the renderer
    SDL_RenderPresent(pRenderer);

    // update the window to reflect the new renderer changes
    SDL_UpdateWindowSurface(pWindow);

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

    // load icon to surface
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

// helper function to get the current window size, if fullscreen it gets the monitor size
struct ScreenSize ye_get_screen_size(){
    struct ScreenSize screenSize;
    if(SDL_GetWindowFlags(pWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP){
        SDL_DisplayMode DM;
        SDL_GetDesktopDisplayMode(0, &DM);
        screenSize.width = DM.w;
        screenSize.height = DM.h;
    }
    else if(SDL_GetWindowFlags(pWindow) & SDL_WINDOW_FULLSCREEN){
        SDL_GetWindowSize(pWindow, &screenSize.width, &screenSize.height);
    }
    else {
        SDL_GetWindowSize(pWindow, &screenSize.width, &screenSize.height);
    }
    return screenSize;
}