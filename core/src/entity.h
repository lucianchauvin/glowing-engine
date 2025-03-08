#ifndef ENTITY_H
#define ENTITY_H

#include <vector>

#include <glm/glm.hpp>

#include <model.h>
#include <shader.h>

class Entity {
public:
    bool fade;
    float ttl;
    float max_ttl;

    Entity(
        Model* model, 
        glm::vec3 position = glm::vec3(0.0f), 
        glm::vec3 scale = glm::vec3(1.0f), 
        glm::vec3 color = glm::vec3(1.0f),
        bool fade = false,
        float ttl = 0.0f,
        float max_ttl = 0.0f
    ) : model(model), position(position), scale(scale), color(color), fade(fade), ttl(ttl), max_ttl(max_ttl) {
    }

    virtual ~Entity() = default;

    glm::mat4 get_model_matrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::rotate(model, rotation);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        return model;
    }

    void draw(Shader shader) {
        model->draw(shader);
    }

    bool draw(Shader shader, float time) {
        ttl -= time;
        if (ttl <= 0) return false;

        model->draw(shader);
        return true;
    }

    bool collides(const glm::vec3& pos, const glm::vec3& dir, glm::vec3& hit_pos) {
        glm::vec3 oc = pos - position;  // vector from ray pos to entity pos
        float a = glm::dot(dir, dir);
        float b = 2.0f * glm::dot(oc, dir);
        float c = glm::dot(oc, oc) - 1.0f;  // sphere radius is 1

        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0) 
            return false;

        float t = (-b - glm::sqrt(discriminant)) / (2.0f * a);
        if (t < 0) {
            t = (-b + glm::sqrt(discriminant)) / (2.0f * a);
            if (t < 0) {
                return false;
            }
        }
        hit_pos = pos + t * dir;
        return true;
    }

    glm::vec3 get_color() { return color; }

private:
    Model* model;
    glm::vec3 position; 
    glm::vec3 rotation; 
    glm::vec3 scale;
    glm::vec3 color;
};
#endif
