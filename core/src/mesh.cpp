#include "mesh.h"

#define CHECK_GL_ERROR() { \
    GLenum err = glGetError(); \
    if (err != GL_NO_ERROR) { \
        std::cerr << "OpenGL error at line " << __LINE__ << ": " << err << std::endl; \
    } \
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setup_mesh();
}

void Mesh::setup_mesh() {
    glGenVertexArrays(1, &VAO); CHECK_GL_ERROR();
    glGenBuffers(1, &VBO); CHECK_GL_ERROR();
    glGenBuffers(1, &EBO); CHECK_GL_ERROR();

    glBindVertexArray(VAO); CHECK_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, VBO); CHECK_GL_ERROR();

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);   CHECK_GL_ERROR();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); CHECK_GL_ERROR();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW); CHECK_GL_ERROR();

                 // vertex positions CHECK_GL_ERROR();
    glEnableVertexAttribArray(0);	 CHECK_GL_ERROR();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); CHECK_GL_ERROR();
    // vertex normals CHECK_GL_ERROR();
    glEnableVertexAttribArray(1);	 CHECK_GL_ERROR();
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal)); CHECK_GL_ERROR();
    // vertex texture coords CHECK_GL_ERROR();
    glEnableVertexAttribArray(2);	 CHECK_GL_ERROR();
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords)); CHECK_GL_ERROR();

    glBindVertexArray(0); CHECK_GL_ERROR();
}

void Mesh::draw(Shader &shader) {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);

        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}  