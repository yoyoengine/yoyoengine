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

/*
    Texture used for missing textures
*/
SDL_Texture *missing_texture = NULL;

// TODO: move most of engine runtime state into struct engine_data engine_state

char * render_scale_quality = "linear"; // also available: best (high def, high perf), nearest (sharp edges, pixel-y)

TTF_Font * ye_load_font(const char *pFontPath, int fontSize) {
    if(fontSize > 500){
        ye_logf(error, "ERROR: FONT SIZE TOO LARGE\n");
        return YE_STATE.engine.pEngineFont;
    }
    const char *fontpath = pFontPath;
    if(access(fontpath, F_OK) == -1){
        ye_logf(error, "Could not access file '%s'.\n", fontpath);
        return YE_STATE.engine.pEngineFont;
    }
    TTF_Font *pFont = TTF_OpenFont(fontpath, fontSize);
    if (pFont == NULL) {
        ye_logf(error, "Failed to load font: %s\n", TTF_GetError());
        return YE_STATE.engine.pEngineFont;
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

SDL_Texture * ye_create_image_texture(const char *pPath) {
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

// variables for render all :3
int frame_counter = 0;
int desired_frame_time = 0;
int fpsUpdateTime = 0;
int fps = 0;

/*
    TODO: Viewports are a blessing and should actually be used for dynamic screen sizing.
*/
void ye_render_all() {
    int frameStart = SDL_GetTicks();

    // TODO: potential optimization here, only count fps if we need to.
    if(true){
        // increment the frame counter
        frame_counter++;

        // if we have waited long enough to update the display
        if (SDL_GetTicks() - fpsUpdateTime >= 250) {
            // get the elapsed time and scale it to our time factor to get fps
            fpsUpdateTime = SDL_GetTicks();
            fps = frame_counter * 4;
            frame_counter = 0; // reset counted frames
            YE_STATE.runtime.fps = fps;
        }
    }

    /*
        Clear the screen
    */
    // SDL_SetRenderTarget(pRenderer, NULL); Disabled, redundant since we dont change targets anymore
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pRenderer);

    /*
        If we are in editor mode paint to a viewport

        This viewport is for the scene preview
    */
    if(YE_STATE.editor.editor_mode){
        SDL_Rect viewport;
        viewport.x = 0;
        viewport.y = 35;
        viewport.w = YE_STATE.engine.screen_width / 1.5;
        viewport.h = 35 + YE_STATE.engine.screen_height / 1.5;
        SDL_RenderSetViewport(pRenderer, &viewport);
    }

    /*
        If we are not keeping the viewport aspect ratio,we can calculate
        our scale to scale the results as if we were zooming in or out
    */
    if(!YE_STATE.engine.stretch_viewport){
        float scaleX = (float)YE_STATE.engine.screen_width / (float)YE_STATE.engine.target_camera->camera->view_field.w;
        float scaleY = (float)YE_STATE.engine.screen_height / (float)YE_STATE.engine.target_camera->camera->view_field.h;
        SDL_RenderSetScale(pRenderer, scaleX, scaleY);
    }

    ye_system_renderer(pRenderer);

    /*
        Reset the viewport and scale to render the ui on top.

        TODO: profile the performance of doing this even if these havent changed
    */
    SDL_RenderSetViewport(pRenderer, NULL);
    SDL_RenderSetScale(pRenderer, 1.0f, 1.0f);

    ui_render();

    SDL_RenderPresent(pRenderer);
    SDL_UpdateWindowSurface(pWindow);

    // set the end of the render frame
    int frameEnd = SDL_GetTicks();

    YE_STATE.runtime.paint_time = frameEnd - frameStart;

    // if we arent on vsync we need to preform some frame calculations to delay next frame
    if(YE_STATE.engine.framecap != -1){
        // check the desired FPS cap and add delay if needed
        if(frameEnd - frameStart < desired_frame_time){
            SDL_Delay(desired_frame_time - (frameEnd - frameStart));
        }
    }
}


void ye_init_graphics(){
    // test for video init, alarm if failed
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        ye_logf(debug, "SDL initialization failed: %s\n", SDL_GetError());
        exit(1);
    }

    ye_logf(info, "SDL initialized.\n");

    // Set the texture filtering hint
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, render_scale_quality);

    // test for window init, alarm if failed
    pWindow = SDL_CreateWindow(YE_STATE.engine.window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, YE_STATE.engine.screen_width, YE_STATE.engine.screen_height, SDL_WINDOW_SHOWN | YE_STATE.engine.window_mode | SDL_WINDOW_ALLOW_HIGHDPI);
    if (pWindow == NULL) {
        ye_logf(debug, "Window creation failed: %s\n", SDL_GetError());
        exit(1);
    }
    
    ye_logf(info, "Window initialized.\n");
    
    // set our fps cap to the frame cap param
    // (-1) for vsync
    desired_frame_time = (int)(1000 / YE_STATE.engine.framecap);  

    // if vsync is on
    if(YE_STATE.engine.framecap == -1) {
        ye_logf(info, "Starting renderer with vsync... \n");
        pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    }
    else {
        ye_logf(debug, "Starting renderer with maxfps %d... \n",YE_STATE.engine.framecap);
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

    // set the runtime window and renderer references
    YE_STATE.runtime.window = pWindow;
    YE_STATE.runtime.renderer = pRenderer;

    init_ui(pWindow,pRenderer);

    // test for TTF init, alarm if failed
    if (TTF_Init() == -1) {
        ye_logf(error, "SDL2_ttf could not initialize! SDL2_ttf Error: %s\n", TTF_GetError());
        exit(1);
    }
    ye_logf(info, "TTF initialized.\n");

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        ye_logf(error, "IMG_Init error: %s", IMG_GetError());
        // print the img error
        printf("IMG_Init error: %s", IMG_GetError());
        exit(1);
    }
    ye_logf(info, "IMG initialized.\n");

    // load icon to surface
    SDL_Surface *pIconSurface = IMG_Load(YE_STATE.engine.icon_path);
    if (pIconSurface == NULL) {
        ye_logf(error, "IMG_Load error: %s", IMG_GetError());
        exit(1);
    }
    // set icon
    SDL_SetWindowIcon(pWindow, pIconSurface);
    
    // release surface
    SDL_FreeSurface(pIconSurface);

    ye_logf(info, "Window icon set.\n");
}

void ye_shutdown_graphics(){
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