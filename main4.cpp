// main4.cpp
//
// 本文件演示使用 OpenGL 绘制一个三角形的最基本流程。
// 与前面的练习相比，这里使用：
//   1. 顶点数组保存三角形的 3 个顶点；
//   2. VBO 将顶点数据上传到 GPU；
//   3. VAO 记录顶点属性的解释方式；
//   4. glDrawArrays 直接按照顶点顺序进行绘制。
//
// 本示例没有使用 EBO/索引缓冲，因为 3 个顶点只使用一次，
// 不需要通过索引复用顶点。

#include "mylog.h"  // 着色器和着色器程序的日志输出函数
#include "main4.h"  // 对外暴露 main4() 的声明

#include <iostream>

// 匿名命名空间中的内容只在本文件内可见。
// 因此这里的辅助函数、常量和着色器源代码不会与其他练习文件冲突。
namespace
{
    // 处理窗口输入。
    // 按下 ESC 后设置 GLFW 的“窗口应该关闭”标志，
    // 主循环检测到该标志后就会退出。
    void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }

    // 窗口大小改变时由 GLFW 调用这个回调函数。
    //
    // OpenGL 的视口表示：
    // “将 OpenGL 最终生成的图像映射到窗口的哪一块区域”。
    // 窗口大小变化后，需要使用新的宽度和高度更新视口。
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    // 窗口的初始尺寸。
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 800;

    // 顶点着色器源码。
    //
    // 顶点着色器会对每个顶点执行一次。这里的工作很简单：
    // 将输入的 aPos 直接作为顶点的裁剪空间坐标输出。
    //
    // layout (location = 0)：
    //   表示 aPos 对应顶点属性位置 0。
    //   这个 0 必须与后面 glVertexAttribPointer 的第一个参数一致。
    //
    // vec3 aPos：
    //   每个顶点包含 3 个 float，分别表示 x、y、z。
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    // 片段着色器源码。
    //
    // 三角形经过光栅化后会生成许多片段。
    // 片段着色器会为每个片段计算最终颜色。
    //
    // out vec4 FragColor：
    //   输出一个 RGBA 颜色，四个分量分别是红、绿、蓝和透明度。
    const char* fragmentShaderSource1 =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";
}

// main.cpp 通过 main4.h 调用这个函数。
// 这是本练习对外提供的唯一入口；其他函数都只在 main4.cpp 内部使用。
int main4()
{
    // ============================================================
    // 一、初始化 GLFW，并设置 OpenGL 上下文的版本和模式
    // ============================================================

    // GLFW 负责创建窗口、创建 OpenGL 上下文以及处理窗口事件。
    glfwInit();

    // 请求 OpenGL 3.3。
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // 使用 Core Profile。
    // Core Profile 只保留现代 OpenGL 的核心功能，
    // 需要显式创建 VAO、VBO 并配置顶点属性。
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ============================================================
    // 二、创建窗口并建立当前 OpenGL 上下文
    // ============================================================

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

    // 将窗口关联的 OpenGL 上下文设置为当前上下文。
    // 只有设置完成后，后续 OpenGL 函数调用才知道作用于哪个上下文。
    glfwMakeContextCurrent(window);

    // 注册窗口大小回调。
    // 当窗口尺寸变化时，GLFW 会自动调用 framebuffer_size_callback。
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ============================================================
    // 三、加载 OpenGL 函数
    // ============================================================

    // GLAD 需要通过 GLFW 获取当前平台上的 OpenGL 函数地址。
    // 必须在创建并激活 OpenGL 上下文之后调用。
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ============================================================
    // 四、创建、编译顶点着色器
    // ============================================================

    // 创建一个顶点着色器对象。
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // 将 C++ 字符串中的 GLSL 源码交给 OpenGL。
    //
    // 参数含义：
    //   vertexShader：目标着色器对象；
    //   1           ：源码字符串的数量；
    //   &vertexShaderSource：源码字符串数组的地址；
    //   nullptr     ：让 OpenGL 自动计算字符串长度。
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

    // 将 GLSL 源码编译为 GPU 可以执行的着色器。
    glCompileShader(vertexShader);

    // 输出编译日志。
    // 如果 GLSL 语法错误，可以从这里查看驱动程序给出的信息。
    printShaderLog(vertexShader);

    // ============================================================
    // 五、创建、编译片段着色器
    // ============================================================

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource1, nullptr);
    glCompileShader(fragmentShader);
    printShaderLog(fragmentShader);

    // ============================================================
    // 六、链接着色器程序
    // ============================================================

    // 着色器对象只是单独的顶点处理器和片段处理器。
    // 必须将它们链接为一个完整的着色器程序，绘制时才能使用。
    GLuint shaderProgram = glCreateProgram();

    // 将两个着色器附加到程序对象。
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // 检查着色器之间的接口是否匹配，并生成最终可执行程序。
    glLinkProgram(shaderProgram);
    printProgramLog(shaderProgram);

    // 链接完成后，着色器源码对象已经不再需要。
    // 它们已经被复制/链接到 shaderProgram 中。
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ============================================================
    // 七、准备三角形顶点数据
    // ============================================================

    // 三角形有 3 个顶点。
    // 每个顶点由 3 个 float 组成：x、y、z。
    float vertices[] = {
        -0.4f, -0.6f, 0.0f,
         0.4f, -0.6f, 0.0f,
         0.0f,  0.0f, 0.0f,
        -0.4f,  0.6f, 0.0f,
         0.4f,  0.6f, 0.0f,
         0.0f,  0.0f, 0.0f
    };

    // ============================================================
    // 八、创建 VAO 和 VBO
    // ============================================================

    // VBO（Vertex Buffer Object）：
    //   GPU 中用于保存顶点数据的缓冲区。
    //
    // VAO（Vertex Array Object）：
    //   保存顶点属性配置的对象。
    //   例如：顶点属性 0 从哪个 VBO 读取、每个顶点有几个分量、
    //   相邻顶点之间的字节间隔是多少等。
    GLuint VBO = 0;
    GLuint VAO = 0;

    // 让 OpenGL 创建一个 VAO 和一个 VBO，并返回它们的编号。
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 先绑定 VAO。
    // 后面进行的顶点属性配置会记录到当前绑定的 VAO 中。
    glBindVertexArray(VAO);

    // 将 VBO 绑定到 GL_ARRAY_BUFFER 目标。
    // 绑定后，GL_ARRAY_BUFFER 相关操作会作用于这个 VBO。
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 将 vertices 中的数据复制到 GPU。
    //
    // 参数含义：
    //   GL_ARRAY_BUFFER：当前操作的是顶点数据缓冲区；
    //   sizeof(vertices)：要上传的字节数；
    //   vertices：CPU 端数据的首地址；
    //   GL_STATIC_DRAW：数据上传后基本不会改变，适合静态模型。
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW);

    // ============================================================
    // 九、告诉 OpenGL 如何解释顶点数据
    // ============================================================

    // vertices 在内存中的布局是：
    //
    //   x, y, z, x, y, z, x, y, z
    //
    // glVertexAttribPointer 用来描述这种布局。
    //
    // 参数含义：
    //   0：顶点属性位置，对应顶点着色器中的 layout location 0；
    //   3：每个顶点包含 3 个数据分量；
    //   GL_FLOAT：每个分量的数据类型是 float；
    //   GL_FALSE：不要将数据自动归一化；
    //   3 * sizeof(float)：从一个顶点跳到下一个顶点需要跨过的字节数；
    //   (void*)0：从 VBO 的第 0 个字节开始读取。
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0);

    // 默认情况下，顶点属性 0 处于关闭状态。
    // 显式启用后，绘制时 OpenGL 才会从 VBO 中读取 aPos。
    glEnableVertexAttribArray(0);

    // 解除 VBO 的普通绑定。
    // 这不是绘制三角形的必要步骤，但可以降低后续代码误改 VBO 的风险。
    // 顶点属性配置已经记录在 VAO 中。
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 解除 VAO 绑定，表示顶点配置阶段结束。
    glBindVertexArray(0);

    // 设置为线框模式。
    // 如果希望显示实心三角形，可以删除这一行，或者改为 GL_FILL。
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // ============================================================
    // 十、渲染循环
    // ============================================================

    // 只要窗口没有被要求关闭，就持续绘制新的一帧。
    while (!glfwWindowShouldClose(window))
    {
        // 处理键盘输入。
        processInput(window);

        // 设置清屏颜色。
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // 清空颜色缓冲区。
        // 如果不清空，上一帧的内容可能残留在当前画面中。
        glClear(GL_COLOR_BUFFER_BIT);

        // 选择当前绘制要使用的着色器程序。
        glUseProgram(shaderProgram);

        // 绑定 VAO，恢复之前记录的顶点属性配置。
        glBindVertexArray(VAO);

        // 使用顶点数组绘制三角形。
        //
        // GL_TRIANGLES：每 3 个顶点组成一个三角形；
        // 0            ：从顶点数组的第 0 个顶点开始；
        // 3            ：读取 3 个顶点。
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // GLFW 默认使用双缓冲。
        // 当前内容先绘制到后缓冲，绘制完成后交换到前缓冲显示，
        // 可以避免用户看到绘制过程中的半成品画面。
        glfwSwapBuffers(window);

        // 处理窗口、键盘和鼠标等事件。
        glfwPollEvents();
    }

    // ============================================================
    // 十一、释放 OpenGL 和 GLFW 资源
    // ============================================================

    // 删除 VAO、VBO 和着色器程序，释放 GPU 资源。
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // 销毁窗口及其 OpenGL 上下文，并关闭 GLFW。
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
