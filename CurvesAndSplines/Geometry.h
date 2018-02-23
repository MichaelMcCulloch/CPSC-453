#pragma once
#include "OGLSupport.h"

using namespace glm;
using namespace std;

struct Geometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  textureBuffer;
	GLuint  colourBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	Geometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
	{}
};

bool InitializeVAO(Geometry *geometry);
bool LoadGeometry(Geometry *geometry, vec2 *vertices, vec3 *colours, int elementCount);
void DestroyGeometry(Geometry *geometry);
void RenderScene(Geometry *geometry, GLuint program);