#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "shader.h"
#include "material.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};
 
class Mesh {
    public:
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        Material material;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material);
        
        void draw(const Shader &shader) const;
        void update_vertex_buffer();

    private:
        unsigned int VAO, VBO, EBO;

        void setup_mesh();
};
#endif
