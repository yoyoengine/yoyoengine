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

#ifndef YE_TIMER_H
#define YE_TIMER_H

#include <stdbool.h>
#include <yoyoengine/yoyoengine.h>

/*
    A timer that can be registered with the engine
    
    Will be called back as many times as it loops (or infinitely if loops is -1)
    after length_ms milliseconds have passed

    TODO: 
    - could add pausing in the future with a pause_ticks field
    - do we need to pass more info than just calling a particular callback?
    - should we allow a void pointer of data to be passed with the callback?
*/
struct ye_timer {
    int start_ticks;
    int loops;
    int length_ms;
    void (*callback)(void);
};

/*
    Linked list of timers
*/
struct ye_timer_node {
    struct ye_timer * timer;
    struct ye_timer_node * next;
};

/*
    Register a timer with the engine
*/
void ye_register_timer(struct ye_timer * timer);

/*
    Unregister a timer with the engine
*/
void ye_unregister_timer(struct ye_timer * timer);

/*
    Unregister all timers with the engine
*/
void ye_unregister_all_timers();

/*
    Update all timers with the engine

    TODO:
    - a timer that flips a flag (can be constructed with
      just a timestamp and a flag ptr to flip auto after done)
*/
void ye_update_timers();

/*
    Initialize the timer system
*/
void ye_init_timers();

/*
    Destroy the timer system
*/
void ye_shutdown_timers();

#endif