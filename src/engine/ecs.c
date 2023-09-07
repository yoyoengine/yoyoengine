#include <yoyoengine/yoyoengine.h>

/*
    Entity
    
    An entity is a collection of components that make up a game object.
*/
struct ye_entity {
    bool active;        // controls whether system will act upon this entity and its components

    int id;             // unique id for this entity
    char *name;         // name that can also be used to access the entity
    char *tags[10];     // up to 10 tags that can also be used to access the entity

    struct ye_component_transform *transform;       // transform component
    struct ye_component_renderer *renderer;         // renderer component
    struct ye_component_script *script;             // script component
    struct ye_component_interactible *interactible; // interactible component
};

/*
    Alignment enum

    Used to describe where an entity is aligned within its bounds.
*/
enum ye_alignment {
    YE_ALIGN_TOP_LEFT,  YE_ALIGN_TOP_CENTER,    YE_ALIGN_TOP_RIGHT,
    YE_ALIGN_MID_LEFT,  YE_ALIGN_MID_CENTER,    YE_ALIGN_MID_RIGHT,
    YE_ALIGN_BOT_LEFT,  YE_ALIGN_BOT_CENTER,    YE_ALIGN_BOT_RIGHT
};

/*
    Transform component
    
    Describes where the entity sits in the world.
    In 2D the Z axis is the layer the entity sits on. (High Z overpaints low Z)
*/
struct ye_component_trasform {
    bool active;    // controls whether system will act upon this component

    SDL_Rect bounds;                // original desired pixel location of entity
    enum ye_alignment alignment;    // alignment of entity within its bounds
    SDL_Rect rect;                  // real location of entity computed from desired alignment
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
    YE_RENDERER_TYPE_IMAGE,
    YE_RENDERER_TYPE_ANIMATION
};

struct ye_component_renderer {
    bool active;    // controls whether system will act upon this component

    SDL_Texture *texture;   // texture to render

    // bool texture_cached;   // whether the texture is cached or not

    enum ye_component_renderer_type type;   // denotes which renderer is needed for this entity

    union { // hold the data for the specific renderer type
        struct ye_component_renderer_text *text;
        struct ye_component_renderer_image *image;
        struct ye_component_renderer_animation *animation;
    };
};

/*
    Interactible component
    
    Holds information on how an entity can be interacted with.
*/
struct ye_component_interactible {
    bool active;    // controls whether system will act upon this component

    void *data;                     // data to communicate when callback finishes
    void (*callback)(void *data);   // callback to run when entity is interacted with
};

/*
    Create a new entity and return its id
*/
int ye_create_entity(){}

/*
    Destroy an entity by ID
*/
void ye_destroy_entity(int id){}

struct ye_entity *ye_get_entity_by_id(int id){}

struct ye_entity *ye_get_entity_by_name(char *name){}

struct ye_entity *ye_get_entity_by_tag(char *tag){}

/*
    Thoughts: we need checks to see if certain components like transform
    are not null before systems act upon other components like interactible

    We need to create the pools/lists that the systems iterate through
    to act upon components

    Order of actions before frame render:
    1. Scripting system runs scripts
    2. Physics updates any physics components
    3. Check for interactions and run callbacks
    4. Rendering system renders all renderable components

    Things I still havent figured out:
    - How can we register callbacks to lua functions?
*/