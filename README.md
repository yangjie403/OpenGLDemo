# OpenGLDemo

这是一个基于 GLFW、GLAD 和 OpenGL 3.3 Core Profile 的 C++ OpenGL 学习项目。

## 环境配置

> 当前项目使用 Visual Studio 2026、C++20 和 x64 平台。

### 第 1 步：准备依赖库

#### 1.1 下载 GLFW（窗口管理库）

1. 打开 GLFW 官网：<https://www.glfw.org/download.html>
2. 下载 **64-bit Windows binaries**，文件名类似 `glfw-3.5.1.bin.WIN64.zip`。
3. 解压到固定目录，例如：

   ```text
   E:\devLibs\glfw\
   ```

解压后需要能够找到：

```text
E:\devLibs\glfw\include\GLFW\glfw3.h
E:\devLibs\glfw\lib-vc2026\glfw3.lib
```

如果压缩包中没有 `lib-vc2026`，可以使用 `lib-vc2022`。Visual Studio 2026 通常可以使用 VS 2022 编译的 GLFW 库。

#### 1.2 生成并下载 GLAD（OpenGL 函数加载器）

可以使用 GLAD 在线生成器：<https://glad.dav1d.de/>

建议选项：

- **Language**：`C/C++`
- **Specification**：`OpenGL`
- **API**：`gl`，选择项目需要的版本；本项目使用 OpenGL 3.3 Core Profile
- **Profile**：`Core`
- **Options**：勾选 `Generate a loader`

生成并下载 `glad.zip`，解压到：

```text
E:\devLibs\glad\
```

关键文件应该存在：

```text
E:\devLibs\glad\include\glad\glad.h
E:\devLibs\glad\src\glad.c
```

GLAD 不仅包含头文件，还包含必须编译进项目的 `glad.c`。

#### 1.3 下载 GLM（OpenGL 数学库）

GLM（OpenGL Mathematics）是一个与 GLSL 风格相近的 C++ 数学库，常用于：

- `glm::vec2`、`glm::vec3`、`glm::vec4` 等向量
- `glm::mat3`、`glm::mat4` 等矩阵
- 模型变换：平移、旋转、缩放
- 摄像机观察矩阵
- 透视投影和正交投影
- 点积、叉积、归一化、角度转换等数学运算

GLM 是**纯头文件库**。它不需要编译 `.cpp` 文件，不需要 `.lib` 文件，也不需要添加链接器依赖项。

下载地址：

- GitHub：<https://github.com/g-truc/glm>
- Releases：<https://github.com/g-truc/glm/releases>

将源码压缩包解压到：

```text
E:\devLibs\glm\
```

目录结构必须类似下面这样：

```text
E:\devLibs\glm\glm\glm.hpp
E:\devLibs\glm\glm\gtc\matrix_transform.hpp
E:\devLibs\glm\glm\gtc\type_ptr.hpp
```

这里要特别注意目录层级：Visual Studio 中应添加 `E:\devLibs\glm`，而不是 `E:\devLibs\glm\glm`。
因为源代码使用的是：

```cpp
#include <glm/glm.hpp>
```

如果添加了 `E:\devLibs\glm\glm`，编译器会尝试查找：

```text
E:\devLibs\glm\glm\glm\glm.hpp
```

这样就会出现“无法打开包括文件 `glm/glm.hpp`”的错误。

---

### 第 2 步：创建或打开项目

1. 使用 Visual Studio 打开 `OpenGLDemo.slnx`。
2. 在顶部工具栏将解决方案平台切换为 **x64**。
3. 右键项目 `OpenGLDemo` → **属性**。
4. 配置窗口左上角选择：
   - **配置**：`所有配置`
   - **平台**：`x64`

本项目已经包含 GLFW 和 GLAD 的基础配置；如果需要使用 GLM，只需按照本文配置包含目录即可。若从头创建项目，必须确保 `glad.c` 被添加到“源文件”中。

---

### 第 3 步：配置 Visual Studio 项目属性

#### 3.1 配置附加包含目录

打开：

```text
项目属性 → C/C++ → 常规 → 附加包含目录
```

添加以下目录：

```text
E:\devLibs\glfw\include
E:\devLibs\glad\include
E:\devLibs\glm
```

其中：

- GLFW 的包含目录中应有 `GLFW\glfw3.h`
- GLAD 的包含目录中应有 `glad\glad.h`
- GLM 的包含目录中应有 `glm\glm.hpp`

GLM 只需要配置包含目录，不要将 GLM 目录添加到“库目录”或“附加依赖项”。

如果直接编辑 `.vcxproj`，x64 配置可以类似下面这样设置：

```xml
<IncludePath>
    E:\devLibs\glfw\include;
    E:\devLibs\glad\include;
    E:\devLibs\glm;
    $(IncludePath)
</IncludePath>
```

#### 3.2 配置库目录

打开：

```text
项目属性 → VC++ 目录 → 库目录
```

添加 GLFW 的库目录：

```text
E:\devLibs\glfw\lib-vc2026
```

GLM 不需要在这里添加任何目录。

> **关于 VS 2026 的特别说明**：如果 GLFW 解压后没有 `lib-vc2026` 文件夹，选择 `lib-vc2022` 即可。Visual Studio 的二进制接口（ABI）向后兼容，2022 版本的库在 2026 中可以正常使用。

#### 3.3 配置链接器依赖项

打开：

```text
项目属性 → 链接器 → 输入 → 附加依赖项
```

添加：

```text
glfw3.lib
opengl32.lib
user32.lib
gdi32.lib
shell32.lib
ole32.lib
uuid.lib
winmm.lib
```

GLM 不需要添加 `glm.lib`。GLM 没有必须链接的库文件。

#### 3.4 检查 GLFW 的预处理器定义

本项目使用静态库 `glfw3.lib`，因此通常不需要 `GLFW_DLL` 宏。

打开：

```text
项目属性 → C/C++ → 预处理器 → 预处理器定义
```

确认没有误加 `GLFW_DLL`。如果链接的是 GLFW 动态库，则配置方式不同，必须按照对应版本的 GLFW 文档配置。

---

### 第 4 步：将 GLAD 源文件添加到项目

在解决方案资源管理器中：

1. 右键“源文件”文件夹。
2. 选择 **添加 → 现有项**。
3. 选择：

   ```text
   E:\devLibs\glad\src\glad.c
   ```

4. 重新生成项目。

如果忘记添加 `glad.c`，通常会出现以下链接错误：

```text
无法解析的外部符号 gladLoadGLLoader
无法解析的外部符号 glad_glCreateShader
无法解析的外部符号 glad_glGenBuffers
```

GLM 与 GLAD 不同：GLM 没有类似 `glad.c` 的源文件，不要尝试把 GLM 的头文件添加为 `.cpp` 文件编译。

### 第 5 步：编译和运行验证

按 `Ctrl + Shift + B` 生成项目，然后按 `Ctrl + F5` 运行。

验证 GLM 配置至少需要满足：

1. `E:\devLibs\glm\glm\glm.hpp` 文件存在。
2. `E:\devLibs\glm` 已添加到“C/C++ → 常规 → 附加包含目录”。
3. 项目可以正常编译，不需要添加任何 `glm.lib`。
4. GLFW、GLAD 和 OpenGL 的原有配置没有被 GLM 配置覆盖。

本 README 只说明 GLM 的下载和项目配置，不展开 GLM API 或具体使用方式。

---

## 快速故障排查

| 报错现象 | 解决方法 |
| :--- | :--- |
| `无法打开包括文件 glm/glm.hpp` | 将 `E:\devLibs\glm` 添加到“C/C++ → 常规 → 附加包含目录”，不要添加 `E:\devLibs\glm\glm` |
| `无法打开包括文件 glad/glad.h` | 检查是否添加了 `E:\devLibs\glad\include` |
| 出现 `glm.lib` 或 GLM 链接错误 | GLM 是纯头文件库，不需要 `.lib`，检查是否误把 GLM 配置到了链接器依赖项 |
| `无法解析的外部符号 gladLoadGLLoader` | 检查是否把 `E:\devLibs\glad\src\glad.c` 添加到了项目 |
| `无法解析的外部符号 glfwInit` | 检查 GLFW 库目录、`glfw3.lib` 和 x64 平台是否配置正确 |
| 运行时调用 OpenGL 崩溃 | 确认顺序为：`glfwCreateWindow` → `glfwMakeContextCurrent` → `gladLoadGLLoader` → 其他 `gl*` 调用 |
| 大量 `__imp_` 开头的错误 | 检查 GLFW 静态库配置，以及 `user32.lib`、`gdi32.lib` 等系统库是否添加 |
| 窗口一闪而过 | 在控制台查看 Shader 编译日志、纹理路径和 OpenGL 初始化错误；确认程序运行工作目录包含 `shaders` 和 `assets` |
