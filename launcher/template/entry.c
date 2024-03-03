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

/*
    DO NOT MODIFY THIS FILE!!!!!!
    If you wish to implement custom behavior, define a function according to the custom C scripting docs.

    If you wish to modify certain startup variables based on things like savedata (user set volume to zero but default launch volume is 100)
    you can do so with YOYO_PRE_INIT and write directly to settings.yoyo
*/

#include <yoyoengine/yoyoengine.h>
#include "yoyo_c_api.h"

// if windows include windows.h
#ifdef _WIN32
    #include <windows.h>
#endif

bool YG_RUNNING; // initialize extern declared in yoyo_c_api.h

/*
    This input handler gets bound if there is no user defined one
*/
#ifndef YOYO_HANDLE_INPUT
void _example_input_handler(SDL_Event event){
    if(event.type == SDL_QUIT){
        YG_RUNNING = false;
    }
}
#endif

#ifndef YOYO_LUA_REGISTER
int exit_game(lua_State *L){
    YG_RUNNING = false;
    return 0;
}

void _register_lua_api(lua_State *L){
    // register custom lua api
    lua_register(L, "exitGame", exit_game);
}
#endif

static void mainloop(void){
    if(YG_RUNNING){
        ye_process_frame();
    }
    else{
        #ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
        #endif

        #ifdef YOYO_PRE_SHUTDOWN
            // run the pre shutdown function
            yoyo_pre_shutdown();
        #endif

        // shutdown engine
        ye_shutdown_engine();

        #ifdef YOYO_POST_SHUTDOWN
            // run the post shutdown function
            yoyo_post_shutdown();
        #endif

        // printf("Game exited successfully\n");
        exit(0);
    }
}

/*
    MAIN ENTRY POINT

    THIS CONTAINS THE GAME LOOP
*/
#ifdef __WIN32__
int main(int argc, char *argv[]){
#else
void main(void){
#endif
    YG_RUNNING = true;

    ye_logf(info, "Starting init\n");
    
    #ifdef YOYO_PRE_INIT
        // run the pre init function
        yoyo_pre_init();
    #endif

    ye_init_engine();

    #ifdef YOYO_POST_INIT
        // run the post init function
        yoyo_post_init();
    #endif

    #ifdef YOYO_HANDLE_INPUT
        YE_STATE.engine.callbacks.input_handler = yoyo_handle_input;
    #else
        YE_STATE.engine.callbacks.input_handler = _example_input_handler;
    #endif

    #ifdef YOYO_LUA_REGISTER
        YE_STATE.engine.callbacks.register_lua = yoyo_lua_register;
    #else
        YE_STATE.engine.callbacks.register_lua = _register_lua_api;
    #endif

    // assign function pointers for custom scripted behavior
    #ifdef YOYO_PRE_FRAME
        YE_STATE.engine.callbacks.pre_frame = yoyo_pre_frame;
    #else
        YE_STATE.engine.callbacks.pre_frame = NULL;
    #endif

    #ifdef YOYO_POST_FRAME
        YE_STATE.engine.callbacks.post_frame = yoyo_post_frame;
    #else
        YE_STATE.engine.callbacks.post_frame = NULL;
    #endif

    #ifdef YOYO_SCENE_LOAD
        YE_STATE.engine.callbacks.scene_load = yoyo_scene_load;
    #else
        YE_STATE.engine.callbacks.scene_load = NULL;
    #endif

    #ifdef YOYO_ADDITIONAL_RENDER
        YE_STATE.engine.callbacks.additional_render = yoyo_additional_render;
    #else
        YE_STATE.engine.callbacks.additional_render = NULL;
    #endif

    #ifdef YOYO_TRIGGER_ENTER
        YE_STATE.engine.callbacks.trigger_enter = yoyo_trigger_enter;
    #else
        YE_STATE.engine.callbacks.trigger_enter = NULL;
    #endif

    #ifdef YOYO_COLLISION
        YE_STATE.engine.callbacks.collision = yoyo_collision;
    #else
        YE_STATE.engine.callbacks.collision = NULL;
    #endif
    // ...etc. colliders and other triggers in future

    /*
        TODO: FIXME: DUMB HACK

        we need to call scene load cb manually
    */
    if(YE_STATE.engine.callbacks.scene_load != NULL)
        YE_STATE.engine.callbacks.scene_load(YE_STATE.runtime.scene_name);

    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(mainloop, 0, 1);
    #else
        while(true){mainloop();}
    #endif
}

#ifdef _WIN32
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
    {
        // Call the main function
        return main(__argc, __argv);
    }
#endif