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

#include <yoyoengine/yoyoengine.h>

// controls whether the game is running. Set this to false at any point and everything will quit.
extern bool YG_RUNNING;

/*
    +---------------------------------------------------+
    |             IMPORTANT INFORMATION:                |
    +---------------------------------------------------+

    In order to use the C api, you must uncomment any macros you wish to use below,
    in order to let the engine entry point know that you are using them.
*/



/*
    Runs one time before the engine is initialized. You cannot access anything within the engine,
    however you could use this function to modify the settings.yoyo file to change the startup behavior of the
    engine once it is actually initialized.
*/
// #define YOYO_PRE_INIT

/*
    Runs one time after the engine is initialized but before the game loop. This could be used to cleanup memory
    or for any other purpose you want. The engine has been initialized so you can access YE_STATE
*/
// #define YOYO_POST_INIT

/*
    Runs once per frame assuming the engine has detected an input event. The engine is using SDL for inputs, so this
    will pass the SDL_Event struct to your function. You can use this to implement custom input handling.

    Make sure you implement this as:
    void yoyo_handle_input(SDL_Event event);
*/
// #define YOYO_HANDLE_INPUT

/*
    Called when lua script components are created, will send the
    yoyo_lua_register function the lua state to register custom lua api.
*/
// #define YOYO_LUA_REGISTER

/*
    Runs once per frame before the engine has done anything, but after the deltatime has been reset for the new frame.
*/
// #define YOYO_PRE_FRAME

/*
    Runs once per frame after the engine has done every system, right before control is returned. Since this happens
    after rendering, either due to vsync or intenral frame delay this will run after we have hit the target amount of time
    for a frame to take.
*/
// #define YOYO_POST_FRAME

/*
    Runs once after the scene has been loaded, before the render loop.
    void yoyo_scene_load(char *scene_name);
*/
// #define YOYO_SCENE_LOAD

/*
    Runs once before the engine is shutdown. You can still access YE_STATE here.
*/
// #define YOYO_PRE_SHUTDOWN

/*
    Runs once after the engine is shutdown. The engine context is completely destroyed.
    You can use this to cleanup anything extra in your game. The program will exit immediately after this function returns.
*/
// #define YOYO_POST_SHUTDOWN



/*
    Everything else below this comment is automatically managed and SHOULD NOT BE TOUCHED
    unless you are absolutely sure you know what you are doing.

    If you wish to implement custom behavior, define a function according to the custom C scripting docs.

    If you wish to modify certain startup variables based on things like savedata (user set volume to zero but default launch volume is 100)
    you can do so with YOYO_PRE_INIT and write directly to settings.yoyo
    
    +---------------------------------------------------+
    |   DO NOT MODIFY ANYTHING BELOW THIS LINE !!!!!!   |
    +---------------------------------------------------+
*/











#ifdef YOYO_PRE_INIT
    void yoyo_pre_init();
#endif

#ifdef YOYO_POST_INIT
    void yoyo_post_init();
#endif

#ifdef YOYO_HANDLE_INPUT
    void yoyo_handle_input(SDL_Event event);
#endif

#ifdef YOYO_PRE_FRAME
    void yoyo_pre_frame();
#endif

#ifdef YOYO_POST_FRAME
    void yoyo_post_frame();
#endif

#ifdef YOYO_SCENE_LOAD
    void yoyo_scene_load(char *scene_name);
#endif

#ifdef YOYO_PRE_SHUTDOWN
    void yoyo_pre_shutdown();
#endif

#ifdef YOYO_POST_SHUTDOWN
    void yoyo_post_shutdown();
#endif