#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>

#include "core/entity.h"
#include "asset/shader.h"
#include "asset/skybox.h"

//struct entity_build {
//        glm::vec3 position
//        bool physics_enabled
//        glm::vec3 model_scale = glm::vec3(1.0f),
//        glm::vec3 color = glm::vec3(1.0f),
//        float mass = 1.0f,
//        glm::quat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
//        bool fade = false,
//        float ttl = 0.0f,
//        float max_ttl = 0.0f,
//        float collider_radius = 1.0f
//};

class Scene {
public:
    Scene(std::string skybox_name);
    ~Scene();

    void include(Entity ntitty);
    // returns the number of hits
    int cast_ray(const glm::vec3& pos, const glm::vec3& dir, glm::vec3& hit_pos);
    void add();

    std::vector<Entity> entities;
    std::vector<Entity> timed_entities;
    Skybox skybox;
};
#endif
