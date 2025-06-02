#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <dearimgui/imgui.h>

#include <shader.h>

class Shader;

class Material {
public:
    Material(
        unsigned int albedo_map = 0, unsigned int normal_map = 0, unsigned int metallic_roughness_map = 0, unsigned int occlusion_map = 0, unsigned int emissive_map = 0) : albedo_map(albedo_map), normal_map(normal_map), metallic_roughness_map(metallic_roughness_map), occlusion_map(occlusion_map), emissive_map(emissive_map)
    {
        has_albedo_map = albedo_map != 0;
        has_normal_map = normal_map != 0;
        has_metallic_roughness_map = metallic_roughness_map != 0;
        has_occlusion_map = occlusion_map != 0;
        has_emissive_map = emissive_map != 0;
    }

    ~Material() {
        // free all textures
    }

    //glm::vec3 base_color;
    
    bool has_albedo_map;
    bool has_normal_map;
    bool has_metallic_roughness_map;
    bool has_occlusion_map;
    bool has_emissive_map;
    unsigned int albedo_map;
    unsigned int normal_map;
    unsigned int metallic_roughness_map;
    unsigned int occlusion_map;
    unsigned int emissive_map;

    //glm::vec2 texture_scale = glm::vec2(1.0f, 1.0f);
    //glm::vec2 texture_offset = glm::vec2(0.0f, 0.0f);

    void apply(Shader& shader) const {
        // apply base color texture
        // apply normal map texture
    }

private:
};
