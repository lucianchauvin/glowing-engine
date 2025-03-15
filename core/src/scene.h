#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include "entity.h"
#include "shader.h"
#include "world_geometry.h"

class Scene {
public:
    Scene() {
        entities = std::vector<Entity>();
        timed_entities = std::vector<Entity>();

        world_geometry.add_wall(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 1.0f), 5.0f);
        world_geometry.add_wall(glm::vec3(-1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), 2.0f);
        world_geometry.setup_buffers();
    }
    ~Scene(){};

    void include(Entity ntitty) {
        if (ntitty.fade) {
            timed_entities.push_back(ntitty);
        }
        else {
            entities.push_back(ntitty);
        }
    }
    // return num of hits
    int cast_ray(const glm::vec3& pos, const glm::vec3& dir, glm::vec3& hit_pos) {
        int hits = 0;
        float min_dist = 999999999.0f;
        glm::vec3 hit_pos_temp = glm::vec3(0.0f);
        for (Entity e : entities) {
            if (e.collides(pos, dir, hit_pos_temp)) {
                hits++;
                float dist = glm::distance(hit_pos_temp, pos);
                if (dist < min_dist) {
                    hit_pos = hit_pos_temp;
                    min_dist = dist;
                }
            }
        }

        return hits;
    }

    void render_world_geometry(Shader& shady) {
        world_geometry.draw(shady);
    }

// private:
    std::vector<Entity> entities;
    std::vector<Entity> timed_entities;
    World_geometry world_geometry;
};
#endif
