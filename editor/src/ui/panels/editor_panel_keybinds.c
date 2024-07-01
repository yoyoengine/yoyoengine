/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <yoyoengine/yoyoengine.h>

#include "editor.h"
#include "editor_panels.h"

void editor_panel_keybinds(struct nk_context *ctx){
    if(nk_begin(ctx, "Keybinds", nk_rect((screenWidth / 2) - 250, (screenHeight / 2) - 150, 500, 350), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE)){
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Keybinds", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Left Click - Select hovered viewport entity", NK_TEXT_LEFT);

        nk_label(ctx, "Right Click - Pan Camera", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Scroll Wheel - Zoom Camera", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Back Quote (`) - Open Console", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "CTRL + S - Save current scene", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "CTRL + R - Build and run the current project", NK_TEXT_LEFT);
        
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "CTRL + SHIFT + R - Reload the current scene", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_layout_row_dynamic(ctx, 20, 1);
        if(nk_button_label(ctx, "Close")){
            remove_ui_component("editor keybinds");
            unlock_viewport();
        }
    
        nk_end(ctx);
    }
}