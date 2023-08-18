#include "../../build/linux/yoyoengine/lib/engine.h"
#include "../../build/linux/yoyoengine/lib/graphics.h"

#include <stdbool.h>

int main() {
    struct engine_data data = {
        .engine_resources_path = "../../build/linux/yoyoengine/engine_resources",
        .log_level = 0, // 0,
        .debug_mode = true,
    };

    initEngine(data);
    
    createImage(1,.5,.5,1,1,getEngineResourceStatic("enginelogo.png"),true,ALIGN_MID_CENTER);

    bool quit = false;
    SDL_Event e;

    while(!quit) {
        while (SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        // render frame
        renderAll();
    }

    shutdownEngine();
}