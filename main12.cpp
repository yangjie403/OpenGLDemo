#include "my_shader.h"
#include "main12.h"
#include "std_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
    glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
    const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    float yaw = -90.0f, pitch = 0.0f;
    double lastX = 0.0, lastY = 0.0;
    bool firstMouse = true;

    void mouseCallback(GLFWwindow*, double x, double y) {
        if (firstMouse) {
            lastX = x;
            lastY = y;
            firstMouse = false;
            return;
        }
        constexpr float sensitivity = 0.1f;
        yaw += static_cast<float>(x - lastX) * sensitivity;
        pitch += static_cast<float>(lastY - y) * sensitivity;
        lastX = x; lastY = y;
        pitch = glm::clamp(pitch, -89.0f, 89.0f);
        float yRad = glm::radians(yaw), pRad = glm::radians(pitch);
        cameraFront = glm::normalize(glm::vec3(glm::cos(yRad) * glm::cos(pRad), glm::sin(pRad), glm::sin(yRad) * glm::cos(pRad)));

    }

    void processInput(GLFWwindow* window, float dt) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        glm::vec3 right = glm::normalize(glm::cross(cameraFront, worldUp));
        glm::vec3 move(0.0f);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            move += cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            move -= cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            move -= right;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            move += right;
        }
        if (glm::length(move) > 0.0f) {
            cameraPos += glm::normalize(move) * 2.5f * dt;
        }
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 800;
}

void main12() {
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
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    Shader ourShader("shaders/shader12.vs", "shaders/shader12.fs");

    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    glm::vec3 cubePositions[] = {
      glm::vec3(0.0f,  0.0f,  0.0f),
      glm::vec3(2.0f,  5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f),
      glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),
      glm::vec3(-1.7f,  3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),
      glm::vec3(1.5f,  2.0f, -2.5f),
      glm::vec3(1.5f,  0.2f, -1.5f),
      glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint textures[2];
    glGenTextures(2, textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
    
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);
    glEnable(GL_DEPTH_TEST);
    double lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float dt = glm::clamp(static_cast<float>(now - lastFrame), 0.0f, 0.1f);
        lastFrame = now;
        processInput(window, dt);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        // 设置当前使用的 shader program。后面的 glGetUniformLocation、glUniformMatrix4fv
        // 和 Shader::setMat4 都会针对当前 program 中的 uniform 进行操作。
        ourShader.use();

        // 三维坐标经过顶点着色器的典型变换链路如下：
        //
        //   局部坐标(Local) --model--> 世界坐标(World)
        //       --view--> 观察/相机坐标(View/Eye)
        //       --projection--> 裁剪坐标(Clip)
        //       --透视除法--> NDC，再由 OpenGL 映射到屏幕坐标
        //
        // 顶点着色器中的代码：
        //   gl_Position = projection * view * model * vec4(aPos, 1.0);
        // 矩阵乘法从右向左执行：先 model，再 view，最后 projection。

        // glm::mat4(1.0f) 构造 4×4 单位矩阵：主对角线为 1，其余元素为 0。
        // 单位矩阵相当于“不做任何变换”，因此适合用作变换矩阵的初始值。
        //glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        // model（模型矩阵）：把顶点从“模型/局部坐标系”变换到“世界坐标系”。
        // 本例的顶点位置 aPos 是以立方体/矩形自身原点为基准定义的局部坐标。
        //
        // glm::rotate(matrix, angle, axis)：在 matrix 的基础上追加旋转。
        //   matrix：待变换的原矩阵，这里是单位矩阵 model。
        //   angle ：旋转角度，GLM 要求使用弧度；正负号决定旋转方向。
        //   axis  ：旋转轴，这里 (1,0,0) 表示绕 x 轴旋转。
        // glm::radians(-55.0f)：将 -55 度转换为弧度；不能直接把角度值当弧度传入。
        // 因为初始 model 是单位矩阵，所以这里得到的是“绕世界原点/模型原点旋转 -55 度”。
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        // view（观察矩阵/相机矩阵）：把世界坐标变换到以相机为原点的观察坐标系。
        // OpenGL 中通常让相机位于原点，并沿 -Z 轴观察；因此把场景沿 Z 轴移动 -3，
        // 等价于把相机向 +Z 方向后移 3 个单位，让位于原点附近的物体进入相机视野。
        // 注意：view 矩阵表示的是“相机变换的逆”，所以代码中移动场景 -3，
        // 从观察者角度理解就是相机后退 3，而不是把相机本身直接向 -Z 移动。
        //
        // glm::translate(matrix, offset)：在 matrix 的基础上追加平移。
        //   matrix：待变换的原矩阵，这里是单位矩阵 view。
        //   offset：平移向量 (x, y, z)，这里沿 Z 轴负方向移动 3 个单位。
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, worldUp);

        // projection（投影矩阵）：把观察坐标变换到裁剪坐标，并定义相机的可见体积。
        // 使用透视投影后，远处物体会显得更小，符合人眼观察效果。
        //
        // glm::perspective(fovy, aspect, near, far)：创建透视投影矩阵。
        //   fovy  ：垂直方向视野角(Field of View)，这里是 45 度；角度越大，视野越宽。
        //   aspect：宽高比，必须使用 framebuffer 宽度 / 高度，否则画面会被拉伸。
        //   near  ：近裁剪面，这里为 0.1；比它更近的物体会被裁剪掉。
        //   far   ：远裁剪面，这里为 100.0；比它更远的物体会被裁剪掉。
        // near 和 far 共同定义深度方向上的可见范围；near 不应设置为 0。
        projection = glm::perspective(
            glm::radians(50.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            100.0f
        );

        // 将 CPU 端计算好的三个矩阵传入 shader 中对应的 uniform mat4。
        // 只有传入 shader 后，shader12.vs 才能在 gl_Position 中使用这些变换。
        // Shader::setMat4 内部会查找 uniform 位置，并将 glm::mat4 上传到 GPU。
        // 下面故意展示三种等价/相近的矩阵数据传递写法，便于理解 API：
        //   1. glm::value_ptr(model)：取得 glm::mat4 连续 float 数据的首地址；
        //   2. &view[0][0]：取得 GLM 矩阵第 0 列第 0 行元素的地址，作为连续数据首地址；
        //   3. Shader::setMat4：将查找 uniform 和上传矩阵封装起来。
        //
        // glGetUniformLocation(program, name)：查询 uniform 在指定 shader program 中的位置。
        //   program：shader program ID，这里是 ourShader.ID；
        //   name   ：GLSL 中声明的 uniform 名称，必须与 shader12.vs 中的名称一致。
        // 返回值是位置编号；如果 uniform 不存在或被编译器优化掉，会返回 -1。
        //unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");

        // glUniformMatrix4fv(location, count, transpose, value)：上传一个或多个 4×4 浮点矩阵。
        //   location ：uniform 位置；
        //   count    ：矩阵数量，这里每次只上传 1 个矩阵；
        //   transpose：是否转置。GLM 默认按列主序存储，与 OpenGL 约定匹配，因此为 GL_FALSE；
        //   value    ：矩阵数据首地址，要求指向连续的 float 数据。
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

        // 上传 projection 矩阵的封装写法。
        ourShader.setMat4("projection", projection);

        // VAO 中记录了顶点属性布局，EBO 中记录了索引；绘制时使用上面已经设置好的矩阵。
        //glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i + (float)glfwGetTime();
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteTextures(2, textures);
    glDeleteProgram(ourShader.ID);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
}
