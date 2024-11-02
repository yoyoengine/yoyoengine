/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file tag.h
 * @brief ECS Tag component
 */

#ifndef YE_TAG_H
#define YE_TAG_H

#include <yoyoengine/export.h>

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
YE_API void ye_add_tag_component(struct ye_entity *entity);

/**
 * @brief Add a tag to an entity, creating a tag component for that entity if it doesn't exist
 * 
 * @param entity The entity to which the tag will be added
 * @param tag The tag to be added
 */
YE_API void ye_add_tag(struct ye_entity *entity, const char *tag);

/**
 * @brief Remove a tag from an entity, removing its tag component if it has no more tags
 * 
 * @param entity The entity from which the tag will be removed
 * @param tag The tag to be removed
 */
YE_API void ye_remove_tag(struct ye_entity *entity, const char *tag);

/**
 * @brief Remove a tag component from an entity
 * 
 * @param entity The entity from which the tag component will be removed
 */
YE_API void ye_remove_tag_component(struct ye_entity *entity);


/**
 * @brief Returns true if an entity has a specified tag
 * 
 * @param entity The entity to check
 * @param tag The tag to check for
 * @return true 
 * @return false 
 */
YE_API bool ye_entity_has_tag(struct ye_entity *entity, const char *tag);

/**
 * @brief Iterates over every tagged entity, passing each one that matches a tag to a specified callback
 * 
 * @param tag The tag to match entities against
 * @param callback The non null callback with a struct ye_entity * parameter
 */
YE_API void ye_for_matching_tag(const char * tag, void(*callback)(struct ye_entity *ent));

#endif