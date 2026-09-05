## 环境配置

> Visual Studio 版本：**2026**

### 第 1 步：下载依赖库文件

#### 1.1 下载 GLFW（窗口管理库）
- 官网：https://www.glfw.org/download.html
- 在“Windows 预编译二进制文件”中，下载 **64-bit Windows binaries**（文件名类似 `glfw-3.5.1.bin.WIN64.zip`）。
- 解压到固定目录，例如 `D:\DevLibs\glfw-3.5.1.bin.WIN64`

#### 1.2 生成并下载 GLAD（OpenGL 加载器）
- 在线服务：https://glad.dav1d.de/
- 按如下选项设置：
  - **Language**：`C/C++`
  - **Specification**：`OpenGL`
  - **API**：选择 `gl` 版本（推荐 `4.6` 或更高，兼容你的显卡）
  - **Profile**：务必勾选 **`Core`**
  - **Options**：勾选 **`Generate a loader`**
- 点击 **GENERATE**，下载生成的 `glad.zip`
- 解压到固定目录，例如 `D:\DevLibs\glad`

> 解压后，关键文件结构如下：
> - GLFW：`D:\DevLibs\glfw-3.5.1.bin.WIN64\include\GLFW\glfw3.h` 和 `lib-vc2026\glfw3.lib`
> - GLAD：`D:\DevLibs\glad\include\glad\glad.h` 和 `D:\DevLibs\glad\src\glad.c`

---

### 第 2 步：在 Visual Studio 中创建项目

1. 打开 VS 2026，创建 **“C++ 空项目”**，命名为 `OpenGLDemo`。
2. **【关键】在顶部工具栏，将“解决方案平台”从 `x86` 切换为 `x64`**（必须与下载的 64 位库匹配）。

---

### 第 3 步：配置项目属性（核心步骤）

右键点击项目 → 选择 **“属性”**。

在弹出窗口中，确保左上角的 **“配置”** 为 **“所有配置”**，**“平台”** 为 **`x64`**。

#### 3.1 配置包含目录（让编译器找到头文件）
- 左侧菜单：**VC++ 目录** → **包含目录** → 点击编辑（下拉箭头 → 编辑）
- 点击黄色文件夹图标（新建），添加以下两条路径：
  ```
  D:\DevLibs\glfw-3.5.1.bin.WIN64\include
  D:\DevLibs\glad\include
  ```
- 点击“确定”

#### 3.2 配置库目录（让链接器找到 .lib 文件）
- 仍在 **VC++ 目录** 页面 → **库目录** → 点击编辑
- 点击新建，添加 GLFW 的库文件夹路径：
  ```
  D:\DevLibs\glfw-3.5.1.bin.WIN64\lib-vc2026
  ```
  > **关于 VS 2026 的特别说明**：如果 GLFW 解压后没有 `lib-vc2026` 文件夹，选择 `lib-vc2022` 即可。Visual Studio 的二进制接口（ABI）向后兼容，2022 版本的库在 2026 中可以正常使用。
- 点击“确定”

#### 3.3 配置链接器依赖项（告诉链接器具体要链接哪些库）
- 左侧菜单：**链接器** → **输入** → **附加依赖项** → 点击编辑
- 在输入框中，**逐行添加**以下库文件名（注意不要有空格或分号，每行一个）：

  ```
  glfw3.lib
  opengl32.lib
  user32.lib
  gdi32.lib
  shell32.lib
  ole32.lib
  uuid.lib
  winmm.lib
  ```

  > **解释**：`glfw3.lib` 是 GLFW 静态库；后面的 7 个是 Windows 系统底层库，用于处理窗口、消息、图形设备接口等，缺少它们会导致上一轮中大量的 `__imp_` 链接错误。

- 点击“确定”，再点击“确定”关闭项目属性页。

---

### 第 4 步：将 GLAD 的源文件添加到项目中（极易遗漏！）

GLAD 不是一个纯头文件库，它附带了一个 **`glad.c`** 源文件，必须编译进你的项目。

- 在“解决方案资源管理器”中，右键点击 **“源文件”** 文件夹 → **“添加”** → **“现有项”**
- 浏览到 `D:\DevLibs\glad\src\`，选中 **`glad.c`**，点击“添加”

> 此时，`glad.c` 会出现在源文件列表里。没有这一步，`gladLoadGLLoader` 和所有 `glad_xxx` 符号都会报 LNK2019 错误。

---

### 第 5 步：检查预处理器定义（避免动态库冲突）

因为我们使用的是 `glfw3.lib`（静态库），**不需要**定义 `GLFW_DLL` 宏。

- 项目属性 → **C/C++** → **预处理器** → **预处理器定义**
- 确保列表中 **没有** `GLFW_DLL`。如果有，请删除它。

---

### 第 6 步：编写并运行验证代码

在“源文件”上右键 → “添加” → “新建项” → 选择 **“C++ 文件(.cpp)”**，命名为 `main.cpp`。  
将您最终提供的代码完整粘贴进去（我已确认其顺序完全正确）：

```cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ★ 关键：在激活上下文之后，在调用任何 gl 函数之前加载 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
```

最后，按 **`Ctrl + F5`**（或“本地 Windows 调试器”）运行。如果看到一个 800x600 的深青色窗口，恭喜你，环境配置完全成功！🎉

---

### 快速故障排查清单（如果还有问题）

| 报错现象                              | 解决方法                                                     |
| :------------------------------------ | :----------------------------------------------------------- |
| `无法打开 源 文件 glad/glad.h`        | 检查“包含目录”是否添加了 `glad\include` 文件夹               |
| `无法解析的外部符号 gladLoadGLLoader` | 检查是否忘记将 `glad.c` 添加到项目源文件中                   |
| `无法解析的外部符号 glfwInit` 等      | 检查“库目录”是否指向了正确的 `lib-vc2026` 文件夹，且平台是 x64 |
| 大量的 `__imp_` 开头错误              | 检查“附加依赖项”是否漏掉了 `user32.lib`、`gdi32.lib` 等系统库 |
| 窗口一闪而过或崩溃                    | 检查显卡驱动是否支持 OpenGL 3.3+，或尝试将 `glfwWindowHint` 中的版本改为 `3.0` |

这份流程已经包含了我们所有调试过程得出的经验，按此操作，一步到位！