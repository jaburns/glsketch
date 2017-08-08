#include <iostream>
#include "input.hpp"
#include "scenes/teapot_fountain.hpp"

static GLFWwindow *s_window;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
    exit(EXIT_FAILURE);
}

static void init_opengl()
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA

    s_window = glfwCreateWindow(1280, 720, "GL Sketch", NULL, NULL);
    if (!s_window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(s_window);
    glfwSwapInterval(1); // 1: vsync, 0: fast

#ifdef _WIN32
    GLint GlewInitResult = glewInit();
    if (GLEW_OK != GlewInitResult) {
        printf("ERROR: %s\n", glewGetErrorString(GlewInitResult));
        exit(EXIT_FAILURE);
    }
#endif
}

int main(int argc, char** argv)
{
    init_opengl();

    TeapotFountain scene;
    Input::bind_handlers(s_window);

    while (!glfwWindowShouldClose(s_window)) {
        const auto input_state = Input::read_state();
        int width, height;
        glfwGetFramebufferSize(s_window, &width, &height);

        scene.step(input_state, width, height);

        glfwSwapBuffers(s_window);
        glfwPollEvents();
    }

    glfwDestroyWindow(s_window);
    glfwTerminate();

    return 0;
}