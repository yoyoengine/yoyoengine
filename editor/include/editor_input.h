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

#ifndef YE_EDITOR_INPUT_H
#define YE_EDITOR_INPUT_H

#include <yoyoengine/yoyoengine.h>

bool is_hovering_editor(int x, int y);

void editor_input_panning(SDL_Event event);

void editor_input_selection(SDL_Event event);

void editor_input_misc(SDL_Event event);

void editor_input_shortcuts(SDL_Event event);

void editor_handle_input(SDL_Event event);

#endif // YE_EDITOR_INPUT_H