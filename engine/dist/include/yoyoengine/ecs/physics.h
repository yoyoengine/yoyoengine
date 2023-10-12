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

#ifndef YE_PHYSICS_H
#define YE_PHYSICS_H

#include <yoyoengine/yoyoengine.h>

/*
    Physics component
    
    Holds information on how an entity moves.

    Velocity and acceleration are in pixels per second.
*/
struct ye_component_physics {
    bool active;                        // controls whether system will act upon this component

    // float mass;                      // mass of entity
    // float drag;                      // drag of entity when accelerating
    struct ye_vec2f velocity;            // velocity of entity
    float rotational_velocity;            // rotational velocity of entity
    // struct ye_vec2 acceleration;     // acceleration of entity
};

void ye_add_physics_component(struct ye_entity *entity, float velocity_x, float velocity_y);

void ye_remove_physics_component(struct ye_entity *entity);

void ye_system_physics();

#endif