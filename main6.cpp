#include "mylog.h"
#include "main6.h"

#include <cmath>
#include <iostream>

namespace
{
    constexpr unsigned int SCR_WIDTH = 800;
    constexpr unsigned int SCR_HEIGHT = 800;

    // 每个颜色状态之间的过渡时间，单位为秒。
    // 一个完整循环包含 3 段过渡，因此完整循环时间为 6 秒。
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

    // 顶点着色器：
    //
    // aPos 是顶点位置输入，对应顶点属性 0。
    // aColor 是顶点颜色输入，对应顶点属性 1。
    //
    // transition 是 CPU 每一帧通过 Uniform 传入的动画进度：
    //
    //   [0, 1)：RGB -> GBR
    //   [1, 2)：GBR -> BRG
    //   [2, 3)：BRG -> RGB
    //
    // vertexColor 是传给片段着色器的输出。
    // 光栅化阶段会根据三个顶点的 vertexColor 自动进行插值。
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 vertexColor;\n"
        "uniform float transition;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos, 1.0);\n"
        "\n"
        "    // 状态 0：每个顶点使用自己的初始颜色。\n"
        "    vec3 state0 = aColor;\n"
        "\n"
        "    // 状态 1：颜色通道循环移动：红->绿，绿->蓝，蓝->红。\n"
        "    vec3 state1 = vec3(aColor.b, aColor.r, aColor.g);\n"
        "\n"
        "    // 状态 2：继续循环移动：红->蓝，绿->红，蓝->绿。\n"
        "    vec3 state2 = vec3(aColor.g, aColor.b, aColor.r);\n"
        "\n"
        "    // transition 的整数部分表示当前处于哪一段过渡。\n"
        "    int currentState = int(floor(transition));\n"
        "\n"
        "    // transition 的小数部分表示当前这一段过渡的进度，范围是 [0, 1)。\n"
        "    float progress = fract(transition);\n"
        "\n"
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
        "    // smoothstep 让过渡开始和结束时更平滑，避免速度突然变化。\n"
        "    float smoothProgress = smoothstep(0.0, 1.0, progress);\n"
        "\n"
        "    // mix(a, b, t) 表示在 a 和 b 之间按照 t 做线性插值。\n"
        "    // t=0 时完全是 fromColor，t=1 时完全是 toColor。\n"
        "    vertexColor = mix(fromColor, toColor, smoothProgress);\n"
        "}\0";

    // 片段着色器：
    //
    // vertexColor 是顶点着色器的输出，同时也是片段着色器的输入。
    // 经过光栅化后，一个片段的颜色通常是三个顶点颜色的插值结果。
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "in vec3 vertexColor;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(vertexColor, 1.0);\n"
        "}\0";
}

int main6()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "LearnOpenGL",
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

    // 将两个着色器链接为一个完整的着色器程序。
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    printProgramLog(shaderProgram);

    // 着色器已经链接到程序中，可以删除临时着色器对象。
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 每个顶点包含 6 个 float：
    //
    //   前 3 个 float：位置 x、y、z
    //   后 3 个 float：颜色 r、g、b
    //
    // 三个顶点的初始颜色分别为红、绿、蓝。
    float vertices[] = {
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // 右下：红
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // 左下：绿
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // 顶部：蓝
    };

    GLuint VAO = 0;
    GLuint VBO = 0;

    // 创建 VAO 和 VBO。
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 绑定 VAO，后续顶点属性配置会记录到它里面。
    glBindVertexArray(VAO);

    // 将交错排列的位置和颜色数据上传到 VBO。
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 配置顶点位置属性，对应顶点着色器中的 aPos。
    //
    // stride = 6 * sizeof(float)：
    // 从一个顶点跳到下一个顶点，需要跨过位置和颜色共 6 个 float。
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);

    // 配置顶点颜色属性，对应顶点着色器中的 aColor。
    //
    // offset = 3 * sizeof(float)：
    // 每个顶点的颜色紧跟在位置数据之后。
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 获取顶点着色器中 Uniform 变量 transition 的位置。
    // 位置在程序链接后就不会改变，因此只需要查询一次。
    GLint transitionLocation =
        glGetUniformLocation(shaderProgram, "transition");

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 选择本帧使用的 GLSL 程序。
        // glUniform 和 glDrawArrays 都会作用于这个程序。
        glUseProgram(shaderProgram);

        // 获取程序运行时间。
        double timeValue = glfwGetTime();

        // 将时间转换为 [0, 3) 范围内的动画进度：
        //
        //   0.0 ~ 1.0：RGB -> GBR
        //   1.0 ~ 2.0：GBR -> BRG
        //   2.0 ~ 3.0：BRG -> RGB
        //
        // fmod 取余后，时间达到完整循环时会重新从 0 开始。
        float transition = static_cast<float>(
            std::fmod(
                timeValue,
                static_cast<double>(TRANSITION_DURATION * 3.0f))
            / static_cast<double>(TRANSITION_DURATION));

        // 将 CPU 端计算出的 transition 传给 GLSL 中的 uniform。
        // glUniform1f 的最后一个参数对应 GLSL 的 float。
        glUniform1f(transitionLocation, transition);

        // 绑定 VAO，恢复位置和颜色属性的配置。
        glBindVertexArray(VAO);

        // 绘制 3 个顶点。
        //
        // 顶点着色器会为三个顶点分别计算 vertexColor；
        // 随后光栅化阶段会在三角形内部插值颜色；
        // 片段着色器最终将插值后的颜色写入 FragColor。
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // 显示后缓冲中的完整画面。
        glfwSwapBuffers(window);

        // 处理窗口和键盘事件。
        glfwPollEvents();
    }

    // 释放 GPU 和窗口资源。
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}
