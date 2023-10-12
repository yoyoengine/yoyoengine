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

#ifndef YE_CAMERA_H
#define YE_CAMERA_H

#include <yoyoengine/yoyoengine.h>

/*
    Camera component

    Holds some information about a camera and its view field (the x and y of the view field are unused, as its location is inferred from its transform)
*/
struct ye_component_camera {
    bool active;    // controls whether system will act upon this component

    SDL_Rect view_field;    // view field of camera
};

void ye_set_camera(struct ye_entity *entity);

void ye_add_camera_component(struct ye_entity *entity, SDL_Rect view_field);

void ye_remove_camera_component(struct ye_entity *entity);

#endif