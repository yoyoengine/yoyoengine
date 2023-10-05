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

// make some editor specific declarations to change engine core behavior
#define YE_EDITOR

#define YE_EDITOR_VERSION "v0.1.0"

// global variables
bool quit = false;
bool dragging = false;
int last_x;
int last_y;
struct ye_entity * editor_camera;
struct ye_entity * origin;
int screenWidth;
int screenHeight;
struct ye_entity_node * entity_list_head;
char *project_path;

// some fields for current selected entity tracking (this will be messy)
// actually lets be really smart and keep local copy
struct ye_entity staged_entity;
struct ye_component_transform staged_transform;

/*
    Registered input handler

    TODO: 
    - zoom in should center on the mouse or the center of the screen
    - zoom should not work when not hovering viewport
*/
void handle_input(SDL_Event event) {
    if(event.type == SDL_QUIT) {
        quit = true;
    }
    else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            default:
                break;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_RIGHT) {
            if (event.button.x > 0 && event.button.x < screenWidth/1.5 &&
                event.button.y > 0 && event.button.y < screenHeight/1.5) {
                dragging = true;
                last_x = event.button.x;
                last_y = event.button.y;
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL));
            }
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_RIGHT) {
            dragging = false;
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        }
    }
    else if (event.type == SDL_MOUSEMOTION) {
        if (dragging) {
            int dx = event.motion.x - last_x;
            int dy = event.motion.y - last_y;
            editor_camera->transform->rect.x -= dx;
            editor_camera->transform->rect.y -= dy;
            last_x = event.motion.x;
            last_y = event.motion.y;
        }
    }
    else if (event.type == SDL_MOUSEWHEEL) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        if (event.wheel.y > 0 && x > 0 && x < screenWidth/1.5 &&
            y > 0 && y < screenHeight/1.5) {
            if(editor_camera->camera->view_field.w > 16){
                editor_camera->camera->view_field.w -= 16;
                editor_camera->camera->view_field.h -= 9;
            }
        }
        else if (event.wheel.y < 0 && x > 0 && x < screenWidth/1.5 &&
            y > 0 && y < screenHeight/1.5) {
            editor_camera->camera->view_field.w += 16;
            editor_camera->camera->view_field.h += 9;
        }
    }
}



// EDITOR PANELS //


void ye_editor_paint_hiearchy(struct nk_context *ctx){
    // if no selected entity its height will be full height, else its half
    int height = engine_runtime_state.selected_entity == NULL ? screenHeight : screenHeight/2;
    if (nk_begin(ctx, "Heiarchy", nk_rect(screenWidth/1.5, 0, screenWidth - screenWidth/1.5, height),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Controls:", NK_TEXT_LEFT);
            if(nk_button_label(ctx, "New Entity")){
                ye_create_entity();
                entity_list_head = ye_get_entity_list_head();
            }

            nk_label(ctx, "Entities:", NK_TEXT_LEFT);

            // iterate through entity_list_head and display the names of each entity as a button
            struct ye_entity_node *current = entity_list_head;
            while(current != NULL){
                /*
                    Honestly, should just leave editor camera in the heiarchy for fun lol
                    Kinda funny that you could just nuke it if you wanted
                */
                if(current->entity == editor_camera || current->entity == origin){
                    current = current->next;
                    continue;
                }
                if(nk_button_label(ctx, current->entity->name)){
                    if(engine_runtime_state.selected_entity == current->entity){
                        engine_runtime_state.selected_entity = NULL;
                        break;
                    }
                    engine_runtime_state.selected_entity = current->entity;
                    entity_list_head = ye_get_entity_list_head();
                    // set all our current entity staging fields
                    staged_entity = *current->entity;
                    break;
                }
                current = current->next;
            }
        nk_end(ctx);
    }
}

/*
    The entity preview will be a snapshot of the entity when selected, it will stash current state, and allow editing of the entity until
    dev clicks "save" or "cancel"
*/
void ye_editor_paint_entity(struct nk_context *ctx){
    if(engine_runtime_state.selected_entity == NULL){
        return;
    }
    if (nk_begin(ctx, "Entity", nk_rect(screenWidth/1.5, screenHeight / 2, screenWidth - screenWidth/1.5, screenHeight),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            if(engine_runtime_state.selected_entity == NULL){
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label_colored(ctx, "No entity selected", NK_TEXT_CENTERED, nk_rgb(255, 255, 255));
            }
            else{
                nk_layout_row_dynamic(ctx, 25, 2);
                nk_label(ctx, "Name:", NK_TEXT_LEFT);
                nk_label(ctx, engine_runtime_state.selected_entity->name, NK_TEXT_LEFT);

                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Components:", NK_TEXT_LEFT);
                // iterate through the components and display them
                // struct ye_component_node *current = selected_entity->component_list_head;
                // while(current != NULL){
                //     nk_layout_row_dynamic(ctx, 25, 1);
                //     nk_label(ctx, current->component->name, NK_TEXT_LEFT);
                //     current = current->next;
                // }

                // save or cancel buttons
                nk_layout_row_dynamic(ctx, 25, 2);
                if(nk_button_label(ctx, "Save")){}
                if(nk_button_label(ctx, "Cancel")){
                    engine_runtime_state.selected_entity = NULL;
                }
            }
        nk_end(ctx);
    }
}

void ye_editor_paint_project(struct nk_context *ctx){
    if (nk_begin(ctx, "Project", nk_rect(0, screenHeight/1.5, screenWidth/1.5 / 2, screenHeight/1.5),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            // button to open project settings
            if(nk_button_label(ctx, "Project Settings")){}
            if(nk_button_label(ctx, "Browse Project Files")){
                char command[256];
                snprintf(command, sizeof(command), "xdg-open \"%s\"", project_path);

                // Execute the command.
                int status = system(command);
            }
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label_colored(ctx, "Copyright (c) Ryan Zmuda 2023", NK_TEXT_CENTERED, nk_rgb(255, 255, 255));
        nk_end(ctx);
    }
}

/*
    TODO:
    - paintbounds somehow starts out checked even though its not, so have to double click to turn on
    - if console is opened when we tick one of these, closing the console crashes
*/
void ye_editor_paint_options(struct nk_context *ctx){
    if (nk_begin(ctx, "Options", nk_rect(screenWidth/1.5 / 2, screenHeight/1.5, screenWidth - screenWidth/1.5, screenHeight - screenHeight/1.5),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            
            nk_label(ctx, "Visual Debugging:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_checkbox_label(ctx, "Display Names", &engine_runtime_state.display_names);
            nk_checkbox_label(ctx, "Paintbounds", &engine_state.paintbounds_visible);

            nk_label(ctx, "Preferences:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_checkbox_label(ctx, "Draw Lines", &engine_runtime_state.editor_display_viewport_lines);

            nk_label(ctx, "Extra:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_checkbox_label(ctx, "Stretch Viewport", &engine_state.stretch_viewport);
        nk_end(ctx);
    }
}


///////////////////



/*
    main function
    accepts one string argument of the path to the project folder
*/
int main(int argc, char **argv) {
    // get our path from the command line
    char *path = argv[1];
    printf("path: %s\n", path);
    project_path = path;

    // print the path
    // printf("path: %s\n", path);

    // make a string "resources path" with resources/ appended
    char resources_path[100];
    snprintf(resources_path, sizeof(resources_path), "%s/resources", path);

    // set window title to "Yoyo Engine Editor - vX.X.X" as defined in YE_EDITOR_VERSION
    char window_title[100];
    snprintf(window_title, sizeof(window_title), "Yoyo Engine Editor - %s", YE_EDITOR_VERSION);

    struct engine_data data = {
        .engine_resources_path = "./engine_resources",
        .game_resources_path = resources_path,
        .window_title = window_title,
        .log_level = 0,
        .volume = 32,
        // .window_mode = SDL_WINDOW_FULLSCREEN_DESKTOP,
        // .override_window_mode = true,
        .handle_input = handle_input,
        .editor_mode = true,

        .override_log_level = true,
        .override_window_title = true,
        .override_volume = true,

        .skipintro = true,

        .game_resources_path_absolute = true,
        // .engine_resources_path_absolute = true,

        .debug_mode = true,
    };
    ye_init_engine(data);

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

    // register all editor ui components
    ui_register_component("heiarchy",ye_editor_paint_hiearchy);
    ui_register_component("entity",ye_editor_paint_entity);
    ui_register_component("options",ye_editor_paint_options);
    ui_register_component("project",ye_editor_paint_project);

    /*
        Heiarchy function that implements listing all items
        This is called by two heiarchy ui's: full and mini. full is default, mini is shown when we have an entity selected

        This is all proposed ^^^ more complicated than just maintaining mini because it will be on screen most of the time anyway
    */

    origin = ye_create_entity_named("origin");
    ye_add_transform_component(origin, (struct ye_rectf){-50, -50, 100, 100}, 0, YE_ALIGN_MID_CENTER);
    ye_temp_add_image_renderer_component(origin, ye_get_engine_resource_static("originwhite.png"));

    // load the scene out of the project settings::entry scene
    json_t * SETTINGS = ye_json_read(ye_get_resource_static("../settings.yoyo"));
    ye_json_log(SETTINGS);

    SDL_Color red = {255, 0, 0, 255};
    ye_cache_color("warning", red);

    // get the scene to load from "entry scene"
    char * entry_scene; 
    if(!ye_json_string(SETTINGS, "entry scene", &entry_scene)){
        ye_logf(error, "entry scene not found in settings file. No scene has been loaded.");
        // TODO: future me create a text entity easily in the center of the scene alerting this fact
        struct ye_entity *text = ye_create_entity_named("warning text");
        ye_add_transform_component(text, (struct ye_rectf){0, 0, 1920, 500}, 900, YE_ALIGN_MID_CENTER);
        ye_temp_add_text_renderer_component(text, "entry scene not found in settings file. No scene has been loaded.", ye_font("default"), ye_color("warning"));
    }
    else{
        ye_load_scene(ye_get_resource_static(entry_scene));
    }

    entity_list_head = ye_get_entity_list_head();

    while(!quit) {
        ye_process_frame();
    }

    ye_shutdown_engine();
    json_decref(SETTINGS);
    // exit
    return 0;
}