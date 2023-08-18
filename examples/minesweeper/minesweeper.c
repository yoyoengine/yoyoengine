#include "../../build/linux/yoyoengine/lib/engine.h"

#include <stdbool.h>

int main() {
    struct engine_data data = {
        .engine_resources_path = "../../build/linux/yoyoengine/engine_resources",
        .log_level = 0, // 0,
        .debug_mode = true,
    };

    initEngine(data);
    shutdownEngine();
}

// see notebook for next steps