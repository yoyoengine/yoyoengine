/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/tag.h>


void ye_add_tag_component(struct ye_entity *entity){
    if(!entity) {
        ye_logf(error, "Could not add tag component to NULL entity.\n");
        return;
    }

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
    if(!entity) {
        ye_logf(error, "Could not add tag \"%s\" to NULL entity.\n", tag);
        return;
    }

    // add tag component if it doesnt exist
    if(!entity->tag){
        ye_add_tag_component(entity);
    }

    if(!entity->tag->active){
        ye_logf(error, "Could not add tag \"%s\" to entity #%d. Tag component is not active.\n", tag, entity->id);
        return; // TODO: is this necessary?
    }

    // perform a first pass to check if it already exists
    for(int i = 0; i < YE_TAG_MAX_NUMBER; i++){
        if(strcmp(tag, entity->tag->tags[i]) == 0
            && entity->tag->tags[i][0] != '\0'
        ){
            ye_logf(error, "Could not add tag \"%s\" to entity #%d. Tag already exists.\n", tag, entity->id);
            return;
        }
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
    if(!entity) {
        ye_logf(error, "Could not remove tag \"%s\" from NULL entity.\n", tag);
        return;
    }

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
    if(!entity) {
        ye_logf(error, "Could not remove tag component from NULL entity.\n");
        return;
    }

    if(!entity->tag){
        ye_logf(error, "Could not remove tag component from entity #%d. Entity has no tag component.\n", entity->id);
        return;
    }

    free(entity->tag);
    entity->tag = NULL;

    // log that we removed a tag and to what ID
    // ye_logf(debug, "Removed tag from entity %d\n", entity->id);

    ye_entity_list_remove(&tag_list_head, entity);
}

bool ye_entity_has_tag(struct ye_entity *entity, const char *tag){
    if(!entity) {
        ye_logf(error, "Could not check if entity has tag \"%s\". Entity is NULL.\n", tag);
        return false;
    }

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

void ye_for_matching_tag(const char * tag, void(*callback)(struct ye_entity *ent)){
    struct ye_entity_node *itr = tag_list_head;
    while(itr != NULL){
        if(itr->entity == NULL){
            ye_logf(error, "??? Entity node in tag list is NULL\n");
            return;
        }

        if(ye_entity_has_tag(itr->entity,tag)){
            /*
                In order to mitigate the risk of a callback
                destroying our list state (like if they remove
                a entity), we will reset to the beginning of the list
                after this. This gives us non-ideal complexity,
                but to be honest I don't think I will ever need to optimize
                this.
            */
            callback(itr->entity);

            itr = tag_list_head;
            continue;
        }

        itr = itr->next;
    }
}