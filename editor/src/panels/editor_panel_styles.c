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
    Thoughts/notes:
    - Its kinda uneccesarially complicated right now to have a combo box appearing everywhere for fonts and colors, it would be better
      to just take in names at each location they are used.
*/

#include <yoyoengine/yoyoengine.h>

#include "editor.h"
#include "editor_panels.h"

json_t * style_data = NULL;

void editor_panel_styles(struct nk_context *ctx)
{
    if (nk_begin(ctx, "Styles", nk_rect(screenWidth / 2 - 500, screenHeight / 2 - 400, 1000, 800), 
    NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE)) {

        if(style_data == NULL){
            style_data = json_load_file(ye_get_resource_static("styles.yoyo"), 0, NULL);
            if(style_data == NULL){
                nk_layout_row_dynamic(ctx, 25, 1);
                // ye_logf(error, "Failed to load styles.yoyo."); cant error because run every frame, maybe come up with generic solution for this
                nk_label(ctx, "Failed to load styles.yoyo", NK_TEXT_CENTERED);
                if(nk_button_label(ctx, "Close")){
                    remove_ui_component("editor styles");
                    unlock_viewport();
                }
                nk_end(ctx);
                return;
            }
        }
        else{
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_button_label(ctx, "Reload Styles")){
                json_decref(style_data);
                style_data = json_load_file(ye_get_resource_static("styles.yoyo"), 0, NULL);
            }
        }

        nk_layout_row_dynamic(ctx, 0, 1);
        nk_label(ctx, "This panel is currently being neglected.", NK_TEXT_CENTERED);
        nk_label(ctx, "Please edit the styles.yoyo manually for now.", NK_TEXT_CENTERED);
        nk_layout_row_dynamic(ctx, 0, 1);
        nk_layout_row_dynamic(ctx, 0, 1);
        if(nk_button_label(ctx, "Open styles.yoyo")){
            ye_logf(debug, "Opening: %s", ye_get_resource_static("styles.yoyo"));

            // open the resource
            char command[256];
            snprintf(command, sizeof(command), "xdg-open \"%s\"", ye_get_resource_static("styles.yoyo"));
            system(command);  // NOTCROSSPLATFORM
        }

        /*
            TODO:
            - Two tabs / sections for fonts and colors.
            - For both, allow adding and deleting row entries (could be grouped as sub tabs)
            - For colors, we can allow direct modification and just create a new dummy color to edit as needed, but for fonts we have to stage before adding and then after the fact we can add the font.
        */

        if(nk_tree_push(ctx, NK_TREE_TAB, "Fonts", NK_MAXIMIZED)){
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Fonts", NK_TEXT_CENTERED);

            const char *font_name;
            json_t *font;
            json_object_foreach(json_object_get(style_data,"fonts"), font_name, font) {
                if(!ye_json_has_key(font,"path")){
                    ye_logf(error,"Font %s does not have a path.\n",font_name);
                    continue;
                }
                if(!ye_json_has_key(font,"size")){
                    ye_logf(error,"Font %s does not have a size.\n",font_name);
                    continue;
                }
                const char *font_path;    ye_json_string(font,"path",&font_path);
                int font_size;      ye_json_int(font,"size",&font_size);
                
                nk_layout_row_dynamic(ctx, 25, 3);
                nk_label(ctx, font_name, NK_TEXT_LEFT); // im tempted to do something really yucky, just straight modify the jansson values.
                nk_label(ctx, font_path, NK_TEXT_LEFT);
                char font_size_str[32];
                snprintf(font_size_str, sizeof(font_size_str), "%d", font_size);
                nk_label(ctx, font_size_str, NK_TEXT_LEFT);
            }

            if(nk_button_label(ctx, "Add Font")){
                
            }

            nk_tree_pop(ctx);
        }

        if(nk_tree_push(ctx, NK_TREE_TAB, "Colors", NK_MAXIMIZED)){
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Colors", NK_TEXT_CENTERED);

            const char *color_name;
            json_t *color;
            json_object_foreach(json_object_get(style_data,"colors"), color_name, color) {
                // if(!ye_json_has_key(color,"path")){
                //     ye_logf(error,"Font %s does not have a path.\n",color_name);
                //     continue;
                // }
                // if(!ye_json_has_key(color,"size")){
                //     ye_logf(error,"Font %s does not have a size.\n",color_name);
                //     continue;
                // }
                // const char *font_path;    ye_json_string(color,"path",&font_path);
                // int font_size;      ye_json_int(font,"size",&font_size);
                
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, color_name, NK_TEXT_LEFT); // im tempted to do something really yucky, just straight modify the jansson values.
                // nk_label(ctx, font_path, NK_TEXT_LEFT);
                // nk_label(ctx, "%d", font_size);
            }

            if(nk_button_label(ctx, "Add Color")){
                SDL_Color new_color = {255, 0, 0, 255};
                ye_cache_color("new color", new_color);
            }

            nk_tree_pop(ctx);
        }
        
        if(nk_button_label(ctx, "Close")){
            remove_ui_component("editor styles");
            unlock_viewport();
        }
    }
    nk_end(ctx);
}