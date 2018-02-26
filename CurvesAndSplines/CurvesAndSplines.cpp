#include "CurvesAndSplines.h"
#define HEIGHT 600
#define WIDTH 800
using namespace std;
using namespace glm;

uint whichPart = 1;

Geometry geo;
GLuint program;
glm::mat4 transformMatrix;
GLuint patchsize;

GLuint InitializeShaders(string shaderName) {
	string vertexName = shaderName + ".vert";
	string fragmentName = shaderName + ".frag";
	string tessControlName = shaderName + ".tesc";
	string tessEvalName = shaderName + ".tese";

	string vertexSource = LoadSource("shaders/" + vertexName);
	string fragmentSource = LoadSource("shaders/" + fragmentName);
	string tcsSource = LoadSource("shaders/" + tessControlName);
	string tesSource = LoadSource("shaders/" + tessEvalName);
	if (vertexSource.empty() || fragmentSource.empty())
		return 0;

	// compile shader source into shader objects
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	GLuint tcs = CompileShader(GL_TESS_CONTROL_SHADER, tcsSource);
	GLuint tes = CompileShader(GL_TESS_EVALUATION_SHADER, tesSource);

	// link shader program
	GLuint program = LinkProgram(vertex, fragment, tcs, tes);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(tcs);
	glDeleteShader(tes);

	if (CheckGLErrors())
		return 0;

	// check for OpenGL errors and return false if error occurred
	return program;
}

//return the set of control points, and their colors, and a transform matrix which puts everything into view
pair<pair<vector<vec2>, vector<vec3>>, glm::mat4> makePart1() {
	vector<vec2> quadBezierVertices = {
		vec2(1, 1),
		vec2(2, -1),
		vec2(0, -1),

		vec2(0, -1),
		vec2(-2, -1),
		vec2(-1, 1),

		vec2(-1, 1),
		vec2(0, 1),
		vec2(1, 1),

		vec2(1.2, 0.5),
		vec2(2.5, 1),
		vec2(1.3, -0.4)

	};

	vec3 white = vec3(1.0, 1.0, 1.0);
	vector<vec3> quadBezierColor;
	for (int i = 0; i < 12; i++) quadBezierColor.push_back(white);

	glm::mat4 transform;
	transform = glm::scale(transform, glm::vec3(0.5, 0.5, 1.0));

	auto controlPoints = std::make_pair(quadBezierVertices, quadBezierColor);
	auto data = std::make_pair(controlPoints, transform);
	return data;
}

GLFWwindow *InitializeGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //declare opengl version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //using only the core profile

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Colors And Effects", NULL, NULL); //create a sqare window object
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{ //initialize Glad
		cout << "Failed to initialize GLAD" << endl;
		return NULL;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	//sets the callbacks
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetErrorCallback(ErrorCallback);
	return window;
}



void RenderScenePH(Geometry *geometry, GLuint program)
{
	// clear screen to a dark grey colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(program);
	glBindVertexArray(geometry->vertexArray);
	

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}

//configure part 1 to be rendered:
//control points, program, transform, patch size
void preparePart1A() {
	//prepare the program
	program = InitializeShaders("part1");

	//prepare the geo
	pair<pair<vector<vec2>, vector<vec3>>, glm::mat4> data = makePart1();
	auto vertices = data.first.first;
	auto colors = data.first.second;
	InitializeVAO(&geo);
	LoadGeometry(&geo, vertices.data(), colors.data(), colors.size());

	//prepare transform
	transformMatrix = data.second;

	//prepare patch size
	patchsize = 3;

	
}

int main() {
	GLFWwindow *window = InitializeGLFW();
	GLuint programPart1 = InitializeShaders("part1");
	if (window == NULL)
		return -1;

	preparePart1A();

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);
		glBindVertexArray(geo.vertexArray);
		glPatchParameteri(GL_PATCH_VERTICES, patchsize);

		int transformLocation = glGetUniformLocation(program, "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));

		glDrawArrays(GL_PATCHES, 0, geo.elementCount);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

}