#include "scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Scene::Scene() {
    entities = std::vector<Entity>();
    timed_entities = std::vector<Entity>();

    world_geometry.add_wall(glm::vec3(-1.0f, 0.0f, 0.0f),  glm::vec3(-1.0f, 0.0f, 1.0f), 5.0f);
    world_geometry.add_wall(glm::vec3(-1.0f, 0.0f, 1.0f),  glm::vec3( 1.0f, 0.0f, 1.0f), 2.0f);
    world_geometry.add_wall(glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(-1.0f, 0.0f, -1.0f), 1.5f);
    world_geometry.setup_buffers();
}

Scene::~Scene() {}

void Scene::include(Entity ntitty) {
    if (ntitty.fade) {
        timed_entities.push_back(ntitty);
    }
    else {
        entities.push_back(ntitty);
    }
}

int Scene::cast_ray(const glm::vec3& pos, const glm::vec3& dir, glm::vec3& hit_pos) {
    int hits = 0;
    float min_dist = 999999999.0f;
    glm::vec3 hit_pos_temp(0.0f);

    for (Entity e : entities) {
        if (e.collides(pos, dir, hit_pos_temp)) {
            hits++;
            float dist = glm::distance(hit_pos_temp, pos);
            if (dist < min_dist) {
                min_dist = dist;
                hit_pos  = hit_pos_temp;
            }
        }
    }

    return hits;
}

void Scene::render_world_geometry(Shader& shady) {
    world_geometry.draw(shady);
}
