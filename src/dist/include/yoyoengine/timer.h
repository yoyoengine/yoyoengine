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