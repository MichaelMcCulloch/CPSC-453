#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc/matrix_transform.hpp>
#include <glm\gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb\stb_image.h"

using namespace std;

struct Geometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  textureBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	Geometry() : vertexBuffer(0), textureBuffer(0), vertexArray(0), elementCount(0)
	{}
};

struct MyTexture
{
	GLuint textureID;
	GLuint target;
	int width;
	int height;

	MyTexture() : textureID(0), target(0) {};
};

//Prepare a window and setup callbacks
GLFWwindow* InitializeGLFW();
bool InitializeVAO(Geometry *geometry);
bool LoadGeometry(Geometry *geometry, vector<float> vertices, vector<float> texCoords);
void DestroyGeometry(Geometry *geometry);

bool InitializeTexture(MyTexture* texture, const char* filename, GLuint target = GL_TEXTURE_2D);
void DestroyTexture(MyTexture *texture);

void configureTexture(const char* fileName, Geometry *geo, MyTexture *tex);

// resize the ogl viewpoint when the user resizes the window  
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//capture keys during runtime
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void ErrorCallback(int error, const char* description);
void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);
void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);


//OpenGL Utility and support functions
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
GLuint InitializeShaders();
bool CheckGLErrors();