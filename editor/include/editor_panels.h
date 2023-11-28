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

#endif