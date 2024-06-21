/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
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

/*
    My vision is this, a simple wrapper on top of SDL2 input handling, which
    has some predefined "events" like up, down, left, right, action1, etc that
    all have cooresponding bindings assigned in a big struct map we can lookup in.
    This way, both C and Lua can easily get common inputs, while the dev can still
    use the manual C callback for more control. 
*/

#include <SDL2/SDL.h>

#include <yoyoengine/ui.h>
#include <yoyoengine/input.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/button.h>
#include <yoyoengine/ecs/transform.h>

/*
    Arbitrary number for now. sue me :)
*/
#define YE_MAX_CONTROLLERS 4

// struct ye_input_bind {
//     Uint32 bind;
// };

// struct ye_input_bindings {
// };

SDL_GameController *controllers[YE_MAX_CONTROLLERS];
void ye_init_input(){
    // if we are in editor mode, dont bother with controllers
    if(!YE_STATE.editor.editor_mode){
        int num_controllers = 0;
        for(int i = 0; i < YE_MAX_CONTROLLERS; i++){
            if(SDL_IsGameController(i)){
                controllers[i] = SDL_GameControllerOpen(i);
                if(controllers[i] != NULL){
                    num_controllers++;
                    ye_logf(info, "Detected GameController %d: %s\n", i, SDL_GameControllerName(controllers[i]));
                }
            }
        }
        ye_logf(info, "Detected %d controller(s).\n", num_controllers);
    }
    ye_logf(info, "Initialized Input Subsystem.\n");
}

// current event tracker
SDL_Event e;

// tracks whether the engine console is on screen 
bool console_visible = false;

// bool to track resize events
bool resized = false;

void ye_system_input() {
    // allow nuklear to intercept events to track ui changes
    ui_begin_input_checks();

    // main event handler
    while (SDL_PollEvent(&e)) {
        ui_handle_input(&e); // throw nuklear the event

        /*
            NOTE: im leaving this for now. It's been around for six months so idk if It's needed later
            // if resize event, set resized to true
            if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED){
                resized = true;
            }
        */

        switch(e.type) {
            // check for any reserved engine buttons (console, etc)
            case SDL_KEYDOWN:
                // if freecam is on (rare) TODO: allow changing of freecam scale
                if(YE_STATE.editor.freecam_enabled){
                    switch(e.key.keysym.sym){     
                        case SDLK_LEFT:
                            YE_STATE.engine.target_camera->transform->x -= 100.0;
                            break;
                        case SDLK_RIGHT:
                            YE_STATE.engine.target_camera->transform->x += 100.0;
                            break;
                        case SDLK_UP:
                            YE_STATE.engine.target_camera->transform->y -= 100.0;
                            break;
                        case SDLK_DOWN:
                            YE_STATE.engine.target_camera->transform->y += 100.0;
                            break;
                    }
                }

                switch(e.key.keysym.sym) {

                    // console toggle //

                    case SDLK_BACKQUOTE:
                        if(console_visible){
                            console_visible = false;
                            remove_ui_component("console");
                        }
                        else{
                            console_visible = true;
                            ui_register_component("console",ye_paint_console);
                        }
                        break;
                }
                break; // breaks out of keydown

            // window events //
            case SDL_WINDOWEVENT:
                switch(e.window.event){
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        resized = true;
                        break;
                }
                break;
        }

        // poll for ECS button events
        if(!YE_STATE.editor.editor_mode){
            ye_system_button(e);
        }

        // send event to callback specified by game (if needed)
        if(YE_STATE.engine.callbacks.input_handler != NULL){
            YE_STATE.engine.callbacks.input_handler(e);
        }

        /*
            Lookup this event in our mappings, and if its mapped to a binding let Lua and C know
        */
        // TODO
    }

    // end nuklear input feeding
    ui_end_input_checks();

    // if we resized, update all the meta that we need so we can render a new clean frame
    if(resized){
        int width, height;
        SDL_GetWindowSize(YE_STATE.runtime.window, &width, &height);
        
        // log the new size
        ye_logf(debug, "Window resized to %d, %d.\n", width, height);

        // update the engine state
        YE_STATE.engine.screen_width = width;
        YE_STATE.engine.screen_height = height;

        // editor camera update handled in editor_input.c

        // recompute pillar or letter boxing
        ye_recompute_boxing();
    }
}