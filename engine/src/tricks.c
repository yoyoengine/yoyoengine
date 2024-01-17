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

#include <stdio.h>

struct ye_trick_node * ye_tricks_head = NULL;

void ye_init_tricks(){
    // no longer needed after v2
    ye_logf(info, "Initialized tricks\n");
}

void ye_run_trick_updates(){
    /*
        Traverse loaded tricks, call their update functions
    */
    struct ye_trick_node * current = ye_tricks_head;
    while(current != NULL){
        if(current->on_update != NULL){
            current->on_update();
        }

        // move to next trick
        current = current->next;
    }
}

void ye_register_trick(struct ye_trick_node trick){
    // add the trick to the LL
    struct ye_trick_node * new_node = malloc(sizeof(struct ye_trick_node));
    new_node->name = strdup(trick.name);
    new_node->author = strdup(trick.author);
    new_node->version = strdup(trick.version);
    new_node->description = strdup(trick.description);
    new_node->next = ye_tricks_head;

    ye_tricks_head = new_node;

    // bind the functions for the trick
    new_node->on_load = trick.on_load;
    new_node->on_unload = trick.on_unload;
    new_node->on_update = trick.on_update;
    new_node->lua_bind = trick.lua_bind;
}

void ye_shutdown_tricks(){
    /*
        Traverse loaded tricks, unload them, then cleanup memory
    */
    struct ye_trick_node * current = ye_tricks_head;
    while(current != NULL){
        // let it clean itself up
        if(current->on_unload != NULL)
            current->on_unload();

        // cleanup memory
        free(current->name);
        free(current->author);
        free(current->version);
        free(current->description);

        // move to next trick
        struct ye_trick_node * next = current->next;
        free(current);
        current = next;
    }
}

// loop through all tricks and pass them a lua_State to register their bindings if they have a lua_bind function
void ye_register_trick_lua_bindings(lua_State *state){
    struct ye_trick_node * current = ye_tricks_head;
    while(current != NULL){
        if(current->lua_bind != NULL){
            current->lua_bind(state);
        }
        current = current->next;
    }
}