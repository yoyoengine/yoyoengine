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
    This is the initialization function that will be called when this trick is loaded.
    You must use this function to register your callbacks (if any).
*/
void yoyo_trick_init();

/*
    Below this is platform specific code to abstract away observing dll load and unload events.
*/

/*
    Define some platform specific pathing through dll loading into yoyo_trick_init
*/
#ifdef _WIN32
    #define YOYO_INITIALIZE_TRICK_LOAD \
    __declspec(dllexport) void DllMain() { yoyo_trick_init(); }
#elif defined(__linux__)
    #define YOYO_INITIALIZE_TRICK_LOAD \
    __attribute__((constructor)) void yoyo_init_wrapper() { yoyo_trick_init(); }
#endif

// actually set the init pathing
YOYO_INITIALIZE_TRICK_LOAD;