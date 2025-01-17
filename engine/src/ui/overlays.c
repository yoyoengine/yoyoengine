/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <string.h>

#include <yoyoengine/engine.h>
#include <yoyoengine/logging.h>
#include <yoyoengine/ui/overlays.h>
#include <yoyoengine/types/vector.h>

#include <yoyoengine/overlays/physics_overlay.h>

struct ye_vector *overlays = NULL;

void ye_init_overlays() {
    overlays = ye_new_vector(sizeof(struct ye_overlay));

    ye_logf(YE_LL_INFO, "Initialized overlays.\n");
}

void ye_shutdown_overlays() {
    YE_VECTOR_FOR_EACH(overlays, struct ye_overlay, overlay) {
        if(overlay.shutdown != NULL)
            overlay.shutdown();
    }
    ye_free_vector(overlays);
    overlays = NULL;

    ye_logf(YE_LL_INFO, "Shut down overlays.\n");
}

void ye_register_overlay(struct ye_overlay overlay) {
    /*
        Check if overlay with the same name already exists
    */
    YE_VECTOR_FOR_EACH(overlays, struct ye_overlay, existing_overlay) {
        if(strcmp(existing_overlay.name, overlay.name) == 0) {
            ye_logf(YE_LL_ERROR, "Failed to register overlay: %s (overlay with the same name already exists)\n", overlay.name);
            return;
        }
    }

    ye_vector_push_back(overlays, &overlay);

    if(overlay.init != NULL)
        overlay.init();

    ye_logf(YE_LL_DEBUG, "Registered overlay: %s\n", overlay.name);
}

void ye_unregister_overlay(const char *name) {
    YE_VECTOR_FOR_EACH_INDEX(overlays, struct ye_overlay, overlay, i) {
        if(strcmp(overlay.name, name) == 0) {
            if(overlay.shutdown != NULL)
                overlay.shutdown();

            ye_vector_remove(overlays, i);

            ye_logf(YE_LL_DEBUG, "Unregistered overlay: %s\n", name);
            return;
        }
    }

    ye_logf(YE_LL_ERROR, "Failed to unregister overlay: %s\n", name);
}

void ye_set_overlay_state(const char *name, bool active) {
    for(size_t i = 0; i < overlays->size; i++) {
        struct ye_overlay *overlay = (struct ye_overlay*)ye_vector_get(overlays, i);
        if(strcmp(overlay->name, name) == 0) {
            overlay->active = active;
            return;
        }
    }

    ye_logf(YE_LL_ERROR, "Failed to set overlay active: %s\n", name);
}

void ye_fire_overlay_event(enum ye_overlay_event event) {
    YE_VECTOR_FOR_EACH(overlays, struct ye_overlay, overlay) {

        if(!overlay.active)
            continue;

        switch(event) {
            case YE_OVERLAY_EVENT_RENDER_PRE_FRAME:
                if(overlay.render_pre_frame != NULL)
                    overlay.render_pre_frame();
                break;
            case YE_OVERLAY_EVENT_RENDER_POST_FRAME:
                if(overlay.render_post_frame != NULL)
                    overlay.render_post_frame();
                break;
            case YE_OVERLAY_EVENT_RENDER_UI:
                if(overlay.render_ui != NULL)
                    overlay.render_ui(YE_STATE.engine.ctx);
                break;
        }
    }
}

void ye_register_default_overlays() {
    struct ye_overlay overlay = {
        .name = "ye_overlay_physics",
        .active = false,
        .render_pre_frame = ye_physics_overlay_pre_frame_paint,
        .render_post_frame = ye_physics_overlay_post_frame_paint,
        .init = ye_physics_overlay_init,
        .shutdown = ye_physics_overlay_shutdown,
        .render_ui = ye_physics_overlay_render_ui_panel,
    };
    ye_register_overlay(overlay);
}

void ye_set_all_overlays(bool state) {
    for(size_t i = 0; i < overlays->size; i++) {
        struct ye_overlay *overlay = (struct ye_overlay*)ye_vector_get(overlays, i);
        overlay->active = state;
    }
}

bool ye_get_overlay_state(const char *name) {
    for(size_t i = 0; i < overlays->size; i++) {
        struct ye_overlay *overlay = (struct ye_overlay*)ye_vector_get(overlays, i);
        if(strcmp(overlay->name, name) == 0) {
            return overlay->active;
        }
    }

    ye_logf(YE_LL_ERROR, "Failed to get overlay active: %s\n", name);
    return false;
}