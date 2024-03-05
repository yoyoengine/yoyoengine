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
 * @file tag.h
 * @brief ECS Tag component
 */

#ifndef YE_TAG_H
#define YE_TAG_H

#include <stdbool.h>
#include <yoyoengine/ecs/ecs.h>

#define YE_TAG_MAX_NUMBER 10
#define YE_TAG_MAX_LENGTH 20

/**
 * @brief The tag component
 */
struct ye_component_tag {
    bool active;    // controls whether system will act upon this component

    char tags[YE_TAG_MAX_NUMBER][YE_TAG_MAX_LENGTH]; // array of tags
};

/**
 * @brief Add a tag component to an entity
 * 
 * @param entity The entity to which the tag component will be added
 */
void ye_add_tag_component(struct ye_entity *entity);

/**
 * @brief Add a tag to an entity, creating a tag component for that entity if it doesn't exist
 * 
 * @param entity The entity to which the tag will be added
 * @param tag The tag to be added
 */
void ye_add_tag(struct ye_entity *entity, const char *tag);

/**
 * @brief Remove a tag from an entity, removing its tag component if it has no more tags
 * 
 * @param entity The entity from which the tag will be removed
 * @param tag The tag to be removed
 */
void ye_remove_tag(struct ye_entity *entity, const char *tag);

/**
 * @brief Remove a tag component from an entity
 * 
 * @param entity The entity from which the tag component will be removed
 */
void ye_remove_tag_component(struct ye_entity *entity);


/**
 * @brief Returns true if an entity has a specified tag
 * 
 * @param entity The entity to check
 * @param tag The tag to check for
 * @return true 
 * @return false 
 */
bool ye_entity_has_tag(struct ye_entity *entity, const char *tag);

/**
 * @brief Iterates over every tagged entity, passing each one that matches a tag to a specified callback
 * 
 * @param tag The tag to match entities against
 * @param callback The non null callback with a struct ye_entity * parameter
 */
void ye_for_matching_tag(const char * tag, void(*callback)(struct ye_entity *ent));

#endif