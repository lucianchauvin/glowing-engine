#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <algorithm>
#include <string>
#include <filesystem>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <stb_image.h>

class Texture_manager {
public:
    Texture_manager() {
        stbi_set_flip_vertically_on_load(true);

        // load default texture into slot 0
        unsigned int zero = load_from_path("../resources/textures/missing.png");
        paths.push_back("../resources/textures/missing.png");
        assert(!textures.empty());
    }
    
    ~Texture_manager() {
        for (unsigned int &texture : textures) {
            if (&texture != 0) {
                glDeleteTextures(1, &texture);
            }
        }
    }

    unsigned int load_from_path(const std::string& file_path) {
        int existing_texture_index = loaded_already(file_path);
        if (existing_texture_index != -1) {
            return existing_texture_index;
        }

        unsigned int texture_id = 0;
        glGenTextures(1, &texture_id);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format = 0;
            if (nrComponents == 1) {
                format = GL_RED;
            }
            else if (nrComponents == 3) {
                format = GL_RGB;
            }
            else if (nrComponents == 4) {
                format = GL_RGBA;
            }

            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // Set texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);

            textures.push_back(texture_id);
            paths.push_back(file_path);

            return textures.size() - 1;
        }
        else {
            std::cout << "Texture failed to load: " << file_path << std::endl;
            stbi_image_free(data);

            return 0;
        }
    }
    
    void bind(int texture_id, unsigned int texture_unit = 0) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, textures[texture_id]);
    }

private:
    std::vector<unsigned int> textures;
    std::vector<std::string> paths;

    // returns -1 if not loaded, else returns index of loaded texture
    int loaded_already(const std::string& new_path) {
        for (size_t i = 0; i < paths.size(); i++) {
            if (new_path == paths[i]) {
                return i;
            }
        }
        return -1;
    }
};
#endif
