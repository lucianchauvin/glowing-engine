#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <GLFW/glfw3.h>

#include <scene.h>
#include <camera.h>
#include <physics.h>
#include <player.h>

class Controller {
public:
    virtual ~Controller() = default;    

    virtual void mouse_callback(GLFWwindow* window, Camera& camera, double xpos, double ypos, float& model_yaw) = 0;
    virtual void scroll_callback(GLFWwindow* window, Camera& camera, double xoffset, double yoffset) = 0;
    virtual void char_callback(GLFWwindow* window, unsigned int key) = 0;

    virtual void process_input(GLFWwindow* window, float deltaTime, Scene& scene, Model* model, Physics_object& player_physics, Camera& camera, float& model_yaw) = 0;
    virtual void update_physics(float deltaTime, Physics_object& player_physics, Camera& camera) = 0;
    virtual void update_camera(Camera& camera, const Physics_object& player_physics, bool crouched, float player_height) = 0;

    virtual glm::vec3 get_weapon_position() const { return glm::vec3(0.0f); } // for FPS camera
};
#endif
