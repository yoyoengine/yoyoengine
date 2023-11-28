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

void editor_panel_styles(struct nk_context *ctx)
{
    if (nk_begin(ctx, "Styles", nk_rect(screenWidth / 2 - 500, screenHeight / 2 - 400, 1000, 800), 
    NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE)) {
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
        
        if(nk_button_label(ctx, "Close")){
            remove_ui_component("editor styles");
            unlock_viewport();
        }
    }
    nk_end(ctx);
}