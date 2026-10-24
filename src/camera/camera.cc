//
// Created by Rushin Shah on 24/10/26.
//

#include <GLFW/glfw3.h>

#include "camera/Camera.h"

inline void Camera::process_cursor_movement(float cursor_x, float cursor_y) noexcept {
    float diff_x = cursor_x - this->cursor_x;
    float diff_y = cursor_y - this->cursor_y;
    diff_x *= sensitivity;
    diff_y *= sensitivity;
    this->cursor_x = cursor_x;
    this->cursor_y = cursor_y;
    if (!first_cursor_move) {
        first_cursor_move = true;
        return;
    }
    pitch += diff_y;
    yaw += diff_x;
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
    if (pitch > 89.0f) {
        pitch = 89.0f;
    }

    glm::vec3 dir(1.0f);
    dir.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = -sin(glm::radians(pitch));
    dir.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_look = glm::normalize(dir);
    view = glm::lookAt(camera_pos, camera_pos + camera_look, up);
}

inline void Camera::process_linear_movement(const Movement movement) noexcept {
    const float delta = (float) glfwGetTime() - prev_time;
    prev_time = (float) glfwGetTime();
    const float multiplier = sensitivity * delta;
    switch (movement) {
        case Movement::FORWARD:
            camera_pos += multiplier * camera_look;
            break;
        case Movement::BACKWARD:
            camera_pos -= multiplier * camera_look;
            break;
        case Movement::LEFT:
            camera_pos += glm::normalize(glm::cross(up, camera_look)) * multiplier;
            break;
        case Movement::RIGHT:
            camera_pos -= glm::normalize(glm::cross(up, camera_look)) * multiplier;
            break;
        default:
            break;
    }
    view = glm::lookAt(camera_pos, camera_pos + camera_look, up);
}

void Camera::update_sensitivity(float sensitivity) noexcept {
    this->sensitivity = sensitivity;
}
