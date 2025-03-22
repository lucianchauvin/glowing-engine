#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <glm/glm.hpp>

class Scene;

inline constexpr float GRAVITY       = 9.8f;
inline constexpr float JUMP_FORCE    = 5.0f;
inline constexpr float FRICTION      = 0.937f;
inline constexpr float ACCELERATION  = 1000.0f; //47.0f
inline constexpr float MAX_VELOCITY  = 20.0f;   //4.3f
inline constexpr float FLOOR_Y       = 0.0f;

struct Collider_3d {
    glm::vec3 pos;
    float radius;

    Collider_3d(glm::vec3 position, float r) : pos(position), radius(r) {}
};

struct Physics_object {
    glm::vec3 position;
    glm::vec3 velocity;
    bool isOnGround;
    bool enabled; // todo implement
    
    Collider_3d collider;

    Physics_object(glm::vec3 pos, float colliderRadius, bool physics_on) 
        : position(pos),
          velocity(0.0f),
          isOnGround(false),
          collider(pos, colliderRadius),
          enabled(physics_on) {}
};

class Physics {
public:
    std::vector<Physics_object*> objects;

    void add_object(Physics_object* obj);
    void load_scene(Scene& scene);
    void step(float deltaTime);
};
#endif
