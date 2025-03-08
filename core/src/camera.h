#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SENSITIVITY =  0.05f;
const float ZOOM        =  45.0f;

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    float yaw;
    float pitch;
    // camera options
    float mouse_sensitivity;
    float zoom;

    // constructor with vectors
    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 world_up_ = glm::vec3(0.0f, 1.0f, 0.0f), float yaw_ = YAW, float pitch_ = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), mouse_sensitivity(SENSITIVITY), zoom(ZOOM) {
        position = pos;
        world_up = world_up_;
        yaw = yaw_;
        pitch = pitch_;
        update_camera_vectors();
    }
    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 get_view_matrix() {
        return glm::lookAt(position, position + front, up);
    }
    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void process_mouse_movement(float xoffset, float yoffset, GLboolean constrainpitch = true) {
        xoffset *= mouse_sensitivity;
        yoffset *= mouse_sensitivity;
        yaw   += xoffset;
        pitch += yoffset;
        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainpitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }
        // update front, right and up Vectors using the updated Euler angles
        update_camera_vectors();
    }
    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void process_mouse_scroll(float yoffset) {
        zoom -= (float)yoffset;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 360.0f)
            zoom = 360.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void update_camera_vectors() {
        // calculate the new front vector
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);
        // also re-calculate the right and up vector
        right = glm::normalize(glm::cross(front, world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up    = glm::normalize(glm::cross(right, front));
    }
};
#endif
