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

// global buffer that contains the status of the editor loading
char editor_loading_buffer[100];

void editor_panel_loading(struct nk_context *ctx){
    if(nk_begin(ctx, "Yoyo Editor", nk_rect((screenWidth / 2) - 150, (screenHeight / 2) - 50, 300, 100), NK_WINDOW_BORDER|NK_WINDOW_TITLE)){
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_label(ctx, editor_loading_buffer, NK_TEXT_CENTERED);
        nk_end(ctx);
    }
}