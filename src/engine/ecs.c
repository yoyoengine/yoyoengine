/*
    Doing today:
    1. finish up the structure of the entities and components
    2. create a storage system for entities and component pools (LL)
    3. start roughing in systems like renderer
*/
#include <yoyoengine/yoyoengine.h>

// entity id counter (used to assign unique ids to entities)
int eid = 0; 

//////////////////////// LINKED LIST //////////////////////////

// Define a linked list structure for storing entities
struct ye_entity_node {
    struct ye_entity *entity;
    struct ye_entity_node *next;
};

// Create a linked list for entities of a specific type (e.g., renderable entities)
struct ye_entity_node *ye_entity_list_create() {
    return NULL; // Initialize an empty list
}

// Add an entity to the linked list
void ye_entity_list_add(struct ye_entity_node **list, struct ye_entity *entity) {
    struct ye_entity_node *newNode = malloc(sizeof(struct ye_entity_node));
    newNode->entity = entity;
    newNode->next = *list;
    *list = newNode;
}

/*
    Remove an entity from the linked list

    NOTE: THIS DOES NOT FREE THE ACTUAL ENTITY ITSELF
    
    This is temporarialy ok because we dont delete any at runtime but TODO
    we need a method of this that does call the destructor for the entity
    probably ye_entity_list_remove_free?
    Not sure how to differentiate when this needs called though... just for actual list
    of entities?
*/ 
void ye_entity_list_remove(struct ye_entity_node **list, struct ye_entity *entity) {
    struct ye_entity_node *current = *list;
    struct ye_entity_node *prev = NULL;

    while (current != NULL) {
        if (current->entity == entity) {
            if (prev == NULL) {
                *list = current->next; // Update the head of the list
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Clean up the entire linked list
void ye_entity_list_destroy(struct ye_entity_node **list) {
    while (*list != NULL) {
        struct ye_entity_node *temp = *list;
        *list = (*list)->next;
        ye_destroy_entity(temp->entity);
        free(temp);
    }
    *list = NULL;
}

///////////////////////////////////////////////////////////////

struct ye_entity_node *entity_list_head;
struct ye_entity_node *transform_list_head;
struct ye_entity_node *renderer_list_head;
struct ye_entity_node *camera_list_head;
// struct ye_entity_node *script_list_head;
// struct ye_entity_node *interactible_list_head;

/*
    Create a new entity and return a pointer to it
*/
struct ye_entity * ye_create_entity(){
    struct ye_entity *entity = malloc(sizeof(struct ye_entity));
    entity->id = eid++; // assign unique id to entity
    entity->active = true;

    //name the entity "entity id"
    char *name = malloc(sizeof(char) * 100);
    snprintf(name, 100, "entity %d", entity->id);
    entity->name = name;

    // assign all copmponents to null
    entity->transform = NULL;
    entity->renderer = NULL;
    entity->camera = NULL;
    entity->script = NULL;
    entity->interactible = NULL;

    // add the entity to the entity list
    ye_entity_list_add(&entity_list_head, entity);
    logMessage(debug, "Created and added an entity\n");

    return entity;
}

/*
    Create a new entity and return a pointer to it (named)

    we must allocate space for the name and copy it
*/
// struct ye_entity * ye_create_entity_named(char *name){
//     struct ye_entity *entity = malloc(sizeof(struct ye_entity));
//     entity->id = eid++; // assign unique id to entity
//     entity->active = true;

//     //name the entity "entity id"
//     entity->name = malloc(sizeof(char) * strlen(name));
//     strcpy(entity->name, name);

//     // assign all copmponents to null
//     entity->transform = NULL;
//     entity->renderer = NULL;
//     entity->camera = NULL;
//     entity->script = NULL;
//     entity->interactible = NULL;

//     // add the entity to the entity list
//     ye_entity_list_add(&entity_list_head, entity);
//     logMessage(debug, "Created and added an entity\n");

//     return entity;
// }

/*
    Destroy an entity by pointer
*/
void ye_destroy_entity(struct ye_entity * entity){
    if(entity == NULL){
        logMessage(warning, "Attempted to destroy a null entity\n");
        return;
    }

    // remove from the entity list (frees its node)
    ye_entity_list_remove(&entity_list_head, entity);

    // check for non null components and free them
    if(entity->transform != NULL) ye_remove_transform_component(entity);
    if(entity->renderer != NULL) ye_remove_renderer_component(entity);
    if(entity->camera != NULL) ye_remove_camera_component(entity);
    // if(entity->script != NULL) ye_remove_script_component(entity);
    // if(entity->interactible != NULL) ye_remove_interactible_component(entity);

    // free the entity name
    free(entity->name);

    // free all tags
    // for(int i = 0; i < 10; i++){
    //     if(entity->tags[i] != NULL) free(entity->tags[i]);
    // }

    // free the entity
    free(entity);

    entity = NULL;

    logMessage(debug, "Destroyed an entity\n");
}

// struct ye_entity *ye_get_entity_by_id(int id){}

// struct ye_entity *ye_get_entity_by_name(char *name){}

// struct ye_entity *ye_get_entity_by_tag(char *tag){}

/////////////////////////  SYSTEMS  ////////////////////////////

/////////////////////////  CAMERA   ////////////////////////////

void ye_set_camera(struct ye_entity *entity){
    engine_state.target_camera = entity;
}

void ye_add_camera_component(struct ye_entity *entity, SDL_Rect view_field){
    entity->camera = malloc(sizeof(struct ye_component_camera));
    entity->camera->active = true;
    entity->camera->view_field = view_field; // the width height is all that matters here, because the actual x and y are inferred by its transform

    // log that we added a transform and to what ID
    char b[100];
    snprintf(b, sizeof(b), "Added camera to entity %d\n", entity->id);
    logMessage(debug, b);

    // add this entity to the camera component list
    ye_entity_list_add(&camera_list_head, entity);
}

void ye_remove_camera_component(struct ye_entity *entity){
    free(entity->camera);
    entity->camera = NULL;

    // remove the entity from the camera component list
    ye_entity_list_remove(&camera_list_head, entity);
}

///////////////////////// TRANSFORM ////////////////////////////

/*
    Problem... we need the actual size of the image to align it in bounds, but
    at the same time we can have a transform without a renderer (sometimes i guess)
    which means we need to query the texture size
*/
void ye_add_transform_component(struct ye_entity *entity, SDL_Rect bounds, int z, enum ye_alignment alignment){
    entity->transform = malloc(sizeof(struct ye_component_transform));
    entity->transform->active = true;
    entity->transform->bounds = bounds;
    entity->transform->z = z;
    
    // we will first set the rect equal to the bounds, for the purposes of rendering the renderer on mount
    // will then calculate the actual rect of the entity based on its alignment and bounds
    entity->transform->rect = bounds;
    entity->transform->alignment = YE_ALIGN_MID_CENTER;

    // add this entity to the transform component list
    ye_entity_list_add(&transform_list_head, entity);

    // log that we added a transform and to what ID
    char b[100];
    snprintf(b, sizeof(b), "Added transform to entity %d\n", entity->id);
    logMessage(debug, b);
}

void ye_remove_transform_component(struct ye_entity *entity){
    free(entity->transform);
    entity->transform = NULL;

    // remove the entity from the transform component list
    ye_entity_list_remove(&transform_list_head, entity);
}

///////////////////////// RENDERER ////////////////////////////

/*
    Renderer TODO:
    - occlusion culling on active camera
*/

void ye_add_renderer_component(struct ye_entity *entity, enum ye_component_renderer_type type, void *data){
    entity->renderer = malloc(sizeof(struct ye_component_renderer));
    entity->renderer->active = true;
    entity->renderer->type = type;
    
    if(type == YE_RENDERER_TYPE_IMAGE){
        entity->renderer->renderer_impl.image = data;
    }
    else if(type == YE_RENDERER_TYPE_TEXT){
        entity->renderer->renderer_impl.text = data;
    }
    else if(type == YE_RENDERER_TYPE_ANIMATION){
        entity->renderer->renderer_impl.animation = data;
    }

    // add this entity to the renderer component list
    ye_entity_list_add(&renderer_list_head, entity);

    // log that we added a renderer and to what ID
    char b[100];
    snprintf(b, sizeof(b), "Added renderer to entity %d\n", entity->id);
    logMessage(debug, b);
}

void ye_temp_add_image_renderer_component(struct ye_entity *entity, char *src){
    struct ye_component_renderer_image *image = malloc(sizeof(struct ye_component_renderer_image));
    // copy src to image->src
    image->src = malloc(sizeof(char) * strlen(src));

    // create the renderer top level
    ye_add_renderer_component(entity, YE_RENDERER_TYPE_IMAGE, image);

    // create the image texture
    struct textureInfo t = createImageTexture(src, false);
    entity->renderer->texture = t.pTexture;

    
    if(entity->transform != NULL){
        // calculate the actual rect of the entity based on its alignment and bounds
        entity->transform->rect = ye_get_real_texture_size_rect(entity->renderer->texture);
        ye_auto_fit_bounds(&entity->transform->bounds, &entity->transform->rect, entity->transform->alignment);
        
        // LEFT FOR DEBUGGING
        // // print the new bounds and rect
        // char b[100];
        // snprintf(b, sizeof(b), "Bounds: %d %d %d %d\n", entity->transform->bounds.x, entity->transform->bounds.y, entity->transform->bounds.w, entity->transform->bounds.h);
        // logMessage(debug, b);
        // snprintf(b, sizeof(b), "Rect: %d %d %d %d\n", entity->transform->rect.x, entity->transform->rect.y, entity->transform->rect.w, entity->transform->rect.h);
        // logMessage(debug, b);

    }
    else{
        logMessage(warning, "Entity has renderer but no transform. Its real paint bounds have not been computed\n");
    }
}

void ye_remove_renderer_component(struct ye_entity *entity){
    // free contents of renderer_impl
    if(entity->renderer->type == YE_RENDERER_TYPE_IMAGE){
        free(entity->renderer->renderer_impl.image->src);
        free(entity->renderer->renderer_impl.image);
    }
    // else if(entity->renderer->type == YE_RENDERER_TYPE_TEXT){
    //     free(entity->renderer->renderer_impl.text->pText);
    //     free(entity->renderer->renderer_impl.text);
    // }
    // else if(entity->renderer->type == YE_RENDERER_TYPE_ANIMATION){
    //     free(entity->renderer->renderer_impl.animation);
    // }

    // destoy the texture. NOTE: if we have some cache system in future this would double free
    SDL_DestroyTexture(entity->renderer->texture);
    
    free(entity->renderer);
    entity->renderer = NULL;

    // remove the entity from the renderer component list
    ye_entity_list_remove(&renderer_list_head, entity);
}

/*
    Renderer system

    Acts upon the list of tracked entities with renderers and paints them
    to the screen.

    Uses the transform component to determine where to paint the entity.
    Skips entity if there is no active transform or renderer is inactive

    This system will paint relative to the active camera, and occlude anything
    outside of the active camera's view field

    TODO:
    - z-sorting

    NOTES:
    - Initially I tried some weird really complicated impl, which I will share details
    of below, for future me who will likely need these in the future.
    For now, we literally just check an intersection of the object and the camera, and if it exists we
    copy the object to the renderer offset by the camera position.
    In the future we might want to return to the weird clip plane system, but for now this is fine.

    Some functions and relevant snippets to the old system:
    - SDL_RenderSetClipRect(renderer, &visibleRect);
*/
void ye_system_renderer(SDL_Renderer *renderer) {
    // check if we have a non-null, active camera targeted
    if (engine_state.target_camera == NULL || engine_state.target_camera->camera == NULL || !engine_state.target_camera->camera->active) {
        logMessage(warning, "No active camera targeted. Skipping renderer system\n");
        return;
    }

    // Get the camera's position in world coordinates
    SDL_Rect camera_rect = engine_state.target_camera->transform->rect;
    SDL_Rect view_field = engine_state.target_camera->camera->view_field;
    camera_rect.w = view_field.w;
    camera_rect.h = view_field.h;
    // update camera rect to contain the view field w,h

    // Traverse tracked entities with renderer components
    struct ye_entity_node *current = renderer_list_head;
    while (current != NULL) {
        if (current->entity->renderer->active) {
            if (current->entity->transform != NULL && 
                current->entity->transform->active &&
                current->entity->transform->z <= engine_state.target_camera->transform->z // only render if the entity is on or in front of the camera
            ) {
                SDL_Rect entity_rect = current->entity->transform->rect; // where the entity is in the world by pixel (x, y, w, h)

                /*
                    Check if the entity is within the camera's view field and render it offset by the camera position in space
                */
                SDL_Rect visibleRect; // output intersection rect
                if (SDL_IntersectRect(&entity_rect, &camera_rect, &visibleRect) == SDL_TRUE) {
                    // Adjust the entity's position to be relative to the camera
                    entity_rect.x = entity_rect.x + camera_rect.x;
                    entity_rect.y = entity_rect.y + camera_rect.y;

                    // Copy the texture to the renderer within the clipping area and adjusted destination rectangle
                    SDL_RenderCopy(renderer, current->entity->renderer->texture, NULL, &entity_rect);
                    
                    // paint bounds, my beloved <3
                    if (engine_state.paintbounds_visible) {
                        // create entity bounds offset by camera
                        SDL_Rect entity_bounds = current->entity->transform->bounds;
                        entity_bounds.x = entity_bounds.x + camera_rect.x;
                        entity_bounds.y = entity_bounds.y + camera_rect.y;
                        
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                        SDL_RenderDrawRect(renderer, &entity_bounds);
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                        SDL_RenderDrawRect(renderer, &entity_rect);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    }
                }
            }
        }
        current = current->next;
    }
}

/////////////////////////   ECS    ////////////////////////////

void ye_init_ecs(){
    entity_list_head = ye_entity_list_create();
    transform_list_head = ye_entity_list_create();
    renderer_list_head = ye_entity_list_create();
    camera_list_head = ye_entity_list_create();
    logMessage(info, "Initialized ECS\n");
}

void ye_shutdown_ecs(){
    ye_entity_list_destroy(&entity_list_head);
    
    /* 
        destroy the other, now empty lists (all items are null)

        this mainly consists of freeing the malloced nodes (2 pointers to ent and nxt)
    */

    ye_entity_list_destroy(&transform_list_head);
    ye_entity_list_destroy(&renderer_list_head);
    ye_entity_list_destroy(&camera_list_head);

    logMessage(info, "Shut down ECS\n");
}

/*
    Function that iterates through entity list and logs all entities with their ids
*/
void ye_print_entities(){
    struct ye_entity_node *current = entity_list_head;
    int i = 0;
    while(current != NULL){
        char b[100];
        snprintf(b, sizeof(b), "\"%s\" -> ID:%d T:%d R:%d C:%d I:%d S:%d\n",
            current->entity->name, current->entity->id, 
            current->entity->transform != NULL, 
            current->entity->renderer != NULL, 
            current->entity->camera != NULL,
            current->entity->interactible != NULL,
            current->entity->script != NULL
        );
        logMessage(debug, b);
        current = current->next;
        i++;
    }
    char b[100];
    snprintf(b, sizeof(b), "Total entities: %d", i);
    logMessage(debug, b);
}

/*
    TODO: we need to deny adding multiple of the same component - or do we?
    right now nothing is stopping from dropping without freeing, we need non null
    check
*/