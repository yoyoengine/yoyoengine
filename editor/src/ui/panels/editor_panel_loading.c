/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
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