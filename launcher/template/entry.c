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

        printf("Game exited successfully\n");
        exit(0);
    }
}

/*
    MAIN ENTRY POINT

    THIS CONTAINS THE GAME LOOP
*/
void main(void){
    YG_RUNNING = true;

    ye_logf(info, "Starting init\n");
    
    ye_init_engine();

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