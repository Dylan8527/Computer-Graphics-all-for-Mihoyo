#include <utils.h>
#include <mesh.h>
#include <shader.h>
#include <light.h>
#include <camera.h>
#include <stb_image.h>
#include <iostream>

void processKeyboard(GLFWwindow *window);
void processMouseClick(GLFWwindow *window, int button, int action, int mode);
void processMouseMovement(GLFWwindow *window, double x_pos, double y_pos);
void processMouseScroll(GLFWwindow *window, double x_offset, double y_offset);

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;
constexpr float aspect_ratio = (float)WIDTH / (float)HEIGHT;

GLFWwindow *window;
Camera camera(glm::vec3 {0.0f, 0.5f, 5.0f});

int main() {
    WindowGuard windowGuard(window, WIDTH, HEIGHT, "EARTH_NERF_NGP");

    //* callbacks
    glfwSetMouseButtonCallback(window, processMouseClick);
    glfwSetCursorPosCallback(window, processMouseMovement);
    glfwSetScrollCallback(window, processMouseScroll);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    //* load mesh
    // Mesh mesh("assets/bunny.object");
    // Mesh mesh("assets/plane.object");
    // Mesh mesh("assets/sphere.object");
    //* construct mesh locally
    Mesh mesh;

    //* draw mesh
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(vec3)));
    glEnableVertexAttribArray(2);

    //* texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char *data = stbi_load("../earth/earthmap.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture!" << std::endl;
    }
    stbi_image_free(data);

    //* shader for rendering
    // Shader object_shader("shader/obj_vs.glsl", "shader/single_source.glsl");
    Shader object_shader("shader/obj_vs.glsl", "shader/obj_fs.glsl");
    // object_shader.use();
    // object_shader.setInt("Texture", 0);

    //* object property
    // white
    glm::vec3 object_color = glm::vec3 {1.0f, 1.0f, 1.0f};
    // pink
    // glm::vec3 object_color = glm::vec3 {0.9843137264f, 0.7254902124f, 0.9137254953f};

    //* light
    Light light(mesh);

    //* tranformation matrix
    glm::mat4 world = glm::mat4(1.0f);
    glm::mat4 projection = glm::ortho(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, 0.1f, 10.0f);
    camera.transform << "Orthogonal matrix:\n[\n"
                     << projection << "]\n\n";


    //* N camera positions uniform distributing on the sphere
    uint n_images = 121;
    // std::cout << "Input the number of images you want :" ;
    // std::cin >> n_images;

    //* phi = 2 * pi * u1
    //* theta = acos(1 - 2 u2)
    uint sqrt_n = sqrt(n_images);
    float inv_sqrt_n = 1.0 / sqrt_n;
    std::vector<glm::vec3> camera_positions;
    for(uint i = 0; i < sqrt_n; ++i){
        float u1 = i * inv_sqrt_n;
        float phi = acos(1 - 2 * u1);
        for(uint j = 0; j < sqrt_n; ++j){
            float u2 = j * inv_sqrt_n;
            float theta = 2 * M_PI * u2;
            camera_positions.emplace_back(glm::vec3{
                3.5f * cos(theta) * sin(phi),
                3.5f * sin(theta) * sin(phi),
                3.5f * cos(phi)
            });
            if(phi == M_PI || phi == 0)
                break;
        }
    }

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
    for(auto && pos: camera_positions){
        processKeyboard(window);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //* tranformation matrix
        camera.SetViewMatrix(pos);
        glm::mat4 view = camera.GetViewMatrix();
        // glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), aspect_ratio, 0.1f, 100.0f);
        object_shader.use();

        // //* uniforms
        object_shader.setVec3("object_color", object_color);
        object_shader.setVec3("camera_position", camera.position);
        object_shader.setMat4("world", world);
        object_shader.setMat4("view", view);
        object_shader.setMat4("projection", projection);
        light.ignite(object_shader);

        glBindVertexArray(VAO);
        mesh.draw();
        camera.DrawDraw();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    break;
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    return 0;
}

void processKeyboard(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    camera.ProcessKeyboard(window);
}

void processMouseClick(GLFWwindow *window, int button, int action, int mode) {

    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
        camera.SetMouseClick(true);
    else
        camera.SetMouseClick(false);
}

void processMouseMovement(GLFWwindow *window, double x_pos, double y_pos) {
    camera.ProcessMouseMovement(window, (float)x_pos, (float)y_pos);
}

void processMouseScroll(GLFWwindow *window, double x_offset, double y_offset) {
    camera.ProcessMouseScroll(window, (float)x_offset, (float)y_offset);
}

