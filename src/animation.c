#include "lib/engine.h"
#include "lib/graphics.h"
#include "lib/logging.h"
#include "lib/animation.h"
#include "lib/linkedlist.h"

// declared in graphics.c, hold an unused unique increasing integer ID
extern int global_id;

/*
    Creates an animation and returns its render object ID
*/
int createAnimation(char *path, char *extension, int frameCount, int frameDelay, int loops, int depth, float x, float y, float w, float h, bool centered, Alignment alignment){ // TODO: update prototype for more compatibility with other engine functions
    // create the animation data object
    AnimationData data = {
        .pAnimationPath = path,
        .pImageFormat = extension,
        .frameCount = (size_t)frameCount,
        .frameDelay = frameDelay,
        .lastUpdated = 0, // we will replace this once we are ready to place the final object into the render queue
        .loops = loops,
        .currentFrameIndex = 0,
        .frames = (SDL_Texture**)malloc(frameCount * sizeof(SDL_Texture*)),
        .paused = false
    };

    // load all the frames into memory TODO: this could be futurely optimized
    for (size_t i = 0; i < (size_t)frameCount; ++i) {
        char filename[256];  // Assuming a maximum filename length of 255 characters
        snprintf(filename, sizeof(filename), "%s/%d%s", path, (int)i, extension); // TODO: dumb optimization but could cut out all except frame num insertion here

        struct textureInfo textInfo = createImageTexture(filename,false); // we will not cache any animation frames;
        data.frames[i] = textInfo.pTexture;

    }

    // align our object in its bounds TODO: please just move this to a util
    SDL_Rect bounds = createRealPixelRect(centered, x, y, w, h);
    int imgWidth, imgHeight;
    SDL_QueryTexture(data.frames[0], NULL, NULL, &imgWidth, &imgHeight);
    SDL_Rect rect = {0,0,imgWidth,imgHeight};
    autoFitBounds(&bounds, &rect, alignment);

    // now we construct our renderObject and make sure its looking at the first frame
    renderObject staged = {
        .identifier = global_id,
        .depth = depth,
        .type = renderType_Animation,
        .pTexture = data.frames[0], // first frame
        .rect = rect,
        .bounds = bounds,
        .pNext = NULL,
        .cachedTexture = false, // should never be cached
        .centered = centered,
        .alignment = alignment,
        {
            .AnimationData = data
        }
    };

    // lets set the ticks of our new animation now that we have done the heavy lifting
    staged.AnimationData.lastUpdated = SDL_GetTicks();

    // add the object
    addRenderObject(staged);

    // increment our global id and return the previous id
    global_id++;
    return global_id - 1;
}

/*
    Checks if the SDL_GetTicks() - .lastUpdated > .frameDelay
    and if it is we move the SDL_Texture over in the array by one,
    updating the renderObject's current texture
*/
void attemptTickAnimation(void *objPtr) {
    renderObject *obj = (renderObject *)objPtr; // ugly hack im using to avoid circular dependency from graphics.h
    int current = SDL_GetTicks();

    if(current - obj->AnimationData.lastUpdated > (int)obj->AnimationData.frameDelay) {
        obj->AnimationData.currentFrameIndex++; // increment next frame
        // wrap to frame zero if needed
        if(obj->AnimationData.currentFrameIndex > (int)obj->AnimationData.frameCount - 1){
            obj->AnimationData.currentFrameIndex = 0;
            if(obj->AnimationData.loops != -1){
                obj->AnimationData.loops--;
                if(obj->AnimationData.loops <= 0){
                    removeRenderObject(obj->identifier);
                }
            }
        }
        obj->pTexture = obj->AnimationData.frames[obj->AnimationData.currentFrameIndex];
        obj->AnimationData.lastUpdated = current;
    }
}

/*
    Free all memory of an animation
    FOR INTERNAL USE ONLY. DESTROY GAME OBJECTS ONCE NOT THEIR ANIMATIONS
*/
void destroyAnimation(void *animation){
    renderObject *data = (renderObject*)animation;

    // destroy the texture of every frame
    for(size_t i = 0; i<data->AnimationData.frameCount; i++){
        SDL_DestroyTexture(data->AnimationData.frames[i]);
    }

    // free the heap frame list
    free(data->AnimationData.frames);
}