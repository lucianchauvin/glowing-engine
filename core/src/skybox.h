#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>
#include <iostream>

#include <glad/glad.h>
#include <stb_image.h>

class Skybox {
public:
    Skybox(const std::string& skybox_name) {
        std::vector<std::string> faceNames = {
            "px.png", "nx.png", "py.png", "ny.png", "pz.png", "nz.png"
        };

  /*      "right.jpg",
            "left.jpg",
            "top.jpg",
            "bottom.jpg",
            "front.jpg",
            "back.jpg"*/

        std::vector<std::string> faces;
        std::string basePath = "../resources/textures/skyboxes/" + skybox_name + "/";

        for (const auto& name : faceNames) {
            faces.push_back(basePath + name);
            std::cout << basePath + name << std::endl;
        }

        texture_id = loadCubemap(faces);
        setupCube();
    }


    void draw() const {
        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

private:
    unsigned int vao, vbo, texture_id;

    void setupCube() {
        float skyboxVertices[] = {
                -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
                 1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
                 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f

                 //-1.0f, 1.0f, -1.0f,
                //-1.0f, -1.0f, -1.0f,
                //1.0f, -1.0f, -1.0f,
                //1.0f, -1.0f, -1.0f,
                //1.0f, 1.0f, -1.0f,
                //-1.0f, 1.0f, -1.0f,

                //-1.0f, -1.0f, 1.0f,
                //-1.0f, -1.0f, -1.0f,
                //-1.0f, 1.0f, -1.0f,
                //-1.0f, 1.0f, -1.0f,
                //-1.0f, 1.0f, 1.0f,
                //-1.0f, -1.0f, 1.0f,

                //1.0f, -1.0f, -1.0f,
                //1.0f, -1.0f, 1.0f,
                //1.0f, 1.0f, 1.0f,
                //1.0f, 1.0f, 1.0f,
                //1.0f, 1.0f, -1.0f,
                //1.0f, -1.0f, -1.0f,

                //-1.0f, -1.0f, 1.0f,
                //-1.0f, 1.0f, 1.0f,
                //1.0f, 1.0f, 1.0f,
                //1.0f, 1.0f, 1.0f,
                //1.0f, -1.0f, 1.0f,
                //-1.0f, -1.0f, 1.0f,

                //-1.0f, 1.0f, -1.0f,
                //1.0f, 1.0f, -1.0f,
                //1.0f, 1.0f, 1.0f,
                //1.0f, 1.0f, 1.0f,
                //-1.0f, 1.0f, 1.0f,
                //-1.0f, 1.0f, -1.0f,

                //-1.0f, -1.0f, -1.0f,
                //-1.0f, -1.0f, 1.0f,
                //1.0f, -1.0f, -1.0f,
                //1.0f, -1.0f, -1.0f,
                //-1.0f, -1.0f, 1.0f,
                //1.0f, -1.0f, 1.0f
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    //unsigned int loadCubemap(const std::vector<std::string>& faces) {
    //    unsigned int texID;
    //    glGenTextures(1, &texID);
    //    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    //    int width, height, channels;
    //    for (unsigned int i = 0; i < faces.size(); ++i) {
    //        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
    //        if (data) {
    //            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
    //                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //            stbi_image_free(data);
    //        }
    //        else {
    //            std::cerr << "Failed to load cubemap face: " << faces[i] << '\n';
    //            stbi_image_free(data);
    //        }
    //    }

    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //    return texID;
    //}

    unsigned int loadCubemap(const std::vector<std::string>& faces) {
        unsigned int texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

        int width, height, channels;
        for (unsigned int i = 0; i < faces.size(); ++i) {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
            if (data) {
                GLenum format;
                if (channels == 1)
                    format = GL_RED;
                else if (channels == 3)
                    format = GL_RGB;
                else if (channels == 4)
                    format = GL_RGBA;
                else {
                    std::cerr << "Unsupported channel count in cubemap: " << channels << '\n';
                    stbi_image_free(data);
                    continue;
                }

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else {
                std::cerr << "Failed to load cubemap face: " << faces[i] << '\n';
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return texID;
    }

};
#endif
