//
// Created by Rushin Shah on 24/10/26.
//

#ifndef OPENGL_CAMERA_H
#define OPENGL_CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    NONE
};

class Camera {
public:
    glm::mat4 view;
    Camera(
        const glm::vec3 camera_pos,
        const glm::vec3 camera_look,
        const glm::vec3 up,
        const float cursor_x,
        const float cursor_y
    ) : camera_pos(camera_pos),
    camera_look(camera_look),
    up(up),
    view(glm::lookAt(camera_pos, camera_pos + camera_look, up)),
    cursor_x(cursor_x),
    cursor_y(cursor_y){};
    inline void process_cursor_movement(float cursor_x, float cursor_y) noexcept;
    inline void process_linear_movement(Movement movement) noexcept;
    inline void update_sensitivity(float sensitivity) noexcept;

private:
    glm::vec3 camera_pos, camera_look, up;
    float cursor_x, cursor_y, pitch = 0.0f, yaw = 0.0f, sensitivity = 0.2f, prev_time = 0.0f;
    bool first_cursor_move = true;
};
#endif //OPENGL_CAMERA_H
