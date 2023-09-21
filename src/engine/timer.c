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
