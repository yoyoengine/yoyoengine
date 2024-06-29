/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
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
#include "editor_panels.h"
#include "editor.h"
#include "editor_input.h"
#include "editor_selection.h"
#include <SDL.h>
#include <SDL_image.h>
#include <Nuklear/style.h>
#include <Nuklear/nuklear_sdl_renderer.h>

// make some editor specific declarations to change engine core behavior
#define YE_EDITOR

/*
    INITIALIZE ALL
*/
struct editor_prefs PREFS = {0};

bool unsaved;
bool saving; // in the process of saving
bool quit;
bool lock_viewport_interaction;
struct ye_entity *editor_camera;
struct ye_entity *origin;
float screenWidth;
float screenHeight;
struct ye_entity_node *entity_list_head;
char *project_path;
struct ye_entity staged_entity;
json_t *SETTINGS;
struct editor_state EDITOR_STATE;

// holds the path to the editor settings file
char editor_settings_path[1024];

int mouse_world_x = 0;
int mouse_world_y = 0;

// selecting info
SDL_Rect editor_selecting_rect;

// panning info
SDL_Point pan_start;
SDL_Point pan_end;
bool editor_panning = false;

// nk_image icons
struct edicons editor_icons;

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
    SETTINGS = ye_json_read(ye_path("settings.yoyo"));
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

void yoyo_loading_refresh(char * status)
{
    // update status
    snprintf(editor_loading_buffer, sizeof(editor_loading_buffer), "%s", status);

    // clear the screen
    SDL_RenderClear(YE_STATE.runtime.renderer);

    SDL_RenderSetViewport(YE_STATE.runtime.renderer, NULL);
    SDL_RenderSetScale(YE_STATE.runtime.renderer, 1.0f, 1.0f);

    // paint just the loading
    editor_panel_loading(YE_STATE.engine.ctx);
    nk_sdl_render(NK_ANTI_ALIASING_ON);

    SDL_RenderPresent(YE_STATE.runtime.renderer);

    SDL_UpdateWindowSurface(YE_STATE.runtime.window);
}

// pointers to destroy icon textures on shutdown
SDL_Texture * style = NULL;
SDL_Texture * gear = NULL;
SDL_Texture * folder = NULL;
SDL_Texture * build = NULL;
SDL_Texture * trick = NULL;
SDL_Texture * play = NULL;
SDL_Texture * buildrun = NULL;
SDL_Texture * pack = NULL;
SDL_Texture * game = NULL;
SDL_Texture * eye = NULL;

/*
    main function
    accepts one string argument of the path to the project folder
*/
int main(int argc, char **argv) {
    // idk why i put this first but whatever
    editor_selecting_rect = (SDL_Rect){0, 0, 0, 0};

    /*
        Define editor state and set any defaults
        for first timers or unserialized values
    */
    EDITOR_STATE = (struct editor_state){
        .zoom_style = ZOOM_MOUSE
    };

    (void)argc; // supress compiler warning

    // build up editor contexts
    editor_settings_ui_init();

    // get our path from the command line
    char *path = argv[1];
    // printf("PATH WAS: %s\n", path);
    ye_logf(info, "Editor recieved path: %s\n",path);
    project_path = path;

    // init the engine. this starts the engine as thinking our editor directory is the game dir. this is ok beacuse we want to configure based off of the editor settings.json
    ye_init_engine();

    // load editor icons //
    // TODO: macro this?

    SDL_Surface *tmp_sur = IMG_Load(ye_get_engine_resource_static("edicon_style.png"));
    SDL_Texture *style = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, tmp_sur);
    editor_icons.style = nk_image_ptr(style);
    SDL_FreeSurface(tmp_sur);

    tmp_sur = IMG_Load(ye_get_engine_resource_static("edicon_gear.png"));
    SDL_Texture *gear = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, tmp_sur);
    editor_icons.gear = nk_image_ptr(gear);
    SDL_FreeSurface(tmp_sur);

    tmp_sur = IMG_Load(ye_get_engine_resource_static("edicon_folder.png"));
    SDL_Texture *folder = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, tmp_sur);
    editor_icons.folder = nk_image_ptr(folder);
    SDL_FreeSurface(tmp_sur);

    tmp_sur = IMG_Load(ye_get_engine_resource_static("edicon_build.png"));
    SDL_Texture *build = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, tmp_sur);
    editor_icons.build = nk_image_ptr(build);
    SDL_FreeSurface(tmp_sur);

    tmp_sur = IMG_Load(ye_get_engine_resource_static("edicon_trick.png"));
    SDL_Texture *trick = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, tmp_sur);
    editor_icons.trick = nk_image_ptr(trick);
    SDL_FreeSurface(tmp_sur);

    tmp_sur = IMG_Load(ye_get_engine_resource_static("edicon_play.png"));
    SDL_Texture *play = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, tmp_sur);
    editor_icons.play = nk_image_ptr(play);
    SDL_FreeSurface(tmp_sur);

    tmp_sur = IMG_Load(ye_get_engine_resource_static("edicon_buildrun.png"));
    SDL_Texture *buildrun = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, tmp_sur);
    editor_icons.buildrun = nk_image_ptr(buildrun);
    SDL_FreeSurface(tmp_sur);

    tmp_sur = IMG_Load(ye_get_engine_resource_static("edicon_pack.png"));
    SDL_Texture *pack = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, tmp_sur);
    editor_icons.pack = nk_image_ptr(pack);
    SDL_FreeSurface(tmp_sur);

    tmp_sur = IMG_Load(ye_get_engine_resource_static("edicon_game.png"));
    SDL_Texture *game = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, tmp_sur);
    editor_icons.game = nk_image_ptr(game);
    SDL_FreeSurface(tmp_sur);

    tmp_sur = IMG_Load(ye_get_engine_resource_static("edicon_eye.png"));
    SDL_Texture *eye = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, tmp_sur);
    editor_icons.eye = nk_image_ptr(eye);
    SDL_FreeSurface(tmp_sur);

    tmp_sur = IMG_Load(ye_get_engine_resource_static("edicon_buildreconfigure.png"));
    SDL_Texture *buildreconfigure = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, tmp_sur);
    editor_icons.buildreconfigure = nk_image_ptr(buildreconfigure);
    SDL_FreeSurface(tmp_sur);

    tmp_sur = IMG_Load(ye_get_engine_resource_static("edicon_duplicate.png"));
    SDL_Texture *duplicate = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, tmp_sur);
    editor_icons.duplicate = nk_image_ptr(duplicate);
    SDL_FreeSurface(tmp_sur);

    ///////////////////////

    // get an initial screen size
    struct ScreenSize ss = ye_get_screen_size();
    screenWidth = ss.width; screenHeight = ss.height;

    // refresh the screen
    yoyo_loading_refresh("Initializing editor window...");

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

    yoyo_loading_refresh("Reading editor settings...");

    /*
        Open the editor settings config.

        TODO: maybe nest fields to make it more
        readible from text editor later on.
    */
    json_t *EDITOR_SETTINGS = ye_json_read(editor_settings_path);
    if(EDITOR_SETTINGS == NULL){
        ye_logf(warning, "editor config file not found. It will be created with defaults.\n");
        EDITOR_SETTINGS = json_object();
    }

    /*
        Read the editor config into the state struct,
        setting defaults as needed
    */
    PREFS.color_scheme_index = ye_config_int(EDITOR_SETTINGS, "color_scheme_index", 5); // amoled by default
    PREFS.min_select_px = ye_config_int(EDITOR_SETTINGS, "min_select_px", 10); // 10px by default

    // close the editor settings file
    json_decref(EDITOR_SETTINGS);
    
    /*
        Actually handle the picked pref initialization
    */
    switch(PREFS.color_scheme_index){
        case 0:
            set_style(YE_STATE.engine.ctx, THEME_BLACK);
            break;
        case 1:
            set_style(YE_STATE.engine.ctx, THEME_DARK);
            break;
        case 2:
            set_style(YE_STATE.engine.ctx, THEME_BLUE);
            break;
        case 3:
            set_style(YE_STATE.engine.ctx, THEME_RED);
            break;
        case 4:
            set_style(YE_STATE.engine.ctx, THEME_WHITE);
            break;
        case 5:
        default:
            set_style(YE_STATE.engine.ctx, THEME_AMOLED);
            break;
    }

    yoyo_loading_refresh("Constructing editor...");

    // update the games knowledge of where the resources path is, now for all the engine is concerned it is our target game
    if (path != NULL)
        ye_update_base_path(path); // GOD THIS IS SUCH A HEADACHE
    else
        ye_logf(error, "No project path provided. Please provide a path to the project folder as the first argument.");

    // let the engine know we also want to custom handle inputs
    YE_STATE.engine.callbacks.input_handler = editor_handle_input;

    // update screenWidth and screenHeight
    struct ScreenSize screenSize = ye_get_screen_size();
    screenWidth = screenSize.width;
    screenHeight = screenSize.height;

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

    SDL_Texture *orgn_tex = SDL_CreateTextureFromSurface(YE_STATE.runtime.renderer, yep_engine_resource_image("originwhite.png"));
    ye_cache_texture_manual(orgn_tex, "originwhite.png");
    ye_add_image_renderer_component_preloaded(origin, 0, orgn_tex);
    origin->renderer->rect = (struct ye_rectf){0, 0, 100, 100};

    yoyo_loading_refresh("Loading entry scene...");

    // load the scene out of the project settings::entry_scene
    SETTINGS = ye_json_read(ye_path("settings.yoyo"));
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
        ye_add_text_renderer_component(text, 900, "entry_scene not found in settings file. No scene has been loaded.", "default", 128, "warning",0);
        text->renderer->rect = (struct ye_rectf){0, 0, 1920, 500};
    }
    else
    {
        ye_load_scene(entry_scene);
    }

    entity_list_head = ye_get_entity_list_head();

    // TODO: remove in future when we serialize editor prefs
    YE_STATE.editor.editor_display_viewport_lines = true;

    ye_logf(info, "Editor fully initialized.\n");
    ye_logf(info, "---------- BEGIN RUNTIME OUTPUT ----------\n");

    while(!quit) {
        if(editor_draw_drag_rect)
            ye_debug_render_rect(editor_selecting_rect.x, editor_selecting_rect.y, editor_selecting_rect.w, editor_selecting_rect.h, (SDL_Color){255, 0, 0, 255}, 10);
        if(editor_panning)
            ye_debug_render_line(pan_start.x, pan_start.y, pan_end.x, pan_end.y, (SDL_Color){255, 255, 255, 255}, 10);
        editor_render_selection_rects();
        ye_process_frame();
    }

    /*
        Before we shutdown the editor, lets re-serialize
        the preferences we initially loaded
    */
    EDITOR_SETTINGS = ye_json_read(editor_settings_path);
    
    json_object_set_new(EDITOR_SETTINGS, "color_scheme_index", json_integer(PREFS.color_scheme_index));
    json_object_set_new(EDITOR_SETTINGS, "min_select_px", json_integer(PREFS.min_select_px));    

    ye_json_write(editor_settings_path, EDITOR_SETTINGS);
    json_decref(EDITOR_SETTINGS);

    // free editor icons
    SDL_DestroyTexture(style);
    SDL_DestroyTexture(gear);
    SDL_DestroyTexture(folder);
    SDL_DestroyTexture(build);
    SDL_DestroyTexture(trick);
    SDL_DestroyTexture(play);
    SDL_DestroyTexture(buildrun);
    SDL_DestroyTexture(pack);
    SDL_DestroyTexture(game);
    SDL_DestroyTexture(eye);

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