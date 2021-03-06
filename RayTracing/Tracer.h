#pragma once
#include <vector>
#include <iostream>

#include "Geometry.h"
#include "imagebuffer.h"
struct Sphere {
	vec4 center;
	vec4 diffuseColor;
	vec4 specularColor;
	float phongExp;
	float radius;
	float reflectance;
	float pad2; //align to 4-float;
};

struct Plane {
	vec4 norm;
	vec4 point;
	vec4 diffuseColor;
	vec4 specularColor;
	float phongExp;
	float reflectance; 
	float pad2;//align to 4-float;
	float pad3;

};

struct Triangle {
	vec4 A;
	vec4 B;
	vec4 C;
	vec4 diffuseColor;
	vec4 specularColor;
	float phongExp;
	float reflectance; 
	float pad2;//align to 4-float;
	float pad3;
};

struct Light {
	vec4 center;
	vec4 color;
	float radius; //radius = 0 -> point light source
	float intensity;
	float padd01;
	float padd02;
};

using namespace std;


//Prepare scenes for the buffer
void LoadScene1(GLuint program);
void LoadScene2(GLuint program);
void LoadScene3(GLuint program);

//load shapes into uniform buffer objects
void LoadShapes(vector<Sphere> spheres, vector<Triangle> triangle, vector<Plane> planes, vector<Light> lights, GLuint program);
//GLFW Callbacks
void ErrorCallback(int error, const char *description);
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);
void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

GLuint InitializeShaders();