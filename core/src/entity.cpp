#include "entity.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

Entity::Entity(
    Model* model, 
    glm::vec3 position, 
    bool physics_enabled, 
    glm::vec3 scale, 
    glm::vec3 color,
    float mass,
    glm::quat orientation,
    bool fade, float ttl, float max_ttl, float collider_radius
) : 
    model(model),
    position(position), 
    scale(scale), color(color),
    fade(fade), ttl(ttl), max_ttl(max_ttl), 
    rotation(glm::vec3(0.0f)),
    physics(position, collider_radius, physics_enabled, mass, orientation)
{
}

Entity::~Entity() = default;

glm::mat4 Entity::get_model_matrix() const {
    glm::mat4 modelMat(1.0f);
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), physics.position);
    glm::mat4 rotation = glm::mat4_cast(physics.orientation);
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);

    modelMat = translation * rotation * scaling;
    return modelMat;
}

void Entity::draw(Shader shader) {
    model->draw(shader);
}

bool Entity::draw(Shader shader, float time) {
    ttl -= time;
    if (ttl <= 0) {
        return false;
    }
    model->draw(shader);
    return true;
}

bool Entity::collides(const glm::vec3& pos, const glm::vec3& dir, glm::vec3& hit_pos) {
    // For a simple sphere collision check with radius=1
    glm::vec3 oc = pos - position;  // from ray start to entity center
    float a = glm::dot(dir, dir);
    float b = 2.0f * glm::dot(oc, dir);
    float c = glm::dot(oc, oc) - 1.0f;  // sphere radius^2 is 1^2 = 1

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0) {
        return false; 
    }

    float sqrtDisc = glm::sqrt(discriminant);
    float t = (-b - sqrtDisc) / (2.0f * a);
    if (t < 0) {
        t = (-b + sqrtDisc) / (2.0f * a);
        if (t < 0) {
            return false;
        }
    }
    hit_pos = pos + t * dir;
    return true;
}

glm::vec3 Entity::get_color() {
    return color;
}
