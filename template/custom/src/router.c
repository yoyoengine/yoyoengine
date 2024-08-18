/*
    This file is derived from a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <yoyoengine/yoyoengine.h>

#include "yoyo_c_api.h"

/*
    Hello! Thank you for checking out yoyoengine :)

    This is the main routing point for any C scripting you wish to do.

    yoyo_register_callbacks is called once on init, and lets you
    give the engine function pointers to your custom functions.

    Unless you set these to persistant, they will be unlinked after a
    scene load.

    I've populated some stubs and examples for you below.
*/

void example_scene_load(char *scene_name){
    /*
        This will be fed the new scene name every time one is loaded.

        You can use this to conditionally register callbacks
    */
}

/*
    Default input handler! :)

    note: if you delete this, you must re-implement a handler for SDL_QUIT.
*/
void handle_input(SDL_Event event){
    if(event.type == SDL_QUIT){
        YG_RUNNING = false;
    }
}

/*
    This function is called once on init by the engine managed entry point.
*/
void yoyo_register_callbacks() {
    // register our input handler (persistent flag means it will never be unlinked)
    ye_register_event_cb(YE_EVENT_HANDLE_INPUT, handle_input, YE_EVENT_FLAG_PERSISTENT);

    // register our example scene load handler
    ye_register_event_cb(YE_EVENT_SCENE_LOAD, example_scene_load, YE_EVENT_FLAG_PERSISTENT);

    /*
        TIP:
        You are allowed to stack callbacks, so if you
        want multiple scene load handlers you can do that!
    */
}