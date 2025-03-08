#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <camera.h>
#include <shader.h>
#include <scene.h>

class Controller {
// private:
public:
    float GRAVITY = 9.8f;
    float JUMP_FORCE = 5.0f;
    float FRICTION = .937f;
    float ACCELERATION = 47.0f;
    float MAX_VELOCITY = 4.3f;
    float PLAYER_HEIGHT = 1.8f;

    const float FLOOR_Y = 0;

    // Camera
    Camera camera;
    float lastX = 0.0f;
    float lastY = 0.0f;
    bool toggle_mouse_lock = true;
    bool is_third_person = false;
    const float THIRD_PERSON_DISTANCE = 5.0f;

    // player physics state
    struct player_physics {
        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 player_position = glm::vec3(0.0f, 5.0f, 0.0f);
        bool isOnGround = false;
        bool isJumping = false;
        bool dashing = false;
    } player_physics;
    bool key_toggles[256] = {false};

    Controller() : camera(glm::vec3(0.0f, player_physics.player_position.y + PLAYER_HEIGHT, 0.0f)) {}

    void char_callback_impl(GLFWwindow *window, unsigned int key) {
        key_toggles[key] = !key_toggles[key];
    }

    void mouse_callback_impl(GLFWwindow* window, double xpos, double ypos) {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    
        lastX = xpos;
        lastY = ypos;
        
        camera.process_mouse_movement(xoffset, yoffset);
    }
    
    void scroll_callback_impl(GLFWwindow* window, double xoffset, double yoffset) {
        camera.process_mouse_scroll(static_cast<float>(yoffset));
    }

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        Controller* controller = static_cast<Controller*>(glfwGetWindowUserPointer(window));
        if (controller) {
            controller->mouse_callback_impl(window, xpos, ypos);
        }
    }

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        Controller* controller = static_cast<Controller*>(glfwGetWindowUserPointer(window));
        if (controller) {
            controller->scroll_callback_impl(window, xoffset, yoffset);
        }
    }

    static void char_callback(GLFWwindow *window, unsigned int key) {
        Controller* controller = static_cast<Controller*>(glfwGetWindowUserPointer(window));
        if (controller) {
            controller->char_callback_impl(window, key);
        }
    }

// public:
    void process_input(GLFWwindow *window, float deltaTime, Scene& scene, Model* model) {
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

        if (key_toggles[(unsigned) 't']) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } 
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            // cast ray ito 
            glm::vec3 hit_pos = glm::vec3(0.0f);
            int hits = scene.cast_ray(camera.position, camera.front, hit_pos);
            
            if (hits != 0) {
                glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
                glm::vec3 color = glm::vec3(0.0, 0.0f, 0.0f);
                Entity e(model, hit_pos, scale, color, true, 0.2f);
                scene.include(e);
            }
        }
        // get movement direction in camera space
        glm::vec3 movement(0.0f);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            movement.z += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            movement.z -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            movement.x -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            movement.x += 1.0f;
        // normalize movement vector if the player is moving diagonally
        if (glm::length(movement) > 0.0f) //{
            movement = glm::normalize(movement);

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player_physics.isOnGround) {
            player_physics.velocity.y = JUMP_FORCE;
            player_physics.isOnGround = false;
            player_physics.isJumping = true;
        }
        // convert camera-relative movement to world space
        glm::vec3 forward = glm::normalize(glm::vec3(camera.front.x, 0.0f, camera.front.z));
        glm::vec3 right = glm::normalize(glm::cross(forward, camera.world_up));
        glm::vec3 acceleration = forward * movement.z + right * movement.x;
        acceleration *= ACCELERATION * deltaTime;
        // apply acceleration
        player_physics.velocity.x += acceleration.x;
        player_physics.velocity.z += acceleration.z;
        // apply friction when on ground
        if (player_physics.isOnGround) {
            player_physics.velocity.x *= FRICTION;
            player_physics.velocity.z *= FRICTION;
        }
        // limit horizontal velocity
        float horizontal_speed = glm::length(glm::vec2(player_physics.velocity.x, player_physics.velocity.z));
        if (horizontal_speed > MAX_VELOCITY) {
            float scale = MAX_VELOCITY / horizontal_speed;
            player_physics.velocity.x *= scale;
            player_physics.velocity.z *= scale;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            player_physics.velocity.x += camera.front.x * 150.0f;
            player_physics.velocity.y += camera.front.y * 10.0f;
            player_physics.velocity.z += camera.front.z * 150.0f;
            player_physics.dashing = true;
        } else {
            player_physics.dashing = false;
        }
    }
    // check collision with the floor
    bool check_floor_collision(const glm::vec3& position) {
        return position.y <= FLOOR_Y;
    }

    void update_player_physics(float deltaTime) {
        // apply gravity
        if (!player_physics.isOnGround) {
            player_physics.velocity.y -= GRAVITY * deltaTime;
        }
        
        // move player
        player_physics.player_position += player_physics.velocity * deltaTime;
        
        // check floor collision
        bool floor_collision = check_floor_collision(player_physics.player_position);
        if (floor_collision) {
            player_physics.isOnGround = true;
            player_physics.velocity.y = 0.0f;
            player_physics.player_position.y = FLOOR_Y; // snap to floor
            player_physics.isJumping = false;
        } else {
            player_physics.isOnGround = false;
        }
        
        // update camera position based on view mode
        if (is_third_person) {
            // third person: position camera behind player
            glm::vec3 offset = -camera.front * THIRD_PERSON_DISTANCE;
            camera.position = player_physics.player_position + offset;
            camera.position.y += 1.0f; // camera slightly above player head
        } else {
            // first person: camera is at player position
            camera.position = player_physics.player_position;
            camera.position.y += PLAYER_HEIGHT;
        }
    }
};
#endif