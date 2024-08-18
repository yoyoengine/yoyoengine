/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    Event system ideation:

    creates an easier C api as well as registration for tricks. you can now stack events

    Event queue does not have an init, it starts out as a blank slate and we can manually append or tear it down
*/

#include <stdlib.h>

#include <yoyoengine/event.h>

struct _ye_event * ye_event_queue = NULL;

int num_events = 0;

int ye_get_num_events(){ return num_events; }

void _ye_add_event(struct _ye_event *event){
    if(ye_event_queue == NULL){
        ye_event_queue = event;
    }
    else{
        struct _ye_event *current = ye_event_queue;
        while(current->next != NULL){
            current = current->next;
        }
        current->next = event;
    }

    num_events++;
}

void ye_register_event_cb(enum ye_event_type type, void *cb, int flags){
    struct _ye_event *event = malloc(sizeof(struct _ye_event));
    event->type = type;
    event->flags = flags;
    event->next = NULL;

    switch(type){
        case YE_EVENT_PRE_INIT:
        case YE_EVENT_POST_INIT:
        case YE_EVENT_PRE_FRAME:
        case YE_EVENT_POST_FRAME:
            event->empty_cb = (void (*)())cb;
            break;

        case YE_EVENT_HANDLE_INPUT:
            event->input_cb = (void (*)(SDL_Event))cb;
            break;

        case YE_EVENT_LUA_REGISTER:
            event->lua_cb = (void (*)(lua_State *))cb;
            break;

        case YE_EVENT_SCENE_LOAD:
            event->scene_load_cb = (void (*)(char *))cb;
            break;

        case YE_EVENT_COLLISION:
            event->collision_cb = (void (*)(struct ye_entity *, struct ye_entity *))cb;
            break;

        case YE_EVENT_CUSTOM:
            event->custom_cb = (void (*)(void *))cb;
            break;
    }

    _ye_add_event(event);
}

void ye_fire_event(enum ye_event_type type, union ye_event_args args){
    struct _ye_event *current = ye_event_queue;
    while(current != NULL){
        if(current->type == type){
            switch(type){
                case YE_EVENT_PRE_INIT:
                case YE_EVENT_POST_INIT:
                case YE_EVENT_PRE_SHUTDOWN:
                case YE_EVENT_POST_SHUTDOWN:
                case YE_EVENT_PRE_FRAME:
                case YE_EVENT_ADDITIONAL_RENDER:
                case YE_EVENT_POST_FRAME:
                    current->empty_cb();
                    break;

                case YE_EVENT_HANDLE_INPUT:
                    current->input_cb(args.input);
                    break;

                case YE_EVENT_LUA_REGISTER:
                    current->lua_cb(args.L);
                    break;

                case YE_EVENT_SCENE_LOAD:
                    current->scene_load_cb(args.scene_name);
                    break;

                case YE_EVENT_COLLISION:
                    current->collision_cb(args.collision.one, args.collision.two);
                    break;

                case YE_EVENT_CUSTOM:
                    current->custom_cb(args.custom_data);
                    break;
            }
        }
        current = current->next;
    }
}

void _ye_remove_event (struct _ye_event *event){
    struct _ye_event *current = ye_event_queue;
    struct _ye_event *previous = NULL;
    while(current != NULL){
        if(current == event){
            if(previous == NULL){
                ye_event_queue = current->next;
            }
            else{
                previous->next = current->next;
            }
            free(current);

            num_events--;
            return;
        }
        previous = current;
        current = current->next;
    }
}

void ye_purge_events(bool destroy_persistent){
    struct _ye_event *current = ye_event_queue;
    struct _ye_event *next = NULL;
    while(current != NULL){
        next = current->next;
        if(destroy_persistent || !(current->flags & YE_EVENT_FLAG_PERSISTENT)){
            _ye_remove_event(current);
        }
        current = next;
    }
}

void ye_unregister_event_cb(void *cb){
    struct _ye_event *current = ye_event_queue;
    while(current != NULL){
        if(
            current->empty_cb == cb         ||
            current->input_cb == cb         ||
            current->lua_cb == cb           ||
            current->scene_load_cb == cb    ||
            current->collision_cb == cb     ||
            current->custom_cb == cb
        ){
            _ye_remove_event(current);
            return;
        }
        current = current->next;
    }
}