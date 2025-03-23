#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "shader.h"

// If you're using OpenGL calls (glDeleteBuffers, etc.) in Model methods,
// you need to ensure glad is included somewhere (usually in .cpp).
// (But you can include <glad/glad.h> here if you do inlined code.)

// NOTE: Importantly, DO NOT #define TINYOBJLOADER_IMPLEMENTATION here.
class Model {
public:
    Model();
    Model(const std::vector<float> &pos, const std::vector<float> &nor);
    ~Model();

    void load_mesh(const std::string &meshName);
    void init();
    void draw(Shader& shader) const;

private:
    // Buffers with vertex data
    std::vector<float> posBuf;
    std::vector<float> norBuf;
    std::vector<float> texBuf;

    // OpenGL buffer IDs
    unsigned int posBufID = 0;
    unsigned int norBufID = 0;
    unsigned int texBufID = 0;
    unsigned int vaoID;
    int numVerts;
};
#endif
