#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include "entity.h"

class Scene {
public:
    Scene() {
        entites = std::vector<Entity>();
    }
    ~Scene(){};

    void include(Entity ntitty) {
        entites.push_back(ntitty);
    }
    // void render() const;

// private:
    std::vector<Entity> entites;
};

#endif
