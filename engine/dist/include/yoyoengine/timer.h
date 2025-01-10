/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file timer.h
 * @brief Provides a simple timer system for the engine.
 */

#ifndef YE_TIMER_H
#define YE_TIMER_H

#include <yoyoengine/export.h>

#include <stdbool.h>

#include <yoyoengine/logging.h>

/**
 * @brief A timer that can be registered with the engine.
 * 
 * Will be called back as many times as it loops (or infinitely if loops is -1)
 * after length_ms milliseconds have passed.
 * 
 * The callback function recieves a pointer to the timer,
 * where you can access its custom data as needed.
 * 
 * Loops is the total number of times you want the timer to run, including the first execution.
 * ie: loops of 5 means the timer runs once, and then repeats
 * 4 more iterations.
*/
struct ye_timer {
    int start_ticks;
    int loops;
    int length_ms;
    void * data;
    void (*callback)(struct ye_timer * timer);
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

/*
    Optional function to be registered by
    user that gives info on timer system
*/
YE_API void ye_timer_overlay(struct nk_context *ctx);

/**
 * @brief Register a timer with the engine.
 * 
 * @note !!! YOU ARE RESPONSIBLE FOR SETTING EVERY FIELD, INCLUDING START TICKS !!!
 * 
 * @param timer The timer to register.
 */
YE_API void ye_register_timer(struct ye_timer * timer);

/**
 * @brief Unregister a timer with the engine.
 * 
 * @param timer A pointer to the timer to unregister.
 */
YE_API void ye_unregister_timer(struct ye_timer * timer);

/**
 * @brief Destroy all timers registered with the engine.
 */
YE_API void ye_unregister_all_timers();

/**
 * @brief Update and tick all timers registered with the engine.
 */
YE_API void ye_update_timers();

/**
 * @brief Initialize the timer system.
 */
YE_API void ye_init_timers();

/**
 * @brief Shutdown the timer system.
 */
YE_API void ye_shutdown_timers();

#endif