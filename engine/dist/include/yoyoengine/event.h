/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef EVENT_H
#define EVENT_H

#include <yoyoengine/export.h>

#include <stdbool.h>

#include <lua.h>
#include <SDL.h>

#include <yoyoengine/ecs/ecs.h>

// get the number of events in the queue
YE_API int ye_get_num_events();

enum ye_event_type {
    YE_EVENT_PRE_INIT,          // empty_cb
    YE_EVENT_POST_INIT,         // empty_cb
    YE_EVENT_HANDLE_INPUT,      // input_cb
    YE_EVENT_LUA_REGISTER,      // lua_cb
    YE_EVENT_PRE_FRAME,         // empty_cb
    YE_EVENT_POST_FRAME,        // empty_cb
    YE_EVENT_SCENE_LOAD,        // scene_load_cb
    YE_EVENT_COLLISION,         // collision_cb
    YE_EVENT_TRIGGER_ENTER,     // collision_cb
    YE_EVENT_PRE_SHUTDOWN,      // empty_cb
    YE_EVENT_POST_SHUTDOWN,     // empty_cb
    YE_EVENT_ADDITIONAL_RENDER, // empty_cb

    YE_EVENT_CUSTOM,    // void * data, user defined event
};

struct _ye_event {
    enum ye_event_type type;
    int flags;

    union {
        void (*empty_cb)();
        void (*scene_load_cb)(char *scene_name);
        void (*input_cb)(SDL_Event event);
        void (*lua_cb)(lua_State *L);
        void (*collision_cb)(struct ye_entity *one, struct ye_entity *two);
        void (*custom_cb)(void *data);
    };
    
    struct _ye_event *next;
};

union ye_event_args {
    char *scene_name;
    SDL_Event input;
    lua_State *L;
    struct {
        struct ye_entity *one;
        struct ye_entity *two;
    } collision;
    void *custom_data;
};

#define YE_EVENT_FLAG_NONE 0

enum ye_event_flag {
    YE_EVENT_FLAG_PERSISTENT = 1 << 0, // event will never be removed (even upon scene change)
};

YE_API void ye_register_event_cb(enum ye_event_type type, void *cb, int flags);

YE_API void ye_fire_event(enum ye_event_type type, union ye_event_args args);

YE_API void _ye_add_event(struct _ye_event *event);

YE_API void _ye_remove_event(struct _ye_event *event);

YE_API void ye_purge_events(bool destroy_persistent);

YE_API void ye_unregister_event_cb(void *cb);

#endif

/*
    Current system:
    - macros inside yoyo_c_api.h are used to define custom behavior which must be implemented as a standalone function with specific sig for entry.c to register and call
    Limitations of current system:
    - entry.c managing some engine logic
    - confusing for user (have to touch header to enable)
    - functionally limited (no way to subscribe multiple functions to an event without explicitly trickling down)

    Proposed system:
    - event system that allows for multiple functions to be subscribed to an event
    Challenges:
    - potentially allowing persistant events
    - events like pre init are meant to be called before the engine is initialized, so how do we handle that? - potentially having this be a standalone system that does not need the engine initialized to fire
        - entry.c can call the engine fire event function itself
    
    should the event system let the user touch it? like can they add their own events with void * data?

    Problems:
    - some callbacks have specific args... dont want to force user to use varargs
*/