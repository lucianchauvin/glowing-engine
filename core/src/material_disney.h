#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <dearimgui/imgui.h>

class Shader;

class Material_disney {
public:
    Material_disney(const std::string& name = "Disney Material");
    ~Material_disney();

    Material_disney(const Material_disney& other);
    Material_disney& operator=(const Material_disney& other);
    Material_disney(Material_disney&& other) noexcept;
    Material_disney& operator=(Material_disney&& other) noexcept;

    std::string name;
    glm::vec3 base_color = glm::vec3(0.8f, 0.8f, 0.8f);
    float subsurface = 0.0f;          // 0-1: Subsurface scattering amount
    float metallic = 0.0f;            // 0-1: How metallic the material is
    float specular = 0.5f;            // 0-1: Specular reflection amount
    float specular_tint = 0.0f;       // 0-1: Tint specular with base color
    float roughness = 0.5f;           // 0-1: Surface roughness
    float anisotropic = 0.0f;         // 0-1: Anisotropic reflection amount
    float sheen = 0.0f;               // 0-1: Sheen amount
    float sheen_tint = 0.5f;          // 0-1: Tint sheen with base color
    float clearcoat = 0.0f;           // 0-1: Clearcoat layer amount
    float clearcoat_gloss = 1.0f;     // 0-1: Clearcoat layer glossiness
    float ior = 1.5f;                 // Index of refraction (usually 1.5)

    GLuint base_color_map = 0;
    GLuint normal_map = 0;
    GLuint roughness_map = 0;
    GLuint metallic_map = 0;
    GLuint specular_map = 0;
    GLuint anisotropic_map = 0;

    // Texture mapping properties
    glm::vec2 texture_scale = glm::vec2(1.0f, 1.0f);
    glm::vec2 texture_offset = glm::vec2(0.0f, 0.0f);

    // Presets
    void set_plastic();
    void set_metal();
    void set_glass();
    void set_silk();
    void set_carpet();
    void set_rubber();

    // Apply material to shader
    void apply(Shader& shader) const;
    
    // ImGui editor function
    bool draw_imgui_editor(ImGuiIO& io);
    
    // Helper to display a color-coded parameter in ImGui
    static void draw_parameter_slider(const char* label, float* value, float min, float max, 
                                    const char* tooltip = nullptr);
    
    // Save/load material to/from file
    bool save_to_file(const std::string& filename) const;
    bool load_from_file(const std::string& filename);
    
private:
    // Helper for showing texture slots in ImGui
    bool draw_texture_slot(const char* label, GLuint* texture_id);
};