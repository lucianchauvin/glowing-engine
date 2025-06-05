#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "shader.h"

struct Debug_line {
    glm::vec3 start;
    glm::vec3 end;
    glm::vec3 color;
};

struct Debug_sphere {
    glm::vec3 center;
    float radius;
    glm::vec3 color;
};

class Renderer_debug {
public:
    Renderer_debug() {}
    ~Renderer_debug() {}

    void init();
    void add_line(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
    void add_sphere(const glm::vec3& center, float radius, const glm::vec3& color);
    void add_axes(const glm::vec3& position, const glm::quat& orientation, float length = 1.0f);
    void add_bbox(const glm::vec3& min, const glm::vec3& max, const glm::vec3& color);
    void render(Shader& debug_shader, const glm::mat4& projection, const glm::mat4& view);

private:
    std::vector<Debug_line> lines;
    unsigned int lineVBO = 0;
    unsigned int lineVAO = 0;

    std::vector<Debug_sphere> spheres;
    unsigned int sphereVBO = 0;
    unsigned int sphereVAO = 0;
    unsigned int sphereEBO = 0;
    int sphereIndexCount = 0;
    
    void build_sphere_geometry();
};
