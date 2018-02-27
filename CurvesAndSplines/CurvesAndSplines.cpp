#include "CurvesAndSplines.h"
#define HEIGHT 600
#define WIDTH 800
using namespace std;
using namespace glm;

uint whichPart = 1;

Geometry geo;
GLuint program, noTess;
glm::mat4 transformMatrix;
GLuint patchsize;
bool drawControls, isCubic;

GLuint InitializeShaders(string shaderName) {
	string vertexName = shaderName + ".vert";
	string fragmentName = shaderName + ".frag";
	string tessControlName = shaderName + ".tesc";
	string tessEvalName = shaderName + ".tese";

	// compile shader source into shader objects
	GLuint vertex, fragment, tcs, tes;

	string vertexSource = LoadSource("shaders/" + vertexName);
	string fragmentSource = LoadSource("shaders/" + fragmentName);
	string tcsSource = LoadSource("shaders/" + tessControlName);
	string tesSource = LoadSource("shaders/" + tessEvalName);
	
	vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	tcs = CompileShader(GL_TESS_CONTROL_SHADER, tcsSource);
	tes = CompileShader(GL_TESS_EVALUATION_SHADER, tesSource);

	if (vertexSource.empty() || fragmentSource.empty())
		return 0;

	GLuint program;
	if (tcsSource.empty() || tesSource.empty()) {
		program = LinkProgram(vertex, fragment, 0, 0);
	}
	else {
		program = LinkProgram(vertex, fragment, tcs, tes);
	}
	

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(tcs);
	glDeleteShader(tes);

	if (CheckGLErrors())
		return 0;

	// check for OpenGL errors and return false if error occurred
	return program;
}

pair<pair<vector<vec2>, vector<vec3>>, glm::mat4> makePart1B() {
	vector<vec2> cubBezierVertices = {
		vec2(1,1),
		vec2(4,0),
		vec2(6,2),
		vec2(9,1),

		vec2(8,2),
		vec2(0,8),
		vec2(0,-2),
		vec2(8, 4),

		vec2(5,3),
		vec2(3,2),
		vec2(3,3),
		vec2(5,2),

		vec2(3, 2.2),
		vec2(3.5,2.7),
		vec2(3.5,3.3),
		vec2(3,3.8),

		vec2(2.8,3.5),
		vec2(2.4,3.8),
		vec2(2.4,3.8),
		vec2(2.8,3.5)
	};

	vec3 white = vec3(1.0, 1.0, 1.0);
	vector<vec3> cubBezierColor;
	for (int i = 0; i < cubBezierVertices.size(); i++) cubBezierColor.push_back(white);

	glm::mat4 transform;
	transform = glm::scale(transform, glm::vec3(0.2, 0.2, 1.0));
	transform = glm::translate(transform, glm::vec3(-5, -2.5, 0));

	auto controlPoints = std::make_pair(cubBezierVertices, cubBezierColor);
	auto data = std::make_pair(controlPoints, transform);
	return data;
}

//return the set of control points, and their colors, and a transform matrix which puts everything into view
pair<pair<vector<vec2>, vector<vec3>>, glm::mat4> makePart1A() {
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
	for (int i = 0; i < quadBezierVertices.size(); i++) quadBezierColor.push_back(white);

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


void preparePart1B() {
	//prepare the program
	program = InitializeShaders("part2");
	drawControls = true;

	//prepare the geo
	pair<pair<vector<vec2>, vector<vec3>>, glm::mat4> data = makePart1B();
	auto vertices = data.first.first;
	auto colors = data.first.second;
	InitializeVAO(&geo);
	LoadGeometry(&geo, vertices.data(), colors.data(), colors.size());

	//prepare transform
	transformMatrix = data.second;

	//prepare patch size
	patchsize = 4;
	isCubic = true;

}
//configure part 1 to be rendered:
//control points, program, transform, patch size
void preparePart1A() {
	//prepare the program
	program = InitializeShaders("part1");
	drawControls = true;

	//prepare the geo
	pair<pair<vector<vec2>, vector<vec3>>, glm::mat4> data = makePart1A();
	auto vertices = data.first.first;
	auto colors = data.first.second;
	InitializeVAO(&geo);
	LoadGeometry(&geo, vertices.data(), colors.data(), colors.size());

	//prepare transform
	transformMatrix = data.second;

	//prepare patch size
	patchsize = 3;
	isCubic = false;
	
}

int main() {
	GLFWwindow *window = InitializeGLFW();
	if (window == NULL)
		return -1;

	noTess = InitializeShaders("noTess");

	preparePart1B();

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);
		glBindVertexArray(geo.vertexArray);
		glPatchParameteri(GL_PATCH_VERTICES, patchsize);

		
		int cubicControlLocation = glGetUniformLocation(program, "isCubic");
		glUniform1i(cubicControlLocation, isCubic);

		int transformLocation = glGetUniformLocation(program, "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));

		glDrawArrays(GL_PATCHES, 0, geo.elementCount);


		if (drawControls) {

			glUseProgram(noTess);

			transformLocation = glGetUniformLocation(noTess, "transform");
			glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));

			for (int i = 0; i < geo.elementCount; i += patchsize) {
				glDrawArrays(GL_LINE_LOOP, i, patchsize);
			}
		}
		
		
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

}