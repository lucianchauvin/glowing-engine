#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Bounding_sphere {
    glm::vec3 center;
    float radius;

    Bounding_sphere(glm::vec3 c, float r) : center(c), radius(r) {}
};

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

class Scene;

inline constexpr float GRAVITY       = 9.8f;
inline constexpr float JUMP_FORCE    = 5.0f;
inline constexpr float FRICTION      = 0.937f;
// inline constexpr float ACCELERATION  = 1000.0f; //47.0f
inline constexpr float ACCELERATION  = 47.0f;
// inline constexpr float MAX_VELOCITY  = 20.0f;   //4.3f
inline constexpr float MAX_VELOCITY  = 4.3f;
inline constexpr float FLOOR_Y       = 0.0f;

struct Physics_object {
    float mass = 1.0f;
    glm::vec3 position;
    glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 velocity;
    glm::vec3 angular_velocity;
    glm::mat3 inertia, inverse_inertia;
    bool isOnGround;
    Bounding_sphere collider;
    bool enabled; // todo implement

    Physics_object(glm::vec3 pos, 
        float colliderRadius, 
        bool physics_on, 
        float m = 1.0f, 
        const glm::quat& orient = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 
        const glm::vec3& angVel = glm::vec3(0.0f), 
        const glm::mat3& inertiaVal = glm::mat3(1.0f), 
        const glm::mat3& invInertiaVal = glm::mat3(1.0f))
        // add to take aabb stuff
        :   mass(m), 
            position(pos),
            orientation(orient), 
            velocity(0.0f),
            angular_velocity(angVel),
            inertia(inertiaVal), 
            inverse_inertia(invInertiaVal), 
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
