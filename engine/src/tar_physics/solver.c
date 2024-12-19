/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/


#include <stdbool.h>
#include <float.h>

#include <Lilith.h>

#include <yoyoengine/types.h>
#include <yoyoengine/ecs/ecs.h>

// struct ye_interval_2d {
//     float min;
//     float max;
// };

///////////////////////////
// HELPERS
///////////////////////////

// float _get_rect_size(struct ye_point_rectf rect) {
//     float size = 0;
//     struct ye_pointf p1 = rect.verticies[0];
//     for(int i = 0; i < 4; i++) {

//     }
// }

///////////////////////////
// DETECTION
///////////////////////////

/*
    https://stackoverflow.com/questions/10962379/how-to-check-intersection-between-2-rotated-rectangles

    https://pastebin.com/03BigiCn

    Separating Axis Theorum - works for convex polygons and probably wasting cycles for just rects
*/
bool ye_detect_rect_rect_collision(struct ye_point_rectf rect1, struct ye_point_rectf rect2) {
    for(int polyi = 0; polyi < 2; polyi++) {
        struct ye_point_rectf rect = polyi == 0 ? rect1 : rect2;

        for(int i1 = 0; i1 < 4; i1++) {
            int i2 = (i1 + 1) % 4;

            float normalx = rect.verticies[i2].y - rect.verticies[i1].y;
            float normaly = rect.verticies[i2].x - rect.verticies[i1].x;

            float mina = FLT_MAX;
            float maxa = -FLT_MAX;
            for(int ai = 0; ai < 4; ai++) {
                float projected = (normalx * rect1.verticies[ai].x) + (normaly * rect1.verticies[ai].y);
                if(projected < mina) mina = projected;
                if(projected > maxa) maxa = projected;
            }

            float minb = FLT_MAX;
            float maxb = -FLT_MIN;
            for(int bi = 0; bi < 4; bi++) {
                float projected = (normalx * rect2.verticies[bi].x) + (normaly * rect2.verticies[bi].y);
                if(projected < minb) minb = projected;
                if(projected > maxb) maxb = projected;
            }

            if(maxa < minb || maxb < mina)
                return false;
        }
    }
    return true;
}

///////////////////////////
// SOLVERS
///////////////////////////