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

void editor_panel_credits(struct nk_context *ctx){
    if(nk_begin(ctx, "Credits", nk_rect((screenWidth / 2) - 200, (screenHeight / 2) - 200, 400, 400), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE)){
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Build Info:", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 20, 2);
        nk_label(ctx, "Yoyo Engine Version:", NK_TEXT_CENTERED);
        nk_label_colored(ctx, YE_ENGINE_VERSION, NK_TEXT_LEFT, nk_rgb(0,255,0));

        // nk_label(ctx, "Scene File Version:", NK_TEXT_CENTERED);
        // nk_label(ctx, YE_ENGINE_SCENE_VERSION, NK_TEXT_CENTERED);
        
        nk_layout_row_dynamic(ctx, 20, 2);
        nk_label(ctx, "Yoyo Editor Version:", NK_TEXT_CENTERED);
        nk_label_colored(ctx, YE_EDITOR_VERSION, NK_TEXT_LEFT, nk_rgb(0,255,0));

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "License:", NK_TEXT_CENTERED);
        nk_label_colored(ctx, "GNU GENERAL PUBLIC LICENSE Version 3", NK_TEXT_CENTERED, nk_rgb(0, 255, 0));
        
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Ryan Zmuda, 2023-2024", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Additional Contributors:", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 20, 2);
        nk_label(ctx, "Editor Icons:", NK_TEXT_CENTERED);
        nk_label(ctx, "Ben Mathes", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "External Libraries:", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 2);

        if(nk_button_label(ctx, "SDL")){
            #ifdef _WIN32
                system("start https://www.libsdl.org/") 
            #else
                system("xdg-open https://www.libsdl.org/");
            #endif
        }

        if(nk_button_label(ctx, "SDL_Mixer")){
            #ifdef _WIN32
                system("start https://www.libsdl.org/projects/SDL_mixer/") 
            #else
                system("xdg-open https://www.libsdl.org/projects/SDL_mixer/");
            #endif
        }

        if(nk_button_label(ctx, "SDL_IMG")){
            #ifdef _WIN32
                system("start https://www.libsdl.org/projects/SDL_image/") 
            #else
                system("xdg-open https://www.libsdl.org/projects/SDL_image/");
            #endif
        }

        if(nk_button_label(ctx, "SDL_TTF")){
            #ifdef _WIN32
                system("start https://www.libsdl.org/projects/SDL_ttf/") 
            #else
                system("xdg-open https://www.libsdl.org/projects/SDL_ttf/");
            #endif
        }

        if(nk_button_label(ctx, "uthash")){
            #ifdef _WIN32
                system("start https://github.com/troydhanson/uthash") 
            #else
                system("xdg-open https://github.com/troydhanson/uthash");
            #endif
        }

        if(nk_button_label(ctx, "Nuklear")){
            #ifdef _WIN32
                system("start https://github.com/Immediate-Mode-UI/Nuklear") 
            #else
                system("xdg-open https://github.com/Immediate-Mode-UI/Nuklear");
            #endif
        }

        if(nk_button_label(ctx, "jansson")){
            #ifdef _WIN32
                system("start https://github.com/akheron/jansson") 
            #else
                system("xdg-open https://github.com/akheron/jansson");
            #endif
        }

        // empty label to take space (odd number of items for even layouting)
        nk_label(ctx, "", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_layout_row_dynamic(ctx, 20, 1);
        if(nk_button_label(ctx, "Close")){
            remove_ui_component("editor_credits");
            unlock_viewport();
        }
    
        nk_end(ctx);
    }
}