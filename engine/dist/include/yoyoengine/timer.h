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

/**
 * @file timer.h
 * @brief Provides a simple timer system for the engine.
 */

#ifndef YE_TIMER_H
#define YE_TIMER_H

#include <stdbool.h>
#include <yoyoengine/yoyoengine.h>

/**
 * @brief A timer that can be registered with the engine.
 * 
 * Will be called back as many times as it loops (or infinitely if loops is -1)
 * after length_ms milliseconds have passed.
*/
struct ye_timer {
    int start_ticks;
    int loops;
    int length_ms;
    void (*callback)();
};

/**
 * @brief Node in the linked list of timers.
 * 
 * @note you must malloc the timer, but its memory will be managed by the timer system.
 */
struct ye_timer_node {
    struct ye_timer * timer;
    struct ye_timer_node * next;
};

/**
 * @brief Register a timer with the engine.
 * 
 * @param timer The timer to register.
 */
void ye_register_timer(struct ye_timer * timer);

/**
 * @brief Unregister a timer with the engine.
 * 
 * @param timer A pointer to the timer to unregister.
 */
void ye_unregister_timer(struct ye_timer * timer);

/**
 * @brief Destroy all timers registered with the engine.
 */
void ye_unregister_all_timers();

/**
 * @brief Update and tick all timers registered with the engine.
 */
void ye_update_timers();

/**
 * @brief Initialize the timer system.
 */
void ye_init_timers();

/**
 * @brief Shutdown the timer system.
 */
void ye_shutdown_timers();

#endif