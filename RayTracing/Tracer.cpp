#include "Tracer.h"
#include <chrono>

#define DIM 1500


int main(int argc, char *argv[]){
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return -1;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	GLFWwindow *window = 0;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	window = glfwCreateWindow(DIM, DIM, "CPSC 453 OpenGL Boilerplate", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, KeyCallback);
	glfwMakeContextCurrent(window);

	//Intialize GLAD
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	// call function to load and compile shader programs
	GLuint program = InitializeShaders();
	if (program == 0) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

	// three vertex positions and assocated colours of a triangle
	vector<vec2> vertices = generatePoint();


	// call function to create and fill buffers with geometry data
	Geometry geometry;
	if (!InitializeVAO(&geometry))
		cout << "Program failed to intialize geometry!" << endl;

	if (!LoadGeometry(&geometry, vertices))
		cout << "Failed to load geometry" << endl;


	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(program);
	glBindVertexArray(geometry.vertexArray);
	float spheres[8] = { 
		-1, -0.5, -3.5, 0.5,
		 1, -0.5, -3.5, 0.5
	};
	vec3 origin = vec3(0.0f, 0.0f, 0.0f);
	GLuint spheresLoc = glGetUniformLocation(program, "spheres");
	GLuint originLoc = glGetUniformLocation(program, "origin");
	glUniform4fv(spheresLoc, 2, spheres);
	glUniform3fv(originLoc, 1, glm::value_ptr(origin));

	
	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
		// call function to draw our scene
		glDrawArrays(GL_POINTS, 0, geometry.elementCount);

		// check for an report any OpenGL errors
		CheckGLErrors();

		glfwSwapBuffers(window);

		glfwPollEvents();

		
	}

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// clean up allocated resources before exit
	DestroyGeometry(&geometry);
	glUseProgram(0);
	glDeleteProgram(program);
	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Goodbye!" << endl;
	return 0;

}


//Generate vertex points on the screen;
vector<vec2> generatePoint() {
	vector<vec2> screen = {};
	
	for (int y = 0; y < DIM; y++) {
		for (int x = 0; x < DIM; x++) {
			screen.push_back(vec2((float)y*2 / (float)DIM, (float)x*2 / (float)DIM)  + vec2(-1, -1));
		}
	}
	return screen;

}

//Generate normalized Direction vectors
vector<vec3> generateRays(int d, float fov) {

	float z = d / (2 * tan(fov / 2.0));
	vec3 topLeft = vec3((float)d / -2.0 + 0.5f, (float)d/2.0 - 0.5f, z);


	vector<vec3> dirs = {};
	for (int i = 0; i < d; i++) {
		for (int j = 0; j < d; j++) {
			dirs.push_back(normalize(topLeft + vec3(i, -j, 0)));
		}
	}
	return dirs;
}

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char *description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

// handles keyboard input events
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	bool updateText = false;
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		default:
			break;
		}
		
	}
}

GLuint InitializeShaders() {
	string name = "ray";
	GLuint vertex, fragment;

	string vertexSource = LoadSource("shaders/" + name + ".vert");
	string fragmentSource = LoadSource("shaders/" + name + ".frag");

	if (vertexSource.empty() || fragmentSource.empty()) {
		return 0;
	}

	vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	GLuint program = LinkProgram(vertex, fragment, 0, 0);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	if (CheckGLErrors()) {
		return 0;
	}
	return program;
}