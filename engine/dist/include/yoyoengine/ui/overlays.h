/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_OVERLAYS_H
#define YE_OVERLAYS_H

#include <stdbool.h>

// get nuklear from logging
#include <yoyoengine/logging.h>

#include <yoyoengine/export.h>
#include <yoyoengine/types/vector.h>

/*
    This is a system to allow compartmentalizing and persisting overlays in the engine/game.

    Examples of this would be the debug overlay, including its children (physics overlay, etd)
*/

extern struct ye_vector *overlays;

struct ye_overlay {
    char name[128];
    bool active;

    void (*init)();
    void (*shutdown)();

    void (*render_pre_frame)();
    void (*render_post_frame)();

    void (*render_ui)(struct nk_context *ctx);
};

enum ye_overlay_event {
    YE_OVERLAY_EVENT_RENDER_PRE_FRAME,
    YE_OVERLAY_EVENT_RENDER_POST_FRAME,

    YE_OVERLAY_EVENT_RENDER_UI
};

YE_API void ye_init_overlays();

YE_API void ye_shutdown_overlays();

YE_API void ye_register_overlay(struct ye_overlay overlay);

YE_API void ye_unregister_overlay(const char *name);

YE_API void ye_set_overlay_state(const char *name, bool active);

YE_API void ye_fire_overlay_event(enum ye_overlay_event event);

YE_API void ye_register_default_overlays();

#endif // YE_OVERLAYS_H