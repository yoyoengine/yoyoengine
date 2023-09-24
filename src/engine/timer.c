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

// list to track timers
struct ye_timer_node * timers;

/*
    Register a timer with the engine
*/
void ye_register_timer(struct ye_timer * timer){
    struct ye_timer_node * node = malloc(sizeof(struct ye_timer_node));
    node->timer = timer;
    node->next = timers;
    timers = node;
}

/*
    Unregister a timer with the engine
*/
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
            free(node);
            return;
        }
        prev = node;
        node = node->next;
    }
}

/*
    Unregister all timers with the engine
*/
void ye_unregister_all_timers(){
    struct ye_timer_node * node = timers;
    while(node != NULL){
        struct ye_timer_node * next = node->next;
        free(node);
        node = next;
    }
    timers = NULL;
}

/*
    Update all timers with the engine
*/
void ye_update_timers(){
    struct ye_timer_node * node = timers;
    while(node != NULL){
        struct ye_timer * timer = node->timer;
        if(timer->start_ticks == -1){
            timer->start_ticks = SDL_GetTicks();
        }
        int ticks = SDL_GetTicks();
        if(ticks - timer->start_ticks >= timer->length_ms){
            timer->callback();
            if(timer->loops == -1){
                timer->start_ticks = SDL_GetTicks();
            }
            else if(timer->loops > 0){
                timer->loops--;
                timer->start_ticks = SDL_GetTicks();
            }
            else {
                ye_unregister_timer(timer);
            }
        }
        node = node->next;
    }
}

/*
    Initialize the timer system
*/
void ye_init_timers(){
    timers = NULL;
}

/*
    Destroy the timer system
*/
void ye_shutdown_timers(){
    ye_unregister_all_timers();
    timers = NULL;
}
