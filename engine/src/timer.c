/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>

#include <yoyoengine/ye_nk.h>

#include <yoyoengine/timer.h>
#include <yoyoengine/logging.h>

// list to track timers
struct ye_timer_node * timers;

// some meta the timer system can keep track of
int num_registered_timers = 0;
int timers_checked_this_frame = 0;

/*
    Optional function to be registered
    that gives info on timer system
*/
void ye_timer_overlay(struct nk_context *ctx){
    char num_reg[40];   snprintf(num_reg, 40, "registered_timers:%d", num_registered_timers);
    char checked[40];   snprintf(checked, 40, "checked this frame:%d", timers_checked_this_frame);
    char cur_ticks[40]; snprintf(cur_ticks, 40, "current_ticks:%d", SDL_GetTicks());

    if (nk_begin(ctx, "timer debug", nk_rect(10, 10, 220, 200),
                    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label(ctx, num_reg, NK_TEXT_LEFT);
        nk_label(ctx, cur_ticks, NK_TEXT_LEFT);
        nk_label(ctx, checked, NK_TEXT_LEFT);
    }
    nk_end(ctx);
}

void ye_register_timer(struct ye_timer * timer){
    struct ye_timer_node * node = malloc(sizeof(struct ye_timer_node));
    node->timer = timer;
    node->next = timers;
    timers = node;

    // state
    num_registered_timers++;
}

void ye_unregister_timer(struct ye_timer * timer){
    struct ye_timer_node * node = timers;
    struct ye_timer_node * prev = NULL;
    while(node != NULL){
        if(node->timer == timer){
            if(prev == NULL){
                timers = node->next;
            }
            else {
                prev->next = node->next;
            }
            // we dont free user data, they should do that
            free(node->timer);
            free(node);
            num_registered_timers--;
            return;
        }
        prev = node;
        node = node->next;
    }
}

void ye_unregister_all_timers(){
    struct ye_timer_node * node = timers;
    while(node != NULL){
        struct ye_timer_node * next = node->next;

        // we dont free user data, they should do that
        free(node->timer);
        free(node);
        node = next;
    }
    timers = NULL;
    num_registered_timers = 0;
}

/*
    Debug notes:
    - not removing any timers during the timespan it has two registered with the most recent still ticking
*/
void ye_update_timers(){
    timers_checked_this_frame = 0;
    struct ye_timer_node * node = timers;
    while(node != NULL){
        timers_checked_this_frame++;

        // left for debug purposes
        // printf("checking timer:\n");
        // printf("    start time: %d:\n",node->timer->start_ticks);
        // printf("    length: %d:\n",node->timer->length_ms);
        // printf("    loops remaining: %d:\n",node->timer->loops);

        struct ye_timer * timer = node->timer;
        if(timer->start_ticks <= 0){
            timer->start_ticks = SDL_GetTicks();
        }
        int ticks = SDL_GetTicks();
        if(ticks - timer->start_ticks >= timer->length_ms){
            timer->callback(timer);
            if(timer->loops == -1){
                timer->start_ticks = SDL_GetTicks();
            }
            else if(timer->loops > 1){
                timer->loops--;
                timer->start_ticks = SDL_GetTicks();
            }
            else {
                // left for debug purposes
                // printf("unregistered a timer\n");
                // ye_unregister_timer(timer);

                // remove a timer, and restart this process, we will reach same timers again, but thats ok
                ye_unregister_timer(timer);
                node = timers;
                continue;

                /*
                    Very important!!!

                    If we unregister a timer, our *node becomes out of date.

                    In order to avoid traversing and executing arbitrary data,
                    we will just defer the rest of the timer checks to the next frame.

                    This means we cannot gaurantee more than one timer running per frame.
                */
                // break;
            }
        }
        node = node->next;
    }
}

void ye_init_timers(){
    timers = NULL;
    ye_logf(info,"%s","Initialized timers.\n");
}

void ye_shutdown_timers(){
    ye_unregister_all_timers();
    timers = NULL;
    ye_logf(info,"%s","Shut down timers.\n");
}