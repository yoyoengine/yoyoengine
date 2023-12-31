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

#ifdef _WIN32
    #include <windows.h>
    #define LIB_HANDLE HMODULE
    #define OPEN_LIB(filename) LoadLibrary(filename)
    #define CLOSE_LIB(lib) FreeLibrary(lib)
    #define GET_LIB_FUNC(lib, funcname) GetProcAddress(lib, funcname)
#else
    #include <dlfcn.h>
    #define LIB_HANDLE void*
    #define OPEN_LIB(filename) dlopen(filename, RTLD_LAZY)
    #define CLOSE_LIB(lib) dlclose(lib)
    #define GET_LIB_FUNC(lib, funcname) dlsym(lib, funcname)
#endif

struct ye_trick_node {
    /*
        Metadata about the trick
    */
    char * name;
    char * author;
    char * description;
    char * version;

    /*
        Function pointers for callbacks
    */
    void (*on_load)();
    void (*on_unload)();
    void (*on_update)();

    LIB_HANDLE lib;

    // LL
    struct ye_trick_node * next;
};

struct ye_trick_node * ye_tricks_head = NULL;

void* _open_trick(const char * path){
    return OPEN_LIB(path);
}

void _close_trick(void* lib){
    CLOSE_LIB(lib);
}

void _bind_trick_callbacks(struct ye_trick_node *node, LIB_HANDLE lib) {
    node->on_load = GET_LIB_FUNC(lib, "yoyo_trick_on_load");
    node->on_unload = GET_LIB_FUNC(lib, "yoyo_trick_on_unload");
    node->on_update = GET_LIB_FUNC(lib, "yoyo_trick_on_update");
}

void ye_init_tricks(){
    json_t * tricks_json = ye_json_read(ye_get_resource_static("../tricks/tricks.yoyo"));

    if(tricks_json == NULL){
        // no tricks to load
        return;
    }

    // get the array called tricks
    json_t * tricks_array = json_object_get(tricks_json, "tricks");

    // for each trick in the array
    for (int i = 0; i < json_array_size(tricks_array); i++){
        json_t * trick = json_array_get(tricks_array, i);
        ye_json_log(trick);
        
        // get the trick name
        const char * name; ye_json_string(trick, "name", &name);
        const char * author; ye_json_string(trick, "author", &author);
        const char * version; ye_json_string(trick, "version", &version);
        const char * filepath; ye_json_string(trick, "filename", &filepath);
        const char * description; ye_json_string(trick, "description", &description);

        ye_logf(info, "Loading trick: %s\n", name);
        ye_logf(info, "Version: %s\n", version);
        ye_logf(info, "Author: %s\n", author);
        ye_logf(info, "Description: %s\n", description);

        // add the trick to the LL
        struct ye_trick_node * new_node = malloc(sizeof(struct ye_trick_node));
        new_node->name = strdup(name);
        new_node->author = strdup(author);
        new_node->version = strdup(version);
        new_node->description = strdup(description);
        new_node->next = ye_tricks_head;

        // create a new string that is ye_get_resource_static("../tricks/FILENAME")
        const char* prefix = "../tricks/";
        size_t path_len = strlen(filepath) + strlen(ye_get_resource_static(prefix)) + 1;

        // Allocate memory for the new string
        char* path = (char*)malloc(path_len);
        if (path == NULL) {
            fprintf(stderr, "Failed to allocate memory for path\n");
            return;
        }

        // Copy the prefix into the new string
        char* result = strcpy(path, ye_get_resource_static(prefix));
        if (result == NULL) {
            fprintf(stderr, "Failed to copy prefix into path\n");
            free(path);
            return;
        }

        // Concatenate the filepath onto the new string
        result = strcat(path, filepath);
        if (result == NULL) {
            fprintf(stderr, "Failed to concatenate filepath onto path\n");
            free(path);
            return;
        }

        ye_tricks_head = new_node;

        // load the trick
        new_node->lib = _open_trick(path);

        // bind the functions for the trick
        _bind_trick_callbacks(new_node, new_node->lib);

        free(path);
    }

    json_decref(tricks_json);

    // call on load for each trick
    struct ye_trick_node * current = ye_tricks_head;
    while(current != NULL){
        if(current->on_load != NULL){
            current->on_load();
        }

        // move to next trick
        current = current->next;
    }
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

        // unload the trick
        _close_trick(current->lib);

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