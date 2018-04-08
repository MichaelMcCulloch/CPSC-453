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

extern int VIEW_WIDTH;
extern int VIEW_HEIGHT;

//GLFW callbacks and support functions
GLFWwindow* InitializeGLFW();
void ErrorCallback(int error, const char *description);
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int modifiers);
void WindowSizeCallback(GLFWwindow *window, int w, int h);
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);