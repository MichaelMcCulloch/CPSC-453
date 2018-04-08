#pragma once

#include <glm/glm.hpp>
#include "glad\glad.h"
#include "Shader.h"

using namespace std;
using namespace glm;

struct Geometry {
	GLuint vertexBuffer;
	GLuint textureBuffer;
	GLuint colorBuffer;
	GLuint vertexArray;
	GLsizei elementCount;

	Geometry(): vertexBuffer(0), textureBuffer(0), colorBuffer(0), vertexArray(0), elementCount(0){}
};

bool InitializeVAO(Geometry *geometry);
bool LoadGeometry(Geometry *geometry, vec3 *vertices, int elementCount);
void DestroyGeometry(Geometry *geometry);