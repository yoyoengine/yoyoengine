#include <yoyoengine/yoyoengine.h>

#include <stdbool.h>

int main() {
    struct engine_data data = {
        .engine_resources_path = "../../build/linux/yoyoengine/engine_resources",
        .log_level = 0, // 0,
        .debug_mode = true,
    };

    initEngine(data);
    
    createImage(1,.5,.5,1,1,getEngineResourceStatic("enginelogo.png"),true,ALIGN_MID_CENTER);

    runscript(getResourceStatic("scripts/fib.lua"));

    bool quit = false;
    SDL_Event e;

    while(!quit) {
        ui_begin_input_checks();
        while (SDL_PollEvent(&e)) {
            ui_handle_input(&e);
            switch(e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }
        ui_end_input_checks();

        // render frame
        renderAll();
    }

    shutdownEngine();
}