#pragma once
#include <vector>
#include <iostream>

#include "Geometry.h"
#

using namespace std;

vector<vec2> generatePoint();

//GLFW Callbacks
void ErrorCallback(int error, const char *description);
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

GLuint InitializeShaders();