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
        .volume = 10,
        .framecap = -1,
        .skipintro = true, // no override exists for this - its a boolean

        .handle_input = handle_input, // function for handling our input

        // make sure we declare overrides - i wish there were a better way but 
        // with compiler initialization of fields this is the best way i can think of
        .override_log_level = true,
        .override_volume = true,
        .override_framecap = true,
        // if you provide a false/bad override the engine will segfault...
    };

    // initialize engine with out data
    ye_init_engine(data);

    // createImage(1,.5,.5,1,1,ye_get_engine_resource_static("enginelogo.png"),true,ALIGN_MID_CENTER);
    // createImage(2,.5,.5,1,1,ye_get_resource_static("images/anno.png"),true,ALIGN_MID_CENTER); // idk just for fun
    // runscript(ye_get_resource_static("scripts/fib.lua"));

    // camera
    cam = ye_create_entity();
    ye_add_transform_component(cam, (SDL_Rect){0, 0, 0, 0}, 999, YE_ALIGN_MID_CENTER);
    ye_add_camera_component(cam, (SDL_Rect){0, 0, 1920, 1080});

    // set camera
    ye_set_camera(cam);

    struct ye_entity * entity = ye_create_entity();
    ye_add_transform_component(entity, (SDL_Rect){0, 0, 1920, 1080}, 0, YE_ALIGN_MID_CENTER);
    ye_temp_add_image_renderer_component(entity, ye_get_resource_static("images/wolf.jpeg"));

    struct ye_entity * anno = ye_create_entity();
    ye_add_transform_component(anno, (SDL_Rect){500, -500, 1920, 1080}, -1, YE_ALIGN_MID_CENTER);
    ye_temp_add_image_renderer_component(anno, ye_get_resource_static("images/anno.png"));

    struct ye_entity * origin = ye_create_entity();
    ye_add_transform_component(origin, (SDL_Rect){0, 0, 200, 200}, 700, YE_ALIGN_MID_CENTER);
    ye_temp_add_image_renderer_component(origin, ye_get_engine_resource_static("origin.png"));

    struct ye_entity * rei = ye_create_entity();
    ye_add_transform_component(rei, (SDL_Rect){600, -600, 1920, 1080}, -1, YE_ALIGN_MID_CENTER);
    ye_temp_add_image_renderer_component(rei, ye_get_resource_static("images/rei.png"));

    struct ye_entity * snerfbot = ye_create_entity();
    ye_add_transform_component(snerfbot, (SDL_Rect){-900, 900, 1920, 1080}, -1, YE_ALIGN_MID_CENTER);
    ye_temp_add_image_renderer_component(snerfbot, ye_get_resource_static("images/snerfbot.jpg"));

    TTF_Font * font = loadFont(ye_get_engine_resource_static("RobotoMono-Regular.ttf"), 40);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color red = {255, 0, 0, 255};

    struct ye_entity * nic = ye_create_entity();
    ye_add_transform_component(nic, (SDL_Rect){0, 0, 500, 300}, 800, YE_ALIGN_MID_CENTER);
    ye_temp_add_text_renderer_component(nic, "Hello, World!", font, &white);

    struct ye_entity * congratulations = ye_create_entity();
    ye_add_transform_component(congratulations, (SDL_Rect){0, 400, 500, 300}, 820, YE_ALIGN_MID_CENTER);
    ye_temp_add_animation_renderer_component(congratulations, ye_get_resource_static("animations/congratulations"), "jpg", 24, 100, -1);

    struct ye_entity * text = ye_create_entity();
    ye_add_transform_component(text, (SDL_Rect){0, 200, 500, 300}, 800, YE_ALIGN_MID_CENTER);
    ye_temp_add_text_outlined_renderer_component(text, "Congratulations!", font, &white, &red, 5);
    ye_add_physics_component(text, 1, 0); // TODO: we need to refactor transform to use floats instead of integers to go lower than this, and this is pretty fast for being the lowest
    text->renderer->alpha = 100;

    struct ye_entity * dummy = ye_create_entity_named("dummy");
    ye_rename_entity(dummy, "idiot");

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
        if(text->transform->rect.x < 0 || text->transform->rect.x > 1000) {
            text->physics->velocity.x *= -1;
        }
        
        ye_process_frame();
    }

    ye_shutdown_engine();
}