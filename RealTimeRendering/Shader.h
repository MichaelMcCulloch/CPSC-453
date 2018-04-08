#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;
using namespace glm;
//Shader support Functions
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertShader, GLuint fragShader);
GLuint LinkProgram(GLuint vertShader, GLuint fragShader, GLuint tescShader, GLuint teseShader);
GLuint InitializeShaders();
bool CheckGLErrors();
void QueryGLVersion();