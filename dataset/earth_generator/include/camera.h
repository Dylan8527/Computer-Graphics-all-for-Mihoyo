#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "utils.h"
#include "stb_image_write.h"

#include <fstream>
#include <vector>

// Default camera settings
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  1.0f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;

    static int index;
    std::ofstream transform;

    std::vector<uint8_t> buffer;

    float yaw;
    float pitch;
    float movement_speed;
    float mouse_sensitivity;
    float zoom;

    Camera(
        glm::vec3 position = glm::vec3 {0.0f, 0.0f, 0.0f},
        glm::vec3 up = glm::vec3 {0.0f, 1.0f, 0.0f},
        float yaw = YAW,
        float pitch = PITCH
    );
    Camera(
        float pos_x, float pos_y, float pos_z,
        float up_x, float up_y, float up_z,
        float yaw, float pitch
    );

    glm::mat4 GetViewMatrix();
    void ProcessKeyboard(GLFWwindow* window);
    void SetMouseClick(bool mouse_click);
    void ProcessMouseMovement(GLFWwindow* window, float x_pos, float y_pos);
    void ProcessMouseScroll(GLFWwindow* window, float x_offset, float y_offset);
    void SetViewMatrix(glm::vec3& pos);
    void DrawDraw();

private:
    void UpdateCameraSettings();

    // for keyboard
    float current_time;
    float last_time;
    float delta_time;

    // for mouse move
    bool click;
    bool first_move;
    bool constrain_pitch;
    float last_x;
    float last_y;
};

#endif // !_CAMERA_H_