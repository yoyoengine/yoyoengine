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

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/tag.h>


void ye_add_tag_component(struct ye_entity *entity){
    if(entity->tag){
        ye_logf(error, "Entity %d already has a tag component\n", entity->id);
        return;
    }

    entity->tag = malloc(sizeof(struct ye_component_tag));
    entity->tag->active = true;

    // tags are already malloced, set them to be empty
    for(int i = 0; i < YE_TAG_MAX_NUMBER; i++){
        entity->tag->tags[i][0] = '\0';
    }

    // log that we added a tag and to what ID
    // ye_logf(debug, "Added tag component to entity %d\n", entity->id);

    ye_entity_list_add(&tag_list_head, entity);
}

void ye_add_tag(struct ye_entity *entity, const char *tag){
    // add tag component if it doesnt exist
    if(!entity->tag){
        ye_add_tag_component(entity);
    }

    if(!entity->tag->active){
        ye_logf(error, "Could not add tag \"%s\" to entity #%d. Tag component is not active.\n", tag, entity->id);
        return; // TODO: is this necessary?
    }

    // find the first empty tag slot
    int i = 0;
    while(entity->tag->tags[i][0] != '\0'){
        i++;
        if(i >= YE_TAG_MAX_NUMBER){
            ye_logf(error, "Could not add tag \"%s\" to entity #%d. Tags list is full.\n", tag, entity->id);
            return;
        }
    }

    // copy the tag into the slot
    strcpy(entity->tag->tags[i], tag);

    // log that we added a tag and to what ID
    // ye_logf(debug, "Added tag \"%s\" to entity %d\n", tag, entity->id);
}

void ye_remove_tag(struct ye_entity *entity, const char *tag){
    // check if tag component exists
    if(!entity->tag){
        ye_logf(error, "Could not remove tag \"%s\" from entity #%d. Entity has no tag component.\n", tag, entity->id);
    }

    if(!entity->tag->active){
        ye_logf(error, "Could not remove tag \"%s\" from entity #%d. Tag component is not active.\n", tag, entity->id);
        return; // TODO: is this necessary?
    }

    // find the tag
    int i = 0;
    while(strcmp(entity->tag->tags[i], tag) != 0){
        i++;
        if(i >= YE_TAG_MAX_NUMBER){
            ye_logf(error, "Could not remove tag \"%s\" from entity #%d. Tag not found.\n", tag, entity->id);
            return;
        }
    }

    // remove the tag
    entity->tag->tags[i][0] = '\0';

    // log that we removed a tag and to what ID
    // ye_logf(debug, "Removed tag from entity %d\n", entity->id);

    // if the tag component is empty, remove it
    bool empty = true;
    for(int i = 0; i < YE_TAG_MAX_NUMBER; i++){
        if(entity->tag->tags[i][0] != '\0'){
            empty = false;
            break;
        }
    }
    if(empty){
        ye_remove_tag_component(entity);
    }
}

void ye_remove_tag_component(struct ye_entity *entity){
    free(entity->tag);
    entity->tag = NULL;

    // log that we removed a tag and to what ID
    // ye_logf(debug, "Removed tag from entity %d\n", entity->id);

    ye_entity_list_remove(&tag_list_head, entity);
}

bool entity_has_tag(struct ye_entity *entity, const char *tag){
    // TODO: add more checks for active and such?
    if(entity->tag != NULL){
        for(int i = 0; i < YE_TAG_MAX_NUMBER; i++){
            if(strcmp(tag, entity->tag->tags[i]) == 0){
                return true;
            }
        }
    }
    return false;
}