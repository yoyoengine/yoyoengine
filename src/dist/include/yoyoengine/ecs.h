#ifndef ECS_H
#define ECS_H

#include <yoyoengine/yoyoengine.h>
#include <yoyoengine/utils.h>
#include <stdbool.h>

/*
    Linked list structure for storing entities

    The intent of this is to keep lists of entities that contain
    certain components, so that systems can iterate through them
    and act upon their components.
*/
struct ye_entity_list_item {
    struct ye_entity *entity;
    struct ye_entity_list_item *next;
};

/*
    Entity
    
    An entity is a collection of components that make up a game object.
*/
struct ye_entity {
    bool active;        // controls whether system will act upon this entity and its components

    int id;             // unique id for this entity
    char *name;         // name that can also be used to access the entity

    /*
        TODO: tag should become its own component, we dont want to check EVERY entity
        for posessing a tag when only 1/100 entities have a tag component
    */
    // char *tags[10];     // up to 10 tags that can also be used to access the entity

    struct ye_component_transform *transform;       // transform component
    struct ye_component_renderer *renderer;         // renderer component
    struct ye_component_script *script;             // script component
    struct ye_component_interactible *interactible; // interactible component
    struct ye_component_camera *camera;             // camera component

    /* NOTE/TODO:
        We can have arrays in the future malloced to the
        size of struct ye_component_* and then we can have
        multiple components of the same type on an entity.

        This would be good for tags, but for all else we would actually need
        to explicitely link which transform and which components are
        used for other components which have been assuming them by default
    */
};

/*
    Camera component

    Holds some information about a camera and its view field
*/
struct ye_component_camera {
    bool active;    // controls whether system will act upon this component

    SDL_Rect view_field;    // view field of camera
};

/*
    Transform component
    
    Describes where the entity sits in the world.
    In 2D the Z axis is the layer the entity sits on. (High Z overpaints low Z)
*/
struct ye_component_transform {
    bool active;    // controls whether system will act upon this component

    SDL_Rect bounds;                // original desired pixel location of entity
    enum ye_alignment alignment;    // alignment of entity within its bounds
    SDL_Rect rect;                  // real location of entity computed from desired alignment

    int z;                          // layer the entity sits on
};

/*
    Script component
    
    Holds information on a script that is attatched to an entity.
    This script will have its main loop run once per frame.
*/
struct ye_component_script {
    bool active;    // controls whether system will act upon this component

    char *script_path;
};

/*
    Enum to store different unique types of renderers.
    This is how we identify steps needed to render different types of entities.

    Ex: animation renderer knows it needs to increment frames, text renderer knows how to reconstruct text, etc
*/
enum ye_component_renderer_type {
    YE_RENDERER_TYPE_TEXT,
    YE_RENDERER_TYPE_TEXT_OUTLINED,
    YE_RENDERER_TYPE_IMAGE,
    YE_RENDERER_TYPE_ANIMATION
};

struct ye_component_renderer {
    bool active;    // controls whether system will act upon this component

    SDL_Texture *texture;   // texture to render

    // bool texture_cached;   // whether the texture is cached or not

    enum ye_component_renderer_type type;   // denotes which renderer is needed for this entity

    union renderer_impl{ // hold the data for the specific renderer type
        struct ye_component_renderer_text *text;
        struct ye_component_renderer_text *text_outlined;
        struct ye_component_renderer_image *image;
        struct ye_component_renderer_animation *animation;
    } renderer_impl;
};

struct ye_component_renderer_image {
    char *src;  // path to image
};

struct ye_component_renderer_text {
    char *text;         // text to render
    TTF_Font *font;     // font to use
    SDL_Color *color;    // color of text
};

struct ye_component_renderer_text_outlined {
    char *text;                 // text to render
    int outline_size;           // size of text outline
    TTF_Font *font;             // font to use
    SDL_Color *color;           // color of text
    SDL_Color *outline_color;   // color of text outline
};

struct ye_component_renderer_animation {
    char *animation_path;       // path to animation folder
    char *image_format;         // format of image files in animation

    size_t frame_count;         // number of frames in animation

    int frame_delay;            // delay between frames in ms
    int last_updated;           // SDL_GetTicks() last frame advance
    int loops;                  // number of loops, -1 for infinite
    int current_frame_index;    // current frame index

    SDL_Texture** frames;       // array of textures for each frame

    bool paused;
};

/*
    Interactible component
    
    Holds information on how an entity can be interacted with.
*/
struct ye_component_interactible {
    bool active;    // controls whether system will act upon this component

    void *data;                     // data to communicate when callback finishes
    void (*callback)(void *data);   // callback to run when entity is interacted with

    /*
        Currently, we will get the transform of an object with a
        interactible component to check bounds for clicks.

        In the future, we would want a collider_2d component with
        an assosciated type and relative position so we can more
        presicely check for interactions. I dont forsee the need
        for this for a while though. Maybe future me in a year reading
        this wishes I did it now xD
    */
};

/////// General entitys ///////

/*
    Create a new entity and return a pointer to it
*/
struct ye_entity * ye_create_entity();

/*
    Destroy an entity by pointer
*/
void ye_destroy_entity(struct ye_entity * entity);



/////// Camera component ///////

void ye_set_camera(struct ye_entity *entity);

void ye_add_camera_component(struct ye_entity *entity, SDL_Rect view_field);

void ye_remove_camera_component(struct ye_entity *entity);



/////// Transform Component ///////

void ye_add_transform_component(struct ye_entity *entity, SDL_Rect bounds, int z, enum ye_alignment alignment);

void ye_remove_transform_component(struct ye_entity *entity);



/////// Renderer Component ///////

/*
    DO NOT USE THIS DIRECTLY UNLESS YOU KNOW WHAT YOURE DOING

    YOU MUST PASS A VOID POINTER TO A STRUCT OF MATCHING TYPE
*/
void ye_add_renderer_component(struct ye_entity *entity, enum ye_component_renderer_type type, void *data);

void ye_temp_add_image_renderer_component(struct ye_entity *entity, char *src);

void ye_remove_renderer_component(struct ye_entity *entity);



/////// SYSTEMS ///////

void ye_system_renderer(SDL_Renderer *renderer);



/////// General ECS ///////

void ye_init_ecs();

void ye_shutdown_ecs();

// helper functions

void ye_print_entities();

#endif