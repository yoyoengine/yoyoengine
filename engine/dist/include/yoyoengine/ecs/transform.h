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

/**
 * @file transform.h
 * @brief ECS Transform component
 */

#ifndef YE_TRANSFORM_H
#define YE_TRANSFORM_H

#include <yoyoengine/yoyoengine.h>

/**
 * @brief The transform component
 * 
 * Describes where the entity sits in the world.
*/
struct ye_component_transform {
    // bool active;    // controls whether system will act upon this component

    float x;        // the transform x position
    float y;        // the transform y position
};

/**
 * @brief Adds a transform component to an entity
 * 
 * @param entity The entity to add the component to
 * @param x The x position of the transform
 * @param y The y position of the transform
 */
void ye_add_transform_component(struct ye_entity *entity, int x, int y);

/**
 * @brief Removes a transform component from an entity
 * 
 * @param entity The entity to remove the component from
 */
void ye_remove_transform_component(struct ye_entity *entity);

#endif