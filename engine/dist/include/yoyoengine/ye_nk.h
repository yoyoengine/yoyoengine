/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    Wrapper around Nuklear.h so I stop going crazy debugging it...
*/

#ifndef YE_NK_H
#define YE_NK_H
#ifndef __cplusplus

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_STANDARD_BOOL

#include <Nuklear/nuklear.h>
#include <Nuklear/nuklear_sdl_renderer.h>
#include <Nuklear/style.h>

#endif // __cplusplus
#endif // YE_NK_H