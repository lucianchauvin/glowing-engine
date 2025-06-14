#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <GLFW/glfw3.h>
#include <dearimgui/imgui.h>
#include <dearimgui/imgui_impl_glfw.h>
#include <dearimgui/imgui_impl_opengl3.h>

#include "core/scene.h"
#include "core/camera.h"
#include "asset/shader.h"

class Controller {
public:
    // glm::vec3 wep_pos;
    
    virtual ~Controller() = default;    

    virtual void mouse_callback(GLFWwindow* window, Camera& camera, double xpos, double ypos, float& model_yaw) = 0;
    virtual void scroll_callback(GLFWwindow* window, Camera& camera, double xoffset, double yoffset) = 0;
    virtual void char_callback(GLFWwindow* window, unsigned int key) = 0;

    virtual void process_input(GLFWwindow* window, float deltaTime, Scene& scene, Camera& camera, float& model_yaw) = 0;
    virtual void update_camera(Camera& camera, bool crouched, float player_height) = 0;

    virtual glm::vec3 get_weapon_position() const { return glm::vec3(0.0f); } // for FPS camera

    virtual void draw_hud(Shader& shader) const {};
    virtual void debug_hud(ImGuiIO& io) {};
};
#endif
