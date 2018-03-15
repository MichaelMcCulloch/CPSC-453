#pragma once
#include "OGLSupport.h"
#include <vector>

using namespace glm;
using namespace std;

struct Geometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	Geometry() : vertexBuffer(0), vertexArray(0), elementCount(0)
	{}
};

bool InitializeVAO(Geometry *geometry);
bool LoadGeometry(Geometry *geometry, vector<vec2> vertices);
void DestroyGeometry(Geometry *geometry);
void RenderScene(Geometry *geometry, GLuint program);