#include "Tracer.h"

#define DIM 1024

using namespace std;
using namespace glm;

ImageBuffer *ib;
GLuint program;

glm::mat4 trans;
glm::mat4 oTrans;


glm::vec3 cameraPosition;
float pitchAmt = 0, yawAmt = 0;
float xOld = 0, yOld = 0;

bool leftPressed = false;

int main(int argc, char *argv[])
{
	// initialize the GLFW windowing system
	if (!glfwInit())
	{
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
	if (!window)
	{
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
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
	program = InitializeShaders();
	if (program == 0)
	{
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

	// three vertex positions and assocated colours of a triangle
	vector<vec2> vertices = {
		vec2(-1, -1),
		vec2(-1, 1),
		vec2(1, -1),
		vec2(1,1)

	};


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
	vec3 origin = vec3(0.0f, 2.0f, 5.0f);
	GLuint originLoc = glGetUniformLocation(program, "cameraOrigin");
	GLuint fov = glGetUniformLocation(program, "fov");
	GLuint transform = glGetUniformLocation(program, "transform");
	GLuint oTransform = glGetUniformLocation(program, "oTransform");
	
	glUniform3fv(originLoc, 1, glm::value_ptr(origin));
	glUniform1f(fov, 30);

	LoadScene1(program);

	ib = new ImageBuffer();
	ib->Initialize();

	float yoff = 0.0;
	
	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{

		vec3 origin = vec3(0, 0, 0);
		glUniform3fv(originLoc, 1, glm::value_ptr(origin));

		glm::mat4 trans;
		glm::mat4 oTrans;
		
		trans = glm::rotate(trans, pitchAmt /500, vec3(0, 1, 0));
		trans = glm::rotate(trans, yawAmt /500, vec3(1, 0, 0));
		oTrans = glm::translate(oTrans, cameraPosition);
		
		
		glUniformMatrix4fv(transform, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(oTransform, 1, GL_FALSE, glm::value_ptr(oTrans));
		// call function to draw our scene
		glDrawArrays(GL_TRIANGLE_STRIP, 0, geometry.elementCount);

		
		yoff += 0.001;
		if (yoff > 4) {
			yoff = 0;
		}

		// check for an report any OpenGL errors
		CheckGLErrors();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// reset state to default (no shader or geometry bound)

	// clean up allocated resources before exit
	DestroyGeometry(&geometry);
	glBindVertexArray(0);
	glUseProgram(0);
	glDeleteProgram(program);
	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Goodbye!" << endl;
	return 0;
}

void LoadScene1(GLuint program)
{
	vector<Light> lights = {};
	vector<Sphere> spheres = {};
	vector<Triangle> triangles = {};
	vector<Plane> planes = {};
	Light l1 = {
		vec4(0, 2.4, -7.75, 1),
		vec4(1, 1, 1, 1),
		1.0f,
		0.5f};

	lights.push_back(l1);

	Sphere reflectiveGrey = {
		vec4(0.9, -1.925, -6.69, 1),
		vec4(0.5, 0.5, 0.5, 1),
		vec4(1, 1, 1, 1),
		10.0f,
		0.825f,
		0.5};

	spheres.push_back(reflectiveGrey);

	//Blue pyramid
	Triangle blueT1 = {
		vec4(-0.4, -2.75, -9.55, 1),
		vec4(-0.93, 0.55, -8.51, 1),
		vec4(0.11, -2.75, -7.98, 1),
		vec4(0, 0, 1, 0),
		vec4(1, 1, 1, 1),
		10.f,
		0.5};
	Triangle blueT2 = {
		vec4(0.11, -2.75, -7.98, 1),
		vec4(-0.93, 0.55, -8.51, 1),
		vec4(-1.46, -2.75, -7.47, 1),
		vec4(0, 0, 1, 0),
		vec4(1, 1, 1, 1),
		10.f,
		0.5 };
	Triangle blueT3 = {
		vec4(-1.46, -2.75, -7.47, 1),
		vec4(-0.93, 0.55, -8.51, 1),
		vec4(-1.97, -2.75, -9.04, 1),
		vec4(0, 0, 1, 0),
		vec4(1, 1, 1, 1),
		10.f,
		0.5 };
	Triangle blueT4 = {
		vec4(-1.97, -2.75, -9.0, 14),
		vec4(-0.93, 0.55, -8.51, 1),
		vec4(-0.4, -2.75, -9.55, 1),
		vec4(0, 0, 1, 0),
		vec4(1, 1, 1, 1),
		10.f,
		0.5 };

	//Ceiling
	Triangle ceilingT1 = {
		vec4(2.75, 2.75, -10.5, 1),
		vec4(2.75, 2.75, -5, 1),
		vec4(-2.75, 2.75, -5, 1),
		vec4(1, 1, 1, 1),
		vec4(1, 1, 1, 1),
		10.0f};
	Triangle ceilingT2 = {
		vec4(-2.75, 2.75, -10.5, 1),
		vec4(2.75, 2.75, -10.5, 1),
		vec4(-2.75, 2.75, -5, 1),
		vec4(1, 1, 1, 1),
		vec4(1, 1, 1, 1),
		10.0f};

	//Green wall on right
	Triangle wallRightGreenT1 = {
		vec4(2.75, 2.75, -5, 1),
		vec4(2.75, 2.75, -10.5, 1),
		vec4(2.75, -2.75, -10.5, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10.0f,
	0.5};
	Triangle wallRightGreenT2 = {
		vec4(2.75, -2.75, -5, 1),
		vec4(2.75, 2.75, -5, 1),
		vec4(2.75, -2.75, -10.5, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10.0f,
	0.5};

	//Red wall on left
	Triangle wallLeftRedT1 = {
		vec4(-2.75, -2.75, -5, 1),
		vec4(-2.75, -2.75, -10.5, 1),
		vec4(-2.75, 2.75, -10.5, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 1, 1, 1),
		10.0f,
	0.5};
	Triangle wallLeftRedT2 = {
		vec4(-2.75, 2.75, -5, 1),
		vec4(-2.75, -2.75, -5, 1),
		vec4(-2.75, 2.75, -10.5, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 1, 1, 1),
		10.0f,
	0.5};

	//Floor
	Triangle floorT1 = {
		vec4(2.75, -2.75, -5, 1),
		vec4(2.75, -2.75, -10.5, 1),
		vec4(-2.75, -2.75, -10.5, 1),
		vec4(1, 1, 1, 1),
		vec4(1, 1, 1, 1),
		10.0f,
		0.0};
	Triangle floorT2 = {
		vec4(-2.75, -2.75, -5, 1),
		vec4(2.75, -2.75, -5, 1),
		vec4(-2.75, -2.75, -10.5, 1),
		vec4(1, 1, 1, 1),
		vec4(1, 1, 1, 1),
		10.0f,
		0.0 };

	triangles.push_back(blueT1);
	triangles.push_back(blueT2);
	triangles.push_back(blueT3);
	triangles.push_back(blueT4);
	triangles.push_back(ceilingT1);
	triangles.push_back(ceilingT2);
	triangles.push_back(wallRightGreenT1);
	triangles.push_back(wallRightGreenT2);
	triangles.push_back(wallLeftRedT1);
	triangles.push_back(wallLeftRedT2);
	triangles.push_back(floorT1);
	triangles.push_back(floorT2);

	//Back wall
	Plane backWall = {
		vec4(0, 0, 1, 0),
		vec4(0, 0, -10.5, 0),
		vec4(1, 1, 1, 1),
		vec4(1, 1, 1, 1),
		10.0f};

	planes.push_back(backWall);

	LoadShapes(spheres, triangles, planes, lights, program);
}

void LoadScene2(GLuint program)
{

	vector<Light> lights = {};
	vector<Sphere> spheres = {};
	vector<Triangle> triangles = {};
	vector<Plane> planes = {};
	Light light = {
		vec4(4, 6, -1, 1),
		vec4(1, 1, 1, 1),
		1.0,
		0.5};

	lights.push_back(light);
	// Floor
	Plane floorPlane = {
		vec4(0, 1, 0, 0),
		vec4(0, -1, 0, 0),
		vec4(1, 1, 1, 1),
		vec4(1, 1, 1, 1),
		10,
		0.5};

	// Back wall
	Plane wallPlane = {
		vec4(0, 0, 1, 0),
		vec4(0, 0, -12, 0),
		vec4(1, 1, 1, 1),
		vec4(1, 1, 1, 1),
		10};

	planes.push_back(floorPlane);
	planes.push_back(wallPlane);

	// Large yellow sphere
	Sphere sphereYellow = {
		vec4(1, -0.5, -3.5, 1),
		vec4(1, 1, 0, 1),
		vec4(1, 1, 1, 1),
		1000,
		0.5,
		0.0};

	// Reflective grey sphere
	Sphere sphereGrey = {
		vec4(0, 1, -5, 1),
		vec4(0.5, 0.5, 0.5, 0.5),
		vec4(1, 1, 1, 1),
		10,
		0.4,
		0.8};

	// Metallic purple sphere
	Sphere spherePurple = {
		vec4(-0.8, -0.75, -4, 1),
		vec4(1, 0, 1, 1),
		vec4(1, 0, 1, 1),
		100,
		0.25,
		0.33};

	spheres.push_back(sphereYellow);
	spheres.push_back(sphereGrey);
	spheres.push_back(spherePurple);

	// Green cone
	Triangle greenCone0 = {
		vec4(0, -1, -5.8, 1),
		vec4(0, 0.6, -5, 1),
		vec4(0.4, -1, -5.693, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10};
	Triangle greenCone1 = {
		vec4(0.4, -1, -5.693, 1),
		vec4(0, 0.6, -5, 1),
		vec4(0.6928, -1, -5.4, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10};
	Triangle greenCone2 = {
		vec4(0.6928, -1, -5.4, 1),
		vec4(0, 0.6, -5, 1),
		vec4(0.8, -1, -5, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10};
	Triangle greenCone3 = {
		vec4(0.8, -1, -5, 1),
		vec4(0, 0.6, -5, 1),
		vec4(0.6928, -1, -4.6, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10};
	Triangle greenCone4 = {
		vec4(0.6928, -1, -4.6, 1),
		vec4(0, 0.6, -5, 1),
		vec4(0.4, -1, -4.307, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10};
	Triangle greenCone5 = {
		vec4(0.4, -1, -4.307, 1),
		vec4(0, 0.6, -5, 1),
		vec4(0, -1, -4.2, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10};
	Triangle greenCone6 = {
		vec4(0, -1, -4.2, 1),
		vec4(0, 0.6, -5, 1),
		vec4(-0.4, -1, -4.307, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10};
	Triangle greenCone7 = {
		vec4(-0.4, -1, -4.307, 1),
		vec4(0, 0.6, -5, 1),
		vec4(-0.6928, -1, -4.6, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10};
	Triangle greenCone8 = {
		vec4(-0.6928, -1, -4.6, 1),
		vec4(0, 0.6, -5, 1),
		vec4(-0.8, -1, -5, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10};
	Triangle greenCone9 = {
		vec4(-0.8, -1, -5, 1),
		vec4(0, 0.6, -5, 1),
		vec4(-0.6928, -1, -5.4, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10};
	Triangle greenCone10 = {
		vec4(-0.6928, -1, -5.4, 1),
		vec4(0, 0.6, -5, 1),
		vec4(-0.4, -1, -5.693, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10};
	Triangle greenCone11 = {
		vec4(-0.4, -1, -5.693, 1),
		vec4(0, 0.6, -5, 1),
		vec4(0, -1, -5.8, 1),
		vec4(0, 1, 0, 0),
		vec4(1, 1, 1, 1),
		10};

	//, Shiny, red, icosahedron
	Triangle icosahedron0 = {
		vec4(-2, -1, -7, 1),
		vec4(-1.276, -0.4472, -6.474, 1),
		vec4(-2.276, -0.4472, -6.149, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron1 = {
		vec4(-1.276, -0.4472, -6.474, 1),
		vec4(-2, -1, -7, 1),
		vec4(-1.276, -0.4472, -7.526, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron2 = {
		vec4(-2, -1, -7, 1),
		vec4(-2.276, -0.4472, -6.149, 1),
		vec4(-2.894, -0.4472, -7, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron3 = {
		vec4(-2, -1, -7, 1),
		vec4(-2.894, -0.4472, -7, 1),
		vec4(-2.276, -0.4472, -7.851, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron4 = {
		vec4(-2, -1, -7, 1),
		vec4(-2.276, -0.4472, -7.851, 1),
		vec4(-1.276, -0.4472, -7.526, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron5 = {
		vec4(-1.276, -0.4472, -6.474, 1),
		vec4(-1.276, -0.4472, -7.526, 1),
		vec4(-1.106, 0.4472, -7, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron6 = {
		vec4(-2.276, -0.4472, -6.149, 1),
		vec4(-1.276, -0.4472, -6.474, 1),
		vec4(-1.724, 0.4472, -6.149, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron7 = {
		vec4(-2.894, -0.4472, -7, 1),
		vec4(-2.276, -0.4472, -6.149, 1),
		vec4(-2.724, 0.4472, -6.474, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron8 = {
		vec4(-2.276, -0.4472, -7.851, 1),
		vec4(-2.894, -0.4472, -7, 1),
		vec4(-2.724, 0.4472, -7.526, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron9 = {
		vec4(-1.276, -0.4472, -7.526, 1),
		vec4(-2.276, -0.4472, -7.851, 1),
		vec4(-1.724, 0.4472, -7.851, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron10 = {
		vec4(-1.276, -0.4472, -6.474, 1),
		vec4(-1.106, 0.4472, -7, 1),
		vec4(-1.724, 0.4472, -6.149, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron11 = {
		vec4(-2.276, -0.4472, -6.149, 1),
		vec4(-1.724, 0.4472, -6.149, 1),
		vec4(-2.724, 0.4472, -6.474, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron12 = {
		vec4(-2.894, -0.4472, -7, 1),
		vec4(-2.724, 0.4472, -6.474, 1),
		vec4(-2.724, 0.4472, -7.526, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron13 = {
		vec4(-2.276, -0.4472, -7.851, 1),
		vec4(-2.724, 0.4472, -7.526, 1),
		vec4(-1.724, 0.4472, -7.851, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron14 = {
		vec4(-1.276, -0.4472, -7.526, 1),
		vec4(-1.724, 0.4472, -7.851, 1),
		vec4(-1.106, 0.4472, -7, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron15 = {
		vec4(-1.724, 0.4472, -6.149, 1),
		vec4(-1.106, 0.4472, -7, 1),
		vec4(-2, 1, -7, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron16 = {
		vec4(-2.724, 0.4472, -6.474, 1),
		vec4(-1.724, 0.4472, -6.149, 1),
		vec4(-2, 1, -7, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron17 = {
		vec4(-2.724, 0.4472, -7.526, 1),
		vec4(-2.724, 0.4472, -6.474, 1),
		vec4(-2, 1, -7, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron18 = {
		vec4(-1.724, 0.4472, -7.851, 1),
		vec4(-2.724, 0.4472, -7.526, 1),
		vec4(-2, 1, -7, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};
	Triangle icosahedron19 = {
		vec4(-1.106, 0.4472, -7, 1),
		vec4(-1.724, 0.4472, -7.851, 1),
		vec4(-2, 1, -7, 1),
		vec4(1, 0, 0, 0),
		vec4(1, 0.5, 0.5, 1),
		10,
		0.5};

	triangles.push_back(greenCone0);
	triangles.push_back(greenCone1);
	triangles.push_back(greenCone2);
	triangles.push_back(greenCone3);
	triangles.push_back(greenCone4);
	triangles.push_back(greenCone5);
	triangles.push_back(greenCone6);
	triangles.push_back(greenCone7);
	triangles.push_back(greenCone8);
	triangles.push_back(greenCone9);
	triangles.push_back(greenCone10);
	triangles.push_back(greenCone11);

	triangles.push_back(icosahedron0);
	triangles.push_back(icosahedron1);
	triangles.push_back(icosahedron2);
	triangles.push_back(icosahedron3);
	triangles.push_back(icosahedron4);
	triangles.push_back(icosahedron5);
	triangles.push_back(icosahedron6);
	triangles.push_back(icosahedron7);
	triangles.push_back(icosahedron8);
	triangles.push_back(icosahedron9);
	triangles.push_back(icosahedron10);
	triangles.push_back(icosahedron11);
	triangles.push_back(icosahedron12);
	triangles.push_back(icosahedron13);
	triangles.push_back(icosahedron14);
	triangles.push_back(icosahedron15);
	triangles.push_back(icosahedron16);
	triangles.push_back(icosahedron17);
	triangles.push_back(icosahedron18);
	triangles.push_back(icosahedron19);
	LoadShapes(spheres, triangles, planes, lights, program);

}


void LoadScene3(GLuint program) {
	vector<Light> lights = {};
	vector<Sphere> spheres = {};
	vector<Triangle> triangles = {};
	vector<Plane> planes = {};
	Light light = {
		vec4(4, 6, -1, 1),
		vec4(1, 1, 1, 1),
		1.0,
		0.25 };
	Light light2 = {
		vec4(-4, 6, -1, 1),
		vec4(1, 1, 1, 1),
		1.0,
		0.25 };

	lights.push_back(light);
	lights.push_back(light2);
	// Floor
	Plane floorPlane = {
		vec4(0, 1, 0, 0),
		vec4(0, -1, 0, 0),
		vec4(1, 1, 1, 1),
		vec4(1, 1, 1, 1),
		10,
		0.0 };

	// Back wall
	Plane wallPlane = {
		vec4(0, 0, 1, 0),
		vec4(0, 0, -12, 0),
		vec4(1, 1, 1, 1),
		vec4(1, 1, 1, 1),
		10 };

	Sphere sphereA = {
		vec4(0, 0, -5, 1),
		vec4(0, 0, 0, 1),
		vec4(1, 1, 1, 1),
		100,
		1,
		0 };
	Sphere sphereB = {
		vec4(1, 1, -5, 1),
		vec4(0, 0, 0, 1),
		vec4(1, 1, 1, 1),
		100,
		0.5,
		0 };
	Sphere sphereC = {
		vec4(-1, 1, -5, 1),
		vec4(0, 0, 0, 1),
		vec4(1, 1, 1, 1),
		100,
		0.5,
		0 };

	planes.push_back(floorPlane);
	planes.push_back(wallPlane);

	spheres.push_back(sphereA);
	spheres.push_back(sphereB);
	spheres.push_back(sphereC);


	LoadShapes(spheres, triangles, planes, lights, program);
}

void LoadShapes(vector<Sphere> spheres, vector<Triangle> triangles, vector<Plane> planes, vector<Light> lights, GLuint program)
{
	GLuint sphereUBO, triangleUBO, planeUBO, lightUBO;
	glGenBuffers(1, &sphereUBO);
	glGenBuffers(1, &triangleUBO);
	glGenBuffers(1, &planeUBO);
	glGenBuffers(1, &lightUBO);

	glBindBuffer(GL_UNIFORM_BUFFER, sphereUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Sphere) * spheres.size(), spheres.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, triangleUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Triangle) * triangles.size(), triangles.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, planeUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Plane) * planes.size(), planes.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Light) * lights.size(), lights.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, sphereUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, triangleUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, planeUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, lightUBO);

	GLuint sphereCountIndex = glGetUniformLocation(program, "numSpheres");
	GLuint triangleCountIndex = glGetUniformLocation(program, "numTriangles");
	GLuint planeCountIndex = glGetUniformLocation(program, "numPlanes");
	GLuint lightCountIndex = glGetUniformLocation(program, "numLights");
	glUniform1i(sphereCountIndex, spheres.size());
	glUniform1i(triangleCountIndex, triangles.size());
	glUniform1i(planeCountIndex, planes.size());
	glUniform1i(lightCountIndex, lights.size());
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
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_P:
		{
			GLfloat * data = new GLfloat[3 * DIM * DIM];
			glReadPixels(0, 0, DIM, DIM, GL_RGB, GL_FLOAT, data);
			for (int y = 0; y < DIM; y++) {
				for (int x = 0; x < DIM; x++) {
					float rValue = data[3 * (x + y * DIM)];
					float gValue = data[3 * (x + y * DIM) + 1];
					float bValue = data[3 * (x + y * DIM) + 2];

					ib->SetPixel(x, y, vec3(rValue, gValue, bValue));
				}
			}
			free(data);
			ib->Render();
			ib->SaveToFile("scene.jpg");
		}
		case GLFW_KEY_O:
			pitchAmt = 0;
			yawAmt = 0;
			cameraPosition = vec3(0, 0, 0);
			break;
		case GLFW_KEY_1:
			LoadScene1(program);
			break;
		case GLFW_KEY_2:
			LoadScene2(program);
			break;
		case GLFW_KEY_3:
			LoadScene3(program);
			break;
		
		default:
			break;
		}
	}
	else {

		vec3 upVector = vec3(0, 1, 0);
		glm::mat4 trans;

		trans = glm::rotate(trans, pitchAmt / 500, vec3(0, 1, 0));
		trans = glm::rotate(trans, yawAmt / 500, vec3(1, 0, 0));

		vec4 forwardT = trans * vec4(0, 0, 1, 1);
		vec3 forwardVec = normalize(vec3(forwardT.x, forwardT.y, forwardT.z));
		vec3 leftVec = cross(upVector, forwardVec);

		switch (key) {
		case GLFW_KEY_W:
			cameraPosition -= forwardVec * 0.1f;
			break;
		case GLFW_KEY_A:
			cameraPosition -= leftVec * 0.1f;
			break;
		case GLFW_KEY_S:
			cameraPosition += forwardVec * 0.1f;
			break;
		case GLFW_KEY_D:
			cameraPosition += leftVec * 0.1f;
			break;
		default: break;
		}
	}
}


void CursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{

	if (leftPressed)
	{
		pitchAmt -= (xpos - xOld);
		yawAmt -= (ypos - yOld);
	}
	
	xOld = xpos;
	yOld = ypos;
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		leftPressed = (action == GLFW_PRESS);
}

GLuint InitializeShaders()
{
	string name = "ray";
	GLuint vertex, fragment;

	string vertexSource = LoadSource("shaders/" + name + ".vert");
	string fragmentSource = LoadSource("shaders/" + name + ".frag");

	if (vertexSource.empty() || fragmentSource.empty())
	{
		return 0;
	}

	vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	GLuint program = LinkProgram(vertex, fragment, 0, 0);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	if (CheckGLErrors())
	{
		return 0;
	}
	return program;
}