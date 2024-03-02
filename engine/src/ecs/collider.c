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

#include <yoyoengine/utils.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/collider.h>

void ye_add_static_collider_component(struct ye_entity *entity, struct ye_rectf rect){
    struct ye_component_collider *collider = malloc(sizeof(struct ye_component_collider));
    collider->active = true;
    collider->rect = rect;
    collider->is_trigger = false;
    entity->collider = collider;
    ye_entity_list_add(&collider_list_head, entity);
}

void ye_add_trigger_collider_component(struct ye_entity *entity, struct ye_rectf rect){
    ye_add_static_collider_component(entity, rect);
    entity->collider->is_trigger = true;
}

void ye_remove_collider_component(struct ye_entity *entity){
    free(entity->collider);
    entity->collider = NULL;
    ye_entity_list_remove(&collider_list_head, entity);
}