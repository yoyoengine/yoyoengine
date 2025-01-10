/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    My vision is this, a simple wrapper on top of SDL2 input handling, which
    has some predefined "events" like up, down, left, right, action1, etc that
    all have cooresponding bindings assigned in a big struct map we can lookup in.
    This way, both C and Lua can easily get common inputs, while the dev can still
    use the manual C callback for more control. 
*/

#include <SDL.h>

#include <yoyoengine/ui.h>
#include <yoyoengine/input.h>
#include <yoyoengine/event.h>
#include <yoyoengine/engine.h>
#include <yoyoengine/console.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/button.h>
#include <yoyoengine/ecs/transform.h>

void ye_init_input(){
    // if we are in editor mode, dont bother with controllers
    // if(!YE_STATE.editor.editor_mode){
    //     YE_STATE.runtime.num_controllers = 0;
    //     for(int i = 0; i < YE_MAX_CONTROLLERS; i++){
    //         if(SDL_IsGameController(i)){
    //             YE_STATE.runtime.controllers[i] = SDL_GameControllerOpen(i);
    //             if(YE_STATE.runtime.controllers[i] != NULL){
    //                 YE_STATE.runtime.num_controllers++;
    //                 ye_logf(info, "Detected GameController %d: %s\n", i, SDL_GameControllerName(YE_STATE.runtime.controllers[i]));
    //             }
    //         }
    //     }
    //     ye_logf(info, "Detected %d controller(s).\n", YE_STATE.runtime.num_controllers);
    // }

    // ^ now that we observe disconnects and connects, we dont need this initialization... leaving for posterity

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
                            remove_ui_component("ye_dev_console");
                        }
                        else{
                            console_visible = true;
                            ui_register_component("ye_dev_console", ye_paint_developer_console); // TODO: prob decouple this from here
                            ye_console_reset_scroll = true;
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

        // controller stuff that only applies in game mode
        if(!YE_STATE.editor.editor_mode){
            switch(e.type){

                // controller connect/disconnect events //
                case SDL_CONTROLLERDEVICEREMOVED:
                    // remove the controller from the list
                    for(int i = 0; i < YE_MAX_CONTROLLERS; i++){
                        if(YE_STATE.runtime.controllers[i] != NULL){
                            if(SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(YE_STATE.runtime.controllers[i])) == e.cdevice.which){
                                SDL_GameControllerClose(YE_STATE.runtime.controllers[i]);
                                YE_STATE.runtime.controllers[i] = NULL;
                                YE_STATE.runtime.num_controllers--;
                                ye_logf(info, "Disconnected GameController %d.\n", e.cdevice.which);
                            }
                        }
                    }
                    break;

                case SDL_CONTROLLERDEVICEADDED:
                    // add the controller to the list
                    for(int i = 0; i < YE_MAX_CONTROLLERS; i++){
                        if(YE_STATE.runtime.controllers[i] == NULL){
                            YE_STATE.runtime.controllers[i] = SDL_GameControllerOpen(e.cdevice.which);
                            if(YE_STATE.runtime.controllers[i] != NULL){
                                YE_STATE.runtime.num_controllers++;
                                ye_logf(info, "Connected GameController %d: %s\n", e.cdevice.which, SDL_GameControllerName(YE_STATE.runtime.controllers[i]));
                            }
                            break;
                        }
                    }
                    break;

            }
        }

        // poll for ECS button events
        if(!YE_STATE.editor.editor_mode){
            ye_system_button(e);
        }

        // attempt to send event to callback specified by game
        ye_fire_event(YE_EVENT_HANDLE_INPUT, (union ye_event_args){.input = e});

        /*
            Currently lua polls inputs through its own medium (direct query as needed),
            so we don't need to do more here, unless we wanted to add a keybind layer later.
        */
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

        // reset the flag
        resized = false;
    }
}

void ye_shutdown_input(){
    // close all controllers
    for(int i = 0; i < YE_MAX_CONTROLLERS; i++){
        if(YE_STATE.runtime.controllers[i] != NULL){
            SDL_GameControllerClose(YE_STATE.runtime.controllers[i]);
        }
    }

    // might not be needed but whatever
    YE_STATE.runtime.num_controllers = 0;

    ye_logf(info, "Shut down input.\n");
}