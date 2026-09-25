#include "main11.h"
#include "my_shader.h"
#include "std_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
    void processInput(GLFWwindow* window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 800;
}

void main11() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    Shader shader("shaders/shader11.vs", "shaders/shader11.fs");

    float vertices[] = {
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture1, texture2;

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("assets/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("assets/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    shader.use();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // 每一帧都重新绘制场景：先清空上一帧的颜色缓冲区。
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 选择纹理单元，并把对应的纹理绑定到该纹理单元。
        // 着色器中的 sampler2D texture1/texture2 分别指向纹理单元 0/1。
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // glm::mat4：GLM 提供的 4×4 浮点矩阵类型，适合表示 2D/3D 变换。
        // 这里使用 glm::mat4(1.0f) 构造单位矩阵：
        // - 参数 1.0f 会填入主对角线，得到单位矩阵 I；
        // - 其他位置为 0；
        // - I 乘以顶点不会改变顶点的位置。
        // 后面的平移和旋转都会在这个矩阵的基础上继续叠加。
        glm::mat4 transform1 = glm::mat4(1.0f);
        double time = glfwGetTime();

        // glm::translate(mat, offset)：根据平移量创建平移矩阵，并与 mat 组合。
        // 参数说明：
        // - mat：已有的变换矩阵，这里是上面创建的单位矩阵；
        // - offset：平移向量 (x, y, z)，分别表示沿三个坐标轴移动的距离。
        // 返回值：组合后的新矩阵。GLM 通常不会直接修改 mat，而是返回结果。
        // 本例中 (0.5, -0.5, 0.0) 表示向右移动 0.5、向下移动 0.5，z 不变。
        // OpenGL 的标准化设备坐标通常在 [-1, 1] 范围内，所以图形会出现在右下区域。
        transform1 = glm::translate(transform1, glm::vec3(0.5f, -0.5f, 0.0f));

        // glfwGetTime()：返回 GLFW 初始化后经过的时间，单位是秒，类型为 double。
        // 每一帧重新读取它，就能让变换随着时间变化，从而产生动画效果。
        // 前面的 (float) 将 double 转成 float，以匹配 glm::rotate 的常用参数类型。
        float angle = (float)time;

        // glm::rotate(mat, angle, axis)：根据旋转角度和旋转轴创建旋转矩阵，并与 mat 组合。
        // 参数说明：
        // - mat：已有的变换矩阵，这里已经包含平移；
        // - angle：旋转角度，GLM 使用弧度而不是角度（弧度 = 角度 × π / 180）；
        // - axis：旋转轴向量，这里的 (0, 0, 1) 表示绕 z 轴旋转；
        // 返回值：叠加旋转后的新 4×4 矩阵。
        // angle 随时间不断增大，所以图形会持续旋转。
        transform1 = glm::rotate(transform1, angle, glm::vec3(0.0f, 0.0f, 1.0f));

        // 对 OpenGL 使用的列向量写法，最终矩阵可理解为 T * R：
        // 顶点先绕自己的原点旋转，再被平移到 (0.5, -0.5)，
        // 因此图形是在右下位置自转。
        // 如果把两次调用的顺序交换，最终通常会得到 R * T：
        // 顶点会先平移，再连同平移结果一起绕窗口原点旋转，表现为公转。

        glm::mat4 transform2 = glm::mat4(1.0f);

        float scale = 0.25 * sin(time) + 0.75;
        transform2 = glm::scale(transform2, glm::vec3(scale, scale, 1.0f));
        transform2 = glm::translate(transform2, glm::vec3(-0.5f, 0.5f, 0.0f));

        // shader.use()：激活 Shader 程序，使后续的 uniform 设置和绘制调用
        // 都作用于这个 shader。必须在设置 transform uniform 之前调用。
        shader.use();

        // glGetUniformLocation(program, name)：查询 uniform 变量在当前 Shader
        // 程序中的位置（location）。
        // 参数说明：
        // - program：已经链接成功的 Shader 程序 ID，这里是 shader.ID；
        // - name：着色器中的 uniform 名称，必须与 shader11.vs 中的 "transform" 一致。
        // 返回值：变量位置；如果找不到变量会返回 -1。
        // 实际项目中建议用 GLint 保存返回值，因为 -1 是有效的失败标记。
        GLint transformLoc = glGetUniformLocation(shader.ID, "transform");

        // glm::value_ptr(transform)：取得 GLM 矩阵内部连续浮点数据的首地址，
        // 供 OpenGL 从 CPU 内存读取矩阵内容。
        //
        // glUniformMatrix4fv(location, count, transpose, value)：
        // 将一个或多个 4×4 浮点矩阵写入当前 Shader 的 uniform。
        // 参数说明：
        // - location：uniform 的位置，这里是 transformLoc；
        // - count：要上传的矩阵数量，本例只有 1 个；
        // - transpose：是否转置矩阵。GLM 与 OpenGL 的约定匹配，因此使用 GL_FALSE；
        // - value：矩阵数据首地址，这里由 glm::value_ptr(transform) 提供。
        // 执行后，顶点着色器中的 transform 就会得到当前帧的变换矩阵。
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform1));

        // 使用 VAO 中保存的顶点属性和索引数据，绘制 6 个索引组成的两个三角形。
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform2));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // 交换前后缓冲区，并处理窗口事件，使当前帧显示出来。
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);
    glDeleteProgram(shader.ID);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
}
