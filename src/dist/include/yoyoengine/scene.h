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
    Scene manager

    This file is responsible for managing scenes in the engine. It has the capacity to deserialize a scene from file,
    pre cache all its assets and load the entities+components into memory.

    In the future (TODO) I would like to allow serialization of a scene directly to a file, for a better version of a save system.
    This is not strictly necessary for the game I wish to build with this engine, so it will be avoided for now.
*/

#ifndef YE_SCENE_H
#define YE_SCENE_H

#include <yoyoengine/yoyoengine.h>

/*
    Initializes the scene manager
*/
void ye_init_scene_manager();

/*
    Loads a scene from file (.json, .yoyo, etc) and sets the current scene name,
    creating all entities and components as described by the file.
*/
void ye_load_scene(const char *scene_name);

/*
    Returns the current scene name
*/
char *ye_get_scene_name();

/*
    Tears down the scene manager context, freeing the current scene name
*/
void ye_shutdown_scene_manager();

#endif