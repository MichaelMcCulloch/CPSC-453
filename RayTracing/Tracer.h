#pragma once
#include <vector>
#include <iostream>

#include "Geometry.h"

struct Sphere {
	vec4 center;
	vec4 color;
	float radius;
	float pad1;
	float pad2;
	float pad3;
};

struct Plane {
	vec4 norm;
	vec4 point;
	vec4 color;
};

struct Triangle {
	vec4 A;
	vec4 B;
	vec4 C;
	vec4 color;
};

using namespace std;

vector<vec2> generatePoint();

//load shapes into uniform buffer objects
bool LoadShapes(vector<Sphere> spheres, vector<Triangle> triangle, vector<Plane> planes, GLuint program);
//GLFW Callbacks
void ErrorCallback(int error, const char *description);
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

GLuint InitializeShaders();