/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/


#include <stdbool.h>
#include <float.h>

#include <Lilith.h>

#include <yoyoengine/event.h>
#include <yoyoengine/types.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/ecs/collider.h>
#include <yoyoengine/ecs/transform.h>
#include <yoyoengine/ecs/lua_script.h>

///////////////////////////
// HELPERS
///////////////////////////

/*
    bool: whether or not to continue resolving
*/
bool _signaling_helper(struct ye_entity *ent1, struct ye_entity *ent2) {
    // figure out if one is trigger and which one
    bool ent1_trigger = ent1->collider->is_trigger;
    bool ent2_trigger = ent2->collider->is_trigger;

    // two triggers cannot trigger each other
    if(ent1_trigger && ent2_trigger) {
        return false;
    }

    // if one is trigger, send signal accordingly and exit
    if(ent1_trigger) {
        ye_fire_event(YE_EVENT_TRIGGER_ENTER, (union ye_event_args){.collision = {ent2, ent1}});
        ye_lua_signal_trigger_enter(ent2, ent1);
        return false;
    }
    if(ent2_trigger) {
        ye_fire_event(YE_EVENT_TRIGGER_ENTER, (union ye_event_args){.collision = {ent1, ent2}});
        ye_lua_signal_trigger_enter(ent1, ent2);
        return false;
    }

    return true;
}

///////////////////////////
// DETECTION
///////////////////////////

/*
    https://stackoverflow.com/questions/10962379/how-to-check-intersection-between-2-rotated-rectangles

    https://pastebin.com/03BigiCn

    Separating Axis Theorum - works for convex polygons and probably wasting cycles for just rects
*/
bool ye_detect_rect_rect_collision(struct ye_point_rectf rect1, struct ye_point_rectf rect2) {
    for(int polyi = 0; polyi < 2; polyi++) {
        struct ye_point_rectf rect = polyi == 0 ? rect1 : rect2;

        for(int i1 = 0; i1 < 4; i1++) {
            int i2 = (i1 + 1) % 4;

            float normalx = rect.verticies[i2].y - rect.verticies[i1].y;
            float normaly = rect.verticies[i2].x - rect.verticies[i1].x;

            float mina = FLT_MAX;
            float maxa = -FLT_MAX;
            for(int ai = 0; ai < 4; ai++) {
                float projected = (normalx * rect1.verticies[ai].x) + (normaly * rect1.verticies[ai].y);
                if(projected < mina) mina = projected;
                if(projected > maxa) maxa = projected;
            }

            float minb = FLT_MAX;
            float maxb = -FLT_MIN;
            for(int bi = 0; bi < 4; bi++) {
                float projected = (normalx * rect2.verticies[bi].x) + (normaly * rect2.verticies[bi].y);
                if(projected < minb) minb = projected;
                if(projected > maxb) maxb = projected;
            }

            if(maxa < minb || maxb < mina)
                return false;
        }
    }
    return true;
}

///////////////////////////
// SOLVERS
///////////////////////////

void ye_solve_rect_rect_collision(struct ye_entity *ent1, struct ye_entity *ent2) {
    if(!_signaling_helper(ent1, ent2))
        return;
    
    /*
        TODO:

        - apply impulses based on transform relativity or straight to collider's rigidbody as needed
            - colliders without rigidbodies are static!!!!
    */
}