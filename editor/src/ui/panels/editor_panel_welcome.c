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

char welcome_text[512];

json_t *commits = NULL;

bool curl_installed = true;
bool offline = false;
bool error_welcome = false;

void editor_init_panel_welcome() {
    snprintf(welcome_text, sizeof(welcome_text), "You are currently running yoyoeditor %s, powered by yoyoengine core %s", YE_EDITOR_VERSION, YE_ENGINE_VERSION);

    curl_installed = is_curl_installed();

    if(!curl_installed){
        ye_logf(info, "CURL IS NOT INSTALLED!\n");
        offline = true;
        return;
    }

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

void editor_panel_welcome(struct nk_context *ctx){
    if(nk_begin(ctx, "yoyoengine - homepage", nk_rect(0,0,screenWidth,screenHeight), NK_WINDOW_BORDER|NK_WINDOW_TITLE)){
        nk_layout_row_dynamic(ctx, screenHeight - 58, 2);

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

        if(nk_group_begin_titled(ctx, "project_list", "Projects", NK_WINDOW_BORDER|NK_WINDOW_TITLE)){
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "No projects found.", NK_TEXT_CENTERED);
            nk_group_end(ctx);
        }

        nk_end(ctx);
    }
}