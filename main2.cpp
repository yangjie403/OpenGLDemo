#include "main2.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>

namespace
{
    constexpr GLuint numVAOs = 1;

    GLuint renderingProgram = 0;
    GLuint vao[numVAOs] = {};

    GLuint createShaderProgram()
    {
        const char* vshaderSource =
            "#version 330 \n"
            "void main(void) \n"
            "{ gl_Position = vec4(0.0, 0.0, 0.0, 1.0); }";

        const char* fshaderSource =
            "#version 330 \n"
            "out vec4 color; \n"
            "void main(void) \n"
            "{ color = vec4(0.0, 0.0, 1.0, 1.0); }";

        GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vShader, 1, &vshaderSource, nullptr);
        glShaderSource(fShader, 1, &fshaderSource, nullptr);
        glCompileShader(vShader);
        glCompileShader(fShader);

        GLuint vfProgram = glCreateProgram();
        glAttachShader(vfProgram, vShader);
        glAttachShader(vfProgram, fShader);
        glLinkProgram(vfProgram);

        glDeleteShader(vShader);
        glDeleteShader(fShader);
        return vfProgram;
    }

    void init(GLFWwindow* window)
    {
        renderingProgram = createShaderProgram();
        glGenVertexArrays(numVAOs, vao);
        glBindVertexArray(vao[0]);
    }

    void display(GLFWwindow* window, double currentTime)
    {
        glUseProgram(renderingProgram);
        glPointSize(30.0f);
        glDrawArrays(GL_POINTS, 0, 1);
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}

int main2()
{
    if (!glfwInit())
    {
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(600, 600, "program2", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    init(window);

    while (!glfwWindowShouldClose(window))
    {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
