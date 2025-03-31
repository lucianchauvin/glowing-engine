#ifndef CONTROLLER_PLANE_H
#define CONTROLLER_PLANE_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <camera.h>
#include <scene.h>

class Controller_plane : public Controller {
public:
    float lastX = 0.0f;
    float lastY = 0.0f;
    
    // plane controls / state
    glm::vec3 wep_pos = glm::vec3(-0.2f, -0.1f, 0.5f);
    glm::vec3 min_pos = glm::vec3(-0.2f, -0.1f, 0.5f);
    glm::vec3 ads_pos = glm::vec3(-0.0001f, -.07f, 0.25f);
    glm::vec3 wep_rot = glm::vec3(0.0f);
    
    // Plane simulation constants
    glm::quat plane_orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float throttle = 0.0f;
    const float THROTTLE_ACCELERATION = 50.0f;
    const float THROTTLE_DECELERATION = 50.0f;
    const float MAX_THROTTLE = 300.0f;
    const float YAW_SPEED = 60.0f;    // degrees per second
    const float PITCH_SPEED = 60.0f;  // degrees per second
    const float ROLL_SPEED = 60.0f;   // degrees per second
    const float THIRD_PERSON_DISTANCE = 10.0f;
    const float PLANE_HEIGHT = 2.0f;
    
    bool is_third_person = true;
    bool toggle_mouse_lock = true;
    bool key_toggles[256] = {false};

    virtual void mouse_callback(GLFWwindow* window, Camera& camera, double xpos, double ypos, float& model_yaw) override {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        lastX = xpos;
        lastY = ypos;
        camera.process_mouse_movement(xoffset, yoffset);
    }

    virtual void scroll_callback(GLFWwindow* window, Camera& camera, double xoffset, double yoffset) override {
        camera.process_mouse_scroll(static_cast<float>(yoffset));
    }

    virtual void char_callback(GLFWwindow* window, unsigned int key) override {
        key_toggles[key] = !key_toggles[key];
    }

    virtual void process_input(GLFWwindow* window, float deltaTime, Scene& scene, Model* model, Physics_object& player_physics, Camera& camera, float& model_yaw) override {
        // Basic toggles
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
            glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            if (toggle_mouse_lock)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            else
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            toggle_mouse_lock = !toggle_mouse_lock;
        }

        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !key_toggles[GLFW_KEY_TAB]) {
            is_third_person = !is_third_person;
            key_toggles[GLFW_KEY_TAB] = true;
        } else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
            key_toggles[GLFW_KEY_TAB] = false;
        }

        // ADS blending
        glm::vec3 target_pos = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ? ads_pos : min_pos;
        wep_pos = glm::mix(wep_pos, target_pos, deltaTime * 10.0f);

        // Optional: Left mouse button for interaction/shooting
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            glm::vec3 hit_pos(0.0f);
            int hits = scene.cast_ray(camera.position, camera.front, hit_pos);
            if (hits != 0) {
                glm::vec3 scale(1.0f);
                glm::vec3 color(0.0f);
                Entity e(model, hit_pos, true, scale, color, true, 5.2f);
                scene.include(e);
            }
        }

        // --- Plane-specific controls ---
        // Throttle: Increase with W, decrease with S.
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            throttle += THROTTLE_ACCELERATION * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            throttle -= THROTTLE_DECELERATION * deltaTime;
        throttle = glm::clamp(throttle, 0.0f, MAX_THROTTLE);

        // Rotation inputs:
        // Use arrow keys for pitch, Q/E for roll, and A/D for yaw.
        float pitchInput = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            pitchInput += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            pitchInput -= 1.0f;

        float rollInput = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            rollInput += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            rollInput -= 1.0f;

        float yawInput = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            yawInput += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            yawInput -= 1.0f;

        // Compute rotation angles (in radians)
        float yawAngle = glm::radians(YAW_SPEED * yawInput * deltaTime);
        float pitchAngle = glm::radians(PITCH_SPEED * pitchInput * deltaTime);
        float rollAngle = glm::radians(ROLL_SPEED * rollInput * deltaTime);

        // Create quaternions for rotations.
        glm::quat yawRot = glm::angleAxis(yawAngle, glm::vec3(0.0f, 1.0f, 0.0f));      // Yaw about global up
        glm::quat pitchRot = glm::angleAxis(pitchAngle, glm::vec3(1.0f, 0.0f, 0.0f));  // Pitch about local right
        glm::quat rollRot = glm::angleAxis(rollAngle, glm::vec3(0.0f, 0.0f, 1.0f));    // Roll about local forward

        // Update plane orientation
        plane_orientation = glm::normalize(yawRot * plane_orientation * pitchRot * rollRot);
    }
    
    virtual void update_physics(float deltaTime, Physics_object& player_physics, Camera& camera) override {
        // Extract forward direction from orientation
        glm::vec3 forward = plane_orientation * glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = plane_orientation * glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = plane_orientation * glm::vec3(1.0f, 0.0f, 0.0f);
        
        // Set velocity based on throttle and orientation
        player_physics.velocity = forward * throttle;
        
        // Update position based on velocity
        player_physics.position += player_physics.velocity * deltaTime;
        
        // Optional: add some basic physics effects like air resistance
        throttle *= 0.99f; // Simple air resistance
    }
    
    virtual void update_camera(Camera& camera, const Physics_object& player_physics, bool crouched, float player_height) override {
        // Extract forward and up from orientation
        glm::vec3 forward = plane_orientation * glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = plane_orientation * glm::vec3(0.0f, 1.0f, 0.0f);
        
        if (is_third_person) {
            // Position camera behind plane
            glm::vec3 offset = -forward * THIRD_PERSON_DISTANCE + up * 3.0f;
            camera.position = player_physics.position + offset;
            
            // Point camera at plane
            camera.front = -offset;
            camera.front = glm::normalize(camera.front);
            
            // Recalculate camera vectors
            camera.right = glm::normalize(glm::cross(camera.front, camera.world_up));
            camera.up = glm::normalize(glm::cross(camera.right, camera.front));
        } else {
            // First-person from cockpit
            camera.position = player_physics.position + up * PLANE_HEIGHT;
            
            // Set camera orientation to match plane
            camera.front = forward;
            camera.right = glm::normalize(glm::cross(camera.front, up));
            camera.up = glm::normalize(glm::cross(camera.right, camera.front));
        }
    }

    virtual glm::vec3 get_weapon_position() const override {
        return wep_pos;
    }
};
#endif
