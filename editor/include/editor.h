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

#ifndef YE_EDITOR
#define YE_EDITOR

#include <stdbool.h>
#include <jansson/jansson.h>
#include <yoyoengine/yoyoengine.h>
#include <Nuklear/style.h>

// global variables
extern bool quit;
extern bool dragging;
extern bool lock_viewport_interaction;
extern int last_x;
extern int last_y;
extern struct ye_entity * editor_camera;
extern struct ye_entity * origin;
extern int screenWidth;
extern int screenHeight;
extern struct ye_entity_node * entity_list_head;
extern char *project_path;

// some fields for current selected entity tracking (this will be messy)
// actually lets be really smart and keep local copy
extern struct ye_entity staged_entity;
extern struct ye_component_transform staged_transform;
extern json_t * SETTINGS;

extern int mouse_world_x;
extern int mouse_world_y;

extern char editor_settings_path[1024];

bool ye_point_in_rect(int x, int y, SDL_Rect rect);

void editor_reload_settings();

#endif // YE_EDITOR