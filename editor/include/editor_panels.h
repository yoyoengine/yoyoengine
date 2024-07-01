/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/*
    One mega header to combine all the src/panels/.c files

    This will make it easier to handle without adding new headers
    for each panel, beacuse they really are just one public function per panel
    with maybe some private helpers.
*/
#ifndef YE_EDITOR_PANELS_H
#define YE_EDITOR_PANELS_H

#include <yoyoengine/yoyoengine.h>

void editor_panel_keybinds(struct nk_context *ctx);

void editor_panel_credits(struct nk_context *ctx);

void editor_panel_styles(struct nk_context *ctx);

void editor_panel_tricks(struct nk_context *ctx);

extern char editor_loading_buffer[100];
void editor_panel_loading(struct nk_context *ctx);

void editor_panel_scene_settings(struct nk_context *ctx);

#endif