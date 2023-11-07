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

#include "editor.h"
#include "editor_ui.h"
#include <yoyoengine/yoyoengine.h>
#include <unistd.h>

/*
    INITIALIZE VARIABLES FOR JUST THIS FILE
*/
bool project_settings_open = false;
json_t *BUILD_FILE;
/*
    VARIABLES INITIALIZED
*/

/*
    Popout panel for the project settings
*/
char project_name[256];
char project_entry_scene[256];
int project_log_level; // 0-4 (debug, info, warning, error, none)
int project_volume; // 0-128
bool project_debug_mode; // true or false
int project_screen_size_UNPROCESSED; // convert 0: 1920x1080, 1: 2560x1440
int project_window_mode_UNPROCESSED; // 0-2 (windowed, fullscreen, borderless) -> 1, 0, SDL_WINDOW_FULLSCREEN_DESKTOP
// we need to process the window mode, 3 should become SDL_WINDOW_FULLSCREEN_DESKTOP
int project_framecap; // -1 for vsync, else 0-MAXINT
char _project_framecap_label[10];
char project_window_title[256];
bool project_stretch_viewport; // true or false

/*
    Build settings variables
*/
char build_additional_cflags[128];
char build_platform[32];
int build_platform_int;
char build_engine_build_path[256];

/*
    Helper functions
*/
void editor_reload_build_file(){
    json_decref(BUILD_FILE);
    BUILD_FILE = ye_json_read(ye_get_resource_static("../build.yoyo"));
}

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
            nk_checkbox_label(ctx, "Debug Mode", (nk_bool*)&project_debug_mode);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "If checked, the game will run in debug mode, which will allow you to use the console and other debug features.");

            /*
                Checkbox for stretch viewport
            */
            bounds = nk_widget_bounds(ctx);
            nk_checkbox_label(ctx, "Stretch Viewport", (nk_bool*)&project_stretch_viewport);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "If checked, the viewport will stretch to the size of the window, if unchecked, the viewport will render from its actual perspective.");

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label_colored(ctx, "Build Settings:", NK_TEXT_LEFT, nk_rgb(255, 255, 255));

            /*
                Platform (dropdown)
            */
            nk_layout_row_dynamic(ctx, 25, 2);
            bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "Platform:", NK_TEXT_LEFT);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "The platform you are building for.");
            static const char *platforms[] = {"linux", "windows"};
            nk_combobox(ctx, platforms, NK_LEN(platforms), &build_platform_int, 25, nk_vec2(200,200));

            /*
                Engine Build Path (string input)
            */
            nk_layout_row_dynamic(ctx, 25, 2);
            bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "Engine Build Path:", NK_TEXT_LEFT);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "The path to the engine build you want to use.");
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, build_engine_build_path, 256, nk_filter_default);

            /*
                Additional C Flags (string input)
            */
            nk_layout_row_dynamic(ctx, 25, 2);
            bounds = nk_widget_bounds(ctx);
            nk_label(ctx, "Additional C Flags:", NK_TEXT_LEFT);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, "Additional C flags to pass to the compiler.");
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, build_additional_cflags, 256, nk_filter_default);

            nk_layout_row_dynamic(ctx, 25, 1);

            // close button
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_button_label(ctx, "Save and Close")){
                project_settings_open = false;
                lock_viewport_interaction = false;

                // update all the keys in SETTINGS
                json_object_set_new(SETTINGS, "name", json_string(project_name));
                json_object_set_new(SETTINGS, "entry_scene", json_string(project_entry_scene));
                json_object_set_new(SETTINGS, "window_title", json_string(project_window_title));
                json_object_set_new(SETTINGS, "log_level", json_integer(project_log_level));
                json_object_set_new(SETTINGS, "volume", json_integer(project_volume));
                json_object_set_new(SETTINGS, "debug_mode", project_debug_mode ? json_true() : json_false());
                json_object_set_new(SETTINGS, "screen_width", project_screen_size_UNPROCESSED == 0 ? json_integer(1920) : json_integer(2560));
                json_object_set_new(SETTINGS, "screen_height", project_screen_size_UNPROCESSED == 0 ? json_integer(1080) : json_integer(1440));
                if (project_window_mode_UNPROCESSED == 2) {
                    json_object_set_new(SETTINGS, "window_mode", json_integer(SDL_WINDOW_FULLSCREEN_DESKTOP));
                } else {
                    json_object_set_new(SETTINGS, "window_mode", project_window_mode_UNPROCESSED == 0 ? json_integer(0) : json_integer(1));
                }
                json_object_set_new(SETTINGS, "framecap", json_integer(project_framecap));
                json_object_set_new(SETTINGS, "stretch_viewport", project_stretch_viewport ? json_true() : json_false());
                // save the settings file
                ye_json_write(ye_get_resource_static("../settings.yoyo"),SETTINGS);
                
                // update build keys, then save
                json_object_set_new(BUILD_FILE, "cflags", json_string(build_additional_cflags));
                json_object_set_new(BUILD_FILE, "platform", json_string(build_platform_int == 0 ? "linux" : "windows"));
                json_object_set_new(BUILD_FILE, "engine_build_path", json_string(build_engine_build_path));
                ye_json_write(ye_get_resource_static("../build.yoyo"),BUILD_FILE);

                /*
                    This should be done in a modular way, engine should be rebuilt smartly also, we will want button for it later in dev when engine changes all the time
                    TODO TODO TODO TODO TODO TODO
                */

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
    if (nk_begin(ctx, "Project", nk_rect(0, 40 + screenHeight/1.5, screenWidth/1.5 / 2, screenHeight - (screenHeight/1.5) - 40),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
            // TODO: build buttons with icons
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label_colored(ctx, "Build Options:", NK_TEXT_LEFT, nk_rgb(255, 255, 255));
            nk_layout_row_dynamic(ctx, 25, 2);
            if(nk_button_label(ctx, "Build")){
                editor_build();
            }
            if(nk_button_label(ctx, "Build and Run")){
                editor_build_and_run();
            }

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label_colored(ctx, "Additional Actions:", NK_TEXT_LEFT, nk_rgb(255, 255, 255));
            // button to open project settings
            if(nk_button_label(ctx, "Project Settings")){
                if(project_settings_open){
                    project_settings_open = false;
                    lock_viewport_interaction = false;
                    remove_ui_component("project settings");
                }
                else{
                    editor_reload_settings();
                    editor_reload_build_file();
                    project_settings_open = true;
                    lock_viewport_interaction = true;
                    ui_register_component("project settings",ye_editor_paint_project_settings);
                    // ye_json_log(SETTINGS);
                    // setup a bunch of stack variables for showing current settings
                    
                    /*
                        Project Name
                    */
                    const char * tmp_project_name; 
                    if(!ye_json_string(SETTINGS, "name", &tmp_project_name)){
                        strcpy((char*)tmp_project_name, "Yoyo Engine Game");
                    }
                    strncpy(project_name, (char*)tmp_project_name, (size_t)sizeof(project_name) - 1);
                    project_name[(size_t)sizeof(project_name) - 1] = '\0'; // null terminate just in case TODO: write helper?


                    /*
                        Entry Scene
                    */
                    const char * tmp_project_entry_scene;
                    if(!ye_json_string(SETTINGS,"entry_scene", &tmp_project_entry_scene)){
                        strcpy((char*)tmp_project_entry_scene, "scenes/entry.yoyo");
                    }
                    strncpy(project_entry_scene, (char*)tmp_project_entry_scene, (size_t)sizeof(project_entry_scene) - 1);
                    project_entry_scene[(size_t)sizeof(project_entry_scene) - 1] = '\0'; // null terminate just in case TODO: write helper?

                    /*
                        Window Title
                    */
                    const char * tmp_project_window_title;
                    if(!ye_json_string(SETTINGS, "window_title", &tmp_project_window_title)){
                        strcpy((char*)tmp_project_window_title, "Yoyo Engine Game");
                    }
                    strncpy(project_window_title, (char*)tmp_project_window_title, (size_t)sizeof(project_window_title) - 1);
                    project_window_title[(size_t)sizeof(project_window_title) - 1] = '\0'; // null terminate just in case TODO: write helper?

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

                    /*
                        Open our build file
                    */
                    BUILD_FILE = ye_json_read(ye_get_resource_static("../build.yoyo"));
                    if(BUILD_FILE){
                        /*
                            Additional C Flags
                        */
                        const char * tmp_build_additional_cflags;
                        if(!ye_json_string(BUILD_FILE, "cflags", &tmp_build_additional_cflags)){
                            strcpy((char*)tmp_build_additional_cflags,"");
                        }
                        strncpy(build_additional_cflags, (char*)tmp_build_additional_cflags, (size_t)sizeof(build_additional_cflags) - 1);
                        build_additional_cflags[(size_t)sizeof(build_additional_cflags) - 1] = '\0'; // null terminate just in case TODO: write helper?

                        /*
                            Platform
                        */
                        const char * tmp_build_platform;
                        if(!ye_json_string(BUILD_FILE, "platform", &tmp_build_platform)){
                            strcpy((char*)tmp_build_platform,"linux");
                            build_platform_int = 0;
                        }
                        else{
                            if(strcmp(tmp_build_platform, "windows") == 0){
                                build_platform_int = 1;
                            }
                            else{
                                build_platform_int = 0;
                            }
                        }
                        strncpy(build_platform, (char*)tmp_build_platform, (size_t)sizeof(build_platform) - 1);
                        build_platform[(size_t)sizeof(build_platform) - 1] = '\0'; // null terminate just in case TODO: write helper?

                        /*
                            Engine Build Path
                        */
                        const char * tmp_build_engine_build_path;
                        if(!ye_json_string(BUILD_FILE, "engine_build_path", &tmp_build_engine_build_path)){
                            strcpy((char*)tmp_build_engine_build_path,"");
                        }
                        strncpy(build_engine_build_path, (char*)tmp_build_engine_build_path, (size_t)sizeof(build_engine_build_path) - 1);
                        build_engine_build_path[(size_t)sizeof(build_engine_build_path) - 1] = '\0'; // null terminate just in case TODO: write helper?
                    }
                    else{
                        ye_logf(error, "build.yoyo not found.");
                        strcpy((char*)build_additional_cflags,"");
                        strcpy((char*)build_platform,"linux");
                        build_platform_int = 0;
                        strcpy((char*)build_engine_build_path,"");
                    }
                }
            }
            if(nk_button_label(ctx, "Browse Project Files")){
                char command[256];
                snprintf(command, sizeof(command), "xdg-open \"%s\"", project_path);

                // Execute the command.
                system(command);
            }
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label_colored(ctx, "Copyright (c) Ryan Zmuda 2023", NK_TEXT_CENTERED, nk_rgb(255, 255, 255));
        nk_end(ctx);
    }
}

void editor_settings_ui_init(){
    /*
        For lack of a better memory management solution... im tempted to just make all the fields rely on internal jansson strings.. but then what about others proceduarl during runtime?? arghh
    */

    /*
        dumbass, you could just have all of these malloced and then strcpy a temp var extracted from json to them.
    */
}

void editor_settings_ui_shutdown(){
    json_decref(BUILD_FILE);
}