#include "main10.h"

#include "my_shader.h"
#include "std_image.h"

namespace {
    // 每一帧都调用的输入处理函数。
    // 这里和纹理没有直接关系，但它说明了 GLFW 窗口事件通常放在渲染循环中处理。
    void processInput(GLFWwindow* window) {
        // glfwGetKey(window, key)：查询指定窗口中某个按键的当前状态。
        //   window：要查询的 GLFW 窗口。
        //   key   ：按键枚举值，这里是 ESC 键。
        // 返回 GLFW_PRESS、GLFW_RELEASE 等状态值。
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            // 将窗口的“应关闭”标志设为 true；循环条件检测到它后退出。
            glfwSetWindowShouldClose(window, true);
        }
    }

    // 窗口大小发生变化时，通知 OpenGL 新的视口尺寸。
    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        // glViewport(x, y, width, height)：指定 OpenGL 将 NDC 坐标映射到窗口的区域。
        //   x、y       ：视口左下角，相对于 framebuffer 的像素坐标。
        //   width/height：视口宽高，通常使用 framebuffer 的实际像素尺寸。
        // 注意：window 参数在这里未使用；保留它是因为 GLFW 回调函数签名要求如此。
        glViewport(0, 0, width, height);
    }

    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 800;
}

void main10() {
    // 初始化 GLFW。它负责创建窗口、创建 OpenGL 上下文以及处理窗口事件。
    glfwInit();
    // 请求 OpenGL 3.3 Core Profile。纹理 API（如 glTexImage2D）属于 OpenGL 的基础能力，
    // 但 VAO、shader 等对象的可用性与上下文版本/配置有关。
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfwCreateWindow(width, height, title, monitor, share)：创建 GLFW 窗口。
    //   width/height：窗口初始尺寸，单位通常是屏幕坐标而不是 framebuffer 像素。
    //   title       ：窗口标题。
    //   monitor     ：指定非 nullptr 可创建全屏窗口；nullptr 表示普通窗口。
    //   share       ：指定另一个窗口可共享 OpenGL 对象；nullptr 表示不共享。
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    // 让后续 OpenGL 调用作用于这个窗口对应的上下文。
    glfwMakeContextCurrent(window);
    // 当 framebuffer 尺寸变化时调用回调，避免绘制区域仍使用旧尺寸。
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // GLAD 通过 GLFW 提供的地址查询函数加载当前驱动支持的 OpenGL 函数指针。
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Shader 构造函数通常会：读取顶点/片段着色器源码、编译、链接，并把链接后的
    // program 对象 ID 保存在 shader.ID 中。纹理最终由 shader10.fs 中的 sampler2D 采样。
    Shader shader("shaders/shader10.vs", "shaders/shader10.fs");

    // 每个顶点共有 8 个 float，内存布局为：
    //   [位置 x,y,z][颜色 r,g,b][纹理坐标 u,v]
    // 纹理坐标通常称为 UV，u 对应水平方向，v 对应垂直方向；0~1 表示整张纹理。
    float vertices[] = {
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  2.0f,  2.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  2.0f, -1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f,  2.0f
    };
    //float vertices[] = {
    //    // positions          // colors           // texture coords (note that we changed them to 'zoom in' on our texture image)
    //     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.55f, 0.55f, // top right
    //     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.55f, 0.45f, // bottom right
    //    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.45f, 0.45f, // bottom left
    //    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.45f, 0.55f  // top left 
    //};
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int VBO, VAO, EBO;
    // glGenVertexArrays(n, arrays)：创建 n 个 VAO 名称并写入 arrays。
    // glGenBuffers(n, buffers)：创建 n 个 buffer 名称并写入 buffers。
    //   n       ：要创建的对象数量。
    //   arrays/buffers：接收生成对象 ID 的数组首地址。
    // 这里 VAO 记录顶点属性格式以及 EBO 的绑定关系；VBO/EBO 保存实际顶点/索引数据。
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // glBindVertexArray(VAO)：把 VAO 设为当前 VAO；之后的顶点属性设置会记录到它里面。
    glBindVertexArray(VAO);
    // glBindBuffer(target, buffer)：把 buffer 绑定到指定目标。
    //   target：这里 GL_ARRAY_BUFFER 表示“顶点属性数据”目标。
    //   buffer：要绑定的 VBO ID；绑定后，后续针对该 target 的操作作用于它。
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(target, size, data, usage)：向当前 target 对应的 buffer 分配/上传数据。
    //   target：必须与当前绑定目标一致，这里是 GL_ARRAY_BUFFER。
    //   size  ：要复制的字节数；sizeof(vertices) 已经包含整个数组大小。
    //   data  ：CPU 内存首地址；nullptr 表示只分配显存、不立即上传内容。
    //   usage ：使用提示。GL_STATIC_DRAW 表示数据上传后很少改变、会被频繁绘制。
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 绑定 EBO。注意：在 VAO 已绑定时，GL_ELEMENT_ARRAY_BUFFER 的绑定会记录到 VAO。
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // 将索引数组上传到 EBO；glDrawElements 会按这些索引访问 VBO 中的顶点。
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // glVertexAttribPointer(index, size, type, normalized, stride, pointer)：描述 VBO 中
    // 某个顶点属性的内存布局，但不会自动启用该属性。
    //   index     ：shader 中 layout(location=0) 对应的属性槽位。
    //   size      ：每个顶点该属性包含的分量数；位置是 vec3，所以为 3。
    //   type      ：每个分量在 VBO 中的类型，这里是 GL_FLOAT。
    //   normalized：整数类型数据是否映射到 [0,1] 或 [-1,1]；浮点数据通常填 GL_FALSE。
    //   stride    ：从一个顶点的该属性起点跳到下一个顶点同属性起点的字节数。
    //   pointer   ：当前顶点起点到该属性第一个分量的字节偏移；这里位置在起点。
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(index)：启用指定属性槽位，否则 shader 读取的是默认值。
    glEnableVertexAttribArray(0);

    // 位置占 3 个 float，因此颜色从第 3 个 float 开始，即 3*sizeof(float) 字节处。
    // aColor 在 shader10.vs 中声明为 layout(location=1) in vec3。
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 纹理坐标从第 6 个 float 开始，即 6*sizeof(float) 字节处，且每个坐标只有 u、v 两个分量。
    // 这里 size 应为 2，与 shader 中的 `in vec2 aTexCoord` 一致；第三个分量没有意义，
    // 也会使布局描述与真实数据不匹配。
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // ================================================================
    // 纹理使用流程：
    // 1. 创建纹理对象（glGenTextures）
    // 2. 绑定纹理对象（glBindTexture）
    // 3. 设置环绕和过滤参数（glTexParameteri）
    // 4. 通过图片加载库读入 CPU 像素数据（stbi_load）
    // 5. 将像素上传到 GPU（glTexImage2D）
    // 6. 生成 mipmap（glGenerateMipmap）
    // 7. 释放 CPU 图片数据（stbi_image_free）
    // 8. 绘制时将纹理绑定到纹理单元，并让 sampler 指向对应纹理单元。

    GLuint texture[2];
    // glGenTextures(n, textures)：创建 n 个纹理对象名称。
    //   n       ：要创建的纹理对象数量，这里是 2。
    //   textures：接收纹理 ID 的数组首地址；texture[0] 和 texture[1] 会得到不同 ID。
    // 这里只是取得句柄，还没有给纹理指定尺寸、像素内容或采样参数。
    glGenTextures(2, texture);

    // glBindTexture(target, texture)：将纹理对象绑定到当前纹理目标。
    //   target ：纹理类型，这里 GL_TEXTURE_2D 表示二维纹理。
    //   texture：纹理对象 ID；之后 glTexParameteri/glTexImage2D 等调用会操作它。
    // 绑定是 OpenGL 的“当前对象”机制：很多 API 不传对象 ID，而是作用于当前绑定对象。
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    // glTexParameteri(target, pname, param)：设置当前绑定纹理的整数采样参数。
    //   target：纹理目标，必须与绑定时的目标一致，这里是 GL_TEXTURE_2D。
    //   pname ：要设置的参数名称。
    //   param ：参数值；具体合法值取决于 pname。
    // GL_TEXTURE_WRAP_S：设置 S（也就是 u/水平方向）坐标超出 [0,1] 时如何处理。
    // GL_REPEAT：重复纹理，例如 1.2 会回到 0.2；需要超过 1 的纹理坐标才明显。
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    // GL_TEXTURE_WRAP_T：设置 T（也就是 v/垂直方向）坐标超出 [0,1] 时如何处理。
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // GL_TEXTURE_MIN_FILTER：纹理被缩小时使用的过滤方式（一个纹素覆盖多个屏幕像素）。
    // GL_LINEAR：对相邻纹素做线性插值，画面比 GL_NEAREST 平滑，但可能略微模糊。
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // GL_TEXTURE_MAG_FILTER：纹理被放大时使用的过滤方式（一个纹素覆盖多个屏幕像素）。
    // 注意：放大过滤不能使用 mipmap 枚举；常用 GL_LINEAR 或 GL_NEAREST。
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // stb_image 将图片解码到 CPU 内存；width/height/nrChannels 用于接收图片信息。
    // width、height：图片宽高（像素）。nrChannels：文件原本的通道数，如 3 或 4。
    int width, height, nrChannels;
    // stb_image 默认图片原点在左上角，而 OpenGL 纹理坐标通常从左下角开始。
    // 传 true 后，stbi_load 会在返回前垂直翻转像素行，使图片方向与 OpenGL 坐标更匹配。
    // 这是 stb_image 的全局设置，会影响之后的加载；也可使用线程版本设置来隔离线程状态。
    stbi_set_flip_vertically_on_load(true);
    // stbi_load(filename, x, y, channels_in_file, desired_channels)：加载图片并返回像素指针。
    //   filename          ：图片路径；相对路径相对于程序运行时的工作目录，而不是源码目录。
    //   x、y              ：输出图片宽度和高度的指针，单位是像素。
    //   channels_in_file  ：输出图片原始通道数；这里写入 nrChannels。
    //   desired_channels  ：希望返回的通道数；0 表示保留图片原始通道数，不强制转换。
    // 成功返回 unsigned char*，失败返回 nullptr；每个通道通常是 8 位无符号值。
    unsigned char* data = stbi_load("assets/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        // glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels)：
        // 将一张二维图片上传到当前绑定的 GL_TEXTURE_2D。
        //   target      ：纹理目标，GL_TEXTURE_2D。
        //   level       ：mipmap 层级；0 是原始/最大尺寸层，1、2... 是更小的层。
        //   internalformat：GPU 内部存储格式；GL_RGB 表示存成 RGB 三通道。
        //   width/height：本次上传图像的宽高，来自 stbi_load 的输出。
        //   border      ：历史遗留边框参数，OpenGL 3.3 中必须为 0。
        //   format      ：CPU 端 pixels 的通道排列格式；这里 data 是 RGB 排列。
        //   type        ：CPU 端每个通道的数据类型；stb_image 默认是 unsigned char。
        //   pixels      ：CPU 像素数据首地址，即 data。
        // internalformat 描述 GPU 如何存储，format/type 描述输入数据如何解释；两者不要混淆。
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // glGenerateMipmap(target)：根据 level 0 自动生成后续 mipmap 层级。
        //   target：要生成 mipmap 的纹理目标；必须是当前已绑定并已上传 level 0 的纹理。
        // mipmap 用于纹理缩小时选择合适分辨率，减少闪烁和摩尔纹；本例的 MIN_FILTER 是
        // GL_LINEAR，因此运行时实际上不会采样 mipmap，但生成它仍是常见的完整流程示例。
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    // stb_image 分配的 CPU 内存不再需要；纹理数据已经由 glTexImage2D 复制到 GPU。
    // 这个释放不会删除 GPU 纹理对象，GPU 对象要在最后用 glDeleteTextures 删除。
    stbi_image_free(data);

    // 开始处理第二张纹理。绑定新 ID 后，下面的参数和像素上传不会影响 texture[0]。
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 复用前面声明的 width、height、nrChannels；stbi_load 会覆盖它们的输出值。
    data = stbi_load("assets/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data) {
        // PNG 通常包含 RGBA 四个通道，所以这里的内部格式和输入格式都使用 GL_RGBA。
        // 必须保证 format 与 data 的实际排列一致，否则颜色/透明度会错位。
        // 更通用的写法是根据 nrChannels 选择 GL_RGB 或 GL_RGBA，或在 stbi_load 中指定
        // desired_channels=4 统一转换为 RGBA。
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // 将 shader program 设为当前 program。glUniform 类 API 修改的是“当前 program”中的 uniform。
    shader.use();
    // shader.setInt(name, value) 通常内部调用 glGetUniformLocation + glUniform1i：
    //   name ：shader 中 uniform 的名字，必须与 shader10.fs 中的 sampler 名相同。
    //   value：纹理单元编号，不是 texture[0]/texture[1] 这样的纹理对象 ID。
    // sampler2D uniform 本身只是一个“从哪个纹理单元取样”的整数入口；它不保存图片数据。
    // 这里约定 texture1 -> 纹理单元 0，texture2 -> 纹理单元 1。
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

    // glActiveTexture(texture)：选择当前操作的纹理单元。
    //   texture：纹理单元枚举，如 GL_TEXTURE0、GL_TEXTURE1；不是纹理对象 ID。
    // 每个纹理单元都有独立的 GL_TEXTURE_2D 绑定槽位。激活 GL_TEXTURE0 后绑定 texture[0]，
    // 激活 GL_TEXTURE1 后绑定 texture[1]，这样 fragment shader 才能分别取样两张图。
    glActiveTexture(GL_TEXTURE0);
    // glBindTexture 现在会把 texture[0] 绑定到“当前激活纹理单元”的 GL_TEXTURE_2D 槽位。
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);

    float mixValue = 0.2;
    // 主循环：每帧清屏、绑定纹理和 VAO、执行绘制，再交换前后缓冲区。
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            mixValue += 0.0001;
        } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            mixValue -= 0.0001;
        }
        if (mixValue > 1) {
            mixValue = 1.0;
        } else if(mixValue < 0) {
            mixValue = 0.0;
        }

        // glClearColor(r, g, b, a)：设置 glClear 使用的背景颜色（只是设置状态，不会立即清屏）。
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(mask)：清除指定 framebuffer 缓冲区；GL_COLOR_BUFFER_BIT 表示颜色缓冲区。
        glClear(GL_COLOR_BUFFER_BIT);

        // 再次绑定 shader program，确保后面的绘制使用正确的 program。
        shader.use();
        // 绑定 VAO，恢复其中记录的顶点属性布局、VBO/EBO 关系。
        glBindVertexArray(VAO);

        shader.setFloat("mixValue", mixValue);

        // glDrawElements(mode, count, type, indices)：使用 EBO 中的索引进行绘制。
        //   mode   ：图元类型；GL_TRIANGLES 每三个索引组成一个三角形。
        //   count  ：要读取的索引数量，这里 6 个索引组成 2 个三角形。
        //   type   ：EBO 中每个索引的数据类型；indices 数组是 unsigned int，所以为 GL_UNSIGNED_INT。
        //   indices：索引在 EBO 中的字节偏移；nullptr/0 表示从 EBO 开头读取。
        // 在 core profile 下，绑定 EBO 后不能把 CPU 指针当作真实地址传入；这里的 nullptr
        // 会被解释为 EBO 内的偏移 0。
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // 双缓冲：把后备缓冲区显示出来，避免直接在屏幕上看到绘制中的中间状态。
        glfwSwapBuffers(window);
        // 处理窗口、键盘等事件，并更新 GLFW 内部状态。
        glfwPollEvents();
    }

    // 释放 OpenGL 资源。glDeleteTextures(n, textures) 的 n 是纹理数量，textures 是其 ID 数组。
    // 纹理对象不属于 VAO/VBO/EBO，必须单独删除。
    glDeleteTextures(2, texture);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader.ID);

    glfwDestroyWindow(window);
    glfwTerminate();
}
