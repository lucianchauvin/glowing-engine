#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <glm/glm.hpp>

const float GRAVITY = 9.8f;
const float JUMP_FORCE = 5.0f;
const float FRICTION = 0.937f;
const float ACCELERATION = 1000.0f;//47.0f;
const float MAX_VELOCITY = 20.0f;//4.3f;
const float FLOOR_Y = 0;

struct Collider_3d {
    glm::vec3 pos;
    float radius;

    Collider_3d(glm::vec3 position, float r) : pos(position), radius(r) {}
};

struct PhysicsObject {
    glm::vec3 position;
    glm::vec3 velocity;
    bool isOnGround;
    
    Collider_3d collider;

    PhysicsObject(glm::vec3 pos, float colliderRadius) : 
    position(pos), velocity(0.0f), isOnGround(false), collider(pos, colliderRadius) {
    }
};

class Physics {
public:
    std::vector<PhysicsObject> objects;

    void addObject(const PhysicsObject& obj) {
        objects.push_back(obj);
    }

    void step(float deltaTime) {
        for (auto& obj : objects) {
            if (!obj.isOnGround) {
                obj.velocity.y -= GRAVITY * deltaTime;  // Apply gravity
            }

            obj.velocity *= FRICTION;  // Apply friction

            // Limit velocity
            if (glm::length(obj.velocity) > MAX_VELOCITY) {
                obj.velocity = glm::normalize(obj.velocity) * MAX_VELOCITY;
            }

            obj.position += obj.velocity * deltaTime;  // Update position

            // Floor collision check
            if (obj.position.y <= FLOOR_Y) {
                obj.isOnGround = true;
                obj.velocity.y = 0;
                obj.position.y = FLOOR_Y;  // Snap to floor
            } else {
                obj.isOnGround = false;
            }
        }
    }
};
#endif
