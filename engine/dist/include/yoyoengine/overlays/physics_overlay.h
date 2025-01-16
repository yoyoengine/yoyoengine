/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef YE_PHYSICS_OVERLAY_H
#define YE_PHYSICS_OVERLAY_H

#include <yoyoengine/export.h>
#include <yoyoengine/ui/overlays.h>

YE_API void ye_physics_overlay_pre_frame_paint();
YE_API void ye_physics_overlay_post_frame_paint();
YE_API void ye_physics_overlay_render_ui_panel(struct nk_context *ctx);
YE_API void ye_physics_overlay_init();
YE_API void ye_physics_overlay_shutdown();

#endif // YE_PHYSICS_OVERLAY_H