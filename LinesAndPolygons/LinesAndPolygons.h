#pragma once
// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Specify that we want the OpenGL core profile before including GLFW headers
#ifndef LAB_LINUX
#include <glad/glad.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace std;
using namespace glm;
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

struct MyShader
{
	// OpenGL names for vertex and fragment shaders, shader program
	GLuint vertex;
	GLuint fragment;
	GLuint program;

	// initialize shader and program names to zero (OpenGL reserved value)
	MyShader() : vertex(0), fragment(0), program(0)
	{
	}
};

// load, compile, and link shaders, returning true if successful
bool InitializeShaders(MyShader *shader);

// deallocate shader-related objects
void DestroyShaders(MyShader *shader);

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data

struct MyGeometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint vertexBuffer;
	GLuint textureBuffer;
	GLuint colourBuffer;
	GLuint vertexArray;
	GLsizei elementCount;
	GLuint chunkSize;
	GLenum mode;

	// initialize object names to zero (OpenGL reserved value)
	MyGeometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
	{
	}
};

bool InitializeVAO(MyGeometry *geometry);

// create buffers and fill with geometry data, returning true if successful
bool LoadGeometry(MyGeometry *geometry, vec2 *vertices, vec3 *colours, int elementCount);
// deallocate geometry-related objects
void DestroyGeometry(MyGeometry *geometry);

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderScene(MyGeometry *geometry, MyShader *shader);

MyGeometry square;
MyGeometry spiral;
MyGeometry sierpinskiTriangle;
MyGeometry sponge;
MyGeometry koshSnowflake;
MyGeometry barnsleyFern;
MyGeometry *toRender;

enum TargetType
{
	Squares,
	Spiral,
	SierpinskiTriangle,
	Sponge,
	KoshSnowflake,
	BarnsleyFern
};

TargetType currentShape;

int numberKeyPressed(int key);
// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char *description);

// handles keyboard input events
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);



void createSquares(int iteration);
void createSpiral(int revolutions);
void createSierpinskiTriangle(int depth);
void createSponge(int depth);
void createKoshSnowflake(int depth);
pair<vector<vec2>, vector<vec3>> crystalize(int depth);
mat2x2 makeRotationMatrix(float theta);
void createBarnsleyFern(int depth);
pair<vector<vec2>, vector<vec3>> sierpinskiRecurse(pair<vector<vec2>, vector<vec3>> initialPointsAndColors, int depth);
pair<vector<vec2>, vector<vec3>> spongeRecurse(pair<vector<vec2>, vector<vec3>> initialPointsAndColors, int depth);

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion();

bool CheckGLErrors();

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename);

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source);

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
