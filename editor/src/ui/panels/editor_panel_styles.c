/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    Notes:
    maybe in the future we would want to persist this datastructure constantly so
    that we can have combo boxes everywhere listing all possible fonts and colors to use...

    I'm actually liking this idea and might implement it tomorrow.
*/

#include <yoyoengine/yoyoengine.h>

#include "editor.h"
#include "editor_panels.h"

json_t * style_data = NULL;






/*
    Little local API to this panel to represent the fonts and colors
    in a way we can edit them, and serialize and deserialize them. :3
*/





struct style_font{
    char name[32];
    char path[256];
    int size;
};

struct style_color{
    char name[32];
    SDL_Color color;
    struct nk_colorf nuklear_color;
};

struct style_font_node{
    struct style_font font;
    struct style_font_node *next;
};

struct style_color_node{
    struct style_color color;
    struct style_color_node *next;
};

struct style_font_node *font_head = NULL;
struct style_color_node *color_head = NULL;

void editor_panel_styles_freelists()
{
    struct style_font_node *current_font = font_head;
    while(current_font != NULL){
        struct style_font_node *next_font = current_font->next;
        free(current_font);
        current_font = next_font;
    }
    struct style_color_node *current_color = color_head;
    while(current_color != NULL){
        struct style_color_node *next_color = current_color->next;
        free(current_color);
        current_color = next_color;
    }
    font_head = NULL;
    color_head = NULL;
}

void editor_panel_styles_decode(){

    /*
        Create list of fonts
    */
    const char *font_name;
    json_t *font;
    json_object_foreach(json_object_get(style_data,"fonts"), font_name, font) {
        if(!ye_json_has_key(font,"path")){
            ye_logf(error,"Font %s does not have a path.\n",font_name);
            continue;
        }
        /*
        if(!ye_json_has_key(font,"size")){
            ye_logf(error,"Font %s does not have a size.\n",font_name);
            continue;
        }
        */
        const char *font_path;    ye_json_string(font,"path",&font_path);
        // int font_size;      ye_json_int(font,"size",&font_size);

        struct style_font_node *new_node = malloc(sizeof(struct style_font_node));
        strcpy(new_node->font.name, font_name);
        strcpy(new_node->font.path, font_path);
        // new_node->font.size = font_size;
        new_node->next = font_head;
        font_head = new_node;
    }

    /*
        Create list of colors
    */
    const char *color_name;
    json_t *color;
    json_object_foreach(json_object_get(style_data,"colors"), color_name, color) {
        if(!ye_json_has_key(color,"r")){
            ye_logf(error,"Color %s does not have a r value.\n",color_name);
            continue;
        }
        if(!ye_json_has_key(color,"g")){
            ye_logf(error,"Color %s does not have a g value.\n",color_name);
            continue;
        }
        if(!ye_json_has_key(color,"b")){
            ye_logf(error,"Color %s does not have a b value.\n",color_name);
            continue;
        }
        if(!ye_json_has_key(color,"a")){
            ye_logf(error,"Color %s does not have a a value.\n",color_name);
            continue;
        }
        int r;      ye_json_int(color,"r",&r);
        int g;      ye_json_int(color,"g",&g);
        int b;      ye_json_int(color,"b",&b);
        int a;      ye_json_int(color,"a",&a);

        struct style_color_node *new_node = malloc(sizeof(struct style_color_node));
        strcpy(new_node->color.name, color_name);
        new_node->color.color.r = r;
        new_node->color.color.g = g;
        new_node->color.color.b = b;
        new_node->color.color.a = a;

        // the nuklear color is the float version of the rgba
        new_node->color.nuklear_color.r = (float)r / 255.0f;
        new_node->color.nuklear_color.g = (float)g / 255.0f;
        new_node->color.nuklear_color.b = (float)b / 255.0f;
        new_node->color.nuklear_color.a = (float)a / 255.0f;

        new_node->next = color_head;
        color_head = new_node;
    }
}

void editor_panel_styles_encode(){
    /*
        Create list of fonts
    */
    json_t *fonts = json_object_get(style_data,"fonts");
    json_object_clear(fonts);
    struct style_font_node *current_font = font_head;
    while(current_font != NULL){
        json_t *font = json_object();
        json_object_set_new(font,"path",json_string(current_font->font.path));
        // json_object_set_new(font,"size",json_integer(current_font->font.size)); no more font sizes!

        // check if this key already exists in the json and message that it will be replaced if so
        if(ye_json_has_key(fonts,current_font->font.name)){
            ye_logf(warning,"Font %s already exists in styles.yoyo, it will be replaced.\n",current_font->font.name);
        }

        json_object_set_new(fonts,current_font->font.name,font);
        current_font = current_font->next;
    }

    /*
        Create list of colors
    */
    json_t *colors = json_object_get(style_data,"colors");
    json_object_clear(colors);
    struct style_color_node *current_color = color_head;
    while(current_color != NULL){
        json_t *color = json_object();
        json_object_set_new(color,"r",json_integer(current_color->color.color.r));
        json_object_set_new(color,"g",json_integer(current_color->color.color.g));
        json_object_set_new(color,"b",json_integer(current_color->color.color.b));
        json_object_set_new(color,"a",json_integer(current_color->color.color.a));
        
        // check if this key already exists in the json and message that it will be replaced if so
        if(ye_json_has_key(colors,current_color->color.name)){
            ye_logf(warning,"Color %s already exists in styles.yoyo, it will be replaced.\n",current_color->color.name);
        }

        json_object_set_new(colors,current_color->color.name,color);
        current_color = current_color->next;
    }

    ye_json_write(ye_path_resources("styles.yoyo"),style_data);
}





/*
    Begin actual GUI panel
*/





void editor_panel_styles(struct nk_context *ctx)
{
    if (nk_begin(ctx, "Styles", nk_rect(screenWidth / 2 - 500, screenHeight / 2 - 400, 1000, 800), 
    NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE)) {
        if(style_data == NULL){
            style_data = json_load_file(ye_path_resources("styles.yoyo"), 0, NULL);
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
            else{
                int file_version; ye_json_int(style_data,"version",&file_version);
                if(file_version != YE_ENGINE_STYLES_VERSION){
                    ye_logf(error, "The version of styles.yoyo is mismatched with what the engine expected. %d vs %d", file_version, YE_ENGINE_STYLES_VERSION);

                    nk_label(ctx, "styles.yoyo version mismatch", NK_TEXT_CENTERED);
                    if(nk_button_label(ctx, "Close")){
                        remove_ui_component("editor styles");
                        unlock_viewport();
                    }
                    nk_end(ctx);
                    return;
                }
                else{
                    editor_panel_styles_decode();
                }
            }
        }
        else{
            nk_layout_row_dynamic(ctx, 25, 1);
            char buff[256];
            snprintf(buff, sizeof(buff), "Editing styles.yoyo of version %d", YE_ENGINE_STYLES_VERSION);
            nk_label(ctx, buff, NK_TEXT_CENTERED);

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Options:", NK_TEXT_LEFT);
            if(nk_button_label(ctx, "Reload Styles")){
                json_decref(style_data);
                style_data = NULL;
                editor_panel_styles_freelists();
                style_data = json_load_file(ye_path_resources("styles.yoyo"), 0, NULL);
                editor_panel_styles_decode();
            }
        }

        nk_layout_row_dynamic(ctx, 25, 1);
        if(nk_button_label(ctx, "Open in text editor")){
            ye_logf(debug, "Opening: %s", ye_path_resources("styles.yoyo"));

            // open the resource
            char command[256];
            snprintf(command, sizeof(command), "xdg-open \"%s\"", ye_path_resources("styles.yoyo"));
            system(command);  // NOTCROSSPLATFORM
        }

        nk_layout_row_dynamic(ctx, 25, 1);
        if(nk_tree_push(ctx, NK_TREE_TAB, "Fonts", NK_MAXIMIZED)){
            nk_layout_row_dynamic(ctx, 25, 3);
            nk_label(ctx, "name", NK_TEXT_LEFT);
            nk_label(ctx, "path", NK_TEXT_LEFT);
            // nk_label(ctx, "size", NK_TEXT_LEFT); no more font sizes!
            nk_label(ctx, "delete", NK_TEXT_LEFT);

            struct style_font_node *current_font = font_head;
            while(current_font != NULL){
                nk_layout_row_dynamic(ctx, 25, 3);
                nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, current_font->font.name, 32, nk_filter_default);
                nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, current_font->font.path, 256, nk_filter_default);
                // nk_property_int(ctx, "Size:", 0, &current_font->font.size, 100, 1, 1); no more font sizes!
                
                // push some pretty styles for red button!! (thank you nuklear forum!) :D
                nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(35,35,35))); nk_style_push_style_item(ctx, &ctx->style.button.hover, nk_style_item_color(nk_rgb(255,0,0))); nk_style_push_style_item(ctx, &ctx->style.button.active, nk_style_item_color(nk_rgb(255,0,0))); nk_style_push_vec2(ctx, &ctx->style.button.padding, nk_vec2(2,2));
                
                if(nk_button_label(ctx, "Delete")){
                    // delete the node
                    if(current_font == font_head){
                        font_head = current_font->next;
                    }
                    else{
                        struct style_font_node *previous_font = font_head;
                        while(previous_font->next != current_font){
                            previous_font = previous_font->next;
                        }
                        previous_font->next = current_font->next;
                    }
                    free(current_font);
                }
                current_font = current_font->next;

                // pop off our cool red button colors
                nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_vec2(ctx);
            }

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_button_label(ctx, "Add Font")){
                struct style_font_node *new_node = malloc(sizeof(struct style_font_node));
                strcpy(new_node->font.name, "NEW FONT");
                strcpy(new_node->font.path, "NEW FONT");
                new_node->font.size = 12; // no more font sizes, what should happen to this?
                new_node->next = font_head;
                font_head = new_node;
            }

            nk_tree_pop(ctx);
        }

        nk_layout_row_dynamic(ctx, 25, 1); // spacer

        if(nk_tree_push(ctx, NK_TREE_TAB, "Colors", NK_MAXIMIZED)){
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_label(ctx, "name", NK_TEXT_CENTERED);
            nk_label(ctx, "color", NK_TEXT_CENTERED);

            struct style_color_node *current_color = color_head;
            while(current_color != NULL){
                nk_layout_row_dynamic(ctx, 25, 3);
                nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, current_color->color.name, 32, nk_filter_default);
                
                // color picker
                if(nk_combo_begin_color(ctx, nk_rgba(current_color->color.color.r, current_color->color.color.g, current_color->color.color.b, current_color->color.color.a), nk_vec2(200,400))){
                    nk_layout_row_dynamic(ctx, 150, 1);
                    current_color->color.nuklear_color = nk_color_picker(ctx, current_color->color.nuklear_color, NK_RGBA);
                    
                    nk_layout_row_dynamic(ctx, 25, 1);
                    current_color->color.nuklear_color.r = nk_propertyf(ctx, "#R:", 0, current_color->color.nuklear_color.r, 1.0f, 0.01f,0.005f);
                    current_color->color.nuklear_color.g = nk_propertyf(ctx, "#G:", 0, current_color->color.nuklear_color.g, 1.0f, 0.01f,0.005f);
                    current_color->color.nuklear_color.b = nk_propertyf(ctx, "#B:", 0, current_color->color.nuklear_color.b, 1.0f, 0.01f,0.005f);
                    current_color->color.nuklear_color.a = nk_propertyf(ctx, "#A:", 0, current_color->color.nuklear_color.a, 1.0f, 0.01f,0.005f);


                    // update the SDL_Color from the nuklear color
                    current_color->color.color.r = (int)(current_color->color.nuklear_color.r * 255.0f);
                    current_color->color.color.g = (int)(current_color->color.nuklear_color.g * 255.0f);
                    current_color->color.color.b = (int)(current_color->color.nuklear_color.b * 255.0f);
                    current_color->color.color.a = (int)(current_color->color.nuklear_color.a * 255.0f);

                    nk_combo_end(ctx);
                }

                // push some pretty styles for red button!! (thank you nuklear forum!) :D
                nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(35,35,35))); nk_style_push_style_item(ctx, &ctx->style.button.hover, nk_style_item_color(nk_rgb(255,0,0))); nk_style_push_style_item(ctx, &ctx->style.button.active, nk_style_item_color(nk_rgb(255,0,0))); nk_style_push_vec2(ctx, &ctx->style.button.padding, nk_vec2(2,2));
                
                if(nk_button_label(ctx, "Delete")){
                    // delete the node
                    if(current_color == color_head){
                        color_head = current_color->next;
                    }
                    else{
                        struct style_color_node *previous_color = color_head;
                        while(previous_color->next != current_color){
                            previous_color = previous_color->next;
                        }
                        previous_color->next = current_color->next;
                    }
                    free(current_color);
                }
                current_color = current_color->next;
                
                // pop off our cool red button colors
                nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_style_item(ctx); nk_style_pop_vec2(ctx);
            }

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_layout_row_dynamic(ctx, 25, 1);
            if(nk_button_label(ctx, "Add Color")){
                SDL_Color new_color = {255, 0, 0, 255};
                struct style_color_node *new_node = malloc(sizeof(struct style_color_node));
                strcpy(new_node->color.name, "NEW COLOR");
                new_node->color.color = new_color;
                new_node->color.nuklear_color = (struct nk_colorf){1.0f, 0.0f, 0.0f, 1.0f};
                new_node->next = color_head;
                color_head = new_node;
            }

            nk_tree_pop(ctx);
        }
        
        nk_layout_row_dynamic(ctx, 25, 2);
        nk_layout_row_dynamic(ctx, 25, 2);
        if(nk_button_label(ctx, "Discard")){
            remove_ui_component("editor styles");
            unlock_viewport();

            // free the LL memory
            editor_panel_styles_freelists();

            // release the json data
            json_decref(style_data);
            style_data = NULL;
        }
        if(nk_button_label(ctx, "Apply")){
            remove_ui_component("editor styles");
            unlock_viewport();
            
            // update the file to reflect the changes
            editor_panel_styles_encode();

            // free the LL memory
            editor_panel_styles_freelists();

            // TODO: in the future instead of freeing lets use the lists as a record for combo dropdowns to select without typing in inspector

            // release the json data
            json_decref(style_data);
            style_data = NULL;

            // to reflect these new changes, we need to reload the styles
            ye_clear_font_cache();
            ye_clear_color_cache();

            ye_pre_cache_styles("styles.yoyo");

            // recompute any text renderer textures because we might have changed their definitions for colors and fonts!
            struct ye_entity_node *current_entity = renderer_list_head;
            while(current_entity != NULL){
                if(current_entity->entity->renderer->type == YE_RENDERER_TYPE_TEXT || current_entity->entity->renderer->type == YE_RENDERER_TYPE_TEXT_OUTLINED){
                    ye_update_renderer_component(current_entity->entity);
                }
                current_entity = current_entity->next;
            }
        }
    }
    nk_end(ctx);
}