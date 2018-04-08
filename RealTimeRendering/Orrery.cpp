#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "texture.h"
#include "Camera.h"
#include "Geometry.h"
#include "GLFWSupport.h"

using namespace std;
using namespace glm;

int VIEW_WIDTH = 1280;
int VIEW_HEIGHT = 720;

#define PI_F 3.14159265359f

void RenderScene(Geometry *geometry, GLuint program, vec3 color, Camera* camera, mat4 perspectiveMatrix, GLenum rendermode);

int main(int argc, char *argv[]) {
	GLFWwindow *window = InitializeGLFW();
	GLuint program = InitializeShaders();

	QueryGLVersion();

	if (0 == program || NULL == window) {
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	vec3 vertices[] = {
		vec3(-.6f, -.4f, -0.5f),
		vec3(.0f,  .6f, -0.5f),
		vec3(.6f, -.4f,-0.5f)
	};

	vec3 frustumVertices[] = {
		vec3(-1, -1, -1),
		vec3(-1, -1, 1),
		vec3(-1, 1, 1),
		vec3(1, 1, 1),
		vec3(1, 1, -1),
		vec3(-1, 1, -1),
		vec3(-1, -1, -1),
		vec3(1, -1, -1),
		vec3(1, -1, 1),
		vec3(-1, -1, 1),
		vec3(-1, 1, 1),
		vec3(-1, 1, -1),
		vec3(1, 1, -1),
		vec3(1, -1, -1),
		vec3(1, -1, 1),
		vec3(1, 1, 1)
	};
		
	mat4 perspectiveMatrix = glm::perspective(PI_F*0.4f, float(VIEW_WIDTH) / float(VIEW_HEIGHT), 0.1f, 5.f);	//Fill in with Perspective Matrix
	
	for (int i = 0; i < 16; i++) {
		vec4 newPoint = inverse(perspectiveMatrix)*vec4(frustumVertices[i], 1);
		frustumVertices[i] = vec3(newPoint) / newPoint.w;
	}
	Geometry geometry, frustumGeometry;

	// call function to create and fill buffers with geometry data
	if (!InitializeVAO(&geometry))
		cout << "Program failed to intialize geometry!" << endl;

	if (!LoadGeometry(&geometry, vertices, 3))
		cout << "Failed to load geometry" << endl;

	if (!InitializeVAO(&frustumGeometry))
		cout << "Program failed to intialize geometry!" << endl;

	if (!LoadGeometry(&frustumGeometry, frustumVertices, 16))
		cout << "Failed to load geometry" << endl;

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	Camera cam;

	vec2 lastCursorPos;

	float cursorSensitivity = PI_F / 200.f;	//PI/hundred pixels
	float movementSpeed = 0.001f;

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
		////////////////////////
		//Camera interaction
		////////////////////////
		//Translation
		vec3 movement(0.f);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			movement.z += 1.f;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			movement.z -= 1.f;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			movement.x += 1.f;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			movement.x -= 1.f;

		cam.move(movement*movementSpeed);


		//Rotation
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		vec2 cursorPos(xpos, ypos);
		vec2 cursorChange = cursorPos - lastCursorPos;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			cam.rotateHorizontal(-cursorChange.x*cursorSensitivity);
			cam.rotateVertical(-cursorChange.y*cursorSensitivity);
		}

		lastCursorPos = cursorPos;

		///////////
		//Drawing
		//////////

		// clear screen to a dark grey colour
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// call function to draw our scene
		RenderScene(&geometry, program, vec3(1, 0, 0), &cam, perspectiveMatrix, GL_TRIANGLES);
		RenderScene(&frustumGeometry, program, vec3(0, 1, 0), &cam, perspectiveMatrix, GL_LINE_STRIP);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// clean up allocated resources before exit
	DestroyGeometry(&geometry);
	glUseProgram(0);
	glDeleteProgram(program);
	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Goodbye!" << endl;
	return 0;

}

void RenderScene(Geometry *geometry, GLuint program, vec3 color, Camera* camera, mat4 perspectiveMatrix, GLenum rendermode)
{

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(program);

	/*
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	int VIEW_WIDTH = vp[2];
	int VIEW_HEIGHT = vp[3];
	*/

	//Bind uniforms
	GLint uniformLocation = glGetUniformLocation(program, "Colour");
	glUniform3f(uniformLocation, color.r, color.g, color.b);

	mat4 modelViewProjection = perspectiveMatrix * camera->viewMatrix();
	uniformLocation = glGetUniformLocation(program, "modelViewProjection");
	glUniformMatrix4fv(uniformLocation, 1, false, glm::value_ptr(modelViewProjection));

	glBindVertexArray(geometry->vertexArray);
	glDrawArrays(rendermode, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}



