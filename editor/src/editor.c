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

/*
    Use Nuklear to add some editor ui as well as a smaller viewport synced to the current scene event

    Goals:
    - allow easily creating new render objects and events as well as dragging them around to resize and reorient
    - we need a way to put the viewport in a corner or even a seperate window?

    do we want this to live in this folder that its in rn? how to seperate the engine from the core? it needs to ship with the core

    Constraints:
    - editor only supported on linux

    TODO:
    - figure out the viewport position and size and calculate where other windows go
        - this involves going back to the engine and polishing the old shit you wrote
*/

#include <stdio.h>
#include <yoyoengine/yoyoengine.h>
#include "editor_ui.h"
#include "editor_settings_ui.h"
#include "editor.h"
#include "editor_input.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <Nuklear/style.h>

// make some editor specific declarations to change engine core behavior
#define YE_EDITOR

/*
    INITIALIZE ALL
*/
bool quit;
bool dragging;
bool lock_viewport_interaction;
int last_x;
int last_y;
struct ye_entity *editor_camera;
struct ye_entity *origin;
int screenWidth;
int screenHeight;
struct ye_entity_node *entity_list_head;
char *project_path;
struct ye_entity staged_entity;
struct ye_component_transform staged_transform;
json_t *SETTINGS;

// holds the path to the editor settings file
char editor_settings_path[1024];

int mouse_world_x = 0;
int mouse_world_y = 0;

// TESTING
struct ye_entity *snerfbot = NULL;

/*
    ALL GLOBALS INITIALIZED
*/

bool ye_point_in_rect(int x, int y, SDL_Rect rect)
{ // TODO: MOVEME TO ENGINE
    if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h)
        return true;
    return false;
}

void editor_reload_settings(){
    if (SETTINGS)
        json_decref(SETTINGS);
    SETTINGS = ye_json_read(ye_get_resource_static("../settings.yoyo"));
}

void editor_load_scene(char * path){
    ye_load_scene(path);
    editor_re_attach_ecs();
}

void editor_re_attach_ecs(){
    entity_list_head = ye_get_entity_list_head();
    editor_camera = ye_get_entity_by_name("editor_camera");
    origin = ye_get_entity_by_name("origin");
    ye_logf(info, "Re-attatched ECS component pointers.\n");
}

/*
    main function
    accepts one string argument of the path to the project folder
*/
int main(int argc, char **argv) {
    (void)argc; // supress compiler warning

    // build up editor contexts
    editor_settings_ui_init();

    // get our path from the command line
    char *path = argv[1];
    ye_logf(info, "Editor recieved path: %s\n",path);
    project_path = path;

    // init the engine. this starts the engine as thinking our editor directory is the game dir. this is ok beacuse we want to configure based off of the editor settings.json
    ye_init_engine();

    /*
        Do some custom sdl setup for the editor specifically
    */
    // allow window resizing
    SDL_SetWindowResizable(YE_STATE.runtime.window, SDL_TRUE); // maybe expose this in the json later on
    SDL_SetWindowMinimumSize(YE_STATE.runtime.window, 1280, 720); // also maybe expose this as an option.
    /*
        The thing about exposing these in json is that any competant dev (not that I am one) or anyone else (nobody will use this engine but me)
        could easily just add this one line of C code in their init function and get the same result.
    */

    /*
        Set the editor settings path
    */
    char* basePath = SDL_GetBasePath();
    snprintf(editor_settings_path, sizeof(editor_settings_path), "%s./editor.yoyo", basePath);
    free(basePath);

    /*
        Open the editor settings and get "preferences":"theme"
    */
    json_t *EDITOR_SETTINGS = ye_json_read(editor_settings_path);
    json_t *theme;
    if (!ye_json_object(EDITOR_SETTINGS, "preferences", &theme))
    {
        ye_logf(error, "editor settings file is missing preferences object. Please provide a preferences object with a theme string.");
        return 1;
    }
    const char *theme_string;
    if (!ye_json_string(theme, "theme", &theme_string))
    {
        ye_logf(error, "editor settings file is missing theme string. Please provide a preferences object with a theme string.");
        return 1;
    }
    if(strcmp(theme_string, "dark") == 0)
        set_style(YE_STATE.engine.ctx, THEME_DARK);
    else if(strcmp(theme_string, "red") == 0)
        set_style(YE_STATE.engine.ctx, THEME_RED);
    else if(strcmp(theme_string, "black") == 0)
        set_style(YE_STATE.engine.ctx, THEME_BLACK);
    else if(strcmp(theme_string, "white") == 0)
        set_style(YE_STATE.engine.ctx, THEME_WHITE);
    else if(strcmp(theme_string, "blue") == 0)
        set_style(YE_STATE.engine.ctx, THEME_BLUE);
    else if(strcmp(theme_string, "amoled") == 0)
        set_style(YE_STATE.engine.ctx, THEME_AMOLED);
    // if else do nothing, this will result in default style

    set_style(YE_STATE.engine.ctx, THEME_AMOLED);

    // close the editor settings file
    json_decref(EDITOR_SETTINGS);

    // update the games knowledge of where the resources path is, now for all the engine is concerned it is our target game
    if (path != NULL)
        ye_update_resources(path); // GOD THIS IS SUCH A HEADACHE
    else
        ye_logf(error, "No project path provided. Please provide a path to the project folder as the first argument.");

    // let the engine know we also want to custom handle inputs
    YE_STATE.engine.callbacks.input_handler = editor_handle_input;

    // update screenWidth and screenHeight
    struct ScreenSize screenSize = ye_get_screen_size();
    screenWidth = screenSize.width;
    screenHeight = screenSize.height;
    // printf("screen size: %d, %d\n", screenWidth, screenHeight);

    // create our editor camera and register it with the engine
    editor_camera = ye_create_entity_named("editor_camera");
    ye_add_transform_component(editor_camera, 0, 0);
    ye_add_camera_component(editor_camera, 999, (SDL_Rect){0, 0, 2560, 1440});
    ye_set_camera(editor_camera);

    // register all editor ui components
    ui_register_component("heiarchy", ye_editor_paint_hiearchy);
    ui_register_component("entity", ye_editor_paint_inspector);
    ui_register_component("options", ye_editor_paint_options);
    ui_register_component("project", ye_editor_paint_project);
    ui_register_component("editor_menu_bar", ye_editor_paint_menu);

    origin = ye_create_entity_named("origin");
    ye_add_transform_component(origin, -50, -50);
    ye_temp_add_image_renderer_component(origin, 0, ye_get_engine_resource_static("originwhite.png"));
    origin->renderer->rect = (struct ye_rectf){0, 0, 100, 100};

    // load the scene out of the project settings::entry_scene
    SETTINGS = ye_json_read(ye_get_resource_static("../settings.yoyo"));
    // ye_json_log(SETTINGS);

    SDL_Color red = {255, 0, 0, 255};
    ye_cache_color("warning", red);

    // get the scene to load from "entry_scene"
    const char *entry_scene;
    if (!ye_json_string(SETTINGS, "entry_scene", &entry_scene))
    {
        ye_logf(error, "entry_scene not found in settings file. No scene has been loaded.");
        // TODO: future me create a text entity easily in the center of the scene alerting this fact
        struct ye_entity *text = ye_create_entity_named("warning text");
        ye_add_transform_component(text, 0, 0);
        ye_temp_add_text_renderer_component(text, 900, "entry_scene not found in settings file. No scene has been loaded.", "default", 128, "warning");
        text->renderer->rect = (struct ye_rectf){0, 0, 1920, 500};
    }
    else
    {
        ye_load_scene(ye_get_resource_static(entry_scene));
    }

    entity_list_head = ye_get_entity_list_head();

    ye_logf(info, "Editor fully initialized.\n");
    ye_logf(info, "---------- BEGIN RUNTIME OUTPUT ----------\n");

    while(!quit) {
        ye_process_frame();
    }

    ye_shutdown_engine();
    json_decref(SETTINGS);

    // shutdown editor and teardown contextx
    editor_settings_ui_shutdown();

    // exit
    return 0;
}

/*
    Locks the editor viewport from interaction
*/
void lock_viewport(){
    lock_viewport_interaction = true;
}

/*
    Unlocks the editor viewport for interaction
*/
void unlock_viewport(){
    lock_viewport_interaction = false;
}