// /*
//     This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
//     Copyright (C) 2023-2025  Ryan Zmuda

//     Licensed under the MIT license. See LICENSE file in the project root for details.
// */

// /*
//     This is the initialization function that will be called when this trick is loaded.
//     You must use this function to register your callbacks (if any).
// */
// void yoyo_trick_init();

// /*
//     Below this is platform specific code to abstract away observing dll load and unload events.
// */

// /*
//     Define some platform specific pathing through dll loading into yoyo_trick_init
// */
// #ifdef _WIN32
//     #define YOYO_INITIALIZE_TRICK_LOAD \
//     __declspec(dllexport) void DllMain() { yoyo_trick_init(); }
// #elif defined(__linux__)
//     #define YOYO_INITIALIZE_TRICK_LOAD \
//     __attribute__((constructor)) void yoyo_init_wrapper() { yoyo_trick_init(); }
// #endif

// // actually set the init pathing
// YOYO_INITIALIZE_TRICK_LOAD;