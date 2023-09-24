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
    Use Nuklear to add some editor ui as well as a smaller viewport synced to the current scene event

    Goals:
    - allow easily creating new render objects and events as well as dragging them around to resize and reorient
    - we need a way to put the viewport in a corner or even a seperate window?

    do we want this to live in this folder that its in rn? how to seperate the engine from the core? it needs to ship with the core

    Constraints:
    - editor only supported on linux

    TODO:
    - figure out the viewport position and size and calculate where other windows go
        - this involves going back to the engine and polishing the old shit you wrote
*/

#include <stdio.h>
#include <yoyoengine/yoyoengine.h>

// make some editor specific declarations to change engine core behavior
#define YE_EDITOR

// global variables
bool quit = false;

/*
    Registered input handler
*/
void handle_input(SDL_Event event) {
    if(event.type == SDL_QUIT) {
        quit = true;
    }
    else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            default:
                break;
        }
    }
}



// EDITOR PANELS //



void ye_editor_paint_hiearchy(struct nk_context *ctx){
    if (nk_begin(ctx, "Heiarchy", nk_rect(250, 10, 100, 200),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Heiarchy", NK_TEXT_LEFT);
        nk_end(ctx);
    }
}



///////////////////



/*
    main function
    accepts one string argument of the path to the project folder
*/
int main(int argc, char **argv) {
    // get our path from the command line
    char *path = argv[1];

    // print the path
    printf("path: %s\n", path);

    struct engine_data data = {
        .engine_resources_path = "../../build/linux/yoyoengine/engine_resources",
        // .game_resources_path = path,
        .window_title = "Yoyo Engine Editor",
        .log_level = 0,
        .volume = 32,
        .handle_input = handle_input,
        .editor_mode = true,

        .override_log_level = true,
        .override_window_title = true,
        .override_volume = true,
    };
    ye_init_engine(data);

    // create our editor camera and register it with the engine
    struct ye_entity *editor_camera = ye_create_entity_named("editor_camera");
    ye_add_transform_component(editor_camera, (struct ye_rectf){0, 0, 0, 0}, 999, YE_ALIGN_MID_CENTER);
    ye_add_camera_component(editor_camera, (SDL_Rect){0, 0, 1920, 1080});
    ye_set_camera(editor_camera);

    // create a grid backdrop image
    // struct ye_entity *grid = ye_create_entity_named("grid");
    // ye_add_transform_component(grid, (struct ye_rectf){0, 0, 1920, 1080}, 0, YE_ALIGN_MID_CENTER);
    // ye_temp_add_image_renderer_component(grid, ye_get_engine_resource_static("grid.jpeg"));

    // register all editor ui components
    ui_register_component("heiarchy",ye_editor_paint_hiearchy);

    while(!quit) {
        ye_process_frame();
    }

    ye_shutdown_engine();

    // exit
    return 0;
}
