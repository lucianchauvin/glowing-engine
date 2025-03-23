#include "model.h"

// 1) You only define TINYOBJLOADER_IMPLEMENTATION **once**, here:
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glad/glad.h>
#include <iostream>  // for std::cerr, std::endl, etc.
#include <cstdio>    // for printf() if you use it

using std::cerr;
using std::endl;

Model::Model() {
    // Optional: any default setup
}

Model::Model(const std::vector<float> &pos, const std::vector<float> &nor)
{
    posBuf = pos;
    norBuf = nor;
    printf("posBuf sz: %d\n", static_cast<int>(posBuf.size()));
    printf("norBuf sz: %d\n", static_cast<int>(norBuf.size()));
    init();
}

Model::~Model()
{
    // Cleanup OpenGL buffers if they exist
    glDeleteVertexArrays(1, &vaoID);
    glDeleteBuffers(1, &posBufID);
    glDeleteBuffers(1, &norBufID);
    glDeleteBuffers(1, &texBufID);
}

void Model::load_mesh(const std::string &meshName)
{
    // Create local tinyobj structures
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string errStr;

    // Load .obj file
    bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
    if(!rc) {
        cerr << errStr << endl;
        return;
    }

    // Clear any existing data in case this model object is re-used
    posBuf.clear();
    norBuf.clear();
    texBuf.clear();

    // Loop over shapes
    for(size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        // Loop over faces
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = shapes[s].mesh.num_face_vertices[f];
            // Loop over vertices in the face
            for(size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                // Positions
                posBuf.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
                posBuf.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
                posBuf.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
                // Normals
                if(!attrib.normals.empty()) {
                    norBuf.push_back(attrib.normals[3 * idx.normal_index + 0]);
                    norBuf.push_back(attrib.normals[3 * idx.normal_index + 1]);
                    norBuf.push_back(attrib.normals[3 * idx.normal_index + 2]);
                }
                // Texture coords
                if(!attrib.texcoords.empty()) {
                    texBuf.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                    texBuf.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
                }
            }
            index_offset += fv;
        }
    }

    // Finally, set up the OpenGL buffers
    init();
}

void Model::init()
{
    numVerts = (int) posBuf.size() / 3;
    // First, generate and bind our dedicated VAO.
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    // ----- Positions -----
    glGenBuffers(1, &posBufID);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float),
                 posBuf.data(), GL_STATIC_DRAW);
    // We will use location=0 for position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // ----- Normals -----
    if (!norBuf.empty()) {
        glGenBuffers(1, &norBufID);
        glBindBuffer(GL_ARRAY_BUFFER, norBufID);
        glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float),
                     norBuf.data(), GL_STATIC_DRAW);
        // Use location=1 for normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // ----- Texture Coords -----
    if (!texBuf.empty()) {
        glGenBuffers(1, &texBufID);
        glBindBuffer(GL_ARRAY_BUFFER, texBufID);
        glBufferData(GL_ARRAY_BUFFER, texBuf.size() * sizeof(float),
                     texBuf.data(), GL_STATIC_DRAW);
        // Use location=2 for texCoord
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // Unbind the VAO (and the array buffer)
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::draw(Shader &shader) const {
    // Make sure we use the given shader program
    glUseProgram(shader.ID);

    // Bind our VAO (which already has all buffers & pointers)
    glBindVertexArray(vaoID);

    // Draw all triangles
    glDrawArrays(GL_TRIANGLES, 0, numVerts);

    // Unbind for cleanliness
    glBindVertexArray(0);
    // In many engines you might keep the program in use,
    // but it's safe to unuse here if you prefer:
    // glUseProgram(0);
}

// void Model::init()
// {   
//     glGenVertexArrays(1, &vaoID);
//     glBindVertexArray(vaoID);
//     // Positions
//     glGenBuffers(1, &posBufID);
//     glBindBuffer(GL_ARRAY_BUFFER, posBufID);
//     glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float),
//                  posBuf.data(), GL_STATIC_DRAW);

//     // Normals
//     if (!norBuf.empty()) {
//         glGenBuffers(1, &norBufID);
//         glBindBuffer(GL_ARRAY_BUFFER, norBufID);
//         glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float),
//                      norBuf.data(), GL_STATIC_DRAW);
//     }

//     // Texture coordinates
//     if (!texBuf.empty()) {
//         glGenBuffers(1, &texBufID);
//         glBindBuffer(GL_ARRAY_BUFFER, texBufID);
//         glBufferData(GL_ARRAY_BUFFER, texBuf.size() * sizeof(float),
//                      texBuf.data(), GL_STATIC_DRAW);
//     }

//     // Unbind
//     glBindBuffer(GL_ARRAY_BUFFER, 0);

//     printf("SETUP BUFFERS\n");
// }

// void Model::draw(Shader shader) const
// {
//     // Bind position buffer
//     int h_pos = glGetAttribLocation(shader.ID, "aPos");
//     glEnableVertexAttribArray(h_pos);
//     glBindBuffer(GL_ARRAY_BUFFER, posBufID);
//     glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

//     // Bind normal buffer
//     int h_nor = glGetAttribLocation(shader.ID, "aNor");
//     if(h_nor != -1 && norBufID != 0) {
//         glEnableVertexAttribArray(h_nor);
//         glBindBuffer(GL_ARRAY_BUFFER, norBufID);
//         glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
//     }

//     // Bind texcoords buffer
//     int h_tex = glGetAttribLocation(shader.ID, "aTex");
//     if(h_tex != -1 && texBufID != 0) {
//         glEnableVertexAttribArray(h_tex);
//         glBindBuffer(GL_ARRAY_BUFFER, texBufID);
//         glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
//     }

//     // Number of triangles to render
//     int count = static_cast<int>(posBuf.size()) / 3;
//     glDrawArrays(GL_TRIANGLES, 0, count);

//     // Disable
//     if(h_tex != -1) {
//         glDisableVertexAttribArray(h_tex);
//     }
//     if(h_nor != -1) {
//         glDisableVertexAttribArray(h_nor);
//     }
//     glDisableVertexAttribArray(h_pos);

//     // Unbind
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
// }
