#include "material_disney.h"

#include <fstream>

#include <json.hpp> // nlohmann/json
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <dearimgui/imgui.h>
#include <dearimgui/imgui_internal.h>

#include "shader.h"

using json = nlohmann::json;

Material_disney::Material_disney(const std::string& name)
    : name(name) {
    // Default constructor sets default values in the initializer list
}

Material_disney::~Material_disney() {
    // No explicit cleanup needed for simple values
}

// Copy constructor
Material_disney::Material_disney(const Material_disney& other)
    : name(other.name),
    base_color(other.base_color),
    subsurface(other.subsurface),
    metallic(other.metallic),
    specular(other.specular),
    specular_tint(other.specular_tint),
    roughness(other.roughness),
    anisotropic(other.anisotropic),
    sheen(other.sheen),
    sheen_tint(other.sheen_tint),
    clearcoat(other.clearcoat),
    clearcoat_gloss(other.clearcoat_gloss),
    ior(other.ior),
    base_color_map(other.base_color_map),
    normal_map(other.normal_map),
    roughness_map(other.roughness_map),
    metallic_map(other.metallic_map),
    specular_map(other.specular_map),
    anisotropic_map(other.anisotropic_map),
    texture_scale(other.texture_scale),
    texture_offset(other.texture_offset) {
}

// Copy assignment
Material_disney& Material_disney::operator=(const Material_disney& other) {
    if (this != &other) {
        name = other.name;
        base_color = other.base_color;
        subsurface = other.subsurface;
        metallic = other.metallic;
        specular = other.specular;
        specular_tint = other.specular_tint;
        roughness = other.roughness;
        anisotropic = other.anisotropic;
        sheen = other.sheen;
        sheen_tint = other.sheen_tint;
        clearcoat = other.clearcoat;
        clearcoat_gloss = other.clearcoat_gloss;
        ior = other.ior;
        base_color_map = other.base_color_map;
        normal_map = other.normal_map;
        roughness_map = other.roughness_map;
        metallic_map = other.metallic_map;
        specular_map = other.specular_map;
        anisotropic_map = other.anisotropic_map;
        texture_scale = other.texture_scale;
        texture_offset = other.texture_offset;
    }
    return *this;
}

// Move constructor
Material_disney::Material_disney(Material_disney&& other) noexcept
    : name(std::move(other.name)),
    base_color(other.base_color),
    subsurface(other.subsurface),
    metallic(other.metallic),
    specular(other.specular),
    specular_tint(other.specular_tint),
    roughness(other.roughness),
    anisotropic(other.anisotropic),
    sheen(other.sheen),
    sheen_tint(other.sheen_tint),
    clearcoat(other.clearcoat),
    clearcoat_gloss(other.clearcoat_gloss),
    ior(other.ior),
    base_color_map(other.base_color_map),
    normal_map(other.normal_map),
    roughness_map(other.roughness_map),
    metallic_map(other.metallic_map),
    specular_map(other.specular_map),
    anisotropic_map(other.anisotropic_map),
    texture_scale(other.texture_scale),
    texture_offset(other.texture_offset) {
    
    // Clear the moved-from object
    other.base_color_map = 0;
    other.normal_map = 0;
    other.roughness_map = 0;
    other.metallic_map = 0;
    other.specular_map = 0;
    other.anisotropic_map = 0;
}

// Move assignment
Material_disney& Material_disney::operator=(Material_disney&& other) noexcept {
    if (this != &other) {
        name = std::move(other.name);
        base_color = other.base_color;
        subsurface = other.subsurface;
        metallic = other.metallic;
        specular = other.specular;
        specular_tint = other.specular_tint;
        roughness = other.roughness;
        anisotropic = other.anisotropic;
        sheen = other.sheen;
        sheen_tint = other.sheen_tint;
        clearcoat = other.clearcoat;
        clearcoat_gloss = other.clearcoat_gloss;
        ior = other.ior;
        base_color_map = other.base_color_map;
        normal_map = other.normal_map;
        roughness_map = other.roughness_map;
        metallic_map = other.metallic_map;
        specular_map = other.specular_map;
        anisotropic_map = other.anisotropic_map;
        texture_scale = other.texture_scale;
        texture_offset = other.texture_offset;
        
        // Clear the moved-from object
        other.base_color_map = 0;
        other.normal_map = 0;
        other.roughness_map = 0;
        other.metallic_map = 0;
        other.specular_map = 0;
        other.anisotropic_map = 0;
    }
    return *this;
}

// Preset: Plastic
void Material_disney::set_plastic() {
    base_color = glm::vec3(0.8f, 0.8f, 0.8f);
    subsurface = 0.0f;
    metallic = 0.0f;
    specular = 0.5f;
    specular_tint = 0.0f;
    roughness = 0.3f;
    anisotropic = 0.0f;
    sheen = 0.0f;
    sheen_tint = 0.5f;
    clearcoat = 0.0f;
    clearcoat_gloss = 1.0f;
    ior = 1.5f;
}

// Preset: Metal
void Material_disney::set_metal() {
    base_color = glm::vec3(0.95f, 0.64f, 0.54f); // Copper-like
    subsurface = 0.0f;
    metallic = 1.0f;
    specular = 0.5f;
    specular_tint = 1.0f;
    roughness = 0.2f;
    anisotropic = 0.0f;
    sheen = 0.0f;
    sheen_tint = 0.5f;
    clearcoat = 0.0f;
    clearcoat_gloss = 1.0f;
    ior = 1.5f;
}

// Preset: Glass
void Material_disney::set_glass() {
    base_color = glm::vec3(0.95f, 0.95f, 0.95f);
    subsurface = 0.0f;
    metallic = 0.0f;
    specular = 0.9f;
    specular_tint = 0.0f;
    roughness = 0.05f;
    anisotropic = 0.0f;
    sheen = 0.0f;
    sheen_tint = 0.5f;
    clearcoat = 1.0f;
    clearcoat_gloss = 0.9f;
    ior = 1.5f;
}

// Preset: Silk
void Material_disney::set_silk() {
    base_color = glm::vec3(0.8f, 0.8f, 0.9f);
    subsurface = 0.5f;
    metallic = 0.0f;
    specular = 0.5f;
    specular_tint = 0.0f;
    roughness = 0.3f;
    anisotropic = 0.8f;
    sheen = 1.0f;
    sheen_tint = 0.8f;
    clearcoat = 0.0f;
    clearcoat_gloss = 1.0f;
    ior = 1.5f;
}

// Preset: Carpet
void Material_disney::set_carpet() {
    base_color = glm::vec3(0.5f, 0.3f, 0.1f);
    subsurface = 0.2f;
    metallic = 0.0f;
    specular = 0.1f;
    specular_tint = 0.0f;
    roughness = 1.0f;
    anisotropic = 0.0f;
    sheen = 0.5f;
    sheen_tint = 1.0f;
    clearcoat = 0.0f;
    clearcoat_gloss = 1.0f;
    ior = 1.5f;
}

// Preset: Rubber
void Material_disney::set_rubber() {
    base_color = glm::vec3(0.2f, 0.2f, 0.2f);
    subsurface = 0.0f;
    metallic = 0.0f;
    specular = 0.5f;
    specular_tint = 0.0f;
    roughness = 0.8f;
    anisotropic = 0.0f;
    sheen = 0.0f;
    sheen_tint = 0.5f;
    clearcoat = 0.0f;
    clearcoat_gloss = 1.0f;
    ior = 1.5f;
}

// Apply material parameters to a shader
void Material_disney::apply(Shader& shader) const {
    // Activate shader
    // shader.use();

    // Set all Disney BRDF parameters
    shader.setVec3("base_color", base_color);
    shader.setFloat("subsurface", subsurface);
    shader.setFloat("metallic", metallic);
    shader.setFloat("specular", specular);
    shader.setFloat("specular_tint", specular_tint);
    shader.setFloat("roughness", roughness);
    shader.setFloat("anisotropic", anisotropic);
    shader.setFloat("sheen", sheen);
    shader.setFloat("sheen_tint", sheen_tint);
    shader.setFloat("clearcoat", clearcoat);
    shader.setFloat("clearcoat_gloss", clearcoat_gloss);
    shader.setFloat("IOR", ior);
    
    // Set texture scales and offsets
    shader.setVec2("texture_scale", texture_scale);
    shader.setVec2("texture_offset", texture_offset);
    
    // Bind textures if they exist
    int texture_unit = 0;
    
    // Base color map
    if (base_color_map) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, base_color_map);
        shader.setInt("base_color_map", texture_unit);
        shader.setBool("has_base_color_map", true);
        texture_unit++;
    } else {
        shader.setBool("has_base_color_map", false);
    }
    
    // Normal map
    if (normal_map) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, normal_map);
        shader.setInt("normal_map", texture_unit);
        shader.setBool("has_normal_map", true);
        texture_unit++;
    } else {
        shader.setBool("has_normal_map", false);
    }
    
    // Roughness map
    if (roughness_map) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, roughness_map);
        shader.setInt("roughness_map", texture_unit);
        shader.setBool("has_roughness_map", true);
        texture_unit++;
    } else {
        shader.setBool("has_roughness_map", false);
    }
    
    // Metallic map
    if (metallic_map) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, metallic_map);
        shader.setInt("metallic_map", texture_unit);
        shader.setBool("has_metallic_map", true);
        texture_unit++;
    } else {
        shader.setBool("has_metallic_map", false);
    }
    
    // Other texture maps (similar pattern)
    // ...
}

// ImGui parameter slider with color coding
void Material_disney::draw_parameter_slider(const char* label, float* value, float min, float max, const char* tooltip) {
    // Calculate color based on parameter value (gradient from red to green)
    ImVec4 color = ImVec4(1.0f - *value, *value, 0.2f, 1.0f);
    
    // Tint slider color
    ImGui::PushStyleColor(ImGuiCol_FrameBg, 
        ImVec4(color.x * 0.2f, color.y * 0.2f, color.z * 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, color);
    
    // Show slider
    ImGui::SliderFloat(label, value, min, max, "%.2f");
    
    // Reset color
    ImGui::PopStyleColor(2);
    
    // Show tooltip if provided
    if (tooltip && ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", tooltip);
    }
}

// Draw texture slot selector in ImGui
bool Material_disney::draw_texture_slot(const char* label, GLuint* texture_id) {
    bool changed = false;
    
    // Get texture name if it exists
    const char* texture_name = *texture_id ? "Texture loaded" : "None";
    
    if (ImGui::BeginCombo(label, texture_name)) {
        // Option to clear texture
        bool is_none = (*texture_id == 0);
        if (ImGui::Selectable("None", is_none)) {
            *texture_id = 0;
            changed = true;
        }
        
        // Here you would typically list available textures
        // This is just an example - you'll need to integrate with your texture system
        
        // Example texture options (replace with your actual texture loading system)
        if (ImGui::Selectable("Load from file...", false)) {
            // Here you would open a file dialog
            // For now, let's assume texture ID 1 for demo purposes
            *texture_id = 1; // Replace with actual texture loading
            changed = true;
        }
        
        ImGui::EndCombo();
    }
    
    // Preview texture
    // if (*texture_id) {
    //     ImGui::SameLine();
    //     ImGui::Image((void*)(intptr_t)(*texture_id), ImVec2(32, 32));
    // }
    
    return changed;
}

// ImGui material editor window
bool Material_disney::draw_imgui_editor(ImGuiIO& io) {
    bool modified = false;
    
    // Create window with unique ID based on material name
    ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
    std::string window_title = name + " Properties###Material_disney_Editor" + std::to_string((intptr_t)this);
    
    if (!ImGui::Begin(window_title.c_str())) {
        ImGui::End();
        return false;
    }
    
    // Edit material name
    char name_buffer[128];
    strncpy(name_buffer, name.c_str(), sizeof(name_buffer) - 1);
    name_buffer[sizeof(name_buffer) - 1] = '\0';
    if (ImGui::InputText("Name", name_buffer, sizeof(name_buffer))) {
        name = name_buffer;
        modified = true;
    }
    
    // Presets dropdown
    if (ImGui::BeginCombo("Presets", "Select")) {
        if (ImGui::Selectable("Plastic")) {
            set_plastic();
            modified = true;
        }
        if (ImGui::Selectable("Metal")) {
            set_metal();
            modified = true;
        }
        if (ImGui::Selectable("Glass")) {
            set_glass();
            modified = true;
        }
        if (ImGui::Selectable("Silk")) {
            set_silk();
            modified = true;
        }
        if (ImGui::Selectable("Carpet")) {
            set_carpet();
            modified = true;
        }
        if (ImGui::Selectable("Rubber")) {
            set_rubber();
            modified = true;
        }
        ImGui::EndCombo();
    }
    
    // Save/Load buttons
    if (ImGui::Button("Save")) {
        // Here you would add a file dialog
        save_to_file(name + ".mat");
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        // Here you would add a file dialog
        load_from_file(name + ".mat");
        modified = true;
    }
    
    ImGui::Separator();
    
    // Base color with color picker
    if (ImGui::ColorEdit3("Base Color", &base_color[0])) {
        modified = true;
    }
    
    // Disney BRDF parameters
    ImGui::Text("Surface Properties");
    if (ImGui::TreeNodeEx("Base Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
        modified |= ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Controls surface micro-roughness");
        
        modified |= ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("0 = dielectric, 1 = metallic");
        
        modified |= ImGui::SliderFloat("Specular", &specular, 0.0f, 1.0f);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Specular reflection amount");
        
        modified |= ImGui::SliderFloat("IOR", &ior, 1.0f, 2.5f);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Index of refraction (1.5 is typical for most materials)");
            
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNodeEx("Advanced Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Using the colored parameter slider helper
        draw_parameter_slider("Subsurface", &subsurface, 0.0f, 1.0f, 
                           "Subsurface scattering amount");
                           
        draw_parameter_slider("Specular Tint", &specular_tint, 0.0f, 1.0f, 
                           "Tints specular reflection towards base color");
                           
        draw_parameter_slider("Anisotropic", &anisotropic, 0.0f, 1.0f, 
                           "Directional reflection, like brushed metal");
                           
        draw_parameter_slider("Sheen", &sheen, 0.0f, 1.0f, 
                           "Velvet-like rim reflection");
                           
        draw_parameter_slider("Sheen Tint", &sheen_tint, 0.0f, 1.0f, 
                           "Tints sheen towards base color");
                           
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNodeEx("Clearcoat Layer", ImGuiTreeNodeFlags_DefaultOpen)) {
        draw_parameter_slider("Clearcoat", &clearcoat, 0.0f, 1.0f, 
                           "Secondary specular layer");
                           
        draw_parameter_slider("Clearcoat Gloss", &clearcoat_gloss, 0.0f, 1.0f, 
                           "Glossiness of clearcoat layer");
                           
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNodeEx("Textures", 0)) {
        ImGui::Text("Texture Coordinates");
        if (ImGui::DragFloat2("Scale", &texture_scale[0], 0.01f, 0.1f, 10.0f)) {
            modified = true;
        }
        if (ImGui::DragFloat2("Offset", &texture_offset[0], 0.01f, -1.0f, 1.0f)) {
            modified = true;
        }
        
        ImGui::Separator();
        ImGui::Text("Texture Maps");
        
        // Texture slots
        if (draw_texture_slot("Base Color Map", &base_color_map)) modified = true;
        if (draw_texture_slot("Normal Map", &normal_map)) modified = true;
        if (draw_texture_slot("Roughness Map", &roughness_map)) modified = true;
        if (draw_texture_slot("Metallic Map", &metallic_map)) modified = true;
        if (draw_texture_slot("Specular Map", &specular_map)) modified = true;
        if (draw_texture_slot("Anisotropic Map", &anisotropic_map)) modified = true;
        
        ImGui::TreePop();
    }
    
    ImGui::End();
    
    return modified;
}

// Save material to file
bool Material_disney::save_to_file(const std::string& filename) const {
    try {
        // Create JSON object
        json j;
        
        // Save material properties
        j["name"] = name;
        j["base_color"] = {base_color.x, base_color.y, base_color.z};
        j["subsurface"] = subsurface;
        j["metallic"] = metallic;
        j["specular"] = specular;
        j["specular_tint"] = specular_tint;
        j["roughness"] = roughness;
        j["anisotropic"] = anisotropic;
        j["sheen"] = sheen;
        j["sheen_tint"] = sheen_tint;
        j["clearcoat"] = clearcoat;
        j["clearcoat_gloss"] = clearcoat_gloss;
        j["ior"] = ior;
        
        // Texture scaling and offset
        j["texture_scale"] = {texture_scale.x, texture_scale.y};
        j["texture_offset"] = {texture_offset.x, texture_offset.y};
        
        // Note: Texture IDs are not saved directly
        // In a real implementation, you'd save paths to textures instead
        
        // Write to file
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(4); // Pretty print with 4-space indent
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

// Load material from file
bool Material_disney::load_from_file(const std::string& filename) {
    try {
        // Read JSON from file
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        // Load material properties
        name = j["name"];
        
        auto& j_base_color = j["base_color"];
        base_color = glm::vec3(j_base_color[0], j_base_color[1], j_base_color[2]);
        
        subsurface = j["subsurface"];
        metallic = j["metallic"];
        specular = j["specular"];
        specular_tint = j["specular_tint"];
        roughness = j["roughness"];
        anisotropic = j["anisotropic"];
        sheen = j["sheen"];
        sheen_tint = j["sheen_tint"];
        clearcoat = j["clearcoat"];
        clearcoat_gloss = j["clearcoat_gloss"];
        ior = j["ior"];
        
        // Texture scaling and offset
        auto& j_texture_scale = j["texture_scale"];
        texture_scale = glm::vec2(j_texture_scale[0], j_texture_scale[1]);
        
        auto& j_texture_offset = j["texture_offset"];
        texture_offset = glm::vec2(j_texture_offset[0], j_texture_offset[1]);
        
        // Note: Textures would need to be loaded by your texture system
        // In a real implementation, you'd load textures from saved paths
        
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}