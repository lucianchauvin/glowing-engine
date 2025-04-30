#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <algorithm>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <glad/glad.h>
#include <stb_image.h>
#include <vector>

class Texture_manager {
public:
    Texture_manager() {
        stbi_set_flip_vertically_on_load(true);
    }
    
    ~Texture_manager() {
        for (auto& pair : textures) {
            if (pair.second.id != 0) {
                glDeleteTextures(1, &pair.second.id);
            }
        }
    }

    bool init(const std::string& texture_base_path = "../resources/textures/") {
        base_path = texture_base_path;
        create_fallback_texture();
        return true;
    }

    struct TextureInfo {
        unsigned int id = 0;
        int width = 0;
        int height = 0;
        int channels = 0;
        GLenum format = GL_RGB;
        GLenum internal_format = GL_RGB;
        bool has_alpha = false;
        std::string path;
    };

    bool load_texture(const std::string& name, const std::string& file_path, 
                      bool generate_mipmaps = true, 
                      GLenum wrap_mode = GL_REPEAT,
                      GLenum min_filter = GL_LINEAR_MIPMAP_LINEAR,
                      GLenum mag_filter = GL_LINEAR) {
        
        std::string full_path = base_path + file_path;
        
        if (!std::filesystem::exists(full_path)) {
            std::cerr << "Texture file not found: " << full_path << std::endl;
            return false;
        }
        
        // Check if texture is already loaded
        auto it = textures.find(name);
        if (it != textures.end()) {
            // Delete the old texture if it exists
            if (it->second.id != 0) {
                glDeleteTextures(1, &it->second.id);
            }
        }
        
        int width, height, channels;
        unsigned char* data = stbi_load(full_path.c_str(), &width, &height, &channels, 0);
        
        if (!data) {
            std::cerr << "Failed to load texture: " << full_path << std::endl;
            std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
            return false;
        }
        
        GLenum format, internal_format;
        bool has_alpha = false;
        
        switch (channels) {
            case 1:
                format = GL_RED;
                internal_format = GL_RED;
                break;
            case 2:
                format = GL_RG;
                internal_format = GL_RG;
                break;
            case 3:
                format = GL_RGB;
                internal_format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                internal_format = GL_RGBA;
                has_alpha = true;
                break;
            default:
                std::cerr << "Unsupported number of channels: " << channels << std::endl;
                stbi_image_free(data);
                return false;
        }
        
        // Generate and bind texture
        unsigned int texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
        
        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        
        if (generate_mipmaps) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        
        TextureInfo info = {
            texture_id,
            width,
            height,
            channels,
            format,
            internal_format,
            has_alpha,
            file_path
        };
        
        textures[name] = info;
        
        // Free image data
        stbi_image_free(data);
        
        std::cout << "Loaded texture: " << name << " (" << width << "x" << height << ", " << channels << " channels) from " << file_path << std::endl;
        
        return true;
    }

    // Get a texture ID by name
    unsigned int get_texture_id(const std::string& name) const {
        auto it = textures.find(name);
        if (it != textures.end()) {
            return it->second.id;
        }
        
        std::cerr << "Texture '" << name << "' not found, returning fallback texture" << std::endl;
        
        // Return fallback texture
        auto fallback_it = textures.find("fallback");
        if (fallback_it != textures.end()) {
            return fallback_it->second.id;
        }
        
        return 0; // 0 is not a valid texture ID
    }
    
    // Get texture info by name
    const TextureInfo* get_texture_info(const std::string& name) const {
        auto it = textures.find(name);
        if (it != textures.end()) {
            return &it->second;
        }
        
        std::cerr << "Texture '" << name << "' not found!" << std::endl;
        return nullptr;
    }
    
    // Bind a texture to a specific texture unit
    bool bind_texture(const std::string& name, unsigned int texture_unit = 0) {
        unsigned int texture_id = get_texture_id(name);
        if (texture_id == 0) {
            return false;
        }
        
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        return true;
    }
    
    // Create a placeholder texture for when loading fails
    void create_fallback_texture() {
        // Create a simple checkerboard pattern
        const int width = 64, height = 64;
        const int channels = 3;
        unsigned char data[width * height * channels];
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                unsigned char color = ((x & 8) ^ (y & 8)) * 255;
                data[(y * width + x) * channels + 0] = color;
                data[(y * width + x) * channels + 1] = 128;
                data[(y * width + x) * channels + 2] = color;
            }
        }
        
        unsigned int texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        
        TextureInfo info = {
            texture_id,
            width,
            height,
            channels,
            GL_RGB,
            GL_RGB,
            false,
            "internal/fallback"
        };
        
        textures["fallback"] = info;
        
        std::cout << "Created fallback texture" << std::endl;
    }
    
    // Load all textures in a directory
    int load_directory(const std::string& directory = "", bool recursive = false) {
        std::string full_path = base_path;
        if (!directory.empty()) {
            full_path += directory;
        }
        
        if (!std::filesystem::exists(full_path) || !std::filesystem::is_directory(full_path)) {
            std::cerr << "Directory not found: " << full_path << std::endl;
            return 0;
        }
        
        int count = 0;
        for (const auto& entry : std::filesystem::directory_iterator(full_path)) {
            if (entry.is_regular_file()) {
                std::string file_path = entry.path().string();
                std::string ext = entry.path().extension().string();
                
                // Convert extensions to lowercase for comparison
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                
                // Check if this is an image file
                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga") {
                    std::string file_name = entry.path().stem().string();
                    std::string relative_path;
                    
                    if (directory.empty()) {
                        relative_path = entry.path().filename().string();
                    } else {
                        relative_path = directory + "/" + entry.path().filename().string();
                    }
                    
                    if (load_texture(file_name, relative_path)) {
                        count++;
                    }
                }
            }
            else if (recursive && entry.is_directory()) {
                // Recursively load textures in subdirectories
                std::string subdir = directory.empty() ? 
                    entry.path().filename().string() : 
                    directory + "/" + entry.path().filename().string();
                count += load_directory(subdir, true);
            }
        }
        
        return count;
    }
    
    // Get list of available texture names
    std::vector<std::string> get_texture_names() const {
        std::vector<std::string> names;
        names.reserve(textures.size());
        
        for (const auto& pair : textures) {
            names.push_back(pair.first);
        }
        
        return names;
    }
    
private:
    std::string base_path;
    std::unordered_map<std::string, TextureInfo> textures;
};

#endif // TEXTURE_MANAGER_H