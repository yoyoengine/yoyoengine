/*
    This file is a part of yoyoengine. (https://github.com/yoyoengine)
    Copyright (C) 2023-2024  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <yoyoengine/export.h>

#include <yoyoengine/logging.h>
#include <yoyoengine/ecs/ecs.h>
#include <yoyoengine/tar_physics/rigidbody.h>

////////////////////////////////////////////////

#include <stdlib.h> // NULL

/*
    ECS Stuff
*/
void ye_add_rigidbody_component(struct ye_entity *ent, float mass, float restitution, float kinematic_friction, float rotational_kinematic_friction) {
    if(!ent) {
        ye_logf(YE_LL_ERROR, "ye_add_rigidbody_component: entity is NULL");
        return;
    }

    if(ent->rigidbody) {
        ye_logf(YE_LL_WARNING, "ye_add_rigidbody_component: entity already has a rigidbody component");
        return;
    }
    
    struct ye_component_rigidbody *rb = malloc(sizeof(struct ye_component_rigidbody));
    rb->active = true;
    rb->mass = mass;
    rb->restitution = restitution;
    rb->kinematic_friction = kinematic_friction;
    rb->rotational_kinematic_friction = rotational_kinematic_friction;
    rb->velocity.x = 0.0f;
    rb->velocity.y = 0.0f;
    rb->rotational_velocity = 0.0f;

    ent->rigidbody = rb;
    ye_entity_list_add(&rigidbody_list_head, ent);
}

void ye_remove_rigidbody_component(struct ye_entity *ent) {
    if(!ent) {
        ye_logf(YE_LL_ERROR, "ye_remove_rigidbody_component: entity is NULL");
        return;
    }

    if(!ent->rigidbody) {
        ye_logf(YE_LL_WARNING, "ye_remove_rigidbody_component: entity does not have a rigidbody component");
        return;
    }

    free(ent->rigidbody);
    ent->rigidbody = NULL;
    ye_entity_list_remove(&rigidbody_list_head, ent);
}

////////////////////////////////////////////////