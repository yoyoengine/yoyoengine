/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_EDITOR_INPUT_H
#define YE_EDITOR_INPUT_H

#include <yoyoengine/yoyoengine.h>

bool is_hovering_editor(int x, int y);

void editor_input_panning(SDL_Event event);

void editor_input_misc(SDL_Event event);

void editor_input_shortcuts(SDL_Event event);

void editor_handle_input(SDL_Event event);

#endif // YE_EDITOR_INPUT_H