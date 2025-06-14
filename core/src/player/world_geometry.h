// #ifndef WORLD_GEOMETRY_H
// #define WORLD_GEOMETRY_H

// #include <vector>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include "shader.h"
// #include "general/colors.h"

// struct World_quad {
//     glm::vec3 vertices[4];
//     glm::vec2 texCoords[4];
//     glm::vec3 normal;
//     glm::vec3 color;
//     bool is_collidable;
    
//     void calculate_normal() {
//         glm::vec3 edge1 = vertices[1] - vertices[0];
//         glm::vec3 edge2 = vertices[2] - vertices[0];
//         normal = glm::normalize(glm::cross(edge1, edge2));
//     }
// };

// class World_geometry {
// public:
//     World_geometry() = default;

//     ~World_geometry() {
//         glDeleteBuffers(1, &posBufID);
//         glDeleteBuffers(1, &norBufID);
//         glDeleteBuffers(1, &texBufID);
//     }

//     void add_wall(const glm::vec3& start, const glm::vec3& end, float height, 
//                  const glm::vec3& color = color::gray, bool collidable = true) {
//         // glm::vec3 direction = glm::normalize(end - start);
//         // glm::vec3 up(0.0f, 1.0f, 0.0f);
//         // glm::vec3 right = glm::cross(direction, up);
        
//         World_quad quad;
//         quad.vertices[0] = start;
//         quad.vertices[1] = start + glm::vec3(0.0f, height, 0.0f);
//         quad.vertices[2] = end + glm::vec3(0.0f, height, 0.0f);
//         quad.vertices[3] = end;
//         quad.texCoords[0] = glm::vec2(0.0f, 0.0f);
//         quad.texCoords[1] = glm::vec2(0.0f, 1.0f);
//         quad.texCoords[2] = glm::vec2(1.0f, 1.0f);
//         quad.texCoords[3] = glm::vec2(1.0f, 0.0f);

//         quad.color = color;
//         quad.is_collidable = collidable;
//         quad.calculate_normal();
        
//         quads.push_back(quad);
//         is_dirty = true;
//     }

//     // void getDebugLines(std::vector<std::pair<glm::vec3, glm::vec3>>& lines, 
//     //                     const glm::vec3& lineColor = glm::vec3(0.0f, 1.0f, 0.0f)) const {
//     //     for (const auto& quad : quads) {
//     //         // Draw the outline of each quad
//     //         lines.push_back({quad.vertices[0], quad.vertices[1]});
//     //         lines.push_back({quad.vertices[1], quad.vertices[2]});
//     //         lines.push_back({quad.vertices[2], quad.vertices[3]});
//     //         lines.push_back({quad.vertices[3], quad.vertices[0]});
            
//     //         // Optionally draw the normal
//     //         glm::vec3 center = (quad.vertices[0] + quad.vertices[1] + 
//     //                            quad.vertices[2] + quad.vertices[3]) / 4.0f;
//     //         lines.push_back({center, center + quad.normal * 0.5f});
//     //     }
//     // }
//     // single pass kachow
//     void draw(Shader& shader) const {
//         if (quads.empty()) return;
//         // Bind position buffer
//         int h_pos = glGetAttribLocation(shader.ID, "aPos");
//         glEnableVertexAttribArray(h_pos);
//         glBindBuffer(GL_ARRAY_BUFFER, posBufID);
//         glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
//         // Bind normal buffer
//         int h_nor = glGetAttribLocation(shader.ID, "aNor");
//         if(h_nor != -1 && norBufID != 0) {
//             glEnableVertexAttribArray(h_nor);
//             glBindBuffer(GL_ARRAY_BUFFER, norBufID);
//             glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
//         }
//         // Bind texcoords buffer
//         int h_tex = glGetAttribLocation(shader.ID, "aTex");
//         if(h_tex != -1 && texBufID != 0) {
//             glEnableVertexAttribArray(h_tex);
//             glBindBuffer(GL_ARRAY_BUFFER, texBufID);
//             glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
//         }
//         shader.setVec3("objectColor", color::white);
//         // Draw
//         int count = posBuf.size() / 3; // number of indices to be rendered
//         glDrawArrays(GL_TRIANGLES, 0, count);
//         // Disable and unbind
//         if(h_tex != -1) {
//             glDisableVertexAttribArray(h_tex);
//         }
//         if(h_nor != -1) {
//             glDisableVertexAttribArray(h_nor);
//         }
//         glDisableVertexAttribArray(h_pos);
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//     }
//     // Check collision between a point and all world geometry
//     bool checkPointCollision(const glm::vec3& point, float radius, 
//                              glm::vec3& collisionNormal, float& penetration_depth) {
//         bool collision = false;
//         penetration_depth = 0.0f;
        
//         for (const auto& quad : quads) {
//             if (!quad.is_collidable) continue;
//             glm::vec3 center = (quad.vertices[0] + quad.vertices[1] + 
//                                quad.vertices[2] + quad.vertices[3]) / 4.0f;
//             glm::vec3 toPoint = point - center;
//             float distance = glm::dot(toPoint, quad.normal);
//             // if point is behind plane, with respect to normal
//             //  sphere overlapping line
//             if (distance < radius) {
//                 if (point_in_quad(point, quad)) {
//                     float depth = radius - distance;
//                     if (!collision || depth > penetration_depth) {
//                         penetration_depth = depth;
//                         collisionNormal = quad.normal;
//                         collision = true;
//                     }
//                 }
//             }
//         }
        
//         return collision;
//     }

//     void setup_buffers() {
//         posBuf.clear();
//         norBuf.clear();
//         texBuf.clear();
        
//         for (const auto& quad : quads) {
//             int indices1[] = {0, 1, 2};
//             for (int i : indices1) {
//                 posBuf.push_back(quad.vertices[i].x);
//                 posBuf.push_back(quad.vertices[i].y);
//                 posBuf.push_back(quad.vertices[i].z);
//                 norBuf.push_back(quad.normal.x);
//                 norBuf.push_back(quad.normal.y);
//                 norBuf.push_back(quad.normal.z);
//                 texBuf.push_back(quad.texCoords[i].x);
//                 texBuf.push_back(quad.texCoords[i].y);
//             }
            
//             int indices2[] = {0, 2, 3};
//             for (int i : indices2) {
//                 posBuf.push_back(quad.vertices[i].x);
//                 posBuf.push_back(quad.vertices[i].y);
//                 posBuf.push_back(quad.vertices[i].z);
//                 norBuf.push_back(quad.normal.x);
//                 norBuf.push_back(quad.normal.y);
//                 norBuf.push_back(quad.normal.z);
//                 texBuf.push_back(quad.texCoords[i].x);
//                 texBuf.push_back(quad.texCoords[i].y);
//             }
//         }
        
//         if (posBufID == 0) glGenBuffers(1, &posBufID);
//         glBindBuffer(GL_ARRAY_BUFFER, posBufID);
//         glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), posBuf.data(), GL_STATIC_DRAW);
        
//         if (!norBuf.empty()) {
//             if (norBufID == 0) glGenBuffers(1, &norBufID);
//             glBindBuffer(GL_ARRAY_BUFFER, norBufID);
//             glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float), norBuf.data(), GL_STATIC_DRAW);
//         }
        
//         if (!texBuf.empty()) {
//             if (texBufID == 0) glGenBuffers(1, &texBufID);
//             glBindBuffer(GL_ARRAY_BUFFER, texBufID);
//             glBufferData(GL_ARRAY_BUFFER, texBuf.size() * sizeof(float), texBuf.data(), GL_STATIC_DRAW);
//         }
        
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//     }
    
//     const std::vector<World_quad>& get_quads() const {
//         return quads;
//     }
    
//     void clear() {
//         quads.clear();
//         is_dirty = true;
//     }

// private:
//     std::vector<World_quad> quads;

//     std::vector<float> posBuf;
//     std::vector<float> norBuf;
//     std::vector<float> texBuf;
//     unsigned int posBufID = 0;
//     unsigned int norBufID = 0;
//     unsigned int texBufID = 0;

//     bool is_dirty = true;
    
//     bool point_in_quad(const glm::vec3& point, const World_quad& quad) {
//         // project onto the quad plane
//         glm::vec3 projected = point - glm::dot(point - quad.vertices[0], quad.normal) * quad.normal;
        
//         glm::vec3 edge1 = glm::normalize(quad.vertices[1] - quad.vertices[0]);
//         glm::vec3 edge2 = glm::normalize(quad.vertices[3] - quad.vertices[0]);
        
//         // Calculate the relative position
//         glm::vec3 toPoint = projected - quad.vertices[0];
//         float proj1 = glm::dot(toPoint, edge1);
//         float proj2 = glm::dot(toPoint, edge2);
        
//         // Get the dimensions of the quad
//         float width = glm::length(quad.vertices[1] - quad.vertices[0]);
//         float height = glm::length(quad.vertices[3] - quad.vertices[0]);
        
//         // Check if the point is inside
//         return (proj1 >= 0 && proj1 <= width && proj2 >= 0 && proj2 <= height);
//     }
// };
// #endif
