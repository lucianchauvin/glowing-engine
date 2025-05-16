#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <dearimgui/imgui.h>

#include <shader.h>

class Shader;

class Material {
public:
    Material(unsigned int base_color_texture_id, unsigned int normal_map_texture_id) {
        
    }

    ~Material() {

    }

    glm::vec3 base_color = glm::vec3(1.0f, 0.4f, 0.7f);
    
    bool has_albedo_map;
    unsigned int albedo_map;

    bool has_normal_map;
    unsigned int normal_map = -1;
    
    //GLuint roughness_metallic_map = 0;

    glm::vec2 texture_scale = glm::vec2(1.0f, 1.0f);
    glm::vec2 texture_offset = glm::vec2(0.0f, 0.0f);

    void apply(Shader& shader) const {
        // apply base color texture
        // apply normal map texture
    }

private:
};
