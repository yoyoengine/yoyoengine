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

#include <yoyoengine/yoyoengine.h>

/*
    Problem... we need the actual size of the image to align it in bounds, but
    at the same time we can have a transform without a renderer (sometimes i guess)
    which means we need to query the texture size
*/
void ye_add_transform_component(struct ye_entity *entity, struct ye_rectf bounds, int z, enum ye_alignment alignment){
    entity->transform = malloc(sizeof(struct ye_component_transform));
    entity->transform->active = true;
    entity->transform->bounds = bounds;
    entity->transform->z = z;

    // must be modified outside of this constructor if non default desired
    entity->transform->rotation = 0;
    entity->transform->flipped_x = false;
    entity->transform->flipped_y = false;
    entity->transform->center = (SDL_Point){bounds.w / 2, bounds.h / 2}; // default center is the center of the bounds
    
    // we will first set the rect equal to the bounds, for the purposes of rendering the renderer on mount
    // will then calculate the actual rect of the entity based on its alignment and bounds
    entity->transform->rect = bounds;
    entity->transform->alignment = YE_ALIGN_MID_CENTER;

    // add this entity to the transform component list
    ye_entity_list_add(&transform_list_head, entity);

    // log that we added a transform and to what ID
    // ye_logf(debug, "Added transform to entity %d\n", entity->id);
}

void ye_remove_transform_component(struct ye_entity *entity){
    free(entity->transform);
    entity->transform = NULL;

    // remove the entity from the transform component list
    ye_entity_list_remove(&transform_list_head, entity);
}