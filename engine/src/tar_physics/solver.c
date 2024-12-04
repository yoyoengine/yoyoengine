// /*
//     This file is a part of yoyoengine. (https://github.com/yoyoengine)
//     Copyright (C) 2023-2024  Ryan Zmuda

//     Licensed under the MIT license. See LICENSE file in the project root for details.
// */

// /////////////////////////////////////

// #include <math.h>
// #include <stdbool.h>

// #include <yoyoengine/types.h>
// #include <yoyoengine/ecs/ecs.h>

// // detection

// bool ye_detect_circle_circle_collision(struct ye_vec2f pos1, float radius1, struct ye_vec2f pos2, float radius2){
//     float dx = pos1.x - pos2.x;
//     float dy = pos1.y - pos2.y;
//     float distance = sqrt(dx * dx + dy * dy);
//     return distance < radius1 + radius2;
// }

// bool ye_detect_circle_rect_collision(struct ye_vec2f circle_pos, float circle_radius, struct ye_rectf rect, float rotation) {
//     // 1. Translate circle to rectangle's local space
//     float rect_center_x = rect.x + rect.w / 2;
//     float rect_center_y = rect.y + rect.h / 2;

//     float sin_rot = sinf(-rotation); // Negate to reverse rotation
//     float cos_rot = cosf(-rotation);

//     // Circle's position relative to rectangle center
//     float local_circle_x = cos_rot * (circle_pos.x - rect_center_x) - sin_rot * (circle_pos.y - rect_center_y);
//     float local_circle_y = sin_rot * (circle_pos.x - rect_center_x) + cos_rot * (circle_pos.y - rect_center_y);

//     // 2. Treat rectangle as axis-aligned in local space
//     float dx = fabs(local_circle_x);
//     float dy = fabs(local_circle_y);

//     if (dx > rect.w / 2 + circle_radius) return false; // Circle too far in X
//     if (dy > rect.h / 2 + circle_radius) return false; // Circle too far in Y

//     if (dx <= rect.w / 2) return true; // Circle overlaps rectangle horizontally
//     if (dy <= rect.h / 2) return true; // Circle overlaps rectangle vertically

//     // 3. Check corner distance
//     float corner_distance = (dx - rect.w / 2) * (dx - rect.w / 2) +
//                             (dy - rect.h / 2) * (dy - rect.h / 2);

//     return corner_distance <= circle_radius * circle_radius;
// }

// // helpers
// void ye_get_rect_corners(struct ye_rectf rect, float cos_rot, float sin_rot, struct ye_vec2f* corners) {
//     float hw = rect.w / 2; // Half width
//     float hh = rect.h / 2; // Half height

//     // Local corner offsets
//     struct ye_vec2f offsets[4] = {
//         { -hw, -hh }, { hw, -hh }, { hw, hh }, { -hw, hh }
//     };

//     // Rotate and translate corners
//     for (int i = 0; i < 4; i++) {
//         float rotated_x = offsets[i].x * cos_rot - offsets[i].y * sin_rot;
//         float rotated_y = offsets[i].x * sin_rot + offsets[i].y * cos_rot;
//         corners[i].x = rect.x + rotated_x;
//         corners[i].y = rect.y + rotated_y;
//     }
// }

// struct ye_vec2f ye_get_normal(struct ye_vec2f point1, struct ye_vec2f point2) {
//     struct ye_vec2f edge = { point2.x - point1.x, point2.y - point1.y };
//     return (struct ye_vec2f){ -edge.y, edge.x }; // Perpendicular vector
// }

// void ye_project_onto_axis(struct ye_vec2f* corners, int count, struct ye_vec2f axis, float* min, float* max) {
//     *min = *max = (corners[0].x * axis.x + corners[0].y * axis.y); // Dot product

//     for (int i = 1; i < count; i++) {
//         float projection = (corners[i].x * axis.x + corners[i].y * axis.y);
//         if (projection < *min) *min = projection;
//         if (projection > *max) *max = projection;
//     }
// }

// bool ye_overlap_on_axis(struct ye_vec2f* corners1, struct ye_vec2f* corners2, struct ye_vec2f axis) {
//     // Project corners of both rectangles onto the axis
//     float min1, max1, min2, max2;
//     ye_project_onto_axis(corners1, 4, axis, &min1, &max1);
//     ye_project_onto_axis(corners2, 4, axis, &min2, &max2);

//     // Check for overlap
//     return !(max1 < min2 || max2 < min1); // Return false if no overlap
// }

// bool ye_detect_rect_rect_collision(struct ye_rectf rect1, float rotation1, struct ye_rectf rect2, float rotation2) {
//     // Step 1: Precompute cosine and sine for both rectangles
//     float cos1 = cosf(rotation1), sin1 = sinf(rotation1);
//     float cos2 = cosf(rotation2), sin2 = sinf(rotation2);

//     // Step 2: Get the four corners of both rectangles
//     struct ye_vec2f corners1[4], corners2[4];
//     ye_get_rect_corners(rect1, cos1, sin1, corners1);
//     ye_get_rect_corners(rect2, cos2, sin2, corners2);

//     // Step 3: Define the axes to test
//     struct ye_vec2f axes[4] = {
//         ye_get_normal(corners1[1], corners1[0]),  // Axis 1 from rect1
//         ye_get_normal(corners1[3], corners1[0]),  // Axis 2 from rect1
//         ye_get_normal(corners2[1], corners2[0]),  // Axis 1 from rect2
//         ye_get_normal(corners2[3], corners2[0])   // Axis 2 from rect2
//     };

//     // Step 4: Check for overlaps on all axes
//     for (int i = 0; i < 4; i++) {
//         if (!ye_overlap_on_axis(corners1, corners2, axes[i])) {
//             return false; // Separation found, no collision
//         }
//     }

//     // Step 5: If no separation axis, collision detected
//     return true;
// }

// /////////////////////////////////////

// NOTE: DO IT YOURSELF NO CHATGPT ORACLE SHIT!