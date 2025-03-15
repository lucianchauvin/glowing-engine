#ifndef WORLD_GEOMETRY_H
#define WORLD_GEOMETRY_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "general/colors.h"

struct World_quad {
    glm::vec3 vertices[4];
    glm::vec3 normal;
    glm::vec3 color;
    bool is_collidable;
    
    void calculate_normal() {
        glm::vec3 edge1 = vertices[1] - vertices[0];
        glm::vec3 edge2 = vertices[2] - vertices[0];
        normal = glm::normalize(glm::cross(edge1, edge2));
    }
};

class World_geometry {
public:
    World_geometry() : vao_initialized(false) {}

    ~World_geometry() {
        if (vao_initialized) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }
    }

    void add_quad(const World_quad& quad) {
        quads.push_back(quad);
        is_dirty = true;
    }
    
    void add_wall(const glm::vec3& start, const glm::vec3& end, float height, 
                 const glm::vec3& color = color::gray, bool collidable = true) {
        glm::vec3 direction = glm::normalize(end - start);
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::cross(direction, up);
        
        World_quad quad;
        quad.vertices[0] = start;
        quad.vertices[1] = start + glm::vec3(0.0f, height, 0.0f);
        quad.vertices[2] = end + glm::vec3(0.0f, height, 0.0f);
        quad.vertices[3] = end;
        quad.color = color;
        quad.is_collidable = collidable;
        quad.calculate_normal();
        
        add_quad(quad);
    }

    // void getDebugLines(std::vector<std::pair<glm::vec3, glm::vec3>>& lines, 
    //                     const glm::vec3& lineColor = glm::vec3(0.0f, 1.0f, 0.0f)) const {
    //     for (const auto& quad : quads) {
    //         // Draw the outline of each quad
    //         lines.push_back({quad.vertices[0], quad.vertices[1]});
    //         lines.push_back({quad.vertices[1], quad.vertices[2]});
    //         lines.push_back({quad.vertices[2], quad.vertices[3]});
    //         lines.push_back({quad.vertices[3], quad.vertices[0]});
            
    //         // Optionally draw the normal
    //         glm::vec3 center = (quad.vertices[0] + quad.vertices[1] + 
    //                            quad.vertices[2] + quad.vertices[3]) / 4.0f;
    //         lines.push_back({center, center + quad.normal * 0.5f});
    //     }
    // }
    // single pass kachow
    void draw(Shader& shader) {
        if (quads.empty()) return;
        
        if (!vao_initialized || is_dirty) {
            setup_buffers();
        }
        
        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    // Check collision between a point and all world geometry
    bool checkPointCollision(const glm::vec3& point, float radius, 
                             glm::vec3& collisionNormal, float& penetration_depth) {
        bool collision = false;
        penetration_depth = 0.0f;
        
        for (const auto& quad : quads) {
            if (!quad.is_collidable) continue;
            glm::vec3 center = (quad.vertices[0] + quad.vertices[1] + 
                               quad.vertices[2] + quad.vertices[3]) / 4.0f;
            glm::vec3 toPoint = point - center;
            float distance = glm::dot(toPoint, quad.normal);
            // if point is behind plane, with respect to normal
            //  sphere overlapping line
            if (distance < radius) {
                if (point_in_quad(point, quad)) {
                    float depth = radius - distance;
                    if (!collision || depth > penetration_depth) {
                        penetration_depth = depth;
                        collisionNormal = quad.normal;
                        collision = true;
                    }
                }
            }
        }
        
        return collision;
    }

    void setup_buffers() {
        vertex_data.clear();
        indices.clear();
        
        // position (3), normal (3), color (3)
        for (size_t i = 0; i < quads.size(); i++) {
            const auto& quad = quads[i];
            
            for (int v = 0; v < 4; v++) {
                vertex_data.push_back(quad.vertices[v].x);
                vertex_data.push_back(quad.vertices[v].y);
                vertex_data.push_back(quad.vertices[v].z);
                vertex_data.push_back(quad.normal.x);
                vertex_data.push_back(quad.normal.y);
                vertex_data.push_back(quad.normal.z);
                vertex_data.push_back(quad.color.r); // could think about per vertex color
                vertex_data.push_back(quad.color.g);
                vertex_data.push_back(quad.color.b);
            }
            
            // one quad = two triangles
            unsigned int baseIndex = i * 4;
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + 1);
            indices.push_back(baseIndex + 2);
            
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + 2);
            indices.push_back(baseIndex + 3);
        }
        
        if (!vao_initialized) {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            vao_initialized = true;
        }
        
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        // Color attribute
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        
        glBindVertexArray(0);
        is_dirty = false;
    }
    
    const std::vector<World_quad>& get_quads() const {
        return quads;
    }
    
    void clear() {
        quads.clear();
        is_dirty = true;
    }

private:
    std::vector<World_quad> quads;
    std::vector<float> vertex_data;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    bool vao_initialized;
    bool is_dirty = true;
    
    bool point_in_quad(const glm::vec3& point, const World_quad& quad) {
        // project onto the quad plane
        glm::vec3 projected = point - glm::dot(point - quad.vertices[0], quad.normal) * quad.normal;
        
        glm::vec3 edge1 = glm::normalize(quad.vertices[1] - quad.vertices[0]);
        glm::vec3 edge2 = glm::normalize(quad.vertices[3] - quad.vertices[0]);
        
        // Calculate the relative position
        glm::vec3 toPoint = projected - quad.vertices[0];
        float proj1 = glm::dot(toPoint, edge1);
        float proj2 = glm::dot(toPoint, edge2);
        
        // Get the dimensions of the quad
        float width = glm::length(quad.vertices[1] - quad.vertices[0]);
        float height = glm::length(quad.vertices[3] - quad.vertices[0]);
        
        // Check if the point is inside
        return (proj1 >= 0 && proj1 <= width && proj2 >= 0 && proj2 <= height);
    }
};
#endif
