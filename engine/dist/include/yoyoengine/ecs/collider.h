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
    TODO:
    - the flexibility of having "round" colliders or triange colliders
    - the ability to have multiple colliders on a single entity
    - on enter, on exit on stay
    - trigger colliders
*/

#ifndef YE_COLLIDER_H
#define YE_COLLIDER_H

#include <yoyoengine/yoyoengine.h>

/*
    Its possible the collider needs broken out into
    a two different colliders, trigger and static to enable
    multiple colliders on a single entity.
*/
struct ye_component_collider {
    bool active;            // controls whether system will act upon this component
    struct ye_rectf rect;   // collider rectangle

    bool is_trigger;        // whether this collider is a trigger, if false, it is a static collider

    // trigger specific fields
    // how are we responding to the trigger? broadcasting a message? calling a C callback? calling a lua callback?
};

/*
    Add a static collider to an entity
*/
void ye_add_static_collider_component(struct ye_entity *entity, struct ye_rectf rect);

// TODO: add trigger collider

/*
    Remove an entity's collider component
*/
void ye_remove_collider_component(struct ye_entity *entity);

// need bunch of helper functions... 

// take in a given position and return what entity it would collide with

#endif