#ifndef ENTITY_H
#define ENTITY_H

#include <vector>

#include <glm/glm.hpp>

#include <model.h>
#include <shader.h>

class Entity {
public:
    Entity(
        Model* model, 
        glm::vec3 position = glm::vec3(0.0f), 
        glm::vec3 scale = glm::vec3(1.0f), 
        glm::vec3 color = glm::vec3(1.0f)
    ) : model(model), position(position), scale(scale), color(color) {
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

    glm::vec3 get_color() { return color; }

private:
    Model* model;
    glm::vec3 position; 
    glm::vec3 rotation; 
    glm::vec3 scale;
    glm::vec3 color;
};
#endif
