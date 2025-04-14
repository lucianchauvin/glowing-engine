#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "entity.h"
#include "shader.h"
#include "world_geometry.h"

class Scene {
public:
    Scene();
    ~Scene();

    void include(Entity ntitty);
    // returns the number of hits
    int cast_ray(const glm::vec3& pos, const glm::vec3& dir, glm::vec3& hit_pos);
    void render_world_geometry(Shader& shady);

    std::vector<Entity> entities;
    std::vector<Entity> timed_entities;
    World_geometry world_geometry;
};
#endif
