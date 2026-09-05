#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void printShaderLog(GLuint shader);

void printProgramLog(GLuint prog);

bool checkOpenGLError();