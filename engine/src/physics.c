/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <yoyoengine/logging.h>
#include <yoyoengine/physics.h>
#include <yoyoengine/event.h>
//#include <yoyoengine/ecs/lua_script.h>

static bool ye_physics_collision_entities(struct p2d_cb_data *data, struct ye_entity **one, struct ye_entity **two) {
    if(!data || !data->a || !data->b) {
        return false;
    }

    *one = (struct ye_entity *)data->a->user_data;
    *two = (struct ye_entity *)data->b->user_data;
    return *one && *two;
}

void ye_physics_collision_callback(struct p2d_cb_data* data) {
    struct ye_entity *one = NULL;
    struct ye_entity *two = NULL;
    if(!ye_physics_collision_entities(data, &one, &two)) {
        return;
    }

    ye_fire_event(YE_EVENT_COLLISION, (union ye_event_args){
        .collision = {
            .one = one,
            .two = two
        }
    });
    // ye_lua_signal_collisions(one, two);
}

void ye_physics_trigger_callback(struct p2d_cb_data* data) {
    struct ye_entity *one = NULL;
    struct ye_entity *two = NULL;
    if(!ye_physics_collision_entities(data, &one, &two)) {
        return;
    }

    ye_fire_event(YE_EVENT_TRIGGER_ENTER, (union ye_event_args){
        .collision = {
            .one = one,
            .two = two
        }
    });
    // ye_lua_signal_trigger_enter(one, two);
}
