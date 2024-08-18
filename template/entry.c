/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    WARNING: PLEASE DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!

    Any custom behavior you want to implement can be done in `custom/*`.
    Please reference <https://zoogies.github.io/yoyoengine>.
*/

#include <yoyoengine/yoyoengine.h>

#include "yoyo_c_api.h"

#ifdef _WIN32
    #include <windows.h>
#endif

bool YG_RUNNING; // extern: yoyo_c_api.h

static void mainloop(void){
    if(YG_RUNNING){
        ye_process_frame();
    }
    else{
        #ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
        #endif

        // shutdown engine
        ye_shutdown_engine();

        // printf("Game exited successfully\n");
        exit(0);
    }
}

#ifdef __WIN32__
int main(int argc, char *argv[]){
#else
void main(void){
#endif
    YG_RUNNING = true;

    // register any user callbacks
    yoyo_register_callbacks();

    // initialize engine
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