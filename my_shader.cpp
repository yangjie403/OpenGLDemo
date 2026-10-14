#include "my_shader.h"
#include "mylog.h"

#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	} catch (std::ifstream::failure& e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	GLuint vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(vertex);
	glCompileShader(fragment);
	printShaderLog(vertex);
	printShaderLog(fragment);

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	printProgramLog(ID);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
	// glGetUniformLocation：根据 uniform 名称获取它在当前 shader program 中的位置。
	// glUniformMatrix4fv 的参数含义：
	//   location：uniform 的位置；
	//   1       ：要上传的矩阵数量，这里只上传一个矩阵；
	//   GL_FALSE ：不转置矩阵。GLM 默认按列主序存储，与 OpenGL 的矩阵布局匹配；
	//   value_ptr(value)：取得 glm::mat4 底层连续 float 数据的首地址。
	glUniformMatrix4fv(
		glGetUniformLocation(ID, name.c_str()),
		1,
		GL_FALSE,
		glm::value_ptr(value)
	);
}
