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

// make some editor specific declarations to change engine core behavior
#define YE_EDITOR

#define YE_EDITOR_VERSION "v0.1.0"

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
    printf("path: %s\n", path);
    project_path = path;

    // init the engine. this starts the engine as thinking our editor directory is the game dir. this is ok beacuse we want to configure based off of the editor settings.json
    ye_init_engine();

    // update the games knowledge of where the resources path is, now for all the engine is concerned it is our target game
    if (path != NULL)
        ye_update_resources(path); // GOD THIS IS SUCH A HEADACHE
    else
        ye_logf(error, "No project path provided. Please provide a path to the project folder as the first argument.");

    YE_STATE.engine.handle_input = editor_handle_input;

    // update screenWidth and screenHeight
    struct ScreenSize screenSize = ye_get_screen_size();
    screenWidth = screenSize.width;
    screenHeight = screenSize.height;
    // printf("screen size: %d, %d\n", screenWidth, screenHeight);

    // create our editor camera and register it with the engine
    editor_camera = ye_create_entity_named("editor_camera");
    ye_add_transform_component(editor_camera, (struct ye_rectf){0, 0, 0, 0}, 999, YE_ALIGN_MID_CENTER);
    ye_add_camera_component(editor_camera, (SDL_Rect){0, 0, 2560, 1440});
    ye_set_camera(editor_camera);

    // create a silly little snerfbot at our mouse world pos
    snerfbot = ye_create_entity_named("snerfbot");
    ye_add_transform_component(snerfbot, (struct ye_rectf){0, 0, 100, 100}, 998, YE_ALIGN_MID_CENTER);
    ye_temp_add_image_renderer_component(snerfbot, ye_get_resource_static("images/snerfbot.jpg"));

    // register all editor ui components
    ui_register_component("heiarchy", ye_editor_paint_hiearchy);
    ui_register_component("entity", ye_editor_paint_entity);
    ui_register_component("options", ye_editor_paint_options);
    ui_register_component("project", ye_editor_paint_project);

    origin = ye_create_entity_named("origin");
    ye_add_transform_component(origin, (struct ye_rectf){-50, -50, 100, 100}, 0, YE_ALIGN_MID_CENTER);
    ye_temp_add_image_renderer_component(origin, ye_get_engine_resource_static("originwhite.png"));

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
        ye_add_transform_component(text, (struct ye_rectf){0, 0, 1920, 500}, 900, YE_ALIGN_MID_CENTER);
        ye_temp_add_text_renderer_component(text, "entry_scene not found in settings file. No scene has been loaded.", ye_font("default"), ye_color("warning"));
    }
    else
    {
        ye_load_scene(ye_get_resource_static(entry_scene));
    }

    entity_list_head = ye_get_entity_list_head();

    while(!quit) {
        snerfbot->transform->rect.x = mouse_world_x;
        snerfbot->transform->rect.y = mouse_world_y;
        ye_process_frame();
    }

    ye_shutdown_engine();
    json_decref(SETTINGS);

    // shutdown editor and teardown contextx
    editor_settings_ui_shutdown();

    // exit
    return 0;
}