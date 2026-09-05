// main3.cpp
//
// 本文件演示使用 OpenGL 绘制图形的基本流程：
// 1. 使用 GLFW 创建窗口并建立 OpenGL 上下文。
// 2. 使用 GLAD 加载 OpenGL 函数。
// 3. 编写、编译并链接顶点着色器和片段着色器。
// 4. 将顶点数据和索引数据上传到 GPU。
// 5. 配置顶点属性。
// 6. 在循环中清屏并发出绘制命令。

#include "mylog.h"
#include "main3.h"

#include <iostream>

namespace
{
    // 窗口的初始尺寸。
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    // 顶点着色器在 GPU 上运行。
    //
    // 顶点数据中的每个顶点包含 3 个 float：
    //     x, y, z
    //
    // layout (location = 0) 表示这个输入变量对应顶点属性 0。
    // 这个位置需要和 glVertexAttribPointer 的第一个参数对应。
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = vec4(aPos, 1.0);\n"
        "}\n";

    // 片段着色器在光栅化后运行。
    //
    // 一个三角形经过光栅化后会产生许多片段，片段着色器决定
    // 每个片段最终写入颜色缓冲区的颜色。
    const char* fragmentShaderSource1 =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main(void)\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n";

    // 处理键盘输入。
    // GLFW_KEY_ESCAPE 被按下时，设置窗口关闭标志，
    // 主循环下一次判断时就会结束。
    void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }

    // 当窗口大小发生变化时，更新 OpenGL 视口。
    //
    // 视口决定 OpenGL 将标准化设备坐标映射到窗口的哪个区域。
    // 如果不更新视口，窗口改变大小后图像可能出现拉伸或显示不完整。
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}

int main3()
{
    // ---------------------------
    // 1. 初始化 GLFW
    // ---------------------------
    //
    // GLFW 负责创建窗口、创建 OpenGL 上下文以及处理输入和窗口事件。
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 告诉 GLFW 我们希望创建什么版本的 OpenGL 上下文。
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口。
    // 此时 OpenGL 上下文已经随窗口创建，但还不是当前上下文。
    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "LearnOpenGL",
        nullptr,
        nullptr);

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 将刚创建的 OpenGL 上下文设置为当前上下文。
    // 后续 OpenGL 函数调用都会作用于这个上下文。
    glfwMakeContextCurrent(window);

    // 注册窗口大小变化回调。
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ---------------------------
    // 2. 初始化 GLAD
    // ---------------------------
    //
    // OpenGL 函数地址通常需要在创建上下文后动态加载。
    // GLAD 根据当前平台加载 glCreateShader、glGenBuffers
    // 等 OpenGL 函数，否则这些函数指针可能为空。
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // ---------------------------
    // 3. 创建并编译顶点着色器
    // ---------------------------
    //
    // glCreateShader 只创建一个着色器对象，指定它的类型；
    // 真正的 GLSL 源代码通过 glShaderSource 提交。
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // 输出着色器编译日志。
    // 编译成功时日志通常为空；如果 GLSL 有语法错误，
    // 这里可以帮助定位问题。
    printShaderLog(vertexShader);

    // ---------------------------
    // 4. 创建并编译片段着色器
    // ---------------------------
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource1, nullptr);
    glCompileShader(fragmentShader);
    printShaderLog(fragmentShader);

    // ---------------------------
    // 5. 链接着色器程序
    // ---------------------------
    //
    // 顶点着色器和片段着色器需要链接成一个完整的 GPU 程序，
    // 绘制时通过 glUseProgram 使用这个程序。
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 链接阶段也可能失败，例如两个着色器的输入输出不匹配。
    printProgramLog(shaderProgram);

    // 着色器已经链接进 shaderProgram，之后可以删除临时的
    // vertexShader 和 fragmentShader 对象。
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ---------------------------
    // 6. 准备顶点数据和索引数据
    // ---------------------------
    //
    // 每 3 个 float 表示一个顶点的 x、y、z 坐标。
    //
    // 这里定义了一个矩形的四个顶点：
    //
    //     3 -------- 0
    //     |          |
    //     |          |
    //     2 -------- 1
    //
    // OpenGL 的坐标范围通常是 -1.0 到 1.0，
    // 因此这些顶点会出现在窗口中央附近。
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // 0: 右上
         0.5f, -0.5f, 0.0f,  // 1: 右下
        -0.5f, -0.5f, 0.0f,  // 2: 左下
        -0.5f,  0.5f, 0.0f   // 3: 左上
    };

    // 索引数组描述如何使用上面的顶点组成三角形。
    //
    // 0, 1, 3 组成第一个三角形；
    // 1, 2, 3 组成第二个三角形。
    // 两个三角形合在一起就是一个矩形。
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    // ---------------------------
    // 7. 创建 OpenGL 对象
    // ---------------------------
    //
    // VBO（Vertex Buffer Object）：
    //     存储顶点数据。
    //
    // EBO（Element Buffer Object）：
    //     存储顶点索引。
    //
    // VAO（Vertex Array Object）：
    //     记录顶点属性的配置，以及相关的 VBO/EBO 状态。
    //     绘制前绑定 VAO，就可以恢复这一组顶点配置。
    unsigned int VBO = 0;
    unsigned int VAO = 0;
    unsigned int EBO = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 先绑定 VAO，之后的顶点属性设置会记录到这个 VAO 中。
    glBindVertexArray(VAO);

    // 将 VBO 绑定到 GL_ARRAY_BUFFER。
    // 绑定后，后续对 GL_ARRAY_BUFFER 的操作都会作用于 VBO。
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW);

    // 将 EBO 绑定到 GL_ELEMENT_ARRAY_BUFFER，并上传索引数据。
    // EBO 的绑定关系会被当前 VAO 记住。
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW);

    // 告诉 OpenGL 如何解释 VBO 中的顶点数据：
    //
    // location = 0：对应顶点着色器中的 aPos。
    // 3           ：每个顶点包含 3 个分量。
    // GL_FLOAT    ：每个分量是 float。
    // GL_FALSE    ：不需要把数据归一化到 [0, 1]。
    // stride      ：相邻两个顶点之间相隔 3 个 float。
    // offset 0    ：从当前顶点数据的第 0 个字节开始读取。
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        static_cast<void*>(nullptr));

    // 默认情况下，顶点属性是关闭的；必须显式启用属性 0。
    glEnableVertexAttribArray(0);

    // 解除 VBO 的普通绑定不是必须的，但有助于避免后续代码
    // 意外修改 VBO。EBO 仍然保留在 VAO 中。
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 解绑 VAO，表示顶点配置阶段结束。
    glBindVertexArray(0);

    // 设置线框模式，便于观察两个三角形的边。
    // 如果希望填充矩形，可以删除这一行，或使用 GL_FILL。
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // ---------------------------
    // 8. 渲染循环
    // ---------------------------
    //
    // 每一帧通常都需要：
    //     处理输入
    //     清空上一帧的颜色
    //     使用着色器程序
    //     绑定 VAO
    //     发出绘制命令
    //     交换前后缓冲
    //     处理窗口事件
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // 设置清屏颜色，并清空颜色缓冲区。
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 选择本次绘制使用的着色器程序。
        glUseProgram(shaderProgram);

        // 恢复 VAO 中记录的顶点属性配置。
        glBindVertexArray(VAO);

        // 使用索引绘制三角形。
        //
        // GL_TRIANGLES：每 3 个索引组成一个三角形。
        // 6            ：本次读取 6 个索引，也就是 2 个三角形。
        // GL_UNSIGNED_INT：索引数组元素的类型。
        // nullptr      ：从 EBO 的第 0 个字节开始读取。
        glDrawElements(
            GL_TRIANGLES,
            6,
            GL_UNSIGNED_INT,
            nullptr);

        // 双缓冲窗口通常有前缓冲和后缓冲：
        // 先在后缓冲绘制，完成后交换缓冲，避免画面闪烁。
        glfwSwapBuffers(window);

        // 处理键盘、鼠标、窗口大小变化等事件。
        glfwPollEvents();
    }

    // ---------------------------
    // 9. 释放资源
    // ---------------------------
    //
    // OpenGL 对象不会因为离开 C++ 变量作用域就自动删除，
    // 因此应当显式释放不再使用的 GPU 资源。
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
