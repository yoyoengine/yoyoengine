/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    This file expands into the nuklear implementation at compile time

    WARNING: through trial and error, these macros and includes need to happen HERE,
    and cannot be indirectly done by simply #include'ing <yoyoengine/ye_nk.h>.

    ye_nk.h exists purely for other files to include. This file is the actual implementation.
*/

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#define NK_INCLUDE_STANDARD_BOOL

#include <Nuklear/nuklear.h>
#include <Nuklear/nuklear_sdl_renderer.h>
#include <Nuklear/style.h>
