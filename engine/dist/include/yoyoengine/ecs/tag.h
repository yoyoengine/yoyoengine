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

#ifndef YE_TAG_H
#define YE_TAG_H

#include <yoyoengine/yoyoengine.h>

#define YE_TAG_MAX_NUMBER 10
#define YE_TAG_MAX_LENGTH 20

struct ye_component_tag {
    bool active;    // controls whether system will act upon this component

    char tags[YE_TAG_MAX_NUMBER][YE_TAG_MAX_LENGTH]; // array of tags
};

/*
    Add a tag component to an entity
*/
void ye_add_tag_component(struct ye_entity *entity);

/*
    Add a tag to an entity, creating a tag component for that entity if it doesnt exist
*/
void ye_add_tag(struct ye_entity *entity, const char *tag);

/*
    Remove a tag from an entity, removing its tag component if it has no more tags
*/
void ye_remove_tag(struct ye_entity *entity, const char *tag);

/*
    Remove a tag component from an entity
*/
void ye_remove_tag_component(struct ye_entity *entity);

#endif