#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "asset/model_ass.h"
#include "asset/model_manager.h"
#include "asset/shader.h"
#include "physics.h"

class Entity {
public:
    Entity(
        Model_ass* model,
        glm::vec3 position    = glm::vec3(0.0f),
        bool physics_enabled  = true,
        glm::vec3 scale       = glm::vec3(1.0f),
        glm::vec3 color       = glm::vec3(1.0f),
        float mass            = 1.0f,
        glm::quat             = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        bool fade             = false,
        float ttl             = 0.0f,
        float max_ttl         = 0.0f,
        float collider_radius = 1.0f
    );

    Entity(
        model_handle model_id,
        glm::vec3 position = glm::vec3(0.0f),
        bool physics_enabled = true,
        glm::vec3 scale = glm::vec3(1.0f),
        glm::vec3 color = glm::vec3(1.0f),
        float mass = 1.0f,
        glm::quat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        bool fade = false,
        float ttl = 0.0f,
        float max_ttl = 0.0f,
        float collider_radius = 1.0f
    );
    
    virtual ~Entity();

    glm::mat4 get_model_matrix() const;

    void draw(const Shader& shader);
    bool draw(const Shader& shader, float time);
    bool collides(const glm::vec3& pos, const glm::vec3& dir, glm::vec3& hit_pos);
    glm::vec3 get_color();



// private:
    Model_ass* model;
    model_handle model_id;
    glm::vec3 position; 
    glm::vec3 scale;
    glm::vec3 color;
    bool fade;
    float ttl;
    float max_ttl;
    glm::quat rotation; 

    Physics_object physics;

};
#endif
