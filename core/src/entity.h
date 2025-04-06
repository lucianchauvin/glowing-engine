#ifndef ENTITY_H
#define ENTITY_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "model.h"
#include "shader.h"
#include "physics.h"

class Entity {
public:
    Entity(
        Model* model,
        glm::vec3 position    = glm::vec3(0.0f),
        bool physics_enabled  = true,
        glm::vec3 scale       = glm::vec3(1.0f),
        glm::vec3 color       = glm::vec3(1.0f),
        float mass = 1.0f,
        glm::quat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        bool fade             = false,
        float ttl             = 0.0f,
        float max_ttl         = 0.0f,
        float collider_radius = 1.0f
    );
    
    virtual ~Entity();

    glm::mat4 get_model_matrix() const;

    void draw(Shader shader);
    bool draw(Shader shader, float time);
    bool collides(const glm::vec3& pos, const glm::vec3& dir, glm::vec3& hit_pos);
    glm::vec3 get_color();



// private:
    Model* model;
    glm::vec3 position; 
    glm::vec3 scale;
    glm::vec3 color;
    bool fade;
    float ttl;
    float max_ttl;
    glm::vec3 rotation; 

    Physics_object physics;

};
#endif
