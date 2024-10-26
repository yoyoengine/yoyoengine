/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>

#include <yoyoengine/commands.h>
#include <yoyoengine/console.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/tag.h>

/*
    argc=0 -> list all entities
    argc>0 -> list entities containing at minimum all of the components listed in args
*/
void ye_cmd_entlist(int argc, const char **argv) {
    /*
        Convention for [help] or [usage]
    */
    if(argc < 0) {
        ye_logf(_YE_RESERVED_LL_SYSTEM, "Usage: entlist (optional)[-c component1 component2 ...] (optional)[-t tag]\n");
        ye_logf(_YE_RESERVED_LL_SYSTEM, "Ex:    \"entlist -c renderer -t example\" will list only entities containing both a renderer component and the tag \"example\"\n");
        return;
    }

    bool include_transform      = false;
    bool include_renderer       = false;
    bool include_camera         = false;
    bool include_lua_script     = false;
    bool include_button         = false;
    bool include_physics        = false;
    bool include_collider       = false;
    bool include_tag            = false;
    bool include_audiosource    = false;

    bool include_all = true;
    bool include_tag_filter = false;
    const char *tag_filter = NULL;

    if(argc > 0) {
        include_all = false;
        for(int i = 0; i < argc; i++) {
            if(strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
                i++;
                if(strcmp(argv[i], "transform") == 0)
                    include_transform = true;
                else if(strcmp(argv[i], "renderer") == 0)
                    include_renderer = true;
                else if(strcmp(argv[i], "camera") == 0)
                    include_camera = true;
                else if(strcmp(argv[i], "lua_script") == 0)
                    include_lua_script = true;
                else if(strcmp(argv[i], "button") == 0)
                    include_button = true;
                else if(strcmp(argv[i], "physics") == 0)
                    include_physics = true;
                else if(strcmp(argv[i], "collider") == 0)
                    include_collider = true;
                else if(strcmp(argv[i], "tag") == 0)
                    include_tag = true;
                else if(strcmp(argv[i], "audiosource") == 0)
                    include_audiosource = true;
                else{
                    ye_logf(_YE_RESERVED_LL_SYSTEM, "Invalid -c specifier: %s\n", argv[i]);
                    ye_logf(_YE_RESERVED_LL_SYSTEM, "Valid specifiers: transform, renderer, camera, lua_script, button, physics, collider, tag, audiosource\n");
                    return;
                }
            } else if(strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
                i++;
                include_tag_filter = true;
                tag_filter = argv[i];
            } else {
                ye_logf(_YE_RESERVED_LL_SYSTEM, "Invalid/Incomplete argument: %s\n", argv[i]);
                return;
            }
        }
    }

    int entity_count = 0;
    int matching_entity_count = 0;
    struct ye_entity_node *current = entity_list_head;
    while(current != NULL) {
        // Add logic to count entities and match based on component flags and tag filter
        entity_count++;
        bool matches = true;

        if(!include_all) {
            if(include_transform && !current->entity->transform)
                matches = false;
            if(include_renderer && !current->entity->renderer)
                matches = false;
            if(include_camera && !current->entity->camera)
                matches = false;
            if(include_lua_script && !current->entity->lua_script)
                matches = false;
            if(include_button && !current->entity->button)
                matches = false;
            if(include_physics && !current->entity->physics)
                matches = false;
            if(include_collider && !current->entity->collider)
                matches = false;
            if(include_tag && !current->entity->tag)
                matches = false;
            if(include_audiosource && !current->entity->audiosource)
                matches = false;
            if(include_tag_filter && !ye_entity_has_tag(current->entity, tag_filter))
                matches = false;
        }

        /*
            TODO: add a complete printout of entity component values
            with a -v flag for verbose
        */
        if(matches){
            matching_entity_count++;
            ye_logf(_YE_RESERVED_LL_SYSTEM, "Entity: \"%s\" [ID: %d]\n", current->entity->name, current->entity->id);
            if(current->entity->transform)
                ye_logf(_YE_RESERVED_LL_SYSTEM, "    [Transform]\n");
            if(current->entity->renderer)
                ye_logf(_YE_RESERVED_LL_SYSTEM, "    [Renderer]\n");
            if(current->entity->camera)
                ye_logf(_YE_RESERVED_LL_SYSTEM, "    [Camera]\n");
            if(current->entity->lua_script)
                ye_logf(_YE_RESERVED_LL_SYSTEM, "    [Lua Script]\n");
            if(current->entity->button)
                ye_logf(_YE_RESERVED_LL_SYSTEM, "    [Button]\n");
            if(current->entity->physics)
                ye_logf(_YE_RESERVED_LL_SYSTEM, "    [Physics]\n");
            if(current->entity->collider)
                ye_logf(_YE_RESERVED_LL_SYSTEM, "    [Collider]\n");
            if(current->entity->tag)
                ye_logf(_YE_RESERVED_LL_SYSTEM, "    [Tag]\n");
            if(current->entity->audiosource)
                ye_logf(_YE_RESERVED_LL_SYSTEM, "    [Audiosource]\n");
        }

        current = current->next;
    }

    // print total entity count
    if(argc == 0)
        ye_logf(_YE_RESERVED_LL_SYSTEM, "Total entities: %d\n", entity_count);
    else
        ye_logf(_YE_RESERVED_LL_SYSTEM, "%d matching entities (%d total)\n", matching_entity_count, entity_count);
}