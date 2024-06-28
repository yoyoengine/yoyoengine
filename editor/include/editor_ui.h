/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
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

#ifndef YE_EDITOR_UI_H
#define YE_EDITOR_UI_H

extern float editor_selection_group_x;
extern float editor_selection_group_y;
extern float editor_selection_last_group_x;
extern float editor_selection_last_group_y;
#define ye_reset_editor_selection_group() editor_selection_group_x = 0; editor_selection_group_y = 0; editor_selection_last_group_x = 0; editor_selection_last_group_y = 0

void ye_editor_paint_hiearchy(struct nk_context *ctx);

/*
    The entity preview will be a snapshot of the entity when selected, it will stash current state, and allow editing of the entity until
    dev clicks "save" or "cancel"
*/
void ye_editor_paint_inspector(struct nk_context *ctx);

/*
    TODO:
    - paintbounds somehow starts out checked even though its not, so have to double click to turn on
    - if console is opened when we tick one of these, closing the console crashes
*/
void ye_editor_paint_options(struct nk_context *ctx);

void ye_editor_paint_menu(struct nk_context *ctx);

#endif // YE_EDITOR_UI