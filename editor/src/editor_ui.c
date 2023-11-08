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
#include <unistd.h>
#include <yoyoengine/yoyoengine.h>
#include "editor.h"
#include "editor_ui.h"
#include "editor_panels.h"
#include <Nuklear/style.h>

const float ratio[] = {0.03f, 0.92f, 0.05};
void ye_editor_paint_hiearchy(struct nk_context *ctx){
    // if no selected entity its height will be full height, else its half
    int height = YE_STATE.editor.selected_entity == NULL ? screenHeight : screenHeight/2;
    if (nk_begin(ctx, "Heiarchy", nk_rect(screenWidth/1.5, 0, screenWidth - screenWidth/1.5, height),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Controls:", NK_TEXT_LEFT);
            if(nk_button_label(ctx, "New Entity")){
                ye_create_entity();
                entity_list_head = ye_get_entity_list_head();
            }

            nk_label(ctx, "Entities:", NK_TEXT_LEFT);

            nk_layout_row_dynamic(ctx, 25, 3);
            nk_label(ctx, "Active", NK_TEXT_LEFT);
            nk_label(ctx, "Name", NK_TEXT_CENTERED);
            nk_label(ctx, "Delete", NK_TEXT_RIGHT);

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
                nk_layout_row(ctx, NK_DYNAMIC, 25, 3, ratio);
                nk_checkbox_label(ctx, "", (nk_bool*)&current->entity->active);

                // if the entity is selected, display it as a different color
                bool flag = false; // messy way to do this, but it works
                if(YE_STATE.editor.selected_entity == current->entity){
                    nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(100,100,100))); nk_style_push_style_item(ctx, &ctx->style.button.hover, nk_style_item_color(nk_rgb(75,75,75))); nk_style_push_style_item(ctx, &ctx->style.button.active, nk_style_item_color(nk_rgb(50,50,50))); nk_style_push_vec2(ctx, &ctx->style.button.padding, nk_vec2(2,2));
                    flag = true;
                }
                else if(!current->entity->active){
                    nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(40,40,40))); nk_style_push_style_item(ctx, &ctx->style.button.hover, nk_style_item_color(nk_rgb(20,20,20))); nk_style_push_style_item(ctx, &ctx->style.button.active, nk_style_item_color(nk_rgb(75,75,75))); nk_style_push_vec2(ctx, &ctx->style.button.padding, nk_vec2(2,2));
                    flag = true;
                }

                if(nk_button_label(ctx, current->entity->name)){
                    if(YE_STATE.editor.selected_entity == current->entity){
                        YE_STATE.editor.selected_entity = NULL;
                        // pop our style items if we pushed them
                        if(flag){ // if we are selected, pop our style items
                            nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_vec2(ctx);
                        }
                        break;
                    }
                    YE_STATE.editor.selected_entity = current->entity;
                    entity_list_head = ye_get_entity_list_head();
                    // set all our current entity staging fields
                    staged_entity = *current->entity; // TODO this is hard because we have to copy all the components too... maybe we just need to let modification of fields directly and skip them if they are invalid
                    // pop our style items if we pushed them
                    if(flag){ // if we are selected, pop our style items
                        nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_vec2(ctx);
                    }
                    break;
                }

                // pop our style items if we pushed them
                if(flag){ // if we are selected, pop our style items
                    nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_vec2(ctx);
                }

                // push some pretty styles for red button!! (thank you nuklear forum!) :D
                nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(35,35,35))); nk_style_push_style_item(ctx, &ctx->style.button.hover, nk_style_item_color(nk_rgb(255,0,0))); nk_style_push_style_item(ctx, &ctx->style.button.active, nk_style_item_color(nk_rgb(255,0,0))); nk_style_push_vec2(ctx, &ctx->style.button.padding, nk_vec2(2,2));
                
                if(nk_button_symbol(ctx, NK_SYMBOL_X)){
                    // if our selected entity is the current entity, close the hiearchy
                    if(YE_STATE.editor.selected_entity == current->entity){
                        YE_STATE.editor.selected_entity = NULL;
                    }

                    ye_destroy_entity(current->entity);
                    entity_list_head = ye_get_entity_list_head();
                    nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_vec2(ctx);
                    break;
                }

                // pop off our cool red button colors
                nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_vec2(ctx);

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
    struct ye_entity *ent = YE_STATE.editor.selected_entity;
    if(ent == NULL){
        return;
    }
    if (nk_begin(ctx, "Entity", nk_rect(screenWidth/1.5, screenHeight / 2, screenWidth - screenWidth/1.5, screenHeight / 2),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            if(ent == NULL){
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label_colored(ctx, "No entity selected", NK_TEXT_CENTERED, nk_rgb(255, 255, 255));
            }
            else{
                nk_layout_row_dynamic(ctx, 25, 2);
                nk_label(ctx, "Name:", NK_TEXT_LEFT);
                // TODO: bugfix name editing, setting to zero len is unhappy, also should be 99 for str term?
                nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, ent->name, 100, nk_filter_default);

                nk_layout_row_dynamic(ctx, 25, 1);
                nk_checkbox_label(ctx, "Active", (nk_bool*)&ent->active);

                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Components:", NK_TEXT_LEFT);
                
                if(ent->transform != NULL){
                    if(nk_tree_push(ctx, NK_TREE_TAB, "Transform", NK_MAXIMIZED)){
                        nk_layout_row_dynamic(ctx, 25, 4);
                        nk_label(ctx, "X:", NK_TEXT_CENTERED);
                        nk_property_float(ctx, "#x", -1000000, &ent->transform->x, 1000000, 1, 5);
                        nk_label(ctx, "Y:", NK_TEXT_CENTERED);
                        nk_property_float(ctx, "#y", -1000000, &ent->transform->y, 1000000, 1, 5);
                        
                        nk_layout_row_dynamic(ctx, 25, 1);
                        if(nk_button_label(ctx, "Remove Component")){
                            ye_remove_transform_component(ent);
                        }

                        nk_tree_pop(ctx);
                    }
                }

                if(ent->renderer != NULL){
                    if(nk_tree_push(ctx, NK_TREE_TAB, "Renderer", NK_MAXIMIZED)){
                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_checkbox_label(ctx, "Active", (nk_bool*)&ent->renderer->active);
                        nk_checkbox_label(ctx, "Relative", (nk_bool*)&ent->renderer->relative);

                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_property_float(ctx, "#x", -1000000, &ent->renderer->rect.x, 1000000, 1, 5);
                        nk_property_float(ctx, "#y", -1000000, &ent->renderer->rect.y, 1000000, 1, 5);
                        nk_property_float(ctx, "#w", -1000000, &ent->renderer->rect.w, 1000000, 1, 5);
                        nk_property_float(ctx, "#h", -1000000, &ent->renderer->rect.h, 1000000, 1, 5);

                        nk_checkbox_label(ctx, "Flipped X", (nk_bool*)&ent->renderer->flipped_x);
                        nk_checkbox_label(ctx, "Flipped Y", (nk_bool*)&ent->renderer->flipped_y);

                        nk_layout_row_dynamic(ctx, 25, 2);
                        // nk_label(ctx, "Alignment:", NK_TEXT_LEFT); TODO
                        nk_property_int(ctx, "#z", -1000000, &ent->renderer->z, 1000000, 1, 5);
                        nk_property_float(ctx, "#Rotation", -1000000, &ent->renderer->rotation, 1000000, 1, 5);

                        if (nk_tree_push(ctx, NK_TREE_TAB, "Alignment", NK_MAXIMIZED))
                        {
                            int i;
                            static int alignment_arr[9] = {0,0,0, 0,0,0, 0,0,0};
                            for (int i = 0; i < 9; i++) {alignment_arr[i] = 0;}
                            // ^ zero the arr then we load its value from actual comp property

                            switch(YE_STATE.editor.selected_entity->renderer->alignment){
                                case 9:
                                    // stretch, nothing selected
                                    break;
                                default:
                                    alignment_arr[YE_STATE.editor.selected_entity->renderer->alignment] = 1;
                                    // printf("alignment: %d\n", YE_STATE.editor.selected_entity->renderer->alignment);
                                    break;
                            }

                            nk_layout_row_static(ctx, 50, 100, 3);
                            for (i = 0; i < 9; ++i) {
                                if (nk_selectable_label(ctx, "X", NK_TEXT_CENTERED, &alignment_arr[i])) {
                                    // // zero any other fields that might have been selected
                                    // for(int j = 0; j < 9; j++){
                                    //     if(j != i){
                                    //         alignment_arr[j] = 0;
                                    //     }
                                    // } TODO: removeme. leaving here because I think i might have to bugfix this soon
                                    
                                    // if the entire array is zero
                                    int sum = 0;
                                    for(int j = 0; j < 9; j++){
                                        sum += alignment_arr[j];
                                    }
                                    if(sum == 0){
                                        ent->renderer->alignment = YE_ALIGN_STRETCH;
                                    }
                                    else{
                                        ent->renderer->alignment = (enum ye_alignment)(i);
                                    }
                                    // printf("Selected alignment %d\n", i);
                                }
                            }
                            switch(ent->renderer->alignment){ // TODO: consolidate, this is yucky
                                case 0:
                                    nk_label(ctx, "Top Left", NK_TEXT_CENTERED);
                                    break;
                                case 1:
                                    nk_label(ctx, "Top Center", NK_TEXT_CENTERED);
                                    break;
                                case 2:
                                    nk_label(ctx, "Top Right", NK_TEXT_CENTERED);
                                    break;
                                case 3:
                                    nk_label(ctx, "Middle Left", NK_TEXT_CENTERED);
                                    break;
                                case 4:
                                    nk_label(ctx, "Middle Center", NK_TEXT_CENTERED);
                                    break;
                                case 5:
                                    nk_label(ctx, "Middle Right", NK_TEXT_CENTERED);
                                    break;
                                case 6:
                                    nk_label(ctx, "Bottom Left", NK_TEXT_CENTERED);
                                    break;
                                case 7:
                                    nk_label(ctx, "Bottom Center", NK_TEXT_CENTERED);
                                    break;
                                case 8:
                                    nk_label(ctx, "Bottom Right", NK_TEXT_CENTERED);
                                    break;
                                case 9:
                                    nk_label(ctx, "Stretch", NK_TEXT_CENTERED);
                                    break;
                                default:
                                    nk_label(ctx, "!!!Corrupted!!!", NK_TEXT_CENTERED);
                                    break;
                            }
                            nk_tree_pop(ctx);
                        }

                        nk_layout_row_dynamic(ctx, 25, 1);
                        if(nk_button_label(ctx, "Remove Component")){
                            ye_remove_renderer_component(ent);
                        }

                        nk_tree_pop(ctx);
                    }
                }

                if(ent->physics != NULL){
                    if(nk_tree_push(ctx, NK_TREE_TAB, "Phyiscs", NK_MAXIMIZED)){
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Active", (nk_bool*)&ent->physics->active);
                        nk_layout_row_dynamic(ctx, 25, 4);
                        nk_label(ctx, "X Velocity:", NK_TEXT_CENTERED);
                        nk_property_float(ctx, "#xv", -1000000, &ent->physics->velocity.x, 1000000, 1, 5);
                        nk_label(ctx, "Y Velocity:", NK_TEXT_CENTERED);
                        nk_property_float(ctx, "#yv", -1000000, &ent->physics->velocity.y, 1000000, 1, 5);
                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_label(ctx, "Rotational Velocity:", NK_TEXT_CENTERED);
                        nk_property_float(ctx, "#rv", -1000000, &ent->physics->rotational_velocity, 1000000, 1, 5);
                        
                        nk_layout_row_dynamic(ctx, 25, 1);
                        if(nk_button_label(ctx, "Remove Component")){
                            ye_remove_physics_component(ent);
                        }
                        
                        nk_tree_pop(ctx);
                    }
                }

                if(ent->collider != NULL){
                    if(nk_tree_push(ctx, NK_TREE_TAB, "Collider", NK_MAXIMIZED)){
                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_checkbox_label(ctx, "Active", (nk_bool*)&ent->collider->active);
                        nk_checkbox_label(ctx, "Relative", (nk_bool*)&ent->collider->relative);
                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_property_float(ctx, "#x", -1000000, &ent->collider->rect.x, 1000000, 1, 5);
                        nk_property_float(ctx, "#y", -1000000, &ent->collider->rect.y, 1000000, 1, 5);
                        nk_property_float(ctx, "#w", -1000000, &ent->collider->rect.w, 1000000, 1, 5);
                        nk_property_float(ctx, "#h", -1000000, &ent->collider->rect.h, 1000000, 1, 5);
                        
                        nk_layout_row_dynamic(ctx, 25, 1);
                        if(nk_button_label(ctx, "Remove Component")){
                            ye_remove_collider_component(ent);
                        }
                        
                        nk_tree_pop(ctx);
                    }
                }

                if(ent->tag != NULL){
                    if(nk_tree_push(ctx, NK_TREE_TAB, "Tag", NK_MAXIMIZED)){
                        // tag components can hold 10 buffers (TODO: sync this somehow with the #define in engine) so we want to just show them all as editable text boxes
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_label(ctx, "Tag Buffers:", NK_TEXT_LEFT);
                        for(int i = 0; i < 10;){
                            nk_layout_row_dynamic(ctx, 25, 2);
                            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, ent->tag->tags[i], 20, nk_filter_default); i++;
                            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, ent->tag->tags[i], 20, nk_filter_default); i++;
                        }

                        nk_layout_row_dynamic(ctx, 25, 1);
                        if(nk_button_label(ctx, "Remove Component")){
                            ye_remove_tag_component(ent);
                        }

                        nk_tree_pop(ctx);
                    }
                }

                if(ent->camera != NULL){
                    if(nk_tree_push(ctx, NK_TREE_TAB, "Camera", NK_MAXIMIZED)){
                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_checkbox_label(ctx, "Active", (nk_bool*)&ent->camera->active);
                        nk_checkbox_label(ctx, "Relative", (nk_bool*)&ent->camera->relative);
                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_property_int(ctx, "#x", -1000000, &ent->camera->view_field.x, 1000000, 1, 5);
                        nk_property_int(ctx, "#y", -1000000, &ent->camera->view_field.y, 1000000, 1, 5);
                        nk_property_int(ctx, "#w", -1000000, &ent->camera->view_field.w, 1000000, 1, 5);
                        nk_property_int(ctx, "#h", -1000000, &ent->camera->view_field.h, 1000000, 1, 5);
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_property_int(ctx, "#z", -1000000, &ent->camera->z, 1000000, 1, 5);
                        
                        nk_layout_row_dynamic(ctx, 25, 1);
                        if(nk_button_label(ctx, "Remove Component")){
                            ye_remove_camera_component(ent);
                        }
                        
                        nk_tree_pop(ctx);
                    }
                }
            }
        nk_end(ctx);
    }
}


/*
    Paint some info on the current scene
*/
void ye_editor_paint_info_overlay(struct nk_context *ctx){
    if (nk_begin(ctx, "Info", nk_rect(0, 40, 200, 200),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Mouse World Pos:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            char buf[32];
            sprintf(buf, "%d,%d", mouse_world_x, mouse_world_y);
            nk_label(ctx, buf, NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Scene Name:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, YE_STATE.runtime.scene_name, NK_TEXT_LEFT);
        nk_end(ctx);
    }
}

/*
    TODO:
    - paintbounds somehow starts out checked even though its not, so have to double click to turn on
    - if console is opened when we tick one of these, closing the console crashes
*/
bool show_info_overlay = false;
void ye_editor_paint_options(struct nk_context *ctx){
    if (nk_begin(ctx, "Options", nk_rect(screenWidth/1.5 / 2, 40 + screenHeight/1.5, screenWidth - screenWidth/1.5, (screenHeight - screenHeight/1.5) - 40),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            
            nk_label(ctx, "Overlays:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_checkbox_label(ctx, "Info", (nk_bool*)&show_info_overlay)){
                if(show_info_overlay){
                    ui_register_component("info_overlay",ye_editor_paint_info_overlay);
                }
                else{
                    remove_ui_component("info_overlay");
                }
            }

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Visual Debugging:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_checkbox_label(ctx, "Display Names", (nk_bool*)&YE_STATE.editor.display_names);
            nk_checkbox_label(ctx, "Paintbounds", (nk_bool*)&YE_STATE.editor.paintbounds_visible);
            nk_checkbox_label(ctx, "Colliders", (nk_bool*)&YE_STATE.editor.colliders_visible);
            nk_checkbox_label(ctx, "Scene Camera Viewport", (nk_bool*)&YE_STATE.editor.scene_camera_bounds_visible);

            nk_label(ctx, "Preferences:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_checkbox_label(ctx, "Draw Lines", (nk_bool*)&YE_STATE.editor.editor_display_viewport_lines);

            nk_label(ctx, "Extra:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_checkbox_label(ctx, "Stretch Viewport", (nk_bool*)&YE_STATE.engine.stretch_viewport);
            nk_checkbox_label(ctx, "Lock Viewport", (nk_bool*)&lock_viewport_interaction);
        nk_end(ctx);
    }
}

/*
    Editor settings window
*/
int editor_ui_color_selection = 0; // TODO: this should default have selected what we loaded from json
void ye_editor_paint_editor_settings(struct nk_context *ctx){
    if (nk_begin(ctx, "Editor Settings", nk_rect(screenWidth/2 - 250, screenHeight/2 - 100, 500,200),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE)) {
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label(ctx, "Yoyo Editor Settings", NK_TEXT_CENTERED);
        nk_label(ctx, "", NK_TEXT_CENTERED);
        nk_layout_row_dynamic(ctx, 25, 2);
        nk_label(ctx, "UI Color Theme:", NK_TEXT_CENTERED);

        // combo box with color theme choices
        static const char *color_schemes[] = {"black", "dark", "blue", "red", "white", "amoled"};
        nk_combobox(ctx, color_schemes, NK_LEN(color_schemes), &editor_ui_color_selection, 25, nk_vec2(200,200));

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label(ctx, "", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 25, 2);
        if(nk_button_label(ctx, "Cancel")){
            remove_ui_component("editor_settings");
            lock_viewport_interaction = !lock_viewport_interaction;
        }
        if(nk_button_label(ctx, "Apply")){

            /*
                Load the editor settings file, and change "preferences":"theme" to the name of the scheme
            */
            json_t *settings = ye_json_read(editor_settings_path);
            json_object_set_new(json_object_get(settings, "preferences"), "theme", json_string(color_schemes[editor_ui_color_selection]));
            ye_json_write(editor_settings_path, settings);
            json_decref(settings);

            if(strcmp(color_schemes[editor_ui_color_selection], "dark") == 0)
                set_style(YE_STATE.engine.ctx, THEME_DARK);
            else if(strcmp(color_schemes[editor_ui_color_selection], "red") == 0)
                set_style(YE_STATE.engine.ctx, THEME_RED);
            else if(strcmp(color_schemes[editor_ui_color_selection], "black") == 0)
                set_style(YE_STATE.engine.ctx, THEME_BLACK);
            else if(strcmp(color_schemes[editor_ui_color_selection], "white") == 0)
                set_style(YE_STATE.engine.ctx, THEME_WHITE);
            else if(strcmp(color_schemes[editor_ui_color_selection], "blue") == 0)
                set_style(YE_STATE.engine.ctx, THEME_BLUE);
            else if(strcmp(color_schemes[editor_ui_color_selection], "amoled") == 0)
                set_style(YE_STATE.engine.ctx, THEME_AMOLED);
            remove_ui_component("editor_settings");
            lock_viewport_interaction = !lock_viewport_interaction;
        }
        nk_end(ctx);
    }
}

/*
    Editor top menu bar

    This function is actually so yucky and should be greatly refactored but tbh this editor code
    should never be seen by another living human being so im ok with living with it.

    TODO: locking viewport should be handled better. It would be nice if when any submenu item is 
    dropped down clicking on it does not select an entity (since its over the viewport)
*/
bool new_scene_popup_open = false;
bool scene_deletion_popup_open = false;
char new_scene_name[256];
void ye_editor_paint_menu(struct nk_context *ctx){
    if (nk_begin(ctx, "Menu", nk_rect(0, 0, screenWidth / 1.5, 35), 0)) {
        
        /*
            Popup for getting information to create new scenes
        */
        if(new_scene_popup_open){
            struct nk_rect s = { 0, 0, 400, 300 };
            if (nk_popup_begin(ctx, NK_POPUP_STATIC, "About", NK_WINDOW_MOVABLE, s)) {
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Scene Name", NK_TEXT_CENTERED);
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, new_scene_name, 256, nk_filter_default);
                nk_layout_row_dynamic(ctx, 25, 2);
                if(nk_button_label(ctx, "Abort")){
                    new_scene_popup_open = false;
                    lock_viewport_interaction = !lock_viewport_interaction;
                }
                if(nk_button_label(ctx, "Create")){
                    
                    /*
                        Build the new scene file and write it to disk
                    */

                    json_t *new_scene = json_object();
                    json_object_set_new(new_scene, "name", json_string(new_scene_name));
                    json_object_set_new(new_scene, "version", json_integer(0)); // TODO: this should be a macro
                    json_object_set_new(new_scene, "styles", json_array());
                    json_object_set_new(new_scene, "prefabs", json_array());

                    json_t *scene = json_object();
                    json_object_set_new(scene, "default camera", json_string("camera"));
                    json_object_set_new(scene, "entities", json_array());
                    json_object_set_new(new_scene, "scene", scene);

                    // In entities, let's make the "camera" default camera
                    json_t *camera = json_object();
                    json_object_set_new(camera, "name", json_string("camera"));
                    json_object_set_new(camera, "active", json_true());

                    json_t *components = json_object();
                    json_object_set_new(components, "transform", json_object());
                    json_object_set_new(components, "camera", json_object());

                    json_object_set_new(json_object_get(components, "transform"), "x", json_integer(0));
                    json_object_set_new(json_object_get(components, "transform"), "y", json_integer(0));

                    json_object_set_new(json_object_get(components, "camera"), "active", json_true());
                    json_object_set_new(json_object_get(components, "camera"), "z", json_integer(999));

                    json_t *view_field = json_object();
                    json_object_set_new(view_field, "w", json_integer(1920));
                    json_object_set_new(view_field, "h", json_integer(1080));
                    json_object_set_new(json_object_get(components, "camera"), "view field", view_field);

                    json_object_set_new(camera, "components", components);
                    json_array_append_new(json_object_get(scene, "entities"), camera);

                    // get the path of our new scene, it will be scenes/NAME.yoyo
                    char new_scene_path[256 + 12];
                    snprintf(new_scene_path, sizeof(new_scene_path), "scenes/%s.yoyo", new_scene_name);

                    // check if this scene already exists
                    if(access(ye_get_resource_static(new_scene_path), F_OK) != -1){
                        // file exists
                        ye_logf(error, "Scene already exists.\n");
                    }
                    else{
                        ye_json_write(ye_get_resource_static(new_scene_path), new_scene);
                        new_scene_popup_open = false;
                        ye_load_scene(ye_get_resource_static(new_scene_path));
                    }
                    
                    // cleanup
                    json_decref(new_scene); // TODO: check for memroy leak, need to free camera?
                    lock_viewport_interaction = !lock_viewport_interaction;
                }
                nk_popup_end(ctx);
            }
        }
        
        /*
            Popup for confirming scene deletion
        */
        if(scene_deletion_popup_open){
            struct nk_rect s = { 0, 0, 450, 150 };
            if (nk_popup_begin(ctx, NK_POPUP_STATIC, "About", NK_WINDOW_MOVABLE, s)) {
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Are you sure you want to delete this scene?", NK_TEXT_CENTERED);
                nk_label(ctx, "This action is irreversible.", NK_TEXT_CENTERED);
                nk_label(ctx, "", NK_TEXT_CENTERED);
                nk_layout_row_dynamic(ctx, 25, 2);
                if(nk_button_label(ctx, "No")){
                    scene_deletion_popup_open = false;
                    lock_viewport_interaction = !lock_viewport_interaction;
                }
                if(nk_button_label(ctx, "Yes")){

                    // delete the scene file
                    char scene_path[256 + 12];
                    snprintf(scene_path, sizeof(scene_path), "scenes/%s.yoyo", YE_STATE.runtime.scene_name);

                    if(remove(ye_get_resource_static(scene_path)) == 0){
                        ye_logf(info, "Deleted scene %s\n", YE_STATE.runtime.scene_name);
                    }
                    else{
                        ye_logf(error, "Failed to delete scene %s\n", YE_STATE.runtime.scene_name);
                    }

                    scene_deletion_popup_open = false;
                    lock_viewport_interaction = !lock_viewport_interaction;
                }
                nk_popup_end(ctx);
            }
        }

        nk_menubar_begin(ctx);
        nk_layout_row_begin(ctx, NK_STATIC, 25, 5);
        nk_layout_row_push(ctx, 45);
        if (nk_menu_begin_label(ctx, "File", NK_TEXT_LEFT, nk_vec2(120, 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT)) {    
                // get the path to this scene file TODO: THIS NEEDS TO BE BASED ON THE SCENE FILE PATH IN SCENE.c EVENTUALLY!!!! RIGHT NOW THIS ASSUMES THE NAME IS THE SAME AS THE PATH
                char scene_path[256 + 12];
                snprintf(scene_path, sizeof(scene_path), "scenes/%s.yoyo", YE_STATE.runtime.scene_name);

                // load the scene file into a json_t
                json_t *scene = ye_json_read(ye_get_resource_static(scene_path));

                // create a json_t array listing all entities in the scene
                json_t *entities = json_array();
                for(int i = 0; i < YE_STATE.runtime.entity_count; i++){
                    json_array_append_new(entities, json_string("meow!"));
                }

                // update the scene file with the new entity list
                json_object_set_new(json_object_get(scene, "scene"), "entities", entities);

                ye_json_log(scene); //TODO: figure out how we update the name version styles and prefabs

                // write the scene file
                // ye_json_write(ye_get_resource_static(scene_path), YE_STATE.runtime.scene);                      
            }
            nk_menu_end(ctx);

            /*
                TODO:
                build, build and run, build and debug, build and run and debug
            */
        }
        nk_layout_row_push(ctx, 55);
        if (nk_menu_begin_label(ctx, "Scene", NK_TEXT_LEFT, nk_vec2(200, 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            
            if (nk_menu_item_label(ctx, "Open Scene", NK_TEXT_LEFT)) { // TODO: save prompt if unsaved
                printf("Open\n"); // TODO
            }

            if (nk_menu_item_label(ctx, "New Scene", NK_TEXT_LEFT)) {
                    new_scene_popup_open = !new_scene_popup_open;
                    lock_viewport_interaction = !lock_viewport_interaction;
                    // reset fields
                    strcpy(new_scene_name, "");
            }
            
            if (nk_menu_item_label(ctx, "Delete Current Scene", NK_TEXT_LEFT)) {
                scene_deletion_popup_open = !scene_deletion_popup_open;
                lock_viewport_interaction = !lock_viewport_interaction;
            }
            nk_menu_end(ctx);
        }
        nk_layout_row_push(ctx, 85);
        if (nk_menu_begin_label(ctx, "Settings", NK_TEXT_LEFT, nk_vec2(160, 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_menu_item_label(ctx, "Editor Settings", NK_TEXT_LEFT)){
                if(!ui_component_exists("editor_settings")){
                    ui_register_component("editor_settings", ye_editor_paint_editor_settings);
                }
                else{
                    remove_ui_component("editor_settings");
                }
                lock_viewport_interaction = !lock_viewport_interaction;
            }

            nk_menu_end(ctx);
        }
        nk_layout_row_push(ctx, 45);
        if (nk_menu_begin_label(ctx, "Help", NK_TEXT_LEFT, nk_vec2(200, 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_menu_item_label(ctx, "Shortcuts", NK_TEXT_LEFT)) {
                if(!ui_component_exists("editor keybinds")){
                    ui_register_component("editor keybinds", editor_panel_keybinds);
                    lock_viewport();
                } else {
                    remove_ui_component("editor keybinds");
                    unlock_viewport();
                }
            }
            if (nk_menu_item_label(ctx, "Documentation", NK_TEXT_LEFT)) {
                #ifdef _WIN32
                    system("start https://github.com/yoyolick/yoyoengine") 
                #else
                    system("xdg-open https://github.com/yoyolick/yoyoengine");
                #endif
            }
            if(nk_menu_item_label(ctx, "Credits", NK_TEXT_LEFT)) {
                if(!ui_component_exists("credits")){
                    ui_register_component("editor_credits", editor_panel_credits);
                    lock_viewport();
                } else {
                    remove_ui_component("editor_credits");
                    unlock_viewport();
                }
            }
            nk_menu_end(ctx);
        }
        nk_layout_row_push(ctx, 45);
        if (nk_menu_begin_label(ctx, "Quit", NK_TEXT_LEFT, nk_vec2(200, 200))) {
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_menu_item_label(ctx, "Exit (without saving)", NK_TEXT_LEFT)) { // TODO: save prompt if unsaved
                quit = true;
            }
            nk_menu_end(ctx);
        }
        nk_menubar_end(ctx);
        nk_end(ctx);
    }
}