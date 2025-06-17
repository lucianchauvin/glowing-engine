#include "mesh.h"
#include "texture_manager.h"

#define CHECK_GL_ERROR() { \
    GLenum err = glGetError(); \
    if (err != GL_NO_ERROR) { \
        std::cerr << "OpenGL error at line " << __LINE__ << ": " << err << std::endl; \
    } \
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material) : material(material) {
    this->vertices = vertices;
    this->indices = indices;

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

    // vertex tangent coords CHECK_GL_ERROR();
    glEnableVertexAttribArray(3);	 CHECK_GL_ERROR();
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent)); CHECK_GL_ERROR();

    // vertex bitangents
    glEnableVertexAttribArray(4); CHECK_GL_ERROR();
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent)); CHECK_GL_ERROR();

    glBindVertexArray(0); CHECK_GL_ERROR();
}

// todo gonna be way different
void Mesh::draw(const Shader* shader) const {

    //shader.setBool("has_diffuse", material.has_albedo);
    //if (material.has_albedo) {
        Texture_manager::bind(material.albedo_map, 0);
        shader->setInt("diffuse", 0);
        //printf("bound diffuse: %s\n", Texture_manager::get_name(material.albedo_map).c_str());
    //}

    shader->setBool("has_normal", material.has_normal);
    if (material.has_normal) {
        Texture_manager::bind(material.normal_map, 1);
        shader->setInt("normal", 1);
        //printf("bound normal: %s\n", Texture_manager::get_name(material.normal_map).c_str());
    }

    // Add metallic-roughness texture
    shader->setBool("has_metallic_roughness", material.metallic_roughness_map != 0);
    if (material.metallic_roughness_map != 0) {
        Texture_manager::bind(material.metallic_roughness_map, 2);
        shader->setInt("metallic_roughness", 2);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}  

void Mesh::update_vertex_buffer() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}