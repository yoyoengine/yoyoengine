/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine/yoyoengine)
    Copyright (C) 2023-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <p2d/p2d.h>

#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/rigidbody.h>

void ye_add_rigidbody_component(struct ye_entity *entity, struct p2d_object p2d_object) {
    if(!entity) {
        ye_logf(YE_LL_ERROR, "could not add rigidbody component: entity is NULL\n");
        return;
    }
    if(entity->rigidbody) {
        ye_logf(YE_LL_ERROR, "could not add rigidbody component to \"%s\": entity already has a rigidbody component\n",entity->name);
        return;
    }

    struct ye_component_rigidbody *rb = malloc(sizeof(struct ye_component_rigidbody));
    if(!rb) {
        ye_logf(YE_LL_ERROR, "could not add rigidbody component: failed to allocate memory\n");
    }

    rb->active = true;
    rb->p2d_object = p2d_object;

    entity->rigidbody = rb;

    ye_entity_list_add(&rigidbody_list_head, entity);

    // register this entity into p2d
    p2d_create_object(&rb->p2d_object);

    // ye_logf(YE_LL_DEBUG, "added rigidbody component to \"%s\"\n", entity->name);
}

void ye_remove_rigidbody_component(struct ye_entity *entity) {
    if(!entity) {
        ye_logf(YE_LL_ERROR, "could not remove rigidbody component: entity is NULL\n");
        return;
    }
    if(!entity->rigidbody) {
        ye_logf(YE_LL_ERROR, "could not remove rigidbody component from \"%s\": entity does not have a rigidbody component\n",entity->name);
        return;
    }

    free(entity->rigidbody);
    entity->rigidbody = NULL;

    ye_entity_list_remove(&rigidbody_list_head, entity);

    // remove from p2d
    p2d_remove_object(&entity->rigidbody->p2d_object);

    // ye_logf(YE_LL_DEBUG, "removed rigidbody component from \"%s\"\n", entity->name);
}