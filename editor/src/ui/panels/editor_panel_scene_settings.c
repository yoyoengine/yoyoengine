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

#include "editor.h"
#include "editor_panels.h"

// TODO: move me to utils for editor and use everywhere
struct nk_rect editor_panel_bounds_centered(int w, int h){
    return nk_rect((screenWidth / 2) - (w / 2), (screenHeight / 2) - (h / 2), w, h);
}

// idk about this macro
#define epbs editor_panel_bounds_centered

json_t * SCENE = NULL;

char * scene_name = NULL;
int scene_version = 0;

char scene_music_path[256];
bool scene_music_loop = true;
float scene_music_volume = 1.0f;

json_t * open_scene_data(const char *path){
    json_t *scene = ye_json_read(ye_path_resources(path));
    if(!scene){
        ye_logf(error,"Failed to load scene data from %s\n", path);
        return NULL;
    }
    return scene;
}

void editor_panel_scene_settings(struct nk_context *ctx){
    if(nk_begin(ctx, "Scene Settings", epbs(500,500), NK_WINDOW_BORDER|NK_WINDOW_TITLE|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE)){
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Scene Settings", NK_TEXT_CENTERED);

        if(SCENE == NULL){
            SCENE = open_scene_data(YE_STATE.runtime.scene_file_path);
            
            /*
                Scene meta
            */
            // ye_json_string(SCENE, "name", &scene_name);
            ye_json_int(SCENE, "version", &scene_version);
            
            /*
                Music
            */
            json_t * _scene; ye_json_object(SCENE, "scene", &_scene);
            json_t * music; ye_json_object(_scene, "music", &music);
            char * mpath = NULL; ye_json_string(music, "src", &mpath);
            if(mpath != NULL)
                strncpy(scene_music_path, mpath, 256);
            else
                strcpy(scene_music_path, "\0");

            bool mloop = false; ye_json_bool(music, "loop", &mloop);            
            scene_music_loop = mloop;

            float mvol = 1.0f; ye_json_float(music, "volume", &mvol);
            scene_music_volume = mvol;
        }

        /*
            Print some info on the currently opened scene
        */
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Scene Info", NK_TEXT_CENTERED);
        
        nk_layout_row_dynamic(ctx, 30, 2);
        nk_label(ctx, "Scene Name: ", NK_TEXT_LEFT);
        nk_label(ctx, YE_STATE.runtime.scene_name, NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 30, 2);
        nk_label(ctx, "Scene Version: ", NK_TEXT_LEFT);
        char version_str[10];
        sprintf(version_str, "%d", scene_version);
        nk_label(ctx, version_str, NK_TEXT_LEFT);
        

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label_colored(ctx, "TODO: prefab and supplemental styles", NK_TEXT_LEFT, nk_rgb(255,255,0));

        /*
            Music
        */
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Scene Music Path:", NK_TEXT_CENTERED);
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, scene_music_path, 256, nk_filter_default);
        nk_layout_row_dynamic(ctx, 30, 2);
        nk_checkbox_label(ctx, "Loop", &scene_music_loop);
        nk_property_float(ctx, "Volume", 0.0f, &scene_music_volume, 1.0f, 0.1f, 0.1f); 

        /*
            Save or Exit
        */
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_layout_row_dynamic(ctx, 30, 2);
        if(nk_button_label(ctx, "Cancel")){
            remove_ui_component("scene_settings");
            unlock_viewport();
            json_decref(SCENE);
        }
        if(nk_button_label(ctx, "Save")){
            /*
                update the keys
                {
                    "scene":{
                        "music":{
                            "src":NEW_VALUE,
                            "loop":NEW_VALUE,
                            "volume":NEW_VALUE
                        }
                    }
                }
            */
            json_t * _scene; ye_json_object(SCENE, "scene", &_scene);
            json_t * music; ye_json_object(_scene, "music", &music);
            json_object_set_new(music, "src", json_string(scene_music_path));
            json_object_set_new(music, "loop", json_boolean(scene_music_loop));
            json_object_set_new(music, "volume", json_real(scene_music_volume));

            // write to file
            ye_json_write(ye_path_resources(YE_STATE.runtime.scene_file_path), SCENE);

            remove_ui_component("scene_settings");
            unlock_viewport();
            json_decref(SCENE);
            SCENE = NULL;
        }

        nk_end(ctx);
    }
}