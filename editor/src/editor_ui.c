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

#include <stdio.h>
#include <yoyoengine/yoyoengine.h>
#include "editor.h"

/*
    INITIALIZE VARIABLES FOR JUST THIS FILE
*/
bool project_settings_open = false;
/*
    VARIABLES INITIALIZED
*/

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

/*
    Popout panel for the project settings
*/
char * project_name;
char * project_entry_scene;
int project_log_level; // 0-4 (debug, info, warning, error, none)
int project_volume; // 0-128
bool project_debug_mode; // true or false
int project_screen_size_UNPROCESSED; // convert 0: 1920x1080, 1: 2560x1440
int project_window_mode_UNPROCESSED; // 0-2 (windowed, fullscreen, borderless) -> 1, 0, SDL_WINDOW_FULLSCREEN_DESKTOP
// we need to process the window mode, 3 should become SDL_WINDOW_FULLSCREEN_DESKTOP
int project_framecap; // -1 for vsync, else 0-MAXINT
char _project_framecap_label[10];
char * project_window_title;
bool project_stretch_viewport; // true or false

void ye_editor_paint_project_settings(struct nk_context *ctx){
    if (nk_begin(ctx, "Settings", nk_rect(screenWidth/2 - 250, screenHeight/2 - 250, 500, 500),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE)) {
            const struct nk_input *in = &ctx->input;
            struct nk_rect bounds;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label_colored(ctx, "Project Settings:", NK_TEXT_LEFT, nk_rgb(255, 255, 255));

            /*
                Project name
            */
            nk_layout_row_dynamic(ctx, 25, 2);
            bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "Project Name:", NK_TEXT_LEFT);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "The name of your game!");
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, project_name, 256, nk_filter_default);

            /*
                Window Title
            */
            nk_layout_row_dynamic(ctx, 25, 2);
            bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "Window Title:", NK_TEXT_LEFT);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "The title of the game window.");
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, project_window_title, 256, nk_filter_default);

            /*
                Entry Scene
            */
            nk_layout_row_dynamic(ctx, 25, 2);
            bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "Entry Scene:", NK_TEXT_LEFT);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "The scene file you want to load first when the game runs (include extension and path relative to /resources/).");
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, project_entry_scene, 256, nk_filter_default);

            /*
                Log Level
            */
            nk_layout_row_dynamic(ctx, 25, 2);
            bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "Log Threshold:", NK_TEXT_LEFT);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "Controls what logs get written at runtime, anything equivalent to, or more severe than your threshold will be logged.");
            // combo box with options cooresponding
            static const char *log_levels[] = {"Debug", "Info", "Warning", "Error", "None"};
            nk_combobox(ctx, log_levels, NK_LEN(log_levels), &project_log_level, 25, nk_vec2(200,200));

            /*
                Volume (slider: 0-128)
            */
            nk_layout_row_dynamic(ctx, 25, 2);
            bounds = nk_widget_bounds(ctx);
            char volume_label[256];
            snprintf(volume_label, sizeof(volume_label), "Volume: %d", project_volume);
            nk_label(ctx, volume_label, NK_TEXT_LEFT);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "The volume of the game, 0-128");
            nk_slider_int(ctx, 0, &project_volume, 128, 1);

            /*
                Screen size (combo box) 1920x1080 2560x1440
            */
            nk_layout_row_dynamic(ctx, 25, 2);
            bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "Window Size:", NK_TEXT_LEFT);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "The size of the game window.");
            static const char *screen_sizes[] = {"1920x1080", "2560x1440"};
            nk_combobox(ctx, screen_sizes, NK_LEN(screen_sizes), &project_screen_size_UNPROCESSED, 25, nk_vec2(200,200));

            /*
                Window Mode (combo box) Fullscreen, Windowed, Borderless
            */
            nk_layout_row_dynamic(ctx, 25, 2);
            bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "Window Mode:", NK_TEXT_LEFT);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "The mode of the game window.");
            static const char *window_modes[] = {"Windowed","Fullscreen","Borderless"};
            nk_combobox(ctx, window_modes, NK_LEN(window_modes), &project_window_mode_UNPROCESSED, 25, nk_vec2(200,200));

            /*
                Framecap (int input, -1 for vsync)
            */
            nk_layout_row_dynamic(ctx, 25, 2);
            bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "FPS Cap:", NK_TEXT_LEFT);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "The maximum number of frames per second the game will run at. -1 for vsync.");
            if(nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, _project_framecap_label, 10, nk_filter_decimal)){
                project_framecap = atoi(_project_framecap_label);
            }

            // lay out our booleans
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label_colored(ctx, "Advanced Project Settings:", NK_TEXT_LEFT, nk_rgb(255, 255, 255));
            nk_layout_row_dynamic(ctx, 25, 2);

            /*
                Checkbox for debug mode
            */
            bounds = nk_widget_bounds(ctx);
            nk_checkbox_label(ctx, "Debug Mode", &project_debug_mode);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "If checked, the game will run in debug mode, which will allow you to use the console and other debug features.");

            /*
                Checkbox for stretch viewport
            */
            bounds = nk_widget_bounds(ctx);
            nk_checkbox_label(ctx, "Stretch Viewport", &project_stretch_viewport);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "If checked, the viewport will stretch to the size of the window, if unchecked, the viewport will render from its actual perspective.");

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label_colored(ctx, "Build Settings:", NK_TEXT_LEFT, nk_rgb(255, 255, 255));

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "TODO: BUILD SETTINGS GO HERE :3", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);

            // close button
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_button_label(ctx, "Save and Close")){
                project_settings_open = false;
                lock_viewport_interaction = false;

                // save the settings (might be long and gross)
                // update all the keys in SETTINGS
                json_object_set_new(SETTINGS, "name", json_string(project_name));
                json_object_set_new(SETTINGS, "entry_scene", json_string(project_entry_scene));
                json_object_set_new(SETTINGS, "window_title", json_string(project_window_title));
                json_object_set_new(SETTINGS, "log_level", json_integer(project_log_level));
                json_object_set_new(SETTINGS, "volume", json_integer(project_volume));
                json_object_set_new(SETTINGS, "debug_mode", project_debug_mode ? json_true() : json_false());
                json_object_set_new(SETTINGS, "screen_width", project_screen_size_UNPROCESSED == 0 ? json_integer(1920) : json_integer(2560));
                if (project_window_mode_UNPROCESSED == 2) {
                    json_object_set_new(SETTINGS, "window_mode", json_integer(SDL_WINDOW_FULLSCREEN_DESKTOP));
                } else {
                    json_object_set_new(SETTINGS, "window_mode", project_window_mode_UNPROCESSED == 0 ? json_integer(0) : json_integer(1));
                }
                json_object_set_new(SETTINGS, "framecap", json_integer(project_framecap));
                json_object_set_new(SETTINGS, "stretch_viewport", project_stretch_viewport ? json_true() : json_false());
                ye_json_write(ye_get_resource_static("../settings.yoyo"),SETTINGS);
                remove_ui_component("project settings");
            }
            if(nk_button_label(ctx, "Cancel")){
                project_settings_open = false;
                lock_viewport_interaction = false;
                remove_ui_component("project settings");
            }
        nk_end(ctx);
    }
}

void ye_editor_paint_project(struct nk_context *ctx){
    if (nk_begin(ctx, "Project", nk_rect(0, screenHeight/1.5, screenWidth/1.5 / 2, screenHeight/1.5),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            // button to open project settings
            if(nk_button_label(ctx, "Project Settings")){
                if(project_settings_open){
                    project_settings_open = false;
                    lock_viewport_interaction = false;
                    remove_ui_component("project settings");
                }
                else{
                    editor_reload_settings();
                    project_settings_open = true;
                    lock_viewport_interaction = true;
                    ui_register_component("project settings",ye_editor_paint_project_settings);
                    // ye_json_log(SETTINGS);
                    // setup a bunch of stack variables for showing current settings
                    
                    /*
                        Project Name
                    */
                    if(!ye_json_string(SETTINGS, "name", &project_name)){
                        project_name = strdup("Yoyo Engine Game");
                    }

                    /*
                        Entry Scene
                    */
                    if(!ye_json_string(SETTINGS,"entry_scene", &project_entry_scene)){
                        project_entry_scene = strdup("scenes/entry.yoyo");
                    }

                    /*
                        Window Title
                    */
                    if(!ye_json_string(SETTINGS, "window_title", &project_window_title)){
                        project_window_title = strdup("Yoyo Engine Game");
                    }

                    /*
                        Log Level
                    */
                    if(!ye_json_int(SETTINGS, "log_level", &project_log_level)){
                        project_log_level = 0;
                    }

                    /*
                        Volume
                    */
                    if(!ye_json_int(SETTINGS, "volume", &project_volume)){
                        project_volume = 128;
                    }

                    /*
                        Debug Mode
                    */
                    if(!ye_json_bool(SETTINGS, "debug_mode", &project_debug_mode)){
                        project_debug_mode = false;
                    }

                    /*
                        Window Size
                    */
                    int sw;
                    if(!ye_json_int(SETTINGS, "screen_width", &sw)){
                        project_screen_size_UNPROCESSED = 0;
                    }
                    else{
                        if(sw == 1920){
                            project_screen_size_UNPROCESSED = 0;
                        }
                        else if(sw == 2560){
                            project_screen_size_UNPROCESSED = 1;
                        }
                        else{
                            project_screen_size_UNPROCESSED = 0; // 1920x1080 defualt
                        }
                    }

                    /*
                        Window Mode
                    */
                    if(!ye_json_int(SETTINGS, "window_mode", &project_window_mode_UNPROCESSED)){
                        project_window_mode_UNPROCESSED = 0;
                    }else{
                        if(project_window_mode_UNPROCESSED == SDL_WINDOW_FULLSCREEN_DESKTOP){
                            project_window_mode_UNPROCESSED = 2;
                        }
                    }

                    /*
                        Framecap
                    */
                    if(!ye_json_int(SETTINGS, "framecap", &project_framecap)){
                        project_framecap = -1;
                        sprintf(_project_framecap_label, "-1");
                    }
                    else{
                        sprintf(_project_framecap_label, "%d", project_framecap);
                    }

                    /*
                        Stretch Viewport
                    */
                    if(!ye_json_bool(SETTINGS, "stretch_viewport", &project_stretch_viewport)){
                        project_stretch_viewport = false;
                    }
                }
            }
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