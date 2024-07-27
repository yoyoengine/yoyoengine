/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <stdlib.h>
#include <stdio.h>

#include <Nuklear/nuklear.h>

#include <yoyoengine/yoyoengine.h>

#include "editor.h"
#include "editor_panels.h"
#include "editor_fs_ops.h"

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

        int commits_size = 50 + (11 * 30) + 10 + 15;
        int margin = panelsize.y - running_height - commits_size - 80;

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

char new_proj_name[128];

void create_project_popup(struct nk_context *ctx) {
    struct nk_vec2 panelsize = nk_window_get_content_region_size(ctx);
    int w = 400;
    int h = 250;
    if(nk_popup_begin(ctx, NK_POPUP_STATIC, "Create Project", NK_WINDOW_BORDER|NK_WINDOW_TITLE, nk_rect((panelsize.x/2) - (w/2), (panelsize.y/2) - (h/2), w, h))){
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, "Project Name:", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, new_proj_name, sizeof(new_proj_name) - 1, nk_filter_default);

        nk_layout_row_dynamic(ctx, 30, 2);
        if(nk_button_label(ctx, "Cancel")){
            create_project_popup_open = false;
            nk_popup_close(ctx);
            nk_popup_end(ctx);
            return;
        }

        if(nk_button_label(ctx, "Create")){
            ye_logf(info, "Create Project\n");
        }

        nk_popup_end(ctx);
    }
}

void group_projects(struct nk_context *ctx) {
    if(nk_group_begin_titled(ctx, "project_list", "Projects", NK_WINDOW_BORDER|NK_WINDOW_TITLE)){
        struct nk_vec2 panelsize = nk_window_get_content_region_size(ctx);

        int rolling_height = 0;

        nk_layout_row_dynamic(ctx, 50, 3);
        ye_h3(nk_label_colored(ctx, "Projects:", NK_TEXT_LEFT, nk_rgb(255, 255, 255)));

        if(nk_button_image_label(ctx, editor_icons.style, "New Project", NK_TEXT_CENTERED)){
            create_project_popup_open = true;
            snprintf(new_proj_name, sizeof(new_proj_name), "");
        }

        if(nk_button_image_label(ctx, editor_icons.folder, "Open Existing Project", NK_TEXT_CENTERED)){
            ye_logf(info, "Open Existing Project\n");
        }

        rolling_height += 50;

        nk_layout_row_dynamic(ctx, 15, 1);
        nk_label(ctx, "", NK_TEXT_LEFT);
        
        rolling_height += 15;

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label_colored(ctx, "Recent Projects:", NK_TEXT_LEFT, nk_rgb(255, 255, 255));
        
        rolling_height += 30;

        struct nk_color custom_color = nk_rgb(10, 10, 10);
        nk_style_push_color(ctx, &ctx->style.window.fixed_background.data.color, custom_color);
        
        nk_layout_row_dynamic(ctx, panelsize.y - rolling_height - 10, 1);
        nk_group_begin(ctx, "recent_projects", NK_WINDOW_BORDER);
        
        nk_layout_row_dynamic(ctx, 30, 4);
        for(int i = 0; i < 100; i++){

            nk_label(ctx, "MM/DD/YYYY", NK_TEXT_LEFT);

            ye_h3(nk_label(ctx, "Project Name", NK_TEXT_LEFT));
        
            if(nk_button_image_label(ctx, editor_icons.folder , "Open", NK_TEXT_CENTERED)){
                ye_logf(info, "Open Project\n");
            }

            if(nk_button_image_label(ctx, editor_icons.trash, "Delete", NK_TEXT_CENTERED)){
                ye_logf(info, "Delete Project\n");
            }
        
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