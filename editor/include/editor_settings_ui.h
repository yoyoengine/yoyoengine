/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_EDITOR_SETTINGS_UI_H
#define YE_EDITOR_SETTINGS_UI_H

void editor_settings_ui_init();

void editor_settings_ui_shutdown();

void ye_editor_paint_project_settings(struct nk_context *ctx);
void ye_editor_paint_project(struct nk_context *ctx);

#endif // YE_EDITOR_SETTINGS_UI_H