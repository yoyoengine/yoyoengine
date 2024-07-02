/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    This is honestly crazy scuff and stupid. If it works i guess it works.

    At this point we have the individual trick meta files which is fine, but the build
    script computes its own meta for being in the build folder for the runtime to read,
    but we also have this one computed by the editor to show on screen the tricks. idk
    this feels wrong but maybe its not awful.
*/

#include "editor.h"
#include "editor_utils.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

json_t *tricks_meta = NULL;

/*
    This is difficult to explain. Basically, ye_path("tricks") is the
    path to a folder which contains n number of subdirectories, each of which contains a trick.yoyo file.

    Our goal is to traverse all n subdirectories and concatenate the contents of each trick.yoyo file into an array
    called tricks in the tricks_meta json object.
*/
void _load_tricks_meta(char *path, char *parent_folder){

    #if defined(__linux__)
        DIR *dir;
        struct dirent *entry;

        if (!(dir = opendir(path)))
            return;

        while ((entry = readdir(dir)) != NULL) {
            char *entry_name = entry->d_name;
            char full_path[1024];

            // Skip the current directory and the directory above
            if (strcmp(entry_name, ".") == 0 || strcmp(entry_name, "..") == 0)
                continue;

            // Construct the full path
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry_name);

            struct stat path_stat;
            stat(full_path, &path_stat);

            // If the entry is a directory, recursively call this function
            if (S_ISDIR(path_stat.st_mode)) {
                _load_tricks_meta(full_path, entry_name);
            } 
            else {
                // If the entry is a file and its name is trick.yoyo, read the file
                if (strcmp(entry_name, "trick.yoyo") == 0) {
                    ye_logf(debug, "Found trick.yoyo file at %s\n", full_path);

                    json_t *trick = ye_json_read(full_path);

                    ye_json_log(trick);

                    // add a key into trick called folder that has the parent folder name
                    json_object_set_new(trick, "folder", json_string(parent_folder));

                    // copy the trick into the tricks array
                    json_array_append_new(json_object_get(tricks_meta, "tricks"), trick);
                }
            }
        }

        closedir(dir);
    #else
        ye_logf(error, "THE EDITOR IS NOT SUPPORTED ON WINDOWS!\n");
        return;
    #endif
}

void _re_cache_tricks_meta(){
    // remove the tricks.yoyo file
    remove(ye_path("tricks/tricks_editor_cache.yoyo"));
    ye_logf(debug, "Removed cached tricks_editor_cache.yoyo file\n");

    // cache for later
    ye_json_write(ye_path("tricks/tricks_editor_cache.yoyo"), tricks_meta);
    ye_logf(debug,"Created tricks meta: '%s'\n", ye_path("tricks/tricks_editor_cache.yoyo"));
}

void load_tricks_meta(const char *path){
    // check if tricks/tricks_editor_cache.yoyo exist, if else build it
    if (access(path, F_OK) != -1) {
        ye_logf(debug,"Found tricks meta: '%s'\n", path);
        tricks_meta = ye_json_read(path);
    } else {
        ye_logf(warning,"Tricks meta nonexistant. Will create.\n");

        tricks_meta = json_object();
        // add a "tricks" key with empty array
        json_object_set_new(tricks_meta, "tricks", json_array());
        _load_tricks_meta(ye_path("tricks"),"tricks_defualt_name");

        // cache for later
        ye_json_write(ye_path("tricks/tricks_editor_cache.yoyo"), tricks_meta);
        ye_logf(debug,"Created tricks meta: '%s'\n", ye_path("tricks/tricks_editor_cache.yoyo"));
    }
}


bool popup_open = false;
void editor_panel_tricks(struct nk_context *ctx){
    if(nk_begin(ctx, "Manage Tricks", nk_rect((screenWidth / 2) - 400, (screenHeight / 2) - 150, 800, 300), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE)){

        if(popup_open){
            struct nk_rect s = { 0, 0, 600, 100 };
            if (nk_popup_begin(ctx, NK_POPUP_STATIC, "Install trick from git", NK_WINDOW_MOVABLE, s)) {
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_label(ctx, "Git url (with extension):", NK_TEXT_LEFT);

                static char url[512];
                nk_layout_row_dynamic(ctx, 30, 1);
                nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, url, sizeof(url), nk_filter_default);

                nk_layout_row_dynamic(ctx, 20, 2);
                if(nk_button_label(ctx, "Cancel")){
                    nk_popup_close(ctx);
                    popup_open = false;
                }
                if (nk_button_label(ctx, "Install")) {
                    ye_logf(debug, "Installing trick from git: %s\n", url);

                    // Get the current working directory
                    char currentDir[1024];
                    if (getcwd(currentDir, sizeof(currentDir)) == NULL) {
                        ye_logf(error, "Failed to get current directory.\n");
                        // Handle error if needed
                        return; // Exit or handle appropriately
                    }

                    // Create the command string for git clone
                    char command[512 + 50];  // Allocate enough space for the command
                    snprintf(command, sizeof(command), "git clone %s", url);

                    // Change to the destination directory and execute git clone
                    if (chdir(ye_path("tricks")) != 0) {
                        ye_logf(error, "Failed to change directory.\n");
                        // Handle error if needed
                        return; // Exit or handle appropriately
                    }

                    int status = system(command); // Execute git clone

                    // Restore the original working directory
                    if (chdir(currentDir) != 0) {
                        ye_logf(error, "Failed to change back to the original directory.\n");
                        // Handle error if needed
                    }

                    if (status == 0) {
                        ye_logf(debug, "Git clone successful.\n");
                        // Handle success if needed
                    } else {
                        ye_logf(error, "Git clone failed.\n");
                        // Handle failure if needed
                    }

                    // Rebuild cache to be safe
                    json_decref(tricks_meta);
                    tricks_meta = NULL;

                    remove(ye_path("tricks/tricks_editor_cache.yoyo"));

                    load_tricks_meta(ye_path("tricks/tricks_editor_cache.yoyo"));
                    _re_cache_tricks_meta();


                    nk_popup_close(ctx);
                    popup_open = false;
                }
                nk_popup_end(ctx);
            }
        }

        if(tricks_meta == NULL){
            load_tricks_meta(ye_path("tricks/tricks_editor_cache.yoyo"));
        }

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Actions:", NK_TEXT_CENTERED);
        nk_layout_row_dynamic(ctx, 20, 2);
        if(nk_button_label(ctx, "hard refresh")){
            json_decref(tricks_meta);
            tricks_meta = NULL;

            // remove the tricks.yoyo file
            remove(ye_path("tricks/tricks_editor_cache.yoyo"));
            ye_logf(debug, "Removed cached tricks_editor_cache.yoyo file\n");
        }
        if(nk_button_label(ctx, "open tricks folder")){
            editor_open_in_system(ye_path("tricks/"));
        }
        if(nk_button_label(ctx, "install from git")){
            popup_open = !popup_open;
        }
        nk_label(ctx,"note: requires git", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 20, 1);

        // name, version, author, delete
        nk_layout_row_dynamic(ctx, 20, 4);

        nk_label(ctx, "Name:", NK_TEXT_CENTERED);
        nk_label(ctx, "Version:", NK_TEXT_CENTERED);
        nk_label(ctx, "Author:", NK_TEXT_CENTERED);
        nk_label(ctx, "", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 30, 4);

        // for each trick in the array
        for (size_t i = 0; i < json_array_size(json_object_get(tricks_meta, "tricks")); i++){
            json_t * trick = json_array_get(json_object_get(tricks_meta, "tricks"), i);
            
            // get the trick name
            const char * name; ye_json_string(trick, "name", &name);
            const char * author; ye_json_string(trick, "author", &author);
            const char * version; ye_json_string(trick, "version", &version);
            const char * description; ye_json_string(trick, "description", &description);


            struct nk_rect bounds = nk_widget_bounds(ctx);
            const struct nk_input *in = &ctx->input;
            nk_label(ctx, name, NK_TEXT_CENTERED);
            if (nk_input_is_mouse_hovering_rect(in, bounds))
                nk_tooltip(ctx, description);
            nk_label(ctx, version, NK_TEXT_CENTERED);
            nk_label(ctx, author, NK_TEXT_CENTERED);

            if(nk_button_label(ctx, "delete")){
                // remove the trick from the array
                // json_array_remove(json_object_get(tricks_meta, "tricks"), i);

                // // remove the trick.yoyo file
                // remove(filepath);
                // ye_logf(debug, "Removed trick.yoyo file at %s\n", filepath);

                /*
                    actually delete the item here, we use the folder name cached in meta
                */

                ye_logf(debug, "Deleting trick: %s\n", name);

                const char * f; ye_json_string(trick, "folder", &f);

                // delete it
                char command[512];
                snprintf(command, sizeof(command), "rm -rf \"%s\"%s", ye_path("tricks/"),f); // NOTCROSSPLATFORM
                
                // Execute the command.
                int res = system(command);

                if(res != 0){
                    ye_logf(error, "Failed to delete trick folder.\n");
                }

                json_decref(tricks_meta);
                tricks_meta = NULL;

                remove(ye_path("tricks/tricks_editor_cache.yoyo"));

                load_tricks_meta(ye_path("tricks/tricks_editor_cache.yoyo"));
                _re_cache_tricks_meta();
            }
        }



        //nk_layout_row_dynamic(ctx, 20, 1);
        //nk_layout_row_dynamic(ctx, 20, 1);

        //nk_text_wrap(ctx, "To install new tricks, either open the tricks folder and move in a new trick, or choose \"install from git\" in actions.",NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_layout_row_dynamic(ctx, 20, 3);
        nk_label(ctx,"", NK_TEXT_CENTERED);
        if(nk_button_label(ctx, "Close")){
            remove_ui_component("trick panel");
            unlock_viewport();

            json_decref(tricks_meta);
            tricks_meta = NULL;
        }
        nk_label(ctx,"", NK_TEXT_CENTERED);
    
        nk_end(ctx);
    }
}