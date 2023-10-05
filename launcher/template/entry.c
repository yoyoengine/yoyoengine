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

/*
    MAIN ENTRY POINT: TODO WORK FOR WINDOWS

    THIS CONTAINS THE GAME LOOP
*/
int main(/*int argc, char** argv*/){
    #ifdef YOYO_PRE_INIT
        // run the pre init function
    #endif

    // assign function pointers for custom scripted behavior
    #ifdef YOYO_PRE_FRAME
    #endif

    #ifdef YOYO_POST_FRAME
    #endif
    // ...etc. colliders and other triggers in future
    
    // intialize the engine (engine will look at ./settings.yoyo for configuration)
    // ye_init_engine();

    #ifdef YOYO_POST_INIT
        // run the post init function
    #endif

    // create a game loop and persist it

    #ifdef YOYO_PRE_SHUTDOWN
        // run the pre shutdown function
    #endif

    // shutdown engine
    // ye_shutdown_engine();

    #ifdef YOYO_POST_SHUTDOWN
        // run the post shutdown function
    #endif

    printf("Game exited successfully\n");
    return 0;
}