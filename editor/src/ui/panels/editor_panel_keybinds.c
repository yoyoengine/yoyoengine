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
        nk_label(ctx, "Left Click - Select Object", NK_TEXT_LEFT);

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