#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>

#include <SDL2/SDL_ttf.h>

#include <yoyoengine/yoyoengine.h>

/*
    TODO PLEASE PLEASE MOVE TO ENGINE CALLBACK HANDLER STUFF FUNCTION FILE
    this is simply un-maintainable...
*/

struct callbackData {
    char *callbackType;
    void (*callback)(struct callbackData *data);
    void *pData;
};

// linked list holding pointers towards button render objects
typedef struct button {
    bool isObject;
    struct renderObject *pObject;
    struct button *pNext;
    struct callbackData *callbackData;
} button;

// SKJDFGLKJSHFGKLFDSGKJHDFKGHDKFJHGKFDJHGKJDFHGKJHSDKHGKFSHGKFDHKGJHDKFJHG

int convertToRealPixelWidth(float in);

int convertToRealPixelHeight(float in);

SDL_Rect createRealPixelRect(bool centered, float x, float y, float w, float h);

// enum denoting all possible renderObject types
typedef enum {
    renderType_Text,
    renderType_Image,
    renderType_Button,
    renderType_Animation
} renderObjectType;

typedef struct TextData {
    TTF_Font *pFont;
    int outlineSize;
    SDL_Color *pColor;
    SDL_Color *pOutlineColor;
    char *pText;
} TextData;

typedef struct ImageData {
    char *pPath;
} ImageData;

typedef struct ButtonData {
    TextData TextData; // nested text for button label
    ImageData ImageData; // nested image for button background
} ButtonData;

// struct defining renderObject(s)
typedef struct renderObject { // TODO: do we have to set each field each time or .access for only what we need
    // common to every render object
    int identifier;
    int depth;
    renderObjectType type;
    SDL_Texture *pTexture;
    SDL_Rect rect;
    struct renderObject *pNext;
    bool cachedTexture;

    // rect is for actual item location, bounds is for its bounds that its centered in
    SDL_Rect bounds;

    bool centered;
    Alignment alignment;

    // union holding data specific to recreating that renderObject
    union {
        TextData TextData;
        ImageData ImageData;
        ButtonData ButtonData;
        AnimationData AnimationData;
    };
} renderObject;

void addRenderObject(renderObject staging);

void removeRenderObject(int identifier);

renderObject *getRenderObject(int identifier);

TTF_Font *loadFont(char *pFontPath, int fontSize);

/*
    struct holding info on texture creations
    TODO: in the future will we need refcounts for arbitrary cache objects holding more than just their texture?
*/
struct textureInfo {
    SDL_Texture *pTexture;
    bool cached;
};

// Create a texture from image path, returns NULL for failure
struct textureInfo createImageTexture(char *pPath, bool shouldCache);

SDL_Texture *createTextTextureWithOutline();

SDL_Texture *createTextTexture(const char *pText, TTF_Font *pFont, SDL_Color *pColor);

TTF_Font *getFont(char *key);

SDL_Color *getColor(char *key, SDL_Color color);

void autoFitBounds(SDL_Rect* bounds, SDL_Rect* obj, Alignment alignment);

int createText(int depth, float x,float y, float width, float height, char *pText, TTF_Font *pFont, SDL_Color *pColor, bool centered, Alignment alignment);

int createImage(int depth, float x, float y, float width, float height, char *pPath, bool centered, Alignment alignment);

int createButton(int depth, float x, float y, float width, float height, char *pText, TTF_Font *pFont, SDL_Color *pColor, bool centered, char *pBackgroundPath, struct callbackData *data, Alignment alignment);

void updateText(int id, char *pText);

void updateImage(int id, char *pSrc);

// function that clears all non engine render objects (depth >= 0)
void clearAll(bool freeEngine);

void removeButton(int id);

void renderAll();

void checkClicked(int x, int y);

void setViewport(int screenWidth, int screenHeight);

void changeWindowMode(Uint32 mode);

void changeFPS(int cap);

struct ScreenSize getCurrentResolution();

void changeResolution(int width, int height);

void initGraphics(int screenWidth,int screenHeight, int windowMode, int framecap, char *title, char *icon_path);

void shutdownGraphics();

#endif