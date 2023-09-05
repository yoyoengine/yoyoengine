#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <jansson.h>

#include <yoyoengine/yoyoengine.h>

// define globals for file
SDL_Window *pWindow = NULL;
SDL_Surface *pScreenSurface = NULL;
SDL_Renderer *pRenderer = NULL;

renderObject *pRenderListHead = NULL;
button *pButtonListHead = NULL;

// int that increments each renderObject created, allowing new unique id's to be assigned
int global_id = 0;
int objectCount = 0;
int lastObjectCount = 0;
int lastChunkCount = 0;
int lastLinesWritten = 0;

// NOTE: negative global_id's are reserved for engine components, 
// and traversing the list to clear renderObjects will only clear 
// non engine renderObjects

// fps related counters
int fpsUpdateTime = 0;
int frameCounter = 0;
int fps = 0;
int startTime = 0;
int fpscap = 0;
int desiredFrameTime = 0;  

// initialize some variables in engine to track screen size
float targetAspectRatio = 16.0f / 9.0f;
int virtualWidth = 1920;
int virtualHeight = 1080;
int xOffset = 0;
int yOffset = 0;

bool forceRefresh = false;

// internal state to paint bounds of objects to the screen
bool paintBounds = false;

int currentResolutionWidth = 1920;
int currentResolutionHeight = 1080;

char * render_scale_quality = "linear"; // also available: best (high def, high perf), nearest (sharp edges, pixel-y)

// create a cache to hold textures colors and fonts
VariantCollection* cache;

// toggles painting bounds in graphics
void togglePaintBounds(){
    paintBounds = !paintBounds;
    logMessage(debug, "Toggled paintBounds\n");
}

// helper function to get renderObjectType as a string from the enum name
char *getRenderObjectTypeString(renderObjectType type) {
    switch (type) {
        case renderType_Text:
            return "Text";
        case renderType_Image:
            return "Image";
        case renderType_Button:
            return "Button";
        default:
            return "Unknown";
    }
}

// converts a relative float to real screen pixel width
int convertToRealPixelWidth(float in){
    return (int)((float)in * (float)virtualWidth);
}

// converts a relative float to real screen pixel height
int convertToRealPixelHeight(float in){
    return (int)((float)in * (float)virtualHeight);
}

// creates a real screen pixel rect from relative float values
SDL_Rect createRealPixelRect(bool centered, float x, float y, float w, float h) {
    int realX = convertToRealPixelWidth(x);
    int realY = convertToRealPixelHeight(y);
    int realW = convertToRealPixelWidth(w);
    int realH = convertToRealPixelHeight(h);

    if (centered) {
        realX -= realW / 2;
        realY -= realH / 2;
    }

    SDL_Rect rect = {realX, realY, realW, realH};
    return rect;
}

/*
    definitive render object creator.
    will take in a staged render object, will fill out REAL rect coordinate values
    and will insert the render object into the renderObject queue in order of depth.
    everything except relative rect coords needs to be filled out in the staging object
*/
void addRenderObject(renderObject staging) {
    // construct and malloc new object
    renderObject *pObj = (renderObject *)malloc(sizeof(renderObject));

    // assign our staged renderObject to our new heap object
    *pObj = staging; // TODO: does this work?

    // if there are no renderObjects in the list, or the depth of this object is lower than the head
    if (pRenderListHead == NULL || pObj->depth < pRenderListHead->depth) {
        // make this object the head
        pObj->pNext = pRenderListHead; 
        pRenderListHead = pObj;
    } 
    else {
        // iterate through the renderObject queue starting at the head
        renderObject *pCurrent = pRenderListHead;

        // while there is a next item and the next items depth is less than 
        // our current objects depth, keep going
        while (pCurrent->pNext != NULL && pCurrent->pNext->depth < pObj->depth) {
            pCurrent = pCurrent->pNext;
        }

        // once we know where our object sits:
        pObj->pNext = pCurrent->pNext; // make our object point to the one after our current
        pCurrent->pNext = pObj; // make our current point to our object

        /*
            Object has been inserted inbetween current and next in order of depth

            object = [2]
            
            BEFORE:
            [0]->[1]->[3]

            AFTER:
            [0]->[1]->[2]->[3]
        */
    }
    
    if(pObj->centered){
        // char buffer[100];
        // snprintf(buffer, sizeof(buffer),  "Added renderObject id#%d\n", staging.identifier);
        // logMessage(debug, buffer);
    }
    else{
        // char buffer[100];
        // snprintf(buffer, sizeof(buffer),  "Added renderObject %s id#%d absolutely at (%d,%d) %dx%d\n",getRenderObjectTypeString(type),identifier,objX,objY,objWidth,objHeight);
        // logMessage(debug, buffer);
    }
    objectCount++;
}

// remove a render object from the queue by its identifier
void removeRenderObject(int identifier) {
    // debug output
    // char buffer[100];
    // snprintf(buffer, sizeof(buffer),  "Remove render object id#%d\n",identifier);
    // logMessage(debug, buffer);
    
    // if our render list has zero items
    if (pRenderListHead == NULL) {
        logMessage(warning, "ERROR REMOVING RENDER OBJECT: HEAD IS NULL\n");
        return;
        // alarm and pass
    }

    // if the head is the item we are looking to remove
    if (pRenderListHead->identifier == identifier) {
        // pop our head into a new temp object
        renderObject *pToDelete = pRenderListHead;

        // set our head to the previous 2nd item
        pRenderListHead = pRenderListHead->pNext;

        if(pToDelete->type == renderType_Animation) {
            // pass the animation data as a void pointer to clear all the frames out
            destroyAnimation(pToDelete);
        }

        // delete the texture of our previous head (if its not cached)
        if(!pToDelete->cachedTexture){
            SDL_DestroyTexture(pToDelete->pTexture);
        } // this allows any cache textures to be freed separately

        // free our previous head from memory
        free(pToDelete);

        // finalize
        objectCount--;
        return;
    }

    // create a temp renderObject pointer to increment the list
    renderObject *pCurrent = pRenderListHead;

    /*
        [1,x]->[2,x]->[3,0x0]
    HEAD ^
    */

    // while the next struct is not null and the next identifier is not our desired ID
    while (pCurrent->pNext != NULL && pCurrent->pNext->identifier != identifier) {
        // scoot over by one
        pCurrent = pCurrent->pNext;
    } // when this resolves, next will match desired ID or NULL

    // if we found the ID to remove
    if (pCurrent->pNext != NULL) {
        // copy our struct to delete to a temp var
        renderObject *pToDelete = pCurrent->pNext;

        // set our current next to what the deleted next pointed to
        pCurrent->pNext = pToDelete->pNext;

        // destroy the texture of our node to be deleted (if its not cached)
        if(!pToDelete->cachedTexture){
            SDL_DestroyTexture(pToDelete->pTexture);
        } // this allows any cache textures to be freed separately

        // free our node from memory
        free(pToDelete);

        objectCount--;
    }
    else{
        // if we couldnt find the ID, alarm
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "ERROR: COULD NOT FIND RENDER OBJECT WITH ID='%d' TO DELETE\n",identifier);
        logMessage(error, buffer);
    }
}

void removeButton(int id){
    // step through button LL and find which node has the ID of the renderobject passed
    // then, we can remove that node from the LL and free it and call removeRenderObject() on the renderObject
    if(pButtonListHead == NULL){
        // logMessage(warning, "ERROR REMOVING BUTTON: HEAD IS NULL\n");
    }
    else{
        button *pCurrent = pButtonListHead;
        while(pCurrent->pNext != NULL && pCurrent->pNext->pObject->identifier != id){
            pCurrent = pCurrent->pNext;
        }
        if(pCurrent->pNext != NULL){
            button *pToDelete = pCurrent->pNext;
            pCurrent->pNext = pToDelete->pNext;

            // remove callback data (game hadnles its own callbackData)
            free(pToDelete->callbackData->callbackType);
            free(pToDelete->callbackData);

            free(pToDelete);
            removeRenderObject(id);
        }
        else{
            char buffer[100];
            snprintf(buffer, sizeof(buffer),  "ERROR: COULD NOT FIND BUTTON WITH ID#%d TO DELETE\n",id);
            logMessage(error, buffer);
        }
    }
}

// helper function for clearAll() to remove all buttons from the button LL and their render objects
void clearAllButtons(){
    if(pButtonListHead == NULL){
        // logMessage(warning, "ERROR REMOVING ALL BUTTONS: HEAD IS NULL\n");
    }
    else{
        button *pCurrent = pButtonListHead;
        while(pCurrent != NULL){
            button *pToDelete = pCurrent;
            pCurrent = pCurrent->pNext;

            // char buffer[100];
            // snprintf(buffer, sizeof(buffer),  "Remove button object id#%d\n", pToDelete->pObject->identifier);
            // logMessage(debug, buffer);
            removeRenderObject(pToDelete->pObject->identifier);
            
            // we decref the json_t object inside the callbackData struct, because its a new ref so it will be freed
            // json_decref(pToDelete->callbackData->pJson); TODO: handle callback data leaks

            // we free our dynamically allocated fields
            free(pToDelete->callbackData->callbackType); // free the malloced type string
            free(pToDelete->callbackData); // free the malloced callbackData struct
            
            free(pToDelete); // free button object
        }
        pButtonListHead = NULL;
    }
}

// returns FIRST render object struct by ID, returns NULL if nonexistant
renderObject *getRenderObject(int identifier) {
    // traversal temp var
    renderObject *pCurrent = pRenderListHead;
    // while traversal var isnt null
    while (pCurrent != NULL) {
        // if our current ID matches the desired identifier
        if (pCurrent->identifier == identifier) {
            return pCurrent; // return our current
        }
        // else increment
        pCurrent = pCurrent->pNext;
    }
    // if no object exists with identifier, return NULL
    return NULL;
}

// load a font into memory and return a pointer to it
// TODO: evaluate where this stands in relation to getFont functionality, does this just extend the backend of get Font?
TTF_Font *loadFont(char *pFontPath, int fontSize) {
    if(fontSize > 500){
        logMessage(error, "ERROR: FONT SIZE TOO LARGE\n");
        return NULL;
    }
    char *fontpath = pFontPath;
    if(access(fontpath, F_OK) == -1){
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Could not access file '%s'.\n", fontpath);
        logMessage(error, buffer);
    }
    TTF_Font *pFont = TTF_OpenFont(fontpath, fontSize);
    if (pFont == NULL) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Failed to load font: %s\n", TTF_GetError());
        logMessage(error, buffer);
        return NULL;
    }
    char buffer[100];
    snprintf(buffer, sizeof(buffer),  "Loaded font: %s\n", pFontPath);
    logMessage(debug, buffer);
    return pFont;
}

/*
    https://gamedev.stackexchange.com/questions/119642/how-to-use-sdl-ttfs-outlines
    
    NOTE: The actual width modifier depends on the resolution of the passed font.
    if we want to allow better consistency over the width between different res,
    we need to identify the font by its loaded width (can be stored with its data in cache)
    and then we could pass in a relative width modifier
*/
SDL_Texture *createTextTextureWithOutline(const char *pText, int width, TTF_Font *pFont, SDL_Color *pColor, SDL_Color *pOutlineColor) {
    int temp = TTF_GetFontOutline(pFont);

    SDL_Surface *fg_surface = TTF_RenderText_Blended(pFont, pText, *pColor); 

    TTF_SetFontOutline(pFont, width);
    
    SDL_Surface *bg_surface = TTF_RenderText_Blended(pFont, pText, *pOutlineColor); 
    
    SDL_Rect rect = {width, width, fg_surface->w, fg_surface->h}; 

    /* blit text onto its outline */ 
    SDL_SetSurfaceBlendMode(fg_surface, SDL_BLENDMODE_BLEND); 
    SDL_BlitSurface(fg_surface, NULL, bg_surface, &rect); 
    SDL_FreeSurface(fg_surface); 
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, bg_surface);
    SDL_FreeSurface(bg_surface);
    
    // error out if texture creation failed
    if (pTexture == NULL) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Failed to create texture: %s\n", SDL_GetError());
        logMessage(error, buffer);
        return NULL;
    }
    
    TTF_SetFontOutline(pFont, temp);

    return pTexture;
}

// Create a texture from text string with specified font and color, returns NULL for failure
SDL_Texture *createTextTexture(const char *pText, TTF_Font *pFont, SDL_Color *pColor) {
    // create surface from parameters
    SDL_Surface *pSurface = TTF_RenderUTF8_Blended(pFont, pText, *pColor); // MEMLEAK: valgrind says so but its not my fault, internal in TTF
    
    // error out if surface creation failed
    if (pSurface == NULL) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Failed to render text: %s\n", TTF_GetError());
        logMessage(error, buffer);
        return NULL;
    }

    // create texture from surface
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);

    // error out if texture creation failed
    if (pTexture == NULL) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Failed to create texture: %s\n", SDL_GetError());
        logMessage(error, buffer);
        return NULL;
    }

    // free the surface memory
    SDL_FreeSurface(pSurface);

    // return the created texture
    return pTexture;
}

// Create a texture from image path, returns its texture pointer and a flag on whether
// or not the texture is also cached
struct textureInfo createImageTexture(char *pPath, bool shouldCache) {
    // try to get it from cache TODO: should we wrap this in if shouldCache?
    Variant *pVariant = getVariant(cache, pPath);
    if (pVariant != NULL) { // found in cache
        // logMessage(debug, "Found texture in cache\n");

        pVariant->refcount++; // increase refcount

        struct textureInfo ret = {pVariant->textureValue, true};
        // printf("refcount for %s: %d\n",pPath,pVariant->refcount);
        return ret;
    }
    else{ // not found in cache
        if(access(pPath, F_OK) == -1){
            char buffer[100];
            snprintf(buffer, sizeof(buffer),  "Could not access file '%s'.\n", pPath);
            logMessage(error, buffer);
            return (struct textureInfo){NULL, NULL}; // TODO: give this a placeholder texture for failures
        }

        // create surface from loading the image
        SDL_Surface *pImage_surface = IMG_Load(pPath);
        
        // error out if surface load failed
        if (!pImage_surface) {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),  "Error loading image: %s\n", IMG_GetError());
            logMessage(error, buffer);
            exit(1); // FIXME
        }

        // create texture from surface
        SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pImage_surface);
        
        // error out if texture creation failed
        if (!pTexture) {
            char buffer[100];
            snprintf(buffer, sizeof(buffer),  "Error creating texture: %s\n", SDL_GetError());
            logMessage(error, buffer);
            exit(1); // FIXME
        }

        // release surface from memory
        SDL_FreeSurface(pImage_surface);
        if(shouldCache){ // we are going to cache it
            // cache the texture
            Variant variant;
            variant.type = VARIANT_TEXTURE;
            variant.textureValue = pTexture;
            variant.refcount = 1;

            addVariant(cache, pPath, variant);

            // char buffer[100];
            // snprintf(buffer, sizeof(buffer),  "Cached a texture. key: %s\n", pPath);
            // logMessage(debug, buffer);

            if(getVariant(cache, pPath) == NULL){
                logMessage(error, "ERROR CACHING TEXTURE\n");
            }

            struct textureInfo ret = {pTexture, true};
            
            // return the created texture
            // printf("refcount for %s: %d\n",pPath,variant.refcount);
            return ret;

        }
        else{ // we are not going to cache it
            struct textureInfo ret = {pTexture, false}; 

            return ret;
        }
    }
}

/*
    takes in a key (relative resource path to a font) and returns a 
    pointer to the loaded font (caches the font if it doesnt exist in cache)
*/
TTF_Font *getFont(char *key){
    Variant *v = getVariant(cache, key);
    if(v == NULL){
        Variant fontVariant;
        fontVariant.type = VARIANT_FONT;
        fontVariant.fontValue = loadFont(key,100);

        addVariant(cache, key, fontVariant);
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Cached a font. key: %s\n", key);
        logMessage(debug, buffer);
        v = getVariant(cache, key);
    }
    else{
        // logMessage(debug, "Found cached font.\n");
    }
    return v->fontValue;
}

/*
    takes in a key (relative resource path to a font) and returns a 
    pointer to the loaded font (caches the font if it doesnt exist in cache)

    This is mainly here for the game, the engine cant utilize this super well as
    it doesnt have an understanding of named colors, it can just cache them for quick
    access by the game
*/
SDL_Color *getColor(char *key, SDL_Color color){
    Variant *v = getVariant(cache, key);
    if(v == NULL){
        Variant colorVariant;
        colorVariant.type = VARIANT_COLOR;
        colorVariant.colorValue = color;

        addVariant(cache, key, colorVariant);
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Cached a color. key: %s\n", key);
        logMessage(debug, buffer);
        v = getVariant(cache, key);
    }
    else{
        // logMessage(debug, "Found cached color.\n");
    }
    SDL_Color *pColor = &v->colorValue;
    return pColor;
}

/*
    Massive function to handle the orientation of an object within a set of bounds
*/
void autoFitBounds(SDL_Rect* bounds, SDL_Rect* obj, Alignment alignment){
    // check if some loser wants to stretch something
    if(alignment == ALIGN_STRETCH){
        obj->w = bounds->w;
        obj->h = bounds->h;
        obj->x = bounds->x;
        obj->y = bounds->y;
        return;
    }

    // actual orientation handling

    float boundsAspectRatio = (float)bounds->w / (float)bounds->h;
    float objectAspectRatio = (float)obj->w / (float)obj->h;

    if (objectAspectRatio > boundsAspectRatio) {
        obj->h = bounds->w / objectAspectRatio;
        obj->w = bounds->w;
    } else {
        obj->w = bounds->h * objectAspectRatio;
        obj->h = bounds->h;
    }

    switch(alignment) {
        case ALIGN_TOP_LEFT:
            obj->x = bounds->x;
            obj->y = bounds->y;
            break;
        case ALIGN_TOP_CENTER:
            obj->x = bounds->x + (bounds->w - obj->w) / 2;
            obj->y = bounds->y;
            break;
        case ALIGN_TOP_RIGHT:
            obj->x = bounds->x + (bounds->w - obj->w);
            obj->y = bounds->y;
            break;
        case ALIGN_MID_LEFT:
            obj->x = bounds->x;
            obj->y = bounds->y + (bounds->h - obj->h) / 2;
            break;
        case ALIGN_MID_CENTER:
            obj->x = bounds->x + (bounds->w - obj->w) / 2;
            obj->y = bounds->y + (bounds->h - obj->h) / 2;
            break;
        case ALIGN_MID_RIGHT:
            obj->x = bounds->x + (bounds->w - obj->w);
            obj->y = bounds->y + (bounds->h - obj->h) / 2;
            break;
        case ALIGN_BOT_LEFT:
            obj->x = bounds->x;
            obj->y = bounds->y + (bounds->h - obj->h);
            break;
        case ALIGN_BOT_CENTER:
            obj->x = bounds->x + (bounds->w - obj->w) / 2;
            obj->y = bounds->y + (bounds->h - obj->h);
            break;
        case ALIGN_BOT_RIGHT:
            obj->x = bounds->x + (bounds->w - obj->w);
            obj->y = bounds->y + (bounds->h - obj->h);
            break;
        default:
            logMessage(error, "Invalid alignment\n");
            break;
    }
}

// add text to the render queue, returns the engine assigned ID of the object
int createText(int depth, float x,float y, float width, float height, char *pText, TTF_Font *pFont, SDL_Color *pColor, bool centered, Alignment alignment){
    SDL_Texture *texture = createTextTexture(pText, pFont, pColor);

    // get our real coordinate bounds for the object
    SDL_Rect bounds = createRealPixelRect(centered, x, y, width, height);
    
    int imgWidth, imgHeight;
    SDL_QueryTexture(texture, NULL, NULL, &imgWidth, &imgHeight);
    SDL_Rect rect = {0,0,imgWidth,imgHeight};
    autoFitBounds(&bounds, &rect, alignment);
    
    // construct a staging object and throw it to addRenderObject()
    renderObject staging = {
        global_id,
        depth,
        renderType_Text,
        texture,
        rect,
        NULL,
        false, // text textures never cached
        bounds,
        centered,
        alignment,
        {
            .TextData = {
                .pFont = pFont,
                .outlineSize = 0,
                .pColor = pColor,
                .pOutlineColor = NULL,
                .pText = pText
            }
        } // TODO: update for createTextWithOutline
    };
    
    addRenderObject(staging);
    global_id++; // increment the global ID for next object
    return global_id - 1; //return 1 less than after incrementation (id last item was assigned)
}


// add an image to the render queue, returns the engine assigned ID of the object
int createImage(int depth, float x, float y, float width, float height, char *pPath, bool centered, Alignment alignment){
    struct textureInfo info = createImageTexture(pPath,true);

    // get our real coordinate bounds for the object
    SDL_Rect bounds = createRealPixelRect(centered, x, y, width, height);
    
    int imgWidth, imgHeight;
    SDL_QueryTexture(info.pTexture, NULL, NULL, &imgWidth, &imgHeight);
    SDL_Rect rect = {0,0,imgWidth,imgHeight};
    autoFitBounds(&bounds, &rect, alignment);

    // construct a staging object and throw it to addRenderObject()
    renderObject staging = {
        global_id,
        depth,
        renderType_Image,
        info.pTexture,
        rect,
        NULL,
        info.cached,
        bounds,
        centered,
        alignment,
        {
            .ImageData = {
                .pPath = pPath
            }
        }
    };
    
    addRenderObject(staging);
    global_id++; // increment the global ID for next object
    return global_id - 1; //return 1 less than after incrementation (id last item was assigned)
}

/*
    method to create an engine button
    Takes in a string path to the background, font, text color, relative x, relative y, relative width, relative height
    CONSIDERATIONS / TODO: 
    - formatting the text such that it can be passed left, center, or right aligned and does not stretch to fill 
    - refactor texture rendering to external function so button textures can be generated and replaced externally in the future, for now buttons are static (maybe that texture can be auto modified by pointer in struct)
    - eventually we need to support outlined text in buttons, think about the best way to accomplish that
*/
int createButton(int depth, float x, float y, float width, float height, char *pText, TTF_Font *pFont, SDL_Color *pColor, bool centered, char *pBackgroundPath, struct callbackData *data, Alignment alignment) {
    // get our real coordinate bounds for the object
    SDL_Rect bounds = createRealPixelRect(centered, x, y, width, height);

    SDL_Texture *textTexture = createTextTexture(pText, pFont, pColor);

    if (textTexture == NULL) {
        logMessage(error, "ERROR CREATING TEXT TEXTURE FOR BUTTON\n");
        return intFail;
    }

    struct textureInfo info = createImageTexture(pBackgroundPath,true);
    SDL_Texture *pImageTexture = info.pTexture;

    if(pImageTexture == NULL){
        logMessage(error, "ERROR CREATING IMAGE TEXTURE FOR BUTTON\n");
        SDL_DestroyTexture(textTexture);
        return intFail;
    }

    int imgWidth, imgHeight;
    SDL_QueryTexture(pImageTexture, NULL, NULL, &imgWidth, &imgHeight);
    SDL_Rect imgRect = {0,0,imgWidth,imgHeight};
    autoFitBounds(&bounds, &imgRect, alignment);

    SDL_Texture* buttonTexture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, imgWidth, imgHeight);

    if (buttonTexture == NULL) {
        logMessage(error, "ERROR CREATING BUTTON TEXTURE\n");
        SDL_DestroyTexture(textTexture);
        return intFail;
    }

    // Set the new texture as the render target
    SDL_SetRenderTarget(pRenderer, buttonTexture);

    // Render the background image onto the new texture
    SDL_Rect backgroundRect = {0, 0, imgWidth, imgHeight};
    SDL_RenderCopy(pRenderer, pImageTexture, NULL, &backgroundRect);

    // Get dimensions of the text texture
    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

    // just text alignement...? ///////////////////////////////////////////

    float widthRatio = (float)imgWidth / textWidth;
    float heightRatio = (float)imgHeight / textHeight;
    float scale = fminf(widthRatio, heightRatio);

    int scaledTextWidth = (int)(textWidth * scale);
    int scaledTextHeight = (int)(textHeight * scale);

    // Calculate the position of the text to center it within the button
    SDL_Rect textRect;
    textRect.x = (imgWidth - scaledTextWidth) / 2;
    textRect.y = (imgHeight - scaledTextHeight) / 2;
    textRect.w = scaledTextWidth;
    textRect.h = scaledTextHeight;

    ///////////////////////////////////////////////////////////////////////

    // Render the text onto the new texture
    SDL_RenderCopy(pRenderer, textTexture, NULL, &textRect);

    // Reset the render target to the default
    SDL_SetRenderTarget(pRenderer, NULL);

    global_id++; // to stay consistant, increment now and refer to global_id - 1 when accessing ID

    renderObject staging = {
        global_id - 1,
        depth,
        renderType_Button,
        buttonTexture,
        imgRect,
        NULL,
        false, // total baked button texture should never be cached
        bounds,
        centered,
        alignment,
        {
            .ButtonData = {
                .TextData = {
                    .pFont = pFont,
                    .outlineSize = 0,
                    .pColor = pColor,
                    .pOutlineColor = NULL,
                    .pText = pText
                },
                .ImageData = {
                    .pPath = pBackgroundPath,
                }
            }
        }
    };

    addRenderObject(staging);

    renderObject *pObj = getRenderObject(global_id - 1);

    // construct and malloc new button
    button *pButton = (button *)malloc(sizeof(button));
    pButton->pObject = pObj;
    pButton->pNext = NULL;
    pButton->callbackData = data;

    // Add the new button to the linked list
    // (sorted by depth, highest at head)
    if(pButtonListHead == NULL){
        pButtonListHead = pButton;
    }
    else{
        button *pCurrent = pButtonListHead;
        while(pCurrent->pNext != NULL && pCurrent->pNext->pObject->depth < pButton->pObject->depth){
            pCurrent = pCurrent->pNext;
        }
        pButton->pNext = pCurrent->pNext;
        pCurrent->pNext = pButton;
    }

    // Cleanup
    SDL_DestroyTexture(textTexture);

    return global_id - 1; // for consistancy
}

/*
    Update text by id
*/
void updateTextByObj(renderObject *pObj, char *pText){
    if(pObj == NULL){
        logMessage(error, "ERROR UPDATING TEXT: RENDER OBJECT NOT FOUND\n");
        return;
    }
    if(pObj->type != renderType_Text){
        logMessage(error, "ERROR UPDATING TEXT: RENDER OBJECT IS NOT TEXT\n");
        return;
    }
    SDL_DestroyTexture(pObj->pTexture);
    SDL_Texture *texture = createTextTexture(pText, pObj->TextData.pFont, pObj->TextData.pColor);
    
    // query stuff so we can resize the rect (not bounds)
    int imgWidth, imgHeight;
    SDL_QueryTexture(texture, NULL, NULL, &imgWidth, &imgHeight);
    SDL_Rect rect = {0,0,imgWidth,imgHeight};
    autoFitBounds(&pObj->bounds, &rect, pObj->alignment);

    pObj->pTexture = texture;
    pObj->TextData.pText = pText;
    
    // update rect
    pObj->rect = rect;
}

void updateText(int id, char *pText){
    renderObject *pObj = getRenderObject(id);
    if(pObj == NULL){
        logMessage(error, "ERROR UPDATING TEXT: RENDER OBJECT NOT FOUND\n");
        return;
    }
    if(pObj->type != renderType_Text){
        logMessage(error, "ERROR UPDATING TEXT: RENDER OBJECT IS NOT TEXT\n");
        return;
    }
    // TODO DESTROY BY REFCOUNTING - should i? text textures are never cached I thought?
    SDL_DestroyTexture(pObj->pTexture);
    SDL_Texture *texture = createTextTexture(pText, pObj->TextData.pFont, pObj->TextData.pColor);
    
    // query stuff so we can resize the rect (not bounds)
    int imgWidth, imgHeight;
    SDL_QueryTexture(texture, NULL, NULL, &imgWidth, &imgHeight);
    SDL_Rect rect = {0,0,imgWidth,imgHeight};
    autoFitBounds(&pObj->bounds, &rect, pObj->alignment);

    pObj->pTexture = texture;
    pObj->TextData.pText = pText;

    // update rect
    pObj->rect = rect;
}

/*
    Will attempt to grab a variant by name, and decrease its refcount, destroying it if it hits 0
*/
void imageDecref(char *name){
    Variant *v = getVariant(cache, name);
    if(v == NULL){
        logMessage(error, "ERROR DECREFFING IMAGE: IMAGE NOT FOUND\n");
        return;
    }
    v->refcount--;
    if(v->refcount <= 0){
        removeVariant(cache,name); // this is responsible for destroying the texture
    }
}

/*
    Updates an image given its renderObject ID and a path to the new src
*/
void updateImage(int id, char *pSrc){
    renderObject *pObj = getRenderObject(id);
    if(pObj == NULL){
        logMessage(error, "ERROR UPDATING IMAGE: RENDER OBJECT NOT FOUND\n");
        return;
    }
    if(pObj->type != renderType_Image){
        printf("type: %d\n", pObj->type);
        logMessage(error, "ERROR UPDATING IMAGE: RENDER OBJECT IS NOT IMAGE\n");
        return;
    }
    imageDecref(pObj->ImageData.pPath);
    struct textureInfo info = createImageTexture(pSrc, true);
    pObj->ImageData.pPath = pSrc;
    pObj->pTexture = info.pTexture;
}

// function that clears all non engine render objects (depth >= 0)
// TODO: refactor this and removeRenderObject() to send pointers to nodes to another function to genericise this
void clearAll(bool includeEngine) {
    // If our render list has zero items
    if (pRenderListHead == NULL) {
        // logMessage(warning, "ERROR CLEARING ALL RENDER OBJECTS: HEAD IS NULL\n");
        return; // alarm and exit
    }

    // attempt to clear all buttons
    clearAllButtons();

    renderObject *pCurrent = pRenderListHead;
    renderObject *pNext = NULL;
    renderObject *pPrev = NULL; // Declaration of pPrev

    while (pCurrent != NULL) {
        pNext = pCurrent->pNext;

        if (includeEngine || pCurrent->identifier >= 0) {
            // Delete the current object as we are either deleting everything or the current object is always deletable
            // char buffer[100];
            // snprintf(buffer, sizeof(buffer),  "Remove render object id#%d\n", pCurrent->identifier);
            // logMessage(debug, buffer);
            
            if(pCurrent->type == renderType_Animation) {
                // pass the animation data as a void pointer to clear all the frames out
                destroyAnimation(pCurrent);
            }

            // Delete the texture of our current object (if its not cached) (also if this is an animation this has already been freed)
            else if(!pCurrent->cachedTexture){
                SDL_DestroyTexture(pCurrent->pTexture);
            } // this allows any cache textures to be freed separately
            
            free(pCurrent);
            objectCount--;
        } else {
            // Pass as we have encountered an engine object that we don't want to delete
            pPrev = pCurrent;
        }

        pCurrent = pNext;

        // Update pPrev if necessary
        if (pPrev != NULL) {
            pPrev->pNext = pCurrent;
        } else {
            pRenderListHead = pCurrent;
        }
    }

    // If we cleared the whole list, set pRenderListHead to NULL
    if (includeEngine && pPrev == NULL) {
        pRenderListHead = NULL;
    }
    // clear cache
    clearVariantCollection(cache); // TODO: evaluate when cache should be cleared intelligently with refcounts
}

// render everything in the scene
void renderAll() {
    int frameStart = SDL_GetTicks();

    // Set background color to black
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);

    // Clear the window with the set background color
    SDL_RenderClear(pRenderer);

    // create iteration var for render list
    renderObject *pCurrent = pRenderListHead;

    // Get paint start timestamp
    Uint32 paintStartTime = SDL_GetTicks();

    // while iteration var is not null
    while (pCurrent != NULL) {

        // if we are counting debug stuff TODO: add a way for engine to know its displaying overlays
        if(1){
            // increment the frame counter
            frameCounter++;

            // if we have waited long enough to update the display
            if (SDL_GetTicks() - fpsUpdateTime >= 250) {
                // get the elapsed time and scale it to our time factor to get fps
                fpsUpdateTime = SDL_GetTicks();
                fps = frameCounter * 4;
                frameCounter = 0; // reset counted frames
            }

        }

        // if we are an animation, we want to go to the next frame if its available 
        // TODO: should we hard cap this to not run every frame? how expensive is this if we profile it
        if(pCurrent->type == renderType_Animation)
            attemptTickAnimation(pCurrent);

        // render our current object
        SDL_RenderCopy(pRenderer, pCurrent->pTexture, NULL, &(pCurrent->rect));
        
        if(paintBounds){
            // draw a red rectangle showing objects bounds
            SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(pRenderer, &(pCurrent->bounds));

            // draw a green rectangle showing the rect
            SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);
            SDL_RenderDrawRect(pRenderer, &(pCurrent->rect));
        }

        // increment
        pCurrent = pCurrent->pNext;
    }

    // Get paint end timestamp
    Uint32 paintEndTime = SDL_GetTicks();

    // Calculate paint time
    Uint32 paintTime = paintEndTime - paintStartTime;

    if(1){ // TODO: see above todo
        ui_paint_debug_overlay(fps,paintTime,objectCount,totalChunks,linesWritten);
    }

    // update ui (TODO: profile if this is an expensive op)
    ui_render();    

    // present our new changes to the renderer
    SDL_RenderPresent(pRenderer);

    // update the window to reflect the new renderer changes
    SDL_UpdateWindowSurface(pWindow);

    // if we arent on vsync we need to preform some frame calculations to delay next frame
    if(fpscap != -1){
        // set the end of the render frame
        int frameEnd = SDL_GetTicks();

        // calculate the current frame time
        int frameTime = frameEnd - frameStart;

        // check the desired FPS cap and add delay if needed
        if (frameTime < desiredFrameTime) {
            SDL_Delay(desiredFrameTime - frameTime);
        }
    }

    if(forceRefresh){
        forceRefresh = false;
    }
}

// function that traverses our LL of buttons and returns the highest depth
// button clicked by ID, NULL if none
void checkClicked(int x, int y){
    // create a temp button pointer to increment the list
    button *pCurrent = pButtonListHead;

    // while the next struct is not null
    while (pCurrent != NULL) {
        // check if we have clicked inside the button
        if (x >= pCurrent->pObject->rect.x +xOffset &&
            x <= pCurrent->pObject->rect.x + pCurrent->pObject->rect.w +xOffset &&
            y >= pCurrent->pObject->rect.y +yOffset &&
            y <= pCurrent->pObject->rect.y + pCurrent->pObject->rect.h + yOffset) 
        {
            // run the buttons callback if its not null
            if(pCurrent->callbackData->callback != NULL && pCurrent->callbackData != NULL && pCurrent->callbackData->pData != NULL){
                logMessage(debug, "Button clicked, running callback\n");
                pCurrent->callbackData->callback(pCurrent->callbackData);
                logMessage(debug, "Callback finished\n");
                return;
            }
            else{
                logMessage(warning, "ERROR: CLICKED BUTTON CALLBACK IS NULL\n");
                return;
            }
            // return pCurrent->pObject->identifier; // return our current
        }
        // else increment
        pCurrent = pCurrent->pNext;
    }
    // if no object exists with identifier, return NULL
    //return intFail;
}

// method to ensure our game content is centered and scaled well
void setViewport(int screenWidth, int screenHeight){
    // get our current aspect ratio
    float currentAspectRatio = (float)screenWidth / (float)screenHeight;

    // if aspect ratio is too wide
    if (currentAspectRatio >= targetAspectRatio) {
        // set our width to be the max that can accomidate the height
        virtualWidth = (int)(screenHeight * targetAspectRatio);
        virtualHeight = screenHeight;
        xOffset = (screenWidth - virtualWidth) / 2;
    } 
    // if aspect ratio is too tall
    else {
        // set our width to be the max that can fit
        virtualWidth = screenWidth;
        virtualHeight = screenWidth / targetAspectRatio;
        yOffset = (screenHeight - virtualHeight) / 2;
    }

    // debug outputs
    char buffer[100];
    snprintf(buffer, sizeof(buffer),  "Targeting aspect ratio: %f\n",targetAspectRatio);
    logMessage(debug, buffer);
    snprintf(buffer, sizeof(buffer),  "Virtual Resolution: %dx%d\n",virtualWidth,virtualHeight);
    logMessage(debug, buffer);
    snprintf(buffer, sizeof(buffer),  "(unused) offset: %dx%d\n",xOffset,yOffset);
    logMessage(debug, buffer);

    // setup viewport with our virtual resolutions
    SDL_Rect viewport;
    viewport.x = xOffset;
    viewport.y = yOffset;
    viewport.w = virtualWidth;
    viewport.h = virtualHeight;
    SDL_RenderSetViewport(pRenderer, &viewport);
}

// /*
//     method to change the window mode, flag passed in will set the window mode
//     method will also change the resolution to match the new window mode:
//     fullscreen - will auto detect screen resolution and set it
//     windowed - will set the resolution to 1920x1080
// */
void changeWindowMode(Uint32 flag)
{
    int success = SDL_SetWindowFullscreen(pWindow, flag);
    if(success < 0) 
    {
        logMessage(error, "ERROR: COULD NOT CHANGE WINDOW MODE\n");
        return;
    }
    else
    {
        logMessage(info, "Changed window mode.\n");
    }

    if(flag == 0){
        changeResolution(1920, 1080);
    }
    else{
        SDL_DisplayMode displayMode;
        if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
            logMessage(error, "SDL_GetCurrentDisplayMode failed!\n");
            return;
        }
        int screenWidth = displayMode.w;
        int screenHeight = displayMode.h;
        
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Inferred screen size: %dx%d\n", screenWidth, screenHeight);
        logMessage(debug, buffer);

        changeResolution(screenWidth, screenHeight);
    }
}

// /*
//     Shuts down current renderer, creates a new renderer with or withou
//     vsync according to passed bool
// */
void setVsync(bool enabled) {
    SDL_DestroyRenderer(pRenderer);
    logMessage(debug, "Renderer destroyed to toggle vsync.\n");

    uint32_t flags = SDL_RENDERER_ACCELERATED;
    if (enabled) {
        flags |= SDL_RENDERER_PRESENTVSYNC;
    }

    // Reset the texture filtering hint
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, render_scale_quality);

    pRenderer = SDL_CreateRenderer(pWindow, -1, flags);
    logMessage(debug, "Renderer re-created.\n");

    if (pRenderer == NULL) {
        logMessage(warning,"ERROR RE-CREATING RENDERER\n");     
        exit(1);
    }

    // NOTE: in fullscreen, resetting the render means we need to reset our viewport
    setViewport(currentResolutionWidth,currentResolutionHeight);
}

// /*
//     Changes the game fps cap to the passed integer
//     TODO: add checks for if we need to change vsync to save performance
// */
void changeFPS(int cap){
    if(cap == -1){
        setVsync(true);
    }
    else{
        setVsync(false);
        fpscap = cap;
        desiredFrameTime = 1000 / fpscap;
    }
}

/*
    returns a ScreenSize struct of the current resolution w,h
*/
struct ScreenSize getCurrentResolution(){
    struct ScreenSize screensize = {currentResolutionWidth,currentResolutionHeight}; // TODO: this sucks
    return screensize;
}

/*
    Changes the game resolution to the passed width and height
*/
void changeResolution(int width, int height) {
    SDL_SetWindowSize(pWindow, width, height);
    setViewport(width, height);
    currentResolutionWidth = width;
    currentResolutionHeight = height;
}

// initialize graphics
void initGraphics(int screenWidth,int screenHeight, int windowMode, int framecap, char *title, char *icon_path){
    // test for video init, alarm if failed
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "SDL initialization failed: %s\n", SDL_GetError());
        logMessage(debug, buffer);
        exit(1);
    }

    logMessage(info, "SDL initialized.\n");

    // Set the texture filtering hint
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, render_scale_quality);

    // test for window init, alarm if failed
    pWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | windowMode | SDL_WINDOW_ALLOW_HIGHDPI);
    if (pWindow == NULL) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Window creation failed: %s\n", SDL_GetError());
        logMessage(debug, buffer);
        exit(1);
    }
    
    logMessage(info, "Window initialized.\n");
    
    // set our fps cap to the frame cap param
    // (-1) for vsync
    fpscap = framecap;
    desiredFrameTime = 1000 / fpscap;  

    // if vsync is on
    if(fpscap == -1) {
        logMessage(info, "Starting renderer with vsync... \n");
        pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    }
    else {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Starting renderer with maxfps %d... \n",framecap);
        logMessage(debug, buffer);
        pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
    }

    if (pRenderer == NULL) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        logMessage(error, buffer);
        exit(1);
    }

    init_ui(pWindow,pRenderer);

    // TODO: only do this in debug mode, we need to check engine state for that
    // TODO: the refactor is that this debug overlay needs to move to this file from ui.c, and use the local vars from here. i dont care enough to do this now
    // ui_register_component(ui_paint_debug_overlay, "debug_overlay");

    // set our viewport to the screen size with neccessary computed offsets
    setViewport(screenWidth, screenHeight);
    
    // test for TTF init, alarm if failed
    if (TTF_Init() == -1) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "SDL2_ttf could not initialize! SDL2_ttf Error: %s\n", TTF_GetError());
        logMessage(error, buffer);
        exit(1);
    }
    logMessage(info, "TTF initialized.\n");

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "IMG_Init error: %s", IMG_GetError());
        logMessage(error, buffer);
        exit(1);
    }
    logMessage(info, "IMG initialized.\n");

    // initialize cache
    cache = createVariantCollection();

    // load icon to surface
    printf("icon path: %s\n", icon_path);
    SDL_Surface *pIconSurface = IMG_Load(icon_path);
    if (pIconSurface == NULL) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "IMG_Load error: %s", IMG_GetError());
        logMessage(error, buffer);
        exit(1);
    }
    // set icon
    SDL_SetWindowIcon(pWindow, pIconSurface);
    
    // release surface
    SDL_FreeSurface(pIconSurface);

    logMessage(info, "Window icon set.\n");

    // set a start time for counting fps
    startTime = SDL_GetTicks();
}

// shuts down all initialzied graphics systems
void shutdownGraphics(){
    // remove all render objects and free everything (including cache)
    clearAll(true);
    
    // destroy cache
    destroyVariantCollection(cache);

    // shutdown TTF
    TTF_Quit();
    logMessage(info, "Shut down TTF.\n");

    // shutdown IMG
    IMG_Quit();
    logMessage(info, "Shut down IMG.\n");

    shutdown_ui();

    // shutdown renderer
    SDL_DestroyRenderer(pRenderer);
    logMessage(info, "Shut down renderer.\n");

    // shutdown window
    SDL_DestroyWindow(pWindow);
    logMessage(info, "Shut down window.\n");
}