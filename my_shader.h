#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	GLuint ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();

	void setBool(const std::string& name, bool value) const;

	void setInt(const std::string& name, int value) const;

	void setFloat(const std::string& name, float value) const;

	// 设置 mat4 类型的 uniform 矩阵。
	// name：GLSL 中 uniform 的名称；value：要上传的 4×4 矩阵。
	void setMat4(const std::string& name, const glm::mat4& value) const;
};
#endif
