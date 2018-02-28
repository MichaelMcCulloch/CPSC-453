#include "CurvesAndSplines.h"
#define HEIGHT 600
#define WIDTH 800
using namespace std;
using namespace glm;


GLFWwindow *window;
GLuint noTess, cubic, quadratic, line, point;
bool drawControls;

Geometry geometry;
vector<MyGlyph> textToBeDisplayed;
float scaleFactorForStaticText = 0.0;
float scaleFactorForScrollText = 0.0;
bool isScrolling = false;
string fontChoice = "Lora-Regular.ttf";
string fox = "The Quick Brown Fox Jumps Over The Lazy Dog";
string myName = "Michael McCulloch";
string stringToRender = myName;

int partOfAssignmentToRender = 0; //0 = pot; 1 = fish; 2 = static name; 3 = scrolling Text


GLuint InitializeShaders(string vert, string frag, string tess)
{
	string vertexName = vert + ".vert";
	string fragmentName = frag + ".frag";
	string tessControlName = tess + ".tesc";
	string tessEvalName = tess + ".tese";

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
	if (tcsSource.empty() || tesSource.empty())
	{
		program = LinkProgram(vertex, fragment, 0, 0);
	}
	else
	{
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

//pot
void draw1A()
{
	vector<vec2> quadraticVertices = {
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
		vec2(1.3, -0.4)};

	vector<vec3> quadraticColors;
	vector<vec3> lineColors;
	for (int i = 0; i < quadraticVertices.size(); i++)
	{
		quadraticColors.push_back(vec3(1, 1, 1));
		lineColors.push_back(vec3(0, 0, 1));
	}

	mat4 transform;
	transform = glm::scale(transform, glm::vec3(0.5, 0.5, 1.0));

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	LoadGeometry(&geometry, quadraticVertices.data(), lineColors.data(), quadraticVertices.size());
	glUseProgram(noTess);
	int transformLocation = glGetUniformLocation(line, "transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
	glBindVertexArray(geometry.vertexArray);
	for (int i = 0; i < geometry.elementCount; i += 3)
	{
		glDrawArrays(GL_LINE_LOOP, i, 3);
	}

	LoadGeometry(&geometry, quadraticVertices.data(), quadraticColors.data(), quadraticVertices.size());
	glUseProgram(quadratic);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	transformLocation = glGetUniformLocation(quadratic, "transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
	glBindVertexArray(geometry.vertexArray);
	glDrawArrays(GL_PATCHES, 0, geometry.elementCount);
}

//fish
void draw1B()
{
	vector<vec2> cubicVertices = {
		vec2(1, 1),
		vec2(4, 0),
		vec2(6, 2),
		vec2(9, 1),

		vec2(8, 2),
		vec2(0, 8),
		vec2(0, -2),
		vec2(8, 4),

		vec2(5, 3),
		vec2(3, 2),
		vec2(3, 3),
		vec2(5, 2),

		vec2(3, 2.2),
		vec2(3.5, 2.7),
		vec2(3.5, 3.3),
		vec2(3, 3.8),

		vec2(2.8, 3.5),
		vec2(2.4, 3.8),
		vec2(2.4, 3.8),
		vec2(2.8, 3.5)};

	vector<vec3> cubicColors;
	vector<vec3> lineColors;
	for (int i = 0; i < cubicVertices.size(); i++)
	{
		cubicColors.push_back(vec3(1, 1, 1));
		lineColors.push_back(vec3(0, 0, 1));
	}

	mat4 transform;
	transform = glm::scale(transform, glm::vec3(0.2, 0.2, 1.0));
	transform = glm::translate(transform, glm::vec3(-5, -2.5, 0));

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	LoadGeometry(&geometry, cubicVertices.data(), lineColors.data(), cubicVertices.size());
	glUseProgram(noTess);
	int transformLocation = glGetUniformLocation(line, "transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
	glBindVertexArray(geometry.vertexArray);
	for (int i = 0; i < geometry.elementCount; i += 4)
	{
		glDrawArrays(GL_LINE_LOOP, i, 4);
	}

	LoadGeometry(&geometry, cubicVertices.data(), cubicColors.data(), cubicVertices.size());
	glUseProgram(cubic);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	transformLocation = glGetUniformLocation(cubic, "transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
	glBindVertexArray(geometry.vertexArray);
	glDrawArrays(GL_PATCHES, 0, geometry.elementCount);
}

//name 4 ways
//shits broke!!

//load glyph
void prepareText(string text, string font) {
	GlyphExtractor ge = GlyphExtractor();
	ge.LoadFontFile("Fonts/" + font);
	textToBeDisplayed = {};
	float adv = 0.0f;
	int spaceCount = 0;
	for (char &c : text)
	{
		if (c == ' ') spaceCount += 1;
		MyGlyph next = ge.ExtractGlyph((int)c);
		textToBeDisplayed.push_back(next);
		adv += next.advance;
		if (spaceCount == 2) scaleFactorForScrollText = 2 / adv; //two words on the screen
	}
	scaleFactorForStaticText = 2 / adv;
	if (scaleFactorForScrollText == 0.0) scaleFactorForScrollText = scaleFactorForStaticText;
	
	
}

void drawText(bool scroll, float step)
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	float advance = 0.0f;
	float scale = scaleFactorForStaticText;

	mat4 transform;
	float trans = advance - 1;

	if (scroll) {
		trans += step;
		scale = scaleFactorForScrollText;
	}

	transform = glm::translate(transform, vec3(trans, 0, 0));
	transform = glm::scale(transform, vec3(scale, scale, 1));

	vector<vec2> points, lines, quads, cubics;
	vector<vec3> pCol, lCol, qCol, cCol;
	for (MyGlyph &g : textToBeDisplayed)
	{

		for (MyContour &c : g.contours)
		{

			for (MySegment &s : c)
			{
				vector<vec2> *target = &points;
				vector<vec3> *colTarget = &pCol;
				switch (s.degree)
				{
				case 0:
					target = &points;
					colTarget = &pCol;

					break;
				case 1:
					target = &lines;
					colTarget = &lCol;
				
					break;
				case 2:
					target = &quads;
					colTarget = &qCol;
					
					break;
				case 3:
					target = &cubics;
					colTarget = &cCol;
				
					break;
				default:
					break;
				}
				for (int i = 0; i <= s.degree; i++)
				{
					target->push_back(vec2(s.x[i], s.y[i]) + vec2(advance, 0));
					colTarget->push_back(vec3(1, 1, 1));
				}
			}
		}
		advance += g.advance;
	}
	GLuint program;
	vector<vec2> *whichVertices = &points;
	vector<vec3> *whichColors = &pCol;
	for (int i = 0; i < 4; i++) {
		if (i == 0) {
			program = point; 
			whichVertices = &points;
			whichColors = &pCol;
		}
		if (i == 1){ 
			program = line;
			whichVertices = &lines;
			whichColors = &lCol;
		}
		if (i == 2) {
			program = quadratic;
			whichVertices = &quads;
			whichColors = &qCol;
		}
		if (i == 3) {
			program = cubic; 
			whichVertices = &cubics;
			whichColors = &cCol;
		}

		glPatchParameteri(GL_PATCH_VERTICES, i + 1);
		glUseProgram(program);
		int transformLocation = glGetUniformLocation(program, "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
		LoadGeometry(&geometry, whichVertices->data(), whichColors->data(), whichColors->size());
		glBindVertexArray(geometry.vertexArray);
		glDrawArrays(GL_PATCHES, 0, geometry.elementCount);
	}

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
		case GLFW_KEY_1: 
			partOfAssignmentToRender = 0;
			break;
		case GLFW_KEY_2:
			partOfAssignmentToRender = 1;
			break;
		case GLFW_KEY_3:
			partOfAssignmentToRender = 2;
			stringToRender = myName;
			updateText = true;
			break;
		case GLFW_KEY_4:
			partOfAssignmentToRender = 3;
			stringToRender = fox;
			updateText = true;
			break;
		case GLFW_KEY_A:
			fontChoice = "Lora-Regular.ttf";
			updateText = true;
			break;
		case GLFW_KEY_B:
			fontChoice = "SourceSansPro-Regular.otf";
			updateText = true;
			break;
		case GLFW_KEY_C:
			fontChoice = "AlexBrush-Regular.ttf";
			updateText = true;
			break;
		case GLFW_KEY_D:
			fontChoice = "Inconsolata.otf";
			updateText = true;
			break;
		default:
			break;
		}
		if (updateText) {
			prepareText(stringToRender, fontChoice);
		}
	}
	
}
int main()
{
	window = InitializeGLFW();
	if (window == NULL)
		return -1;

	noTess = InitializeShaders("noTess", "noTess", "");
	point = InitializeShaders("vert", "frag", "point");
	line = InitializeShaders("vert", "frag", "line");
	quadratic = InitializeShaders("vert", "frag", "quadratic");
	cubic = InitializeShaders("vert", "frag", "cubic");

	InitializeVAO(&geometry);
	prepareText(fox, fontChoice);

	float max = 4.0f * scaleFactorForScrollText / scaleFactorForStaticText;

	float offset = max/2;
	while (!glfwWindowShouldClose(window))
	{
		switch (partOfAssignmentToRender) {
		case 0:
			draw1A();
			break;
		case 1:
			draw1B();
			break;
		case 2:
			drawText(false, 0.0);
			break;
		case 3:
			
			drawText(true, offset - (max / 2.0f));
			offset -= 0.01f;
			if (offset < 0.0) offset = max;
			break;
		default: 
			break;
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}

	glfwTerminate();
}