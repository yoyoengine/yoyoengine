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

#ifndef YE_TRANSFORM_H
#define YE_TRANSFORM_H

#include <yoyoengine/yoyoengine.h>

/*
    Transform component
    
    Describes where the entity sits in the world.
    In 2D the Z axis is the layer the entity sits on. (High Z overpaints low Z)
*/
struct ye_component_transform {
    // bool active;    // controls whether system will act upon this component

    float x;        // the transform x position
    float y;        // the transform y position
};

void ye_add_transform_component(struct ye_entity *entity, int x, int y);

void ye_remove_transform_component(struct ye_entity *entity);

#endif