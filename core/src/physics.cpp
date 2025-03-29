#include "physics.h"

#include "scene.h"
#include "entity.h"

void Physics::add_object(Physics_object* obj) {
    objects.push_back(obj);
}

void Physics::load_scene(Scene& scene) {
    printf("loading scene physics\n");
    for (Entity& entity : scene.entities) {
        if (entity.physics.enabled)
            add_object(&entity.physics);
    }
}

void Physics::step(float deltaTime) {
    // printf("in");
    for (Physics_object* obj : objects) {
        // printf("y[%d] ", obj->position.x);
        if (!obj->isOnGround) {
            obj->velocity.y -= GRAVITY * deltaTime;
        }

        obj->velocity *= FRICTION;
        if (glm::length(obj->velocity) > MAX_VELOCITY) {
            obj->velocity = glm::normalize(obj->velocity) * MAX_VELOCITY;
        }

        obj->position += obj->velocity * deltaTime;

        if (obj->position.y <= FLOOR_Y) {
            obj->isOnGround = true;
            obj->velocity.y = 100.0f;
            obj->position.y = FLOOR_Y;
        } else {
            obj->isOnGround = false;
        }
    }
}
