#include <vector>
#include <string>
#include <iostream>
#include <cassert>

#include <glad/glad.h>
#include <stb_image.h>

#include "texture_manager.h"

namespace Texture_manager {

    static std::vector<unsigned int> textures;
    static std::vector<std::string> paths;
    //static std::vector<texture_data> texture_data;

    bool loaded_already(const std::string& new_path, size_t& existing_idx) {
        for (size_t i = 0; i < paths.size(); i++) {
            if (new_path == paths[i]) {
                existing_idx = i;
                return true;
            }
        }
        return false;
    }

    void init() {
        //stbi_set_flip_vertically_on_load(true);
        texture_handle zero = load_from_path("../resources/textures/missing.png");
        assert(!textures.empty());
    }

    void cleanup() {
        for (unsigned int texture : textures) {
            if (texture != 0) {
                glDeleteTextures(1, &texture);
            }
        }
        textures.clear();
        paths.clear();
    }

    texture_handle load_from_path(const std::string& file_path) {
        size_t existing_texture_index;
        if (loaded_already(file_path, existing_texture_index)) {
            return existing_texture_index;
        }

        unsigned int texture_id = 0;
        glGenTextures(1, &texture_id);
        int width, height, nrComponents;
        unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &nrComponents, 0);

        if (data) {
            GLenum format = 0;
            if (nrComponents == 1) format = GL_RED;
            else if (nrComponents == 3) format = GL_RGB;
            else if (nrComponents == 4) format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
            textures.push_back(texture_id);
            paths.push_back(file_path);
            std::cout << "[TEXTURE] Loaded: " << file_path << std::endl;
            return textures.size() - 1;
        }
        else {
            std::cout << "Texture failed to load: " << file_path << std::endl;
            stbi_image_free(data);
            return 0;
        }
    }

    void bind(texture_handle texture_id, unsigned int texture_unit) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, textures[texture_id]);
    }

    size_t get_texture_count() {
        return textures.size();
    }

    std::string get_name(texture_handle texture_id) {
        return paths[texture_id];
    }
}
