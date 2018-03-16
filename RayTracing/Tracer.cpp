#include "Tracer.h"

#define DIM 1024

using namespace std;


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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

	//set the origin of our scene, so we can move around in it
	vec3 origin = vec3(0.0f, 0.0f, 0.0f);
	GLuint originLoc = glGetUniformLocation(program, "origin");
	glUniform3fv(originLoc, 1, glm::value_ptr(origin));
	
	//test data
	vector<Triangle> t = {};
	
	Triangle t1 = {
		vec4(2.75, -2.75, -5,0),
		vec4(2.75, -2.75, -10.5,0), 
		vec4(-2.75, -2.75, -10.5,0),
		vec4(1,1,1,0)
	};
	Triangle t2 = {
		vec4(-2.75, -2.75, -5, 0),
		vec4(2.75, -2.75, -5, 0),
		vec4(-2.75, -2.75, -10.5, 0),
		vec4(1,1,1,0)
	};
	t.push_back(t1);
	t.push_back(t2);
	
	vector<Plane> p = {};
	Plane p1 = { vec4(0, 0, 1, 0), vec4(0, 0, -10.5, 0), vec4(0,0,1,0) };
	p.push_back(p1);

	vector<Sphere> s = {};
	Sphere s1 = { vec4(0, -1, -2, 0), vec4(1, 0, 0, 1), 0.5f };
	Sphere s2 = { vec4(0, -1, -3.5, 0), vec4(0, 1, 0, 1), 0.5f };
	s.push_back(s1);
	s.push_back(s2);
	
	LoadShapes(s, t, p, program);
	
	
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

bool LoadShapes(vector<Sphere> spheres, vector<Triangle> triangle, vector<Plane> planes, GLuint program) {
	GLuint sphereUBO, triangleUBO, planeUBO;
	glGenBuffers(1, &sphereUBO);
	glGenBuffers(1, &triangleUBO);
	glGenBuffers(1, &planeUBO);
	
	glBindBuffer(GL_UNIFORM_BUFFER, sphereUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Sphere) * spheres.size(), spheres.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, triangleUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Triangle) * triangle.size(), triangle.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, planeUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Plane) * planes.size(), planes.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, sphereUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, triangleUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, planeUBO);

	GLuint sphereCountIndex = glGetUniformLocation(program, "numSpheres");
	GLuint triangleCountIndex = glGetUniformLocation(program, "numTriangles");
	GLuint planeCountIndex = glGetUniformLocation(program, "numPlanes");
	glUniform1i(sphereCountIndex, spheres.size());
	glUniform1i(triangleCountIndex, triangle.size());
	glUniform1i(planeCountIndex, planes.size());

	return true;
}

//Generate vertex points on the screen;
vector<vec2> generatePoint() {
	vector<vec2> screen = {};
	float d = (float)DIM;
	for (float y = 0; y < d; y++) {
		for (float x = 0; x < d; x++) {
			float yPos = 2 * (y + 0.5) / d;
			float xPos = 2 * (x + 0.5) / d;
			screen.push_back(vec2(xPos, yPos)  + vec2(-1, -1));
		}
	}
	return screen;

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