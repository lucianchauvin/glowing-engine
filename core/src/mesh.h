#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include "shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};
 
// TODO move to texture manager
struct Texture {
    unsigned int id;
    std::string type;
    std::string path;  // we store the path of the texture to compare with other textures
};  

class Mesh {
    public:
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture>      textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        
        void draw(Shader &shader);
        void update_vertex_buffer();

    private:
        unsigned int VAO, VBO, EBO;

        void setup_mesh();
};
#endif
