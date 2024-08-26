/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <yoyoengine/yoyoengine.h>

#include "editor.h"
#include "editor_panels.h"
#include "editor_utils.h"

void editor_panel_credits(struct nk_context *ctx){
    if(nk_begin(ctx, "Credits", nk_rect((screenWidth / 2) - 200, (screenHeight / 2) - 200, 400, 400), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_TITLE)){
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Build Info:", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 20, 2);
        nk_label(ctx, "Yoyo Engine Version:", NK_TEXT_CENTERED);
        nk_label_colored(ctx, YOYO_ENGINE_VERSION_STRING, NK_TEXT_LEFT, nk_rgb(0,255,0));

        // nk_label(ctx, "Scene File Version:", NK_TEXT_CENTERED);
        // nk_label(ctx, YE_ENGINE_SCENE_VERSION, NK_TEXT_CENTERED);
        
        nk_layout_row_dynamic(ctx, 20, 2);
        nk_label(ctx, "Yoyo Editor Version:", NK_TEXT_CENTERED);
        nk_label_colored(ctx, YOYO_EDITOR_VERSION_STRING, NK_TEXT_LEFT, nk_rgb(0,255,0));

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "License:", NK_TEXT_CENTERED);
        nk_label_colored(ctx, "MIT", NK_TEXT_CENTERED, nk_rgb(0, 255, 0));
        
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Ryan Zmuda, 2023-2024", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Additional Contributors:", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 20, 2);
        nk_label(ctx, "Editor Icons:", NK_TEXT_CENTERED);
        nk_label(ctx, "Ben Mathes", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 2);
        nk_label(ctx, "Startup SFX:", NK_TEXT_CENTERED);
        nk_label(ctx, "Kaidiak", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "External Libraries:", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 2);

        if(nk_button_label(ctx, "SDL")){
            editor_open_in_system("https://www.libsdl.org/");
        }

        if(nk_button_label(ctx, "SDL_Mixer")){
            editor_open_in_system("https://www.libsdl.org/projects/SDL_mixer/");
        }

        if(nk_button_label(ctx, "SDL_IMG")){
            editor_open_in_system("https://www.libsdl.org/projects/SDL_image/");
        }

        if(nk_button_label(ctx, "SDL_TTF")){
            editor_open_in_system("https://www.libsdl.org/projects/SDL_ttf/");
        }

        if(nk_button_label(ctx, "SDL_Net")){
            editor_open_in_system("https://www.libsdl.org/projects/SDL_net/");
        }

        if(nk_button_label(ctx, "uthash")){
            editor_open_in_system("https://github.com/troydhanson/uthash");
        }

        if(nk_button_label(ctx, "Nuklear")){
            editor_open_in_system("https://github.com/Immediate-Mode-UI/Nuklear");
        }

        if(nk_button_label(ctx, "jansson")){
            editor_open_in_system("https://github.com/akheron/jansson");
        }

        if(nk_button_label(ctx, "zlib")){
            editor_open_in_system("https://zlib.net/");
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