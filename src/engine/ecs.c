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

// Remove an entity from the linked list
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
}

///////////////////////////////////////////////////////////////

struct ye_entity_node *entity_list_head;

/*
    Create a new entity and return a pointer to it
*/
struct ye_entity * ye_create_entity(){
    struct ye_entity *entity = malloc(sizeof(struct ye_entity));
    entity->id = eid++; // assign unique id to entity
    entity->active = true;

    // name the entity "entity: id"
    char *name = malloc(sizeof(char) * 16);
    snprintf(name, "entity: %d", entity->id);
    entity->name = name;

    // add the entity to the entity list
    ye_entity_list_add(&entity_list_head, entity);
    logMessage(debug, "Created and added an entity\n");

    return entity;
}

/*
    Destroy an entity by pointer
*/
void ye_destroy_entity(struct ye_entity * entity){
    // check for non null components and free them
    if(entity->transform != NULL) ye_remove_transform_component(entity);
    // if(entity->renderer != NULL) ye_remove_renderer_component(entity);
    // if(entity->script != NULL) ye_remove_script_component(entity);
    // if(entity->interactible != NULL) ye_remove_interactible_component(entity);

    // free the entity name
    free(entity->name);

    // free all tags
    for(int i = 0; i < 10; i++){
        if(entity->tags[i] != NULL) free(entity->tags[i]);
    }

    // free the entity
    free(entity);

    logMessage(debug, "Destroyed an entity\n");
}

struct ye_entity *ye_get_entity_by_id(int id){}

struct ye_entity *ye_get_entity_by_name(char *name){}

struct ye_entity *ye_get_entity_by_tag(char *tag){}

/////////////////////////  SYSTEMS  ////////////////////////////

///////////////////////// TRANSFORM ////////////////////////////

void ye_add_transform_component(struct ye_entity *entity, SDL_Rect bounds, enum ye_alignment alignment){
    entity->transform = malloc(sizeof(struct ye_component_transform));
    entity->transform->active = true;
    entity->transform->bounds = (SDL_Rect){0, 0, 0, 0};
    entity->transform->alignment = YE_ALIGN_MID_CENTER;
    
    // calculate the actual rect of the entity based on its alignment and bounds
    ye_auto_fit_bounds(&entity->transform->bounds, &entity->transform->rect, entity->transform->alignment);
}

void ye_remove_transform_component(struct ye_entity *entity){
    free(entity->transform);
    entity->transform = NULL;
}

///////////////////////// RENDERER ////////////////////////////

void ye_add_renderer_component(struct ye_entity *entity, enum ye_component_renderer_type type){
    entity->renderer = malloc(sizeof(struct ye_component_renderer));
    entity->renderer->active = true;
    entity->renderer->type = type;
    
    if(type == YE_RENDERER_TYPE_IMAGE){
        entity->renderer->texture = NULL; // TODO rn
    }
    // else if(type == YE_RENDERER_TYPE_TEXT){
    //     entity->renderer->texture = NULL;
    // }
    // else if(type == YE_RENDERER_TYPE_ANIMATION){
    //     entity->renderer->texture = NULL;
    // }
}

/////////////////////////   ECS    ////////////////////////////

void ye_init_ecs(){
    entity_list_head = ye_entity_list_create();
    logMessage(info, "Initialized ECS\n");
}

void ye_shutdown_ecs(){
    ye_entity_list_destroy(&entity_list_head);
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
        snprintf(b, sizeof(b), "Entity: %d", current->entity->id);
        logMessage(debug, b);
        current = current->next;
        i++;
    }
    char b[100];
    snprintf(b, sizeof(b), "Total entities: %d", i);
    logMessage(debug, b);
}