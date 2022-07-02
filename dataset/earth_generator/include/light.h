#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <cmath>

#include "mesh.h"
#include "shader.h"
#include "utils.h"

// modified for this project
class Light {
public:
    Light(const Mesh& mesh);

    void ignite(const Shader& shader);

public:
    float radius;
    glm::vec3 red_position;
    glm::vec3 green_position;
    glm::vec3 blue_position;
};

Light::Light(const Mesh& mesh) {
    this->radius = 1.25f; //sqrt(mesh.max_x * mesh.max_x + mesh.max_y * mesh.max_y + mesh.max_z * mesh.max_z) * 1.5f;
}

void Light::ignite(const Shader& shader) {

    //* 3 light source
    // this->red_position = glm::vec3 {(float)cos(glfwGetTime()) * this->radius, 0.0f, (float)sin(glfwGetTime()) * this->radius};
    // this->green_position = glm::vec3 {(float)sin(glfwGetTime()) * this->radius, (float)cos(glfwGetTime()) * this->radius, 0.0f};
    // this->blue_position = glm::vec3 {0.0f, (float)cos(glfwGetTime()) * this->radius, (float)sin(glfwGetTime()) * this->radius};
    
    this->red_position   = glm::vec3 {this->radius, 0.0f, 0.0f};
    this->green_position = glm::vec3 {0.0f, this->radius, 0.0f};
    this->blue_position  = glm::vec3 {0.0f, 0.0f, this->radius};

    auto x = glm::vec3 {1.0f, 1.0f, 1.0f} * 3.0f;

    shader.setVec3("light_source[0].position", this->red_position);
    shader.setVec3("light_source[0].color", x);

    shader.setVec3("light_source[1].position", this->green_position);
    shader.setVec3("light_source[1].color", x);

    shader.setVec3("light_source[2].position", this->blue_position);
    shader.setVec3("light_source[2].color", x);

    //* 1 light source
    // this->red_position = glm::vec3 {this->radius, 0.0f, 0.0f};
    // shader.setVec3("light_source.position", this->red_position);
    // shader.setVec3("light_source.color", glm::vec3 {1.0f, 1.0f, 1.0f});

}



#endif // !_LIGHT_H_
