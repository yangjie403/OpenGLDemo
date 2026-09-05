#include "mylog.h"
#include "main7.h"

#include <cmath>
#include <iostream>

namespace
{
    constexpr unsigned int SCR_WIDTH = 800;
    constexpr unsigned int SCR_HEIGHT = 800;
    constexpr float CIRCLE_RADIUS = 0.5f;
    constexpr float TRANSITION_DURATION = 2.0f;

    void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    // 顶点着色器只负责把矩形顶点传递到裁剪空间。
    // aPos 的范围是 [-1, 1]，同时作为片段着色器中的坐标输入。
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "out vec2 localPosition;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
        "    localPosition = aPos;\n"
        "}\0";

    // 将 HSV 颜色转换为 RGB 颜色。
    //
    // h：色相，范围通常是 [0, 1)
    // s：饱和度，范围是 [0, 1]
    // v：明度，范围是 [0, 1]
    //
    // h 在圆周方向变化，s 在圆心到边缘方向变化，
    // v 固定为 1.0，因此圆心为白色，边缘为高饱和度颜色。
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "in vec2 localPosition;\n"
        "out vec4 FragColor;\n"
        "uniform float transition;\n"
        "uniform float circleRadius;\n"
        "uniform vec2 resolution;\n"
        "\n"
        "vec3 hsvToRgb(vec3 hsv)\n"
        "{\n"
        "    float h = hsv.x;\n"
        "    float s = hsv.y;\n"
        "    float v = hsv.z;\n"
        "\n"
        "    float h6 = h * 6.0;\n"
        "    float sector = floor(h6);\n"
        "    float fraction = h6 - sector;\n"
        "\n"
        "    float p = v * (1.0 - s);\n"
        "    float q = v * (1.0 - s * fraction);\n"
        "    float t = v * (1.0 - s * (1.0 - fraction));\n"
        "\n"
        "    if (sector < 1.0) return vec3(v, t, p);\n"
        "    if (sector < 2.0) return vec3(q, v, p);\n"
        "    if (sector < 3.0) return vec3(p, v, t);\n"
        "    if (sector < 4.0) return vec3(p, q, v);\n"
        "    if (sector < 5.0) return vec3(t, p, v);\n"
        "    return vec3(v, p, q);\n"
        "}\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // localPosition 是当前片段相对于圆心的坐标。\n"
        "    // 圆心为 (0, 0)，x 和 y 的范围大致是 [-1, 1]。\n"
        "    vec2 position = localPosition;\n"
        "\n"
        "    // 根据窗口宽高修正坐标，避免窗口不是正方形时圆形被拉伸。\n"
        "    float aspect = resolution.x / resolution.y;\n"
        "    if (aspect > 1.0)\n"
        "    {\n"
        "        position.x *= aspect;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        position.y /= aspect;\n"
        "    }\n"
        "\n"
        "    // length(position) 计算当前片段到圆心的距离。\n"
        "    float radius = length(position);\n"
        "\n"
        "    // 矩形覆盖了整个区域，但我们只保留圆内的片段。\n"
        "    // 圆外片段直接丢弃，不会写入颜色缓冲区。\n"
        "    if (radius > circleRadius)\n"
        "    {\n"
        "        discard;\n"
        "    }\n"
        "\n"
        "    // atan(y, x) 返回当前片段相对于 x 轴的弧度角。\n"
        "    // 结果范围是 [-PI, PI]，因此需要转换到 [0, 1) 的色相范围。\n"
        "    float angle = atan(position.y, position.x);\n"
        "    float hue = angle / (2.0 * 3.14159265359);\n"
        "    if (hue < 0.0) hue += 1.0;\n"
        "\n"
        "    // 半径越大，饱和度越高：圆心为白色，圆周颜色最鲜艳。\n"
        "    float saturation = clamp(radius / circleRadius, 0.0, 1.0);\n"
        "    float value = 1.0;\n"
        "    vec3 baseColor = hsvToRgb(vec3(hue, saturation, value));\n"
        "\n"
        "    // transition 的整数部分决定当前颜色状态，\n"
        "    // 小数部分决定当前状态之间的过渡进度。\n"
        "    vec3 state0 = baseColor;\n"
        "    vec3 state1 = vec3(baseColor.b, baseColor.r, baseColor.g);\n"
        "    vec3 state2 = vec3(baseColor.g, baseColor.b, baseColor.r);\n"
        "\n"
        "    int currentState = int(floor(transition));\n"
        "    float progress = fract(transition);\n"
        "    vec3 fromColor;\n"
        "    vec3 toColor;\n"
        "\n"
        "    if (currentState == 0)\n"
        "    {\n"
        "        fromColor = state0;\n"
        "        toColor = state1;\n"
        "    }\n"
        "    else if (currentState == 1)\n"
        "    {\n"
        "        fromColor = state1;\n"
        "        toColor = state2;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        fromColor = state2;\n"
        "        toColor = state0;\n"
        "    }\n"
        "\n"
        "    float smoothProgress = smoothstep(0.0, 1.0, progress);\n"
        "    vec3 color = mix(fromColor, toColor, progress);\n"
        "    FragColor = vec4(color, 1.0);\n"
        "}\0";
}

int main7()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "OpenGL Color Wheel",
        nullptr,
        nullptr);

    if (window == nullptr)
    {
        std::cout << "Failed to create window" << std::endl;
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

    // 创建并编译顶点着色器。
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    printShaderLog(vertexShader);

    // 创建并编译片段着色器。
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    printShaderLog(fragmentShader);

    // 将两个着色器链接为完整的着色器程序。
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    printProgramLog(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 只使用两个三角形组成一个矩形。
    // 圆形的边界和颜色完全由片段着色器计算，
    // 不需要使用大量圆周顶点来近似圆形。
    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    GLuint VAO = 0;
    GLuint VBO = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 每个顶点只有 x、y 两个 float。
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // transition 是片段着色器中的 Uniform。
    // 它控制 RGB -> GBR -> BRG -> RGB 的颜色过渡。
    GLint transitionLocation =
        glGetUniformLocation(shaderProgram, "transition");
    GLint circleRadiusLocation =
        glGetUniformLocation(shaderProgram, "circleRadius");
    GLint resolutionLocation =
        glGetUniformLocation(shaderProgram, "resolution");

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // 将时间映射到 [0, 3)：
        //
        //   [0, 1)：RGB -> GBR
        //   [1, 2)：GBR -> BRG
        //   [2, 3)：BRG -> RGB
        //
        // 每一段持续 TRANSITION_DURATION 秒。
        double timeValue = glfwGetTime();
        float transition = static_cast<float>(
            std::fmod(
                timeValue,
                static_cast<double>(TRANSITION_DURATION * 3.0f))
            / static_cast<double>(TRANSITION_DURATION));

        // 将 CPU 端计算出的动画进度传给 GLSL。
        glUniform1f(transitionLocation, transition);
        glUniform1f(circleRadiusLocation, CIRCLE_RADIUS);

        // 获取实际帧缓冲区尺寸，供片段着色器进行宽高比修正。
        int framebufferWidth = 0;
        int framebufferHeight = 0;
        glfwGetFramebufferSize(
            window,
            &framebufferWidth,
            &framebufferHeight);
        glUniform2f(
            resolutionLocation,
            static_cast<float>(framebufferWidth),
            static_cast<float>(framebufferHeight));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
