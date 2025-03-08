#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using std::string, std::cerr, std::endl;

class Model {
public:
    Model(){};
    ~Model() {
        glDeleteBuffers(1, &posBufID);
        glDeleteBuffers(1, &norBufID);
        glDeleteBuffers(1, &texBufID);
    };

    void load_mesh(const string &meshName) {
        // Load geometry
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        string errStr;
        bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
        if(!rc) {
            cerr << errStr << endl;
        } else {
            // Some OBJ files have different indices for vertex positions, normals,
            // and texture coordinates. For example, a cube corner vertex may have
            // three different normals. Here, we are going to duplicate all such
            // vertices.ini
            // Loop over shapes
            for(size_t s = 0; s < shapes.size(); s++) {
                // Loop over faces (polygons)
                size_t index_offset = 0;
                for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                    size_t fv = shapes[s].mesh.num_face_vertices[f];
                    // Loop over vertices in the face.
                    for(size_t v = 0; v < fv; v++) {
                        // access to vertex
                        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                        posBuf.push_back(attrib.vertices[3*idx.vertex_index+0]);
                        posBuf.push_back(attrib.vertices[3*idx.vertex_index+1]);
                        posBuf.push_back(attrib.vertices[3*idx.vertex_index+2]);
                        if(!attrib.normals.empty()) {
                            norBuf.push_back(attrib.normals[3*idx.normal_index+0]);
                            norBuf.push_back(attrib.normals[3*idx.normal_index+1]);
                            norBuf.push_back(attrib.normals[3*idx.normal_index+2]);
                        }
                        if(!attrib.texcoords.empty()) {
                            texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
                            texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
                        }
                    }
                    index_offset += fv;
                    // per-face material (IGNORE)
                    shapes[s].mesh.material_ids[f];
                }
            }
        }
    }
    // void fitToUnitBox();
    void init() {
        glGenBuffers(1, &posBufID);
        glBindBuffer(GL_ARRAY_BUFFER, posBufID);
        glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);

        if (!norBuf.empty()) {
            glGenBuffers(1, &norBufID);
            glBindBuffer(GL_ARRAY_BUFFER, norBufID);
            glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float), &norBuf[0], GL_STATIC_DRAW);
        }

        if (!texBuf.empty()) {
            glGenBuffers(1, &texBufID);
            glBindBuffer(GL_ARRAY_BUFFER, texBufID);
            glBufferData(GL_ARRAY_BUFFER, texBuf.size() * sizeof(float), &texBuf[0], GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void draw(Shader shader) const {
        // Bind position buffer
        int h_pos = glGetAttribLocation(shader.ID, "aPos");
        glEnableVertexAttribArray(h_pos);
        glBindBuffer(GL_ARRAY_BUFFER, posBufID);
        glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        // Bind normal buffer
        int h_nor = glGetAttribLocation(shader.ID, "aNor");
        if(h_nor != -1 && norBufID != 0) {
            glEnableVertexAttribArray(h_nor);
            glBindBuffer(GL_ARRAY_BUFFER, norBufID);
            glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }

        // Bind texcoords buffer
        int h_tex = glGetAttribLocation(shader.ID, "aTex");
        if(h_tex != -1 && texBufID != 0) {
            glEnableVertexAttribArray(h_tex);
            glBindBuffer(GL_ARRAY_BUFFER, texBufID);
            glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        }

        // Draw
        int count = posBuf.size()/3; // number of indices to be rendered
        glDrawArrays(GL_TRIANGLES, 0, count);

        // Disable and unbind
        if(h_tex != -1) {
            glDisableVertexAttribArray(h_tex);
        }
        if(h_nor != -1) {
            glDisableVertexAttribArray(h_nor);
        }
        glDisableVertexAttribArray(h_pos);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
private:
    std::vector<float> posBuf;
    std::vector<float> norBuf;
    std::vector<float> texBuf;

    unsigned int posBufID = 0;
    unsigned int norBufID = 0;
    unsigned int texBufID = 0;
};

#endif // MODEL_H
