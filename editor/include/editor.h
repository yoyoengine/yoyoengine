/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_EDITOR_H
#define YE_EDITOR_H

#define YE_EDITOR_VERSION "build 0"

#include <stdbool.h>
#include <jansson.h>
#include <yoyoengine/yoyoengine.h>

/*
    Macros for marking dirty and saving status
*/
#define editor_unsaved() unsaved = true 
#define editor_saved() unsaved = saving = false;
#define editor_saving() saving = true

// editor preferences
struct editor_prefs {
    int color_scheme_index;

    // selection settings
    int min_select_px;
};

extern struct editor_prefs PREFS;

// global variables
extern bool unsaved;
extern bool saving;
extern bool quit;
extern bool lock_viewport_interaction;
extern struct ye_entity * editor_camera;
extern struct ye_entity * origin;
extern float screenWidth;
extern float screenHeight;
extern struct ye_entity_node * entity_list_head;
extern char *project_path;

// some fields for current selected entity tracking (this will be messy)
// actually lets be really smart and keep local copy
extern struct ye_entity staged_entity;
extern json_t * SETTINGS;

extern int mouse_world_x;
extern int mouse_world_y;

// selecting info
extern SDL_Rect editor_selecting_rect;

// panning info
extern SDL_Point pan_start;
extern SDL_Point pan_end;
extern bool editor_panning;

extern char editor_settings_path[1024];

/*
    Icons for the editor UI in nuklear
*/
struct edicons {
    struct nk_image style;
    struct nk_image gear;
    struct nk_image folder;
    struct nk_image build;
    struct nk_image trick;
    struct nk_image play;
    struct nk_image buildrun;
    struct nk_image pack;
    struct nk_image game;
    struct nk_image eye;
    struct nk_image buildreconfigure;
    struct nk_image duplicate;
    struct nk_image trash;
};

extern struct edicons editor_icons;

/*
    Struct to hold editor preferences/settings

    TODO: decide how struct members here and editor specific YE_STATE struct members
    will interact. Should they have pointers in here?
*/
struct editor_state {
    /*
        Camera Zoom Style
    */
    enum {
        ZOOM_TOP_LEFT,
        ZOOM_CENTER,
        ZOOM_MOUSE
    } zoom_style;
};

extern struct editor_state EDITOR_STATE;

bool ye_point_in_rect(int x, int y, SDL_Rect rect);

void editor_load_scene(char * path);

void editor_re_attach_ecs();

void editor_reload_settings();

/*
    Really minimal stripped rendering function that will
    load just one frame into the window of only ui

    TODO:
    opportunity to make a cool background screen for this loading
*/
void yoyo_loading_refresh(char * status);

/*
    Locks the editor viewport from interaction
*/
void lock_viewport();

/*
    Unlocks the editor viewport for interaction
*/
void unlock_viewport();

#endif // YE_EDITOR