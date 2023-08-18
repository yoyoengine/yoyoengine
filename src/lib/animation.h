#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL2/SDL.h>

#include "utils.h"
#include "graphics.h"

// animation datastructure
typedef struct AnimationData {
    char *pAnimationPath; // path to animation folder
    char *pImageFormat; // format of image files in animation

    size_t frameCount; // number of frames in animation

    int frameDelay; // delay between frames in ms
    int lastUpdated; // SDL_GetTicks() last frame advance
    int loops; // number of loops, -1 for infinite
    int currentFrameIndex; // current frame index

    SDL_Texture** frames; // array of textures for each frame

    bool paused;
} AnimationData;

int createAnimation(char *path, char *extension, int frameCount, int frameDelay, int loops, int depth, float x, float y, float w, float h, bool centered, Alignment alignment);

void attemptTickAnimation(void *objPtr);

void destroyAnimation(void *animation);

#endif