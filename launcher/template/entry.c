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

/*
    MAIN ENTRY POINT: TODO WORK FOR WINDOWS

    THIS CONTAINS THE GAME LOOP
*/
int main(int argc, char** argv){
    #ifdef YOYO_PRE_INIT
        // run the pre init function
        yoyo_pre_init();
    #endif
    
    // intialize the engine (engine will look at ./settings.yoyo for configuration)
    ye_init_engine();

    #ifdef YOYO_POST_INIT
        // run the post init function
        yoyo_post_init();
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
    // ...etc. colliders and other triggers in future
    printf("Game initialized successfully\n");


    // create a game loop and persist it

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
    return 0;
}

#ifdef _WIN32
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
    {
        // Call the main function
        return main(__argc, __argv);
    }
#endif