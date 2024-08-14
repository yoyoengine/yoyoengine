/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>

#include <Nuklear/nuklear.h>

#include <yoyoengine/yoyoengine.h>

#include "editor.h"
#include "editor_panels.h"
#include "editor_fs_ops.h"
#include "editor_utils.h"

int is_curl_installed() {
    // TODO: NOTCROSSPLATFORM
    #ifdef __linux__
        return system("command -v curl > /dev/null 2>&1") == 0;
    #else
        printf("LMAO. ENJOY PORTING THIS.\n");
        exit(1);
    #endif
}

char *run_curl_command(const char *url) {
    FILE *fp;
    char path[1035];
    char *json_data = NULL;
    size_t json_data_size = 0;

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "curl -s \"%s\"", url);

    fp = popen(cmd, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to run curl command\n");
        return NULL;
    }

    while (fgets(path, sizeof(path), fp) != NULL) {
        size_t len = strlen(path);
        char *new_data = realloc(json_data, json_data_size + len + 1);
        if (new_data == NULL) {
            fprintf(stderr, "Not enough memory\n");
            free(json_data);
            pclose(fp);
            return NULL;
        }
        json_data = new_data;
        memcpy(json_data + json_data_size, path, len);
        json_data_size += len;
    }
    if (json_data) {
        json_data[json_data_size] = '\0';
    }

    pclose(fp);
    return json_data;
}

char *check_remote_version() {
    char *json_data = run_curl_command("https://api.github.com/repos/zoogies/yoyoengine/releases/latest");
    if(json_data){
        json_t *root = json_loads(json_data, 0, NULL);
        if (!root) {
            ye_logf(error, "Could not ping update server: failed to parse json\n");
            free(json_data);
            return NULL;
        }

        json_t *tag_name = json_object_get(root, "tag_name");
        if (!json_is_string(tag_name)) {
            ye_logf(error, "Could not ping update server: failed to get tag_name\n");
            json_decref(root);
            free(json_data);
            return NULL;
        }

        char *tag_name_str = strdup(json_string_value(tag_name));
        json_decref(root);
        free(json_data);
        return tag_name_str;
    }

    ye_logf(error, "Could not ping update server: failed to fetch json\n");
    return NULL;
}

char welcome_text[512];

json_t *commits = NULL;

bool curl_installed = true;
bool offline = false;
bool error_welcome = false;
int latest_version_int = -1;
int current_version_int = -1;
bool update_available = false;
char update_text[128];

void editor_init_panel_welcome() {
    snprintf(welcome_text, sizeof(welcome_text), "You are currently running yoyoeditor %s, powered by yoyoengine core %s", YE_EDITOR_VERSION, YE_ENGINE_VERSION);

    curl_installed = is_curl_installed();

    if(!curl_installed){
        ye_logf(info, "CURL IS NOT INSTALLED!\n");
        offline = true;
        load_project_cache();
        return;
    }

    // check if update is available
    char *latest_version = check_remote_version();
    // parse out the version number from the string "build-XXX"
    if(latest_version){
        char *latest_version_num = latest_version + 6; // remove "build-"
        
        latest_version_int = atoi(latest_version_num);

        char *current_version_num = YE_EDITOR_VERSION + 6; // remove "build "
        current_version_int = atoi(current_version_num);

        free(latest_version);

        ye_logf(info, "Pinged update server. Latest version: %d, Current version: %d\n", latest_version_int, current_version_int);

        if(latest_version_int > current_version_int){
            update_available = true;
        }
    }

    // do always just for testing purposes
    snprintf(update_text, sizeof(update_text), "build %d -> build %d", current_version_int, latest_version_int);
    // update_available = true; DEBUG

    const char *url = "https://api.github.com/repos/zoogies/yoyoengine/commits?sha=main&per_page=10";

    char *json_data = run_curl_command(url);
    if(json_data){
        commits = json_loads(json_data, 0, NULL);
        if (!commits) {
            ye_logf(error, "Failed to parse commits json\n");
            error_welcome = true;
        }

        free(json_data);
    }

    // project list stuff

    load_project_cache();
}

// get a timestamp buffer, format MM/DD/YYYY
void get_stamp_string(char *buffer, size_t buffer_size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Make sure the buffer is large enough
    if (buffer_size >= 11) {
        snprintf(buffer, buffer_size, "%02d/%02d/%d", tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900);
    }
}

void update_available_popup(struct nk_context *ctx) {
    struct nk_vec2 panelsize = nk_window_get_content_region_size(ctx);
    int w = 500;
    int h = 200;
    if(nk_popup_begin(ctx, NK_POPUP_STATIC, "Update Available", NK_WINDOW_BORDER|NK_WINDOW_TITLE, nk_rect((panelsize.x/2) - (w/2), (panelsize.y/2) - (h/2), w, h))){
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "An update is available for yoyoengine!", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label_colored(ctx, update_text, NK_TEXT_CENTERED, nk_rgb(0, 255, 0));

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Would you like to download it?", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 8, 1);
        nk_label(ctx, "", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 30, 2);
        if(nk_button_label(ctx, "No")){
            update_available = false;
            nk_popup_close(ctx);
            nk_popup_end(ctx);
            return;
        }

        if(nk_button_label(ctx, "Yes")){
            ye_logf(info, "Download Update\n");
        }

        nk_popup_end(ctx);
    }
}

void group_welcome(struct nk_context *ctx) {
    if(nk_group_begin_titled(ctx, "news", "Welcome", NK_WINDOW_BORDER|NK_WINDOW_TITLE)){
        int running_height = 0;

        // lightheader is 1350x450, so we need to scale it to keep its aspect ratio
        struct nk_vec2 panelsize = nk_window_get_content_region_size(ctx);
        float scale = panelsize.x / 1350;

        // TODO: ye_clamp? - low prio
        nk_layout_row_static(ctx, 450*(scale/1.5), panelsize.x/1.5, 1);
        running_height += 450*(scale/1.5);

        nk_image(ctx, editor_icons.lightheader);

        nk_layout_row_dynamic(ctx, 50, 1);

        ye_h3(nk_label_colored(ctx, "Welcome to yoyoengine!", NK_TEXT_CENTERED, nk_rgb(255, 255, 255)));
        nk_label_wrap(ctx, welcome_text);
        running_height += 100;

        nk_label_wrap(ctx, "yoyoengine is provided under the MIT license, and is free to use for any purpose. Credit is greatly appreciated, but not required.");
        running_height += 50;

        ye_h3(nk_label(ctx, "Helpful Links:", NK_TEXT_CENTERED));
        running_height += 50;

        // blank link link link blank
        nk_layout_row_dynamic(ctx, 30, 5);

        nk_label(ctx, "", NK_TEXT_LEFT);
        if(nk_button_label(ctx, "Github"))
            editor_open_in_system("https://github.com/zoogies/yoyoengine");
        if(nk_button_label(ctx, "Documentation"))
            editor_open_in_system("https://zoogies.github.io/yoyoengine/");
        if(nk_button_label(ctx, "Report an Issue"))
            editor_open_in_system("https://github.com/zoogies/yoyoengine/issues/new");
        
        nk_label(ctx, "", NK_TEXT_LEFT);

        running_height += 30;

        int commits_size = 50 + (11 * 30) + 10 + 15;
        int margin = panelsize.y - running_height - commits_size - 80 - 8;

        if(margin > 0){
            nk_layout_row_dynamic(ctx, margin, 1);
            nk_label(ctx, "", NK_TEXT_LEFT);
        }

        nk_layout_row_dynamic(ctx, 50, 1);
        ye_h3(nk_label_colored(ctx, "Recent Development:", NK_TEXT_CENTERED, nk_rgb(255, 255, 255)));

        if (error_welcome){
            nk_label_colored(ctx, "An error occurred. Ensure curl is installed and you are online.", NK_TEXT_LEFT, nk_rgb(255, 0, 0));

            nk_group_end(ctx);
            nk_end(ctx);
            return;
        }

        if (!curl_installed){
            nk_label_colored(ctx, "CURL is not installed. Cannot fetch commits.", NK_TEXT_LEFT, nk_rgb(255, 0, 0));
            
            nk_group_end(ctx);
            nk_end(ctx);
            return;
        }

        if (offline){
            nk_label_colored(ctx, "You are offline. Cannot fetch commits.", NK_TEXT_LEFT, nk_rgb(255, 0, 0));
            
            nk_group_end(ctx);
            nk_end(ctx);
            return;
        }

        if(!commits){
            nk_label_colored(ctx, "Failed to fetch commits. Are you offline?", NK_TEXT_LEFT, nk_rgb(255, 0, 0));
            
            nk_group_end(ctx);
            nk_end(ctx);
            return;
        }

        int room_for_message = panelsize.x -
            30 - // eye icon
            75 - // sha
            100 - // author
            10 - // padding
            90 - // date
            20; // extra manual padding

        nk_layout_row_begin(ctx, NK_STATIC, 30, 6);

        nk_layout_row_push(ctx, 30);
        nk_label(ctx, "", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, 75);
        nk_label(ctx, "SHA", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, 100);
        nk_label(ctx, "Author", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, room_for_message);
        nk_label(ctx, "Message", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, 10);
        nk_label(ctx, "", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, 90);
        nk_label(ctx, "Date", NK_TEXT_LEFT);

        for (size_t i = 0; i < json_array_size(commits); i++) {
            json_t *commit = json_array_get(commits, i);
            json_t *commit_data = json_object_get(commit, "commit");
            json_t *author = json_object_get(commit_data, "author");
            json_t *message = json_object_get(commit_data, "message");
            json_t *sha = json_object_get(commit, "sha");

            const char * shastr = json_string_value(json_object_get(commit, "sha"));
            char sha_abbrv[8];
            strncpy(sha_abbrv, shastr, 7);
            sha_abbrv[7] = '\0';

            nk_layout_row_push(ctx, 30);
            nk_image(ctx, editor_icons.eye);

            nk_layout_row_push(ctx, 75);
            nk_label_colored(ctx, sha_abbrv, NK_TEXT_LEFT, nk_rgb(66, 135, 245));
            nk_layout_row_push(ctx, 100);
            nk_label(ctx, json_string_value(json_object_get(author, "name")), NK_TEXT_LEFT);

            nk_layout_row_push(ctx, room_for_message);
            nk_label(ctx, json_string_value(message), NK_TEXT_LEFT);
            
            nk_layout_row_push(ctx, 10);
            nk_label(ctx, "", NK_TEXT_LEFT);
            nk_layout_row_push(ctx, 90);
            nk_label(ctx, json_string_value(json_object_get(author, "date")), NK_TEXT_LEFT);
        }

        nk_layout_row_end(ctx);

        nk_layout_row_dynamic(ctx, 10, 1);
        nk_label(ctx, "", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 15, 1);
        nk_label_colored(ctx, "note: this reflects origin/main, _not_ your local copy :)", NK_TEXT_RIGHT, nk_rgb(200, 200, 200));

        nk_group_end(ctx);
    }
}

// TODO: port this other places, kinda nice
#define align_cent(w,h) nk_rect((screenWidth/2) - (w/2), (screenHeight/2) - (h/2), w, h)

bool create_project_popup_open = false;
struct nk_window *win = NULL;

char new_proj_name[128]; // the name of the project folder to create
char new_proj_path[512]; // the parent directory we will create the project in

json_t *project_cache = NULL;

void create_project_popup(struct nk_context *ctx) {
    struct nk_vec2 panelsize = nk_window_get_content_region_size(ctx);
    int w = 400;
    int h = 250;
    if(nk_popup_begin(ctx, NK_POPUP_STATIC, "Create Project", NK_WINDOW_BORDER|NK_WINDOW_TITLE, nk_rect((panelsize.x/2) - (w/2), (panelsize.y/2) - (h/2), w, h))){
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Project Name:", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, new_proj_name, sizeof(new_proj_name) - 1, nk_filter_default);

        nk_label(ctx, "Project Path:", NK_TEXT_LEFT);
        
        nk_layout_row_begin(ctx, NK_DYNAMIC, 30, 2);

        nk_layout_row_push(ctx, 0.66);

        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, new_proj_path, sizeof(new_proj_path) - 1, nk_filter_default);
        
        nk_layout_row_push(ctx, 0.28 + 0.05);

        if(nk_button_image_label(ctx, editor_icons.folder, "browse", NK_TEXT_CENTERED)){
            char *path = editor_file_dialog_select_folder();
            if(path){
                snprintf(new_proj_path, sizeof(new_proj_path), "%s", path);
                free(path);
            }
        }

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 30, 2);
        if(nk_button_label(ctx, "Cancel")){
            create_project_popup_open = false;
            nk_popup_close(ctx);
            nk_popup_end(ctx);
            return;
        }

        if(nk_button_label(ctx, "Create")){
            ye_logf(info, "Create Project\n");

            char new_proj_full_path[512];
            snprintf(new_proj_full_path, sizeof(new_proj_full_path), "%s/%s", new_proj_path, new_proj_name);

            if(!editor_create_directory(new_proj_full_path)){
                ye_logf(error, "Failed to create project at %s\n", new_proj_full_path);
                
                // TODO: stuff like this is a good case for labels, goto: end
                nk_popup_end(ctx);
                return;
            }

            // copy template
            char template_path[512];
            snprintf(template_path, sizeof(template_path), "%s/.local/share/yoyoengine/source/template", getenv("HOME"));
            printf("template_path: %s\n", template_path);

            char game_path[512];
            snprintf(game_path, sizeof(game_path), "%s/game", new_proj_full_path);

            if(!editor_copy_directory(template_path, game_path)){
                ye_logf(error, "Failed to copy template to %s\n", game_path);
                
                nk_popup_end(ctx);
                return;
            }

            // run git init in new_proj_full_path
            char git_init_cmd[512];
            snprintf(git_init_cmd, sizeof(git_init_cmd), "cd \"%s\" && git init", new_proj_full_path);
            if(system(git_init_cmd) != 0){
                ye_logf(error, "Failed to run git init in %s\n", new_proj_full_path);
                
                nk_popup_end(ctx);
                return;
            }

            // get the desired tag version from a dumb macro hack
            char tag[16];
            snprintf(tag, sizeof(tag), "%s", YE_ENGINE_VERSION);

            // HACK: if we are using the build- convention,
            // the tag uses a - instead of a space.
            if(tag[0] == 'b'){
                tag[5] = '-';
            }

            printf("tag: %s\n", tag);

            // run git submodule add https://github.com/zoogies/yoyoengine.git in new_proj_full_path
            char git_submodule_cmd[512];
            snprintf(git_submodule_cmd, sizeof(git_submodule_cmd), 
                "cd \"%s\" && git submodule add https://github.com/zoogies/yoyoengine.git && cd yoyoengine && git checkout %s", 
                new_proj_full_path, tag);
            if(system(git_submodule_cmd) != 0){

                // special development edge case to not exit early
                if(strcmp(tag, "build-0") != 0){
                    ye_logf(error, "Failed to run git submodule add in %s\n", new_proj_full_path);

                    nk_popup_end(ctx);
                    return;
                }
                
            }

            // set key in project cache
            json_t *project = json_object();
            json_object_set_new(project, "name", json_string(new_proj_name));
            char stamp[11]; get_stamp_string(&stamp, sizeof(stamp));
            json_object_set_new(project, "date", json_string(&stamp));
            json_object_set_new(project, "path", json_string(game_path));

            json_t *projects = json_object_get(project_cache, "projects");

            // append at start of array
            json_array_insert_new(projects, 0, project);

            serialize_projects();

            create_project_popup_open = false;
            nk_popup_close(ctx);
        }

        nk_popup_end(ctx);
    }
}

// TODO: clean up dangling project cache??

//////////////////////////////////
// Helpers for the project list //
//////////////////////////////////

// expands a home dir path to be absolute
const char* expand_tilde(const char *path) {
    static char expanded_path[1024]; // Static buffer to hold the expanded path

    if (path[0] == '~') {
        const char *home = getenv("HOME");
        if (!home) {
            home = getpwuid(getuid())->pw_dir;
        }
        snprintf(expanded_path, sizeof(expanded_path), "%s%s", home, path + 1);
    } else {
        strncpy(expanded_path, path, sizeof(expanded_path));
        expanded_path[sizeof(expanded_path) - 1] = '\0'; // Ensure null-termination
    }

    return expanded_path;
}

void serialize_projects() {
    if(!project_cache){
        project_cache = json_object();
        json_object_set_new(project_cache, "projects", json_array());
    }

    ye_json_write(expand_tilde("~/.local/share/yoyoengine/project_cache.yoyo"), project_cache);
}

void load_project_cache() {
    project_cache = ye_json_read(expand_tilde("~/.local/share/yoyoengine/project_cache.yoyo"));
}

//////////////////////////////////

void group_projects(struct nk_context *ctx) {
    if(nk_group_begin_titled(ctx, "project_list", "Projects", NK_WINDOW_BORDER|NK_WINDOW_TITLE)){
        struct nk_vec2 panelsize = nk_window_get_content_region_size(ctx);

        int rolling_height = 0;

        nk_layout_row_dynamic(ctx, 50, 4);
        ye_h3(nk_label_colored(ctx, "Projects:", NK_TEXT_LEFT, nk_rgb(255, 255, 255)));

        nk_label(ctx, "", NK_TEXT_LEFT);

        if(nk_button_image_label(ctx, editor_icons.style, "New", NK_TEXT_CENTERED)){
            create_project_popup_open = true;
            snprintf(new_proj_name, sizeof(new_proj_name), "");
        }

        if(nk_button_image_label(ctx, editor_icons.folder, "Open", NK_TEXT_CENTERED)){
            ye_logf(info, "Open Existing Project\n");

            char *path = editor_file_dialog_select_folder();

            if(path){
                // check if path/settings.yoyo exists, if so read it into json_t
                char settings_path[1024];
                snprintf(settings_path, sizeof(settings_path), "%s/settings.yoyo", path);
                if(access(settings_path, F_OK) == -1){
                    ye_logf(error, "%s was not a real yoyoengine project.\n", path);
                    free(path);
                    nk_group_end(ctx);
                    return;
                }

                json_t *settings = ye_json_read(settings_path);

                const char * name = json_string_value(json_object_get(settings, "name"));

                char stamp[11]; get_stamp_string(&stamp, sizeof(stamp));

                // set key in project cache
                json_t *project = json_object();
                json_object_set_new(project, "name", json_string(name));
                json_object_set_new(project, "date", json_string(stamp));
                json_object_set_new(project, "path", json_string(path));

                json_t *projects = json_object_get(project_cache, "projects");
                
                // append at start of array
                json_array_insert_new(projects, 0, project);

                serialize_projects();

                free(path);
            }
        }

        rolling_height += 50;

        nk_layout_row_dynamic(ctx, 15, 1);
        nk_label(ctx, "", NK_TEXT_LEFT);
        
        rolling_height += 15;

        nk_layout_row_begin(ctx, NK_STATIC, 40, 4);

        int gap = panelsize.x - 300 /*size of text*/ - 40 /*size of refresh*/ - 40 /*size of gear*/ - 15 /*padding*/;

        nk_layout_row_push(ctx, 300);

        nk_label_colored(ctx, "Recent Projects:", NK_TEXT_LEFT, nk_rgb(255, 255, 255));
        
        nk_layout_row_push(ctx, gap);
        nk_label(ctx, "", NK_TEXT_LEFT);

        nk_layout_row_push(ctx, 40);

        if(nk_button_image(ctx, editor_icons.refresh)){
            ye_logf(info, "Refresh Projects\n");

            if(project_cache){
                json_decref(project_cache);
            }

            load_project_cache();
        }

        nk_layout_row_push(ctx, 40);

        if(nk_button_image(ctx, editor_icons.gear)){
            ye_logf(debug, "Opening Cached Projects File\n");

            editor_open_in_system(expand_tilde("~/.local/share/yoyoengine/project_cache.yoyo"));
        }

        rolling_height += 30;

        struct nk_color custom_color = nk_rgb(10, 10, 10);
        nk_style_push_color(ctx, &ctx->style.window.fixed_background.data.color, custom_color);
        
        nk_layout_row_dynamic(ctx, panelsize.y - rolling_height - 25, 1);
        nk_group_begin(ctx, "recent_projects", NK_WINDOW_BORDER);
        
        json_t *projects = json_object_get(project_cache, "projects");

        // iterate over projects array which contains dicts with keys name date path
        for(size_t i = 0; i < json_array_size(projects); i++){

            json_t *project = json_array_get(projects, i);

            const char *date_str = json_string_value(json_object_get(project, "date"));
            const char *name_str = json_string_value(json_object_get(project, "name"));
            const char *path_str = json_string_value(json_object_get(project, "path"));

            nk_layout_row_begin(ctx, NK_DYNAMIC, 30, 3);

            nk_layout_row_push(ctx, 0.15);

            nk_label_colored(ctx, date_str, NK_TEXT_CENTERED, nk_rgb(235, 235, 235));

            nk_layout_row_push(ctx, 0.44);

            ye_h3(nk_label(ctx, name_str, NK_TEXT_LEFT));
        
            nk_layout_row_push(ctx, 0.2);

            if(nk_button_image_label(ctx, editor_icons.folder , "Open", NK_TEXT_CENTERED)){
                ye_logf(debug, "Open Project\n");

                char stamp[11]; get_stamp_string(&stamp, sizeof(stamp));
                json_object_set_new(project, "date", json_string(&stamp));
                
                // json_array_insert_new(projects, 0, project);
                // json_array_remove(projects, i-1);

                serialize_projects();

                EDITOR_STATE.mode = ESTATE_EDITING;
                
                if(EDITOR_STATE.opened_project_path){
                    free(EDITOR_STATE.opened_project_path);
                }
                EDITOR_STATE.opened_project_path = strdup(path_str);

                nk_group_end(ctx);
                nk_style_pop_color(ctx);
                return;
            }

            nk_layout_row_push(ctx, 0.2);

            // TODO: put a notice somewhere that this only deletes the reference, not the proj files
            if(nk_button_image_label(ctx, editor_icons.trash, "Delete", NK_TEXT_CENTERED)){
                ye_logf(debug, "Delete Project\n");

                json_array_remove(projects, i);
                serialize_projects();
                nk_group_end(ctx);
                nk_style_pop_color(ctx);
                return;
            }
        
        }
        if(json_array_size(projects) == 0){
            nk_layout_row_dynamic(ctx, 30, 1);
            ye_h3(nk_label(ctx, "No projects found.", NK_TEXT_CENTERED));
            nk_label(ctx, "Go on and create your next masterpeice!", NK_TEXT_CENTERED);
        }
        
        nk_group_end(ctx);
        
        // Pop the custom color after rendering the group
        nk_style_pop_color(ctx);

        if(create_project_popup_open){
            create_project_popup(ctx);
        }
    }
}

void editor_panel_welcome(struct nk_context *ctx){
    // if you change window name, change above too
    if(nk_begin(ctx, "yoyoengine - homepage", nk_rect(0,0,screenWidth,screenHeight), NK_WINDOW_BORDER|NK_WINDOW_TITLE)){
        nk_layout_row_dynamic(ctx, screenHeight - 58, 2);

        group_welcome(ctx);

        group_projects(ctx);

        if(update_available){
            update_available_popup(ctx);
        }

        nk_end(ctx);
    }
}