#pragma once

#include <glm/glm.hpp>
#include <memory>

class Shader;

struct Light {
    glm::vec3 position = glm::vec3(2.0f, 2.0f, 2.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 5.0f;
    
    Light(const glm::vec3& pos = glm::vec3(2.0f, 2.0f, 2.0f),
          const glm::vec3& col = glm::vec3(1.0f, 1.0f, 1.0f),
          float intens = 0.707f)
        : position(pos), color(col), intensity(intens) {}
    
    // void apply(Shader& shader) const {
    //     shader.set_vec3("light_position", position);
    //     shader.set_vec3("light_color", color);
    //     shader.set_float("light_intensity", intensity);
    // }
};