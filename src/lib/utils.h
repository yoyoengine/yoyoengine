#ifndef UTILS_H
#define UTILS_H

// used to align things inside bounds
typedef enum {
    ALIGN_TOP_LEFT, ALIGN_TOP_CENTER, ALIGN_TOP_RIGHT,
    ALIGN_MID_LEFT, ALIGN_MID_CENTER, ALIGN_MID_RIGHT,
    ALIGN_BOT_LEFT, ALIGN_BOT_CENTER, ALIGN_BOT_RIGHT,
    ALIGN_STRETCH // for cases where we dgaf about alignment and just want to stretch anything to bounds
} Alignment;

#endif