#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <unordered_map>

#include "controller.h"
#include "controller_fps.h"
#include "controller_thirdperson.h"
#include "controller_plane.h"

#include "core/camera.h"
#include "core/physics.h"
#include "core/scene.h"
// #include <model_ass.h>

enum class ControllerType { FPS, THIRDPERSON, PLANE };

class Player {
public:
    float PLAYER_HEIGHT = 1.8f;

    Camera camera;

    std::unordered_map<ControllerType, std::unique_ptr<Controller>> controllers;
    Controller* controller;

    // Hands hands; // (   ͡°   ͜ʖ    ͡°   )

    // std::unique_ptr<Controller> activeController
    //  std::make_unique<Controller_plane>
    
    // player model
    // player model model matrix
    // glm::vec3 forward;
    float model_yaw = 0.0f;

    bool crouched = false;
    bool dashing = false;
    bool key_toggles[256] = {false};

    // Model_ass wep;

    Player() : camera(glm::vec3(0.0f, PLAYER_HEIGHT, 0.0f)), controller() {
        controllers[ControllerType::FPS] = std::make_unique<Controller_fps>();
        //controllers[ControllerType::THIRDPERSON] = std::make_unique<Controller_thirdperson>();
        //controllers[ControllerType::PLANE] = std::make_unique<Controller_plane>();
        controller = controllers[ControllerType::FPS].get();
    }

    void controller_step(GLFWwindow* window, float deltaTime, Scene& scene) {
        poll_player(window);
        controller->process_input(window, deltaTime, scene, camera, model_yaw);
        controller->update_camera(camera, crouched, PLAYER_HEIGHT);
    }

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        Player* player = static_cast<Player*>(glfwGetWindowUserPointer(window));
        if (player) {
            player->mouse_callback_impl(window, xpos, ypos);
        }
    }

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        Player* player = static_cast<Player*>(glfwGetWindowUserPointer(window));
        if (player) {
            player->scroll_callback_impl(window, xoffset, yoffset);
        }
    }

    static void char_callback(GLFWwindow* window, unsigned int key) {
        Player* player = static_cast<Player*>(glfwGetWindowUserPointer(window));
        if (player) {
            player->char_callback_impl(window, key);
        }
    }

    glm::mat4 get_model_matrix() {
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, position);
        //model = glm::rotate(model, -glm::radians(model_yaw - 90), glm::vec3(0, 1, 0));
        //model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        return model;
    }

    void debug_hud() {
        ImGui::Begin("Player");

        ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", camera.position.x, camera.position.y, camera.position.z);
        ImGui::Text("Camera Facing:   (%.1f, %.1f, %.1f)", camera.front.x, camera.front.y, camera.front.z);
        
        ImGui::End();
    }

private:
    void mouse_callback_impl(GLFWwindow* window, double xpos, double ypos) { // need these guys to pass camera
        if (!key_toggles[(unsigned)'q'])
            controller->mouse_callback(window, camera, xpos, ypos, model_yaw);
    }
    
    void scroll_callback_impl(GLFWwindow* window, double xoffset, double yoffset) {
        controller->scroll_callback(window, camera, xoffset, yoffset);
    }

    void char_callback_impl(GLFWwindow* window, unsigned int key) {
        key_toggles[key] = !key_toggles[key]; // set this key in our player key toggles
        process_player_toggles(window); // run through local stuff based on these keytoggles
        controller->char_callback(window, key); // set controller key toggles
    }
    // key toggle state that is more specific to the idea of a player than a controller
    // think noclip mode vs plane throttle
    void process_player_toggles(GLFWwindow* window) { 
        // T
        // TOGGLE WIREFRAME
        if (key_toggles[(unsigned)'t'])
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // R
        // TOGGLE MOUSE
        if (!key_toggles[(unsigned)'q'])
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        if (key_toggles[(unsigned)'i']) {
            printf("[CONTROLLER] setting fps \n");
            controller = controllers[ControllerType::FPS].get();
            key_toggles[(unsigned)'i'] = false;
        }
        if (key_toggles[(unsigned)'o']) {
            printf("[CONTROLLER] setting third person\n");
            controller = controllers[ControllerType::THIRDPERSON].get();
            key_toggles[(unsigned)'o'] = false;
        }
        if (key_toggles[(unsigned)'p']) {
            printf("[CONTROLLER] setting plane\n");
            controller = controllers[ControllerType::PLANE].get();
            key_toggles[(unsigned)'p'] = false;
        }
    }
    // stuff we're polling for every frame that wouldnt be captured by a keycallback
    // maybe can breakup into 'meta player' vs physics/game state?
    void poll_player(GLFWwindow* window) {
        // CTRL + C
        // CLOSE WINDOW
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) 
            glfwSetWindowShouldClose(window, true);
        // ^ example meta player control ^
        //            vs
        // v    game state control      v
        crouched = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

      /*  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            player_physics.position.x += camera.front.x * 15.0f;
            player_physics.position.y += camera.front.y * 8.0f;
            player_physics.position.z += camera.front.z * 15.0f;
            dashing = true;
        } else {
            dashing = false;
        }*/
    }
};
#endif
