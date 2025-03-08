#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include "entity.h"

class Scene {
public:
    Scene() {
        entities = std::vector<Entity>();
        timed_entities = std::vector<Entity>();
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

// private:
    std::vector<Entity> entities;
    std::vector<Entity> timed_entities;
};
#endif
