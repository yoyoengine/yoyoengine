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
char *project_name;

void ye_editor_paint_project_settings(struct nk_context *ctx){
    if (nk_begin(ctx, "Project Settings", nk_rect(screenWidth/2 - 250, screenHeight/2 - 250, 500, 500),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Project Settings", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Project Settings", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Project Settings", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Project Settings", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Project Settings", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Project Settings", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Project Settings", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Project Settings", NK_TEXT_LEFT);

            // close button
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_button_label(ctx, "Save and Close")){
                project_settings_open = false;
                remove_ui_component("project settings");
            }
            if(nk_button_label(ctx, "Cancel")){
                project_settings_open = false;
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
                    remove_ui_component("project settings");
                }
                else{
                    project_settings_open = true;
                    ui_register_component("project settings",ye_editor_paint_project_settings);
                    ye_json_log(SETTINGS);
                    // setup a bunch of stack variables for showing current settings
                    if(!ye_json_string(SETTINGS, "name", &project_name)){
                        printf("Error getting project name\n");
                    }
                    else{
                        printf("Project name: %s\n", project_name);
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