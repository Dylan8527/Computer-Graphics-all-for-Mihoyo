#include <utils.h>

#ifdef USE_FILESYSTEM
#include <filesystem>

std::string getPath(const std::string &target, int depth) {
    std::string path = target;
    namespace fs = std::filesystem;
    for (int i = 0; i < depth; ++i) {
        if (fs::exists(path)) {
            return path;
        }
        path = "../" + path;
    }
    LOG_ERR("failed to get file: " + target);
    return target;
}
#else
#include <cstdio>

std::string getPath(const std::string &target, int depth) {
    std::string path = target;
    for (int i = 0; i < depth; ++i) {
        FILE *file = fopen(path.c_str(), "r");
        if (file) {
            fclose(file);
            return path;
        }
        path = "../" + path;
    }
    LOG_ERR("failed to get file: " + target);
    return target;
}
#endif

WindowGuard::WindowGuard(GLFWwindow *&window, const int width, const int height, const std::string &title) {
    if (!glfwInit()) LOG_ERR("Failed to init glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // create window
    if (!(window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr)))
        LOG_ERR("failed to create glfw window");

    glfwMakeContextCurrent(window);
    // enable vsync
    glfwSwapInterval(1);
    // init glew
    if (glewInit() != GLEW_OK)
        LOG_ERR("failed to init glew");
    glViewport(0, 0, width, height);
}

WindowGuard::~WindowGuard() { glfwTerminate(); }

std::ostream& operator<<(std::ostream& out, const glm::mat4& m) {
    out << "    [" << m[0][0] << ", " << m[1][0] << ", " << m[2][0] << ", " << m[3][0] << "]\n"
        << "    [" << m[0][1] << ", " << m[1][1] << ", " << m[2][1] << ", " << m[3][1] << "]\n"
        << "    [" << m[0][2] << ", " << m[1][2] << ", " << m[2][2] << ", " << m[3][2] << "]\n"
        << "    [0, 0, 0, 1]\n";
    return out;
}
