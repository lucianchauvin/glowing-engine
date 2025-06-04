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
        has_albedo = albedo_map != 0;
        has_normal = normal_map != 0;
        has_metallic_roughness = metallic_roughness_map != 0;
        has_occlusion = occlusion_map != 0;
        has_emissive = emissive_map != 0;

        printf("a: %d, n: %d\n", has_albedo, has_normal);
    }

    ~Material() {
        // free all textures
    }

    //glm::vec3 base_color;
    
    bool has_albedo;
    bool has_normal;
    bool has_metallic_roughness;
    bool has_occlusion;
    bool has_emissive;
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
