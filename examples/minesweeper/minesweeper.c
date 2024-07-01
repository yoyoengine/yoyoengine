/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <yoyoengine/yoyoengine.h>

#include <stdbool.h>

bool quit = false;

struct ye_entity * cam;

/*
    Game's handle input function, this is where you will handle input for your game
    (the engine does its own checks for inputs for reserved functions and throws any sdl events back to here
    for the game to proccess)
*/
void handle_input(SDL_Event event) {
    if(event.type == SDL_QUIT) {
        quit = true;
    }
    else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            // case SDLK_w:
            //     cam->camera->view_field.w += 1920;
            //     cam->camera->view_field.h += 1080;
            //     break;
            // case SDLK_s:
            //     cam->camera->view_field.w -= 1920;
            //     cam->camera->view_field.h -= 1080;
            //     break;
            default:
                break;
        }
    }
}

int main() {
    struct engine_data data = {
        .engine_resources_path = "../../build/linux/yoyoengine/engine_resources",
        // .log_file_path = "resources/debug.log",
        .log_level = 0, // 0,
        .debug_mode = true, // no override exists for this - its a boolean
        .volume = 0,
        .framecap = -1,
        .window_mode = 0,
        // .screen_width = 2500,
        // .screen_height = 1080,
        // .override_screen_width = true,
        // .override_screen_height = true,
        // .skipintro = true, // no override exists for this - its a boolean

        .handle_input = handle_input, // function for handling our input

        // make sure we declare overrides - i wish there were a better way but 
        // with compiler initialization of fields this is the best way i can think of
        .override_log_level = true,
        .override_volume = true,
        .override_framecap = true,
        .override_window_mode = true,
        // if you provide a false/bad override the engine will segfault...

        // .stretch_viewport = true,
    };

    // initialize engine with out data
    ye_init_engine(data);

    // load styles for this scene
    // ye_pre_cache_styles(ye_get_resource_static("styles.yoyo"));

    // runscript(ye_get_resource_static("scripts/fib.lua"));

    // camera
    cam = ye_create_entity_named("camera");
    ye_add_transform_component(cam, (struct ye_rectf){0, 0, 0, 0}, 999, YE_ALIGN_MID_CENTER);
    ye_add_camera_component(cam, (SDL_Rect){0, 0, 2560, 1440});

    // set camera
    ye_set_camera(cam);

    struct ye_entity *splash = ye_create_entity();
    ye_add_transform_component(splash, (struct ye_rectf){0, 0, 1920, 1080}, 0, YE_ALIGN_MID_CENTER);
    ye_temp_add_image_renderer_component(splash, ye_get_resource_static("images/studiozoogies.png"));

    playSound(ye_get_resource_static("sfx/boot.mp3"),-1,0); // play startup sound

    // get current ticks
    int ticks = SDL_GetTicks();

    // until we are 4000 ticks in the future
    while(SDL_GetTicks() - ticks < 4000){
        // fade in from alpha=0 to alpha=255 over 1 second
        if(SDL_GetTicks() - ticks < 1000){
            splash->renderer->alpha = (SDL_GetTicks() - ticks) / 4;
        }
        // sustain at alpha=255 from 1 to 3 seconds
        else if(SDL_GetTicks() - ticks < 3000){
            splash->renderer->alpha = 255;
        }
        // fade out from alpha=255 to alpha=0 over 1 second
        else {
            splash->renderer->alpha = 255 - ((SDL_GetTicks() - ticks - 3000) / 4);
        }
        
        // process frame
        ye_process_frame();
    }

    ye_destroy_entity(splash);

    // playSound(ye_get_resource_static("music/24songs.mp3"),-1,-1); // play music

    // struct ye_entity * entity = ye_create_entity();
    // ye_add_transform_component(entity, (struct ye_rectf){0, 0, 1920, 1080}, 0, YE_ALIGN_MID_CENTER);
    // ye_temp_add_image_renderer_component(entity, ye_get_resource_static("images/wolf.jpeg"));

    // struct ye_entity * anno = ye_create_entity();
    // ye_add_transform_component(anno, (struct ye_rectf){500, -500, 1920, 1080}, -1, YE_ALIGN_MID_CENTER);
    // ye_temp_add_image_renderer_component(anno, ye_get_resource_static("images/anno.png"));

    // struct ye_entity * origin = ye_create_entity();
    // ye_add_transform_component(origin, (struct ye_rectf){0, 0, 200, 200}, 700, YE_ALIGN_MID_CENTER);
    // ye_temp_add_image_renderer_component(origin, ye_get_engine_resource_static("origin.png"));

    // struct ye_entity * rei = ye_create_entity();
    // ye_add_transform_component(rei, (struct ye_rectf){600, -600, 1920, 1080}, -1, YE_ALIGN_MID_CENTER);
    // ye_temp_add_image_renderer_component(rei, ye_get_resource_static("images/rei.png"));

    // struct ye_entity * snerfbot = ye_create_entity();
    // ye_add_transform_component(snerfbot, (struct ye_rectf){-900, 900, 1920, 1080}, -1, YE_ALIGN_MID_CENTER);
    // ye_temp_add_image_renderer_component(snerfbot, ye_get_resource_static("images/snerfbot.jpg"));

    // ye_cache_font("Roboto40", 40, ye_get_engine_resource_static("RobotoMono-Regular.ttf"));
    // // TTF_Font * font = ye_load_font(ye_get_engine_resource_static("RobotoMono-Regular.ttf"), 40);
    // SDL_Color white = {255, 255, 255, 255};
    // SDL_Color red = {255, 0, 0, 255};
    // ye_cache_color("white", white);
    // ye_cache_color("red", red);

    // struct ye_entity * nic = ye_create_entity();
    // ye_add_transform_component(nic, (struct ye_rectf){0, 0, 500, 300}, 800, YE_ALIGN_MID_CENTER);
    // ye_temp_add_text_renderer_component(nic, "Hello, World!", ye_font("PressStart2P-24"), ye_color("white"));

    // struct ye_entity * congratulations = ye_create_entity();
    // ye_add_transform_component(congratulations, (struct ye_rectf){0, 400, 500, 300}, 820, YE_ALIGN_MID_CENTER);
    // ye_temp_add_animation_renderer_component(congratulations, ye_get_resource_static("animations/congratulations"), "jpg", 24, 100, -1);

    // struct ye_entity * congrats2 = ye_duplicate_entity(congratulations);
    // congrats2->transform->rect.y = 700;

    // struct ye_entity * text = ye_create_entity();
    // ye_add_transform_component(text, (struct ye_rectf){0, 200, 500, 300}, 800, YE_ALIGN_MID_CENTER);
    // ye_temp_add_text_outlined_renderer_component(text, "Congratulations!", ye_font("PressStart2P-24"), ye_color("black"), &red, 5);
    // ye_add_physics_component(text, 100, 0); // TODO: we need to refactor transform to use floats instead of integers to go lower than this, and this is pretty fast for being the lowest

    // // create some text entities that say "flipped x" "flipped y" and "flipped xy"
    // struct ye_entity * flipped_x = ye_create_entity();
    // ye_add_transform_component(flipped_x, (struct ye_rectf){800, 200, 500, 300}, 600, YE_ALIGN_MID_CENTER);
    // ye_temp_add_text_renderer_component(flipped_x, "flipped x", ye_font("Roboto40"), &white);
    // flipped_x->transform->flipped_x = true;

    // struct ye_entity * flipped_y = ye_create_entity();    
    // ye_add_transform_component(flipped_y, (struct ye_rectf){800, 300, 500, 300}, 600, YE_ALIGN_MID_CENTER);
    // ye_temp_add_text_renderer_component(flipped_y, "flipped y", ye_font("Roboto40"), &white);
    // flipped_y->transform->flipped_y = true;

    // struct ye_entity * flipped_xy = ye_create_entity();
    // ye_add_transform_component(flipped_xy, (struct ye_rectf){800, 400, 500, 300}, 600, YE_ALIGN_MID_CENTER);
    // ye_temp_add_text_renderer_component(flipped_xy, "flipped xy", ye_font("Roboto40"), &white);
    // flipped_xy->transform->flipped_x = true;
    // flipped_xy->transform->flipped_y = true;

    // struct ye_entity * dummy = ye_create_entity_named("dummy");
    // ye_rename_entity(dummy, "idiot");

    // struct ye_entity *cat = ye_create_entity_named("cat");
    // ye_add_transform_component(cat, (struct ye_rectf){1000, 800, 500, 500}, 0, YE_ALIGN_MID_CENTER);
    // ye_temp_add_image_renderer_component(cat, ye_get_resource_static("images/cat.png"));
    // ye_add_physics_component(cat, 0, 0);
    // cat->physics->rotational_velocity = 45.0;
    // cat->transform->center = (SDL_Point){100, 100};

    ye_destroy_entity(ye_find_entity_named("camera"));

    ye_load_scene(ye_get_resource_static("scenes/main.yoyo"));
    // ye_logf(info, "current scene: %s\n", ye_get_scene_name());
    // struct ye_entity * denji = ye_find_entity_named("denji");
    // if(denji == NULL) {
    //     ye_logf(error, "denji is null\n");
    // }
    // else {
    //     ye_logf(info, "denji is not null\n");
    // }
    // ye_temp_add_image_renderer_component(denji, ye_get_resource_static("images/rei.png"));
    // // intentionally create an object with a bad texture to see missing
    // struct ye_entity * missing = ye_create_entity_named("missing");
    // ye_add_transform_component(missing, (struct ye_rectf){1000, 800, 500, 500}, 10, YE_ALIGN_MID_CENTER);
    // ye_temp_add_image_renderer_component(missing, ye_get_resource_static("images/missing.png"));

    /*
        Main game loop. We can do any logic the game needs and then tell the engine to
        load and present the next frame, as well as handle its own logic. The engine will
        call appropriate callbacks back into game code as needed during this
    */
    while(!quit) {

        /*
            Any other game frame logic...
            (Hopefully a lot of actions can be moved into
            callbacks so nothing much would need to happen here)
        */

        // if the x of text exceeds below zero or above 1000 reverse the velocity
        // if(text->transform->rect.x < 0 || text->transform->rect.x > 1000) {
        //     text->physics->velocity.x *= -1;
        // }

        // // increase alpha til 255 then decreate to zero then repeat (for text)
        // text->renderer->alpha += 3;
        // if(text->renderer->alpha > 255) {
        //     text->renderer->alpha = 0;
        // }
        
        ye_process_frame();
    }

    ye_shutdown_engine();
}