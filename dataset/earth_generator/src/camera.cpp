#include "camera.h"

int Camera::index = 0;

Camera::Camera(
    glm::vec3 position,
    glm::vec3 world_up,
    float yaw,
    float pitch
) : position(position), world_up(world_up), yaw(yaw), pitch(pitch), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM),
    current_time(0.0f), last_time(0.0f), delta_time(0.0f), click(false), first_move(true), constrain_pitch(true), last_x(0.0f), last_y(0.0f) {
    
    transform.open("../transform.txt");
    buffer.resize(1920 * 1080 * 4);
    
    UpdateCameraSettings();
}

Camera::Camera(
    float pos_x, float pos_y, float pos_z,
    float up_x, float up_y, float up_z,
    float yaw, float pitch
) : Camera(glm::vec3 {pos_x, pos_y, pos_z}, glm::vec3 {up_x, up_y, up_z}, yaw, pitch) {}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(this->position, this->position + this->front, this->up);
}

void Camera::ProcessKeyboard(GLFWwindow* window) {

    this->current_time = (float)glfwGetTime();
    this->delta_time = this->current_time - this->last_time;
    this->last_time = this->current_time;
    float speed = this->movement_speed;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        speed *= 5.0f;

    float displacement = this->delta_time * speed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        this->position += this->up * displacement;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        this->position -= this->up * displacement;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        this->position -= this->right * displacement;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        this->position += this->right * displacement;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        this->position += this->front * displacement;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        this->position -= this->front * displacement;

    UpdateCameraSettings();
}

void Camera::SetMouseClick(bool mouse_click) {
    this->click = mouse_click;
    if (mouse_click == false) {
        this->first_move = true;
    }
}

void Camera::ProcessMouseMovement(GLFWwindow* window, float x_pos, float y_pos) {
    
    if (this->click == false) {
        return;
    }

    if (this->first_move) {
        this->last_x = x_pos;
        this->last_y = y_pos;
        this->first_move = false;
    }

    // get offset, (0, 0) is at down left of screen
    this->yaw += (this->last_x - x_pos) * this->mouse_sensitivity * this->zoom / 100.0f;
    this->pitch += (y_pos - this->last_y) * this->mouse_sensitivity * this->zoom / 100.0f;

    this->last_x = x_pos;
    this->last_y = y_pos;

    if (this->constrain_pitch == true) {
        if (this->pitch > 89.0f) {
            this->pitch = 89.0f;
        }
        if (this->pitch < -89.0f) {
            this->pitch = -89.0f;
        }
    }

    // Done by ProcessKeyboard
    // UpdateCameraSettings();
}

void Camera::ProcessMouseScroll(GLFWwindow* window, float x_offset, float y_offset) {
    this->zoom -= y_offset;
    if (this->zoom < 1.0f) {
        this->zoom = 1.0f;
    }
    if (this->zoom > 100.0f) {
        this->zoom = 100.0f;
    }

    // Done by ProcessKeyboard
    // UpdateCameraSettings();
}

void Camera::UpdateCameraSettings() {
    this->front = glm::vec3 {
        cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
        sin(glm::radians(this->pitch)),
        sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))
    };
    this->front = glm::normalize(this->front);
    this->right = glm::normalize(glm::cross(this->front, this->world_up));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}

void Camera::SetViewMatrix(glm::vec3& pos){
    this->position = pos;
    this->front = -pos;
    //* update camera coordinate axis
    this->front = glm::normalize(this->front);
    this->right = glm::normalize(glm::cross(this->front, this->world_up));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}


void Camera::DrawDraw(){
    auto view = glm::inverse(glm::lookAt(this->position, this->position + this->front, this->up));

    transform << Camera::index << ":\n[\n" << view << "]\n\n";

    glReadPixels(0, 0, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
    char path[64];
    sprintf(path, "../../data/nerf/earth/images/%04d.png", Camera::index);
    stbi_flip_vertically_on_write(true);
    stbi_write_png(path, 1920, 1080, 4, buffer.data(), 0);

    ++Camera::index;
}
