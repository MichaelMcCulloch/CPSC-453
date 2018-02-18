#include "LinesAndPolygons.h"
using namespace std;
using namespace glm;
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

// load, compile, and link shaders, returning true if successful
bool InitializeShaders(MyShader *shader)
{
	// load shader source from files
	string vertexSource = LoadSource("./shaders/vertex.glsl");
	string fragmentSource = LoadSource("./shaders/fragment.glsl");
	if (vertexSource.empty() || fragmentSource.empty())
		return false;

	// compile shader source into shader objects
	shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// link shader program
	shader->program = LinkProgram(shader->vertex, shader->fragment);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate shader-related objects
void DestroyShaders(MyShader *shader)
{
	// unbind any shader programs and destroy shader objects
	glUseProgram(0);
	glDeleteProgram(shader->program);
	glDeleteShader(shader->vertex);
	glDeleteShader(shader->fragment);
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data

bool InitializeVAO(MyGeometry *geometry)
{

	const GLuint VERTEX_INDEX = 0;
	const GLuint COLOUR_INDEX = 1;

	//Generate Vertex Buffer Objects
	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);

	// create another one for storing our colours
	glGenBuffers(1, &geometry->colourBuffer);

	//Set up Vertex Array Object
	// create a vertex array object encapsulating all our vertex attributes
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	// associate the position array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glVertexAttribPointer(
		VERTEX_INDEX, //Attribute index
		2,			  //# of components
		GL_FLOAT,	 //Type of component
		GL_FALSE,	 //Should be normalized?
		sizeof(vec2), //Stride - can use 0 if tightly packed
		0);			  //Offset to first element
	glEnableVertexAttribArray(VERTEX_INDEX);

	// associate the colour array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glVertexAttribPointer(
		COLOUR_INDEX, //Attribute index
		3,			  //# of components
		GL_FLOAT,	 //Type of component
		GL_FALSE,	 //Should be normalized?
		sizeof(vec3), //Stride - can use 0 if tightly packed
		0);			  //Offset to first element
	glEnableVertexAttribArray(COLOUR_INDEX);

	// unbind our buffers, resetting to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return !CheckGLErrors();
}

// create buffers and fill with geometry data, returning true if successful
bool LoadGeometry(MyGeometry *geometry, vec2 *vertices, vec3 *colours, int elementCount)
{
	geometry->elementCount = elementCount;

	// create an array buffer object for storing our vertices
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * geometry->elementCount, vertices, GL_STATIC_DRAW);

	// create another one for storing our colours
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * geometry->elementCount, colours, GL_STATIC_DRAW);

	//Unbind buffer to reset to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate geometry-related objects
void DestroyGeometry(MyGeometry *geometry)
{
	// unbind and destroy our vertex array object and associated buffers
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &geometry->vertexArray);
	glDeleteBuffers(1, &geometry->vertexBuffer);
	glDeleteBuffers(1, &geometry->colourBuffer);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderScene(MyGeometry *geometry, MyShader *shader)
{
	// clear screen to a dark grey colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(shader->program);
	glBindVertexArray(geometry->vertexArray);
	if (geometry->chunkSize != NULL)
	{
		for (int i = 0; i < geometry->elementCount; i += geometry->chunkSize)
		{
			glDrawArrays(geometry->mode, i, geometry->chunkSize);
		}
	}
	else
	{
		glDrawArrays(geometry->mode, 0, geometry->elementCount);
	}

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}

int numberKeyPressed(int key)
{
	switch (key)
	{
	case GLFW_KEY_0:
		return 0;
	case GLFW_KEY_1:
		return 1;
	case GLFW_KEY_2:
		return 2;
	case GLFW_KEY_3:
		return 3;
	case GLFW_KEY_4:
		return 4;
	case GLFW_KEY_5:
		return 5;
	case GLFW_KEY_6:
		return 6;
	case GLFW_KEY_7:
		return 7;
	case GLFW_KEY_8:
		return 8;
	case GLFW_KEY_9:
		return 9;
	default:
		return -1;
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
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_Q:
			currentShape = Squares;
			toRender = &square;
			break;
		case GLFW_KEY_W:
			currentShape = Spiral;
			toRender = &spiral;
			break;
		case GLFW_KEY_E:
			currentShape = SierpinskiTriangle;
			toRender = &sierpinskiTriangle;
			break;
		case GLFW_KEY_R:
			currentShape = Sponge;
			toRender = &sponge;
			break;
		case GLFW_KEY_T:
			currentShape = KoshSnowflake;
			toRender = &koshSnowflake;
			break;
		case GLFW_KEY_Y:
			currentShape = BarnsleyFern;
			toRender = &barnsleyFern;
			break;
		default:
			int number = numberKeyPressed(key);
			if (number == -1)
				break;
			switch (currentShape)
			{
			case Squares:
				createSquares(number);
				break;
			case Spiral:
				createSpiral(number);
				break;
			case SierpinskiTriangle:
				createSierpinskiTriangle(number);
				break;
			case Sponge:
				createSponge(number);
				break;
			case KoshSnowflake:
				createKoshSnowflake(number);
				break;
			case BarnsleyFern:
				break;
			}
		}
	}
}

void createSquares(int iteration)
{
	vector<vec2> vertices = {};
	vector<vec3> colors = {};

	const vec3 lightBlue = vec3(0 / 255, 191 / 255, 255 / 255),
			   darkBlue = vec3(25 / 255, 25 / 255, 112 / 255);
	float xMod = -1, yMod = -1;
	for (int i = 0; i < 4; i++)
	{
		vertices.push_back({xMod, yMod});
		(i % 2 == 0) ? yMod *= -1 : xMod *= -1;
		colors.push_back(darkBlue);
	}
	for (int iter = 1; iter <= (iteration * 2); iter++)
	{
		for (int vertex = 0; vertex < 4; vertex++)
		{
			vec2 a = vertices[(iter * 4 + vertex) - 4];
			vec2 b = vertices[(iter * 4 + vertex) - ((vertex < 3) ? 3 : 7)];
			vec2 next = mix(a, b, 0.5f);
			vertices.push_back(next);
			colors.push_back((iter % 2 == 0) ? darkBlue : lightBlue);
		}
	}

	if (!InitializeVAO(&square))
		cout << "Program failed to intialize geometry!" << endl;

	if (!LoadGeometry(&square, vertices.data(), colors.data(), vertices.size()))
		cout << "Failed to load geometry" << endl;

	square.chunkSize = 4;
	square.mode = GL_LINE_LOOP;
}
void createSpiral(int revolutions)
{
	vector<vec2> pts = {};
	vector<vec3> col = {};
	const float stop = revolutions * 2 * glm::pi<float>();
	float theta = 0.0;
	const vec3 lightBlue = vec3(0 / 255, 191 / 255, 255 / 255),
			   darkBlue = vec3(25 / 255, 25 / 255, 112 / 255);
	do
	{
		pts.push_back(vec2(theta * glm::cos(theta) / stop, theta * glm::sin(theta) / stop));
		col.push_back(vec3(mix(lightBlue, darkBlue, theta / stop)));
		theta += 0.1f;
	} while (theta < stop);

	if (!InitializeVAO(&spiral))
		cout << "Program failed to intialize geometry!" << endl;

	if (!LoadGeometry(&spiral, pts.data(), col.data(), pts.size()))
		cout << "Failed to load geometry" << endl;
	spiral.chunkSize = NULL;
	spiral.mode = GL_LINE_STRIP;
}
void createSierpinskiTriangle(int depth)
{

	const vec3 red = vec3(1, 0, 0),
			   green = vec3(0, 1, 0),
			   blue = vec3(0, 0, 1);
	pair<vector<vec2>, vector<vec3>> triangleData = {};
	if (depth > 0)
	{
		triangleData.first = {
			vec2(-1, -sqrt(3) / 2),
			vec2(1, -sqrt(3) / 2),
			vec2(0, sqrt(3) / 2)};
		triangleData.second = {
			green, blue, red};
	}

	if (depth > 1)
		triangleData = sierpinskiRecurse(triangleData, depth - 2);

	if (!InitializeVAO(&sierpinskiTriangle))
		cout << "Program failed to intialize geometry!" << endl;

	if (!LoadGeometry(&sierpinskiTriangle, triangleData.first.data(), triangleData.second.data(), triangleData.second.size()))
		cout << "Failed to load geometry" << endl;
	sierpinskiTriangle.chunkSize = 3;
	sierpinskiTriangle.mode = GL_TRIANGLES;
}
pair<vector<vec2>, vector<vec3>> sierpinskiRecurse(pair<vector<vec2>, vector<vec3>> initialPointsAndColors, int depth)
{
	vector<vec2> nextPoints = {};
	vector<vec3> nextColors = {};
	auto initialPoints = initialPointsAndColors.first;
	auto initialColors = initialPointsAndColors.second;
	int size = initialPoints.size();
	for (int i = 0; i < size; i += 3)
	{
		vec2 ptA = mix(initialPoints[0 + i], initialPoints[1 + i], 0.5f);
		vec2 ptB = mix(initialPoints[1 + i], initialPoints[2 + i], 0.5f);
		vec2 ptC = mix(initialPoints[2 + i], initialPoints[0 + i], 0.5f);

		vec3 clA = mix(initialColors[0 + i], initialColors[1 + i], 0.5f);
		vec3 clB = mix(initialColors[1 + i], initialColors[2 + i], 0.5f);
		vec3 clC = mix(initialColors[2 + i], initialColors[0 + i], 0.5f);

		vector<vec2> triA = {
			initialPoints[0 + i],
			ptA,
			ptC};
		vector<vec3> colA = {
			initialColors[0 + i],
			clA,
			clC};

		vector<vec2> triB = {
			ptA,
			initialPoints[1 + i],
			ptB};
		vector<vec3> colB = {
			clA,
			initialColors[1 + i],
			clB};

		vector<vec2> triC = {
			ptC,
			ptB,
			initialPoints[2 + i]};
		vector<vec3> colC = {
			clC,
			clB,
			initialColors[2 + i]};
		nextPoints.insert(nextPoints.end(), triA.begin(), triA.end());
		nextPoints.insert(nextPoints.end(), triB.begin(), triB.end());
		nextPoints.insert(nextPoints.end(), triC.begin(), triC.end());

		nextColors.insert(nextColors.end(), colA.begin(), colA.end());
		nextColors.insert(nextColors.end(), colB.begin(), colB.end());
		nextColors.insert(nextColors.end(), colC.begin(), colC.end());
	}
	pair<vector<vec2>, vector<vec3>> next = make_pair(nextPoints, nextColors);
	return (depth == 0) ? next : sierpinskiRecurse(next, depth - 1);
}
pair<vector<vec2>, vector<vec3>> spongeRecurse(pair<vector<vec2>, vector<vec3>> initialPointsAndColors, int depth)
{
	vector<vec2> nextPoints = {};
	vector<vec3> nextColors = {};
	auto initialPoints = initialPointsAndColors.first;
	auto initialColors = initialPointsAndColors.second;
	int size = initialPoints.size();

	const vec3 red = vec3(109.0f/255.0f, 26.0f/255.0f, 26.0f/255.0f),
			   green = vec3(0.0f/255.0f, 76.0f/255.0f, 35.0f/255.0f),
			   blue = vec3(0.0f/255.0f, 19.0f/255.0f, 99.0f/255.0f),
			   purple = vec3(69.0f/255.0f, 0.0f/255.0f, 99.0f/255.0f),
			   grey = vec3(50.0f/255.0f, 50.0f/255.0f, 50.0f/255.0f),
		       white = vec3(1,1,1);

	for (int i = 0; i < size; i += 4)
	{
		vec2 pt01A = mix(initialPoints[0 + i], initialPoints[1 + i], 1.0f / 3.0f);
		vec2 pt01B = mix(initialPoints[0 + i], initialPoints[1 + i], 2.0f / 3.0f);

		vec2 pt13A = mix(initialPoints[1 + i], initialPoints[3 + i], 1.0f / 3.0f);
		vec2 pt13B = mix(initialPoints[1 + i], initialPoints[3 + i], 2.0f / 3.0f);

		vec2 pt32A = mix(initialPoints[3 + i], initialPoints[2 + i], 1.0f / 3.0f);
		vec2 pt32B = mix(initialPoints[3 + i], initialPoints[2 + i], 2.0f / 3.0f);

		vec2 pt20A = mix(initialPoints[2 + i], initialPoints[0 + i], 1.0f / 3.0f);
		vec2 pt20B = mix(initialPoints[2 + i], initialPoints[0 + i], 2.0f / 3.0f);

		vec2 ptSW, ptNW, ptNE, ptSE;

		ptSW = vec2(pt20B.x, pt01A.y);
		ptNW = vec2(pt13A.x, pt01B.y);
		ptNE = vec2(pt13B.x, pt32A.y);
		ptSE = vec2(pt20A.x, pt32B.y);

		vector<vec2> square1 = {
			initialPoints[0 + i], pt01A, pt20B, ptSW};
		vector<vec3> color1 = {
			red,red,red,red
		};
		vector<vec2> square2 = {
			pt01A, pt01B, ptSW, ptNW};
		vector<vec3> color2 = {
			grey,grey,grey,grey
		};
		vector<vec2> square3 = {
			pt01B, initialPoints[1 + i], ptNW, pt13A};
		vector<vec3> color3 = {
			purple,purple,purple,purple
		};
		vector<vec2> square4 = {
			ptNW, pt13A, ptNE, pt13B};
		vector<vec3> color4 = {
			grey,grey,grey,grey
		};
		vector<vec2> square5 = {
			ptNE, pt13B, pt32A, initialPoints[3 + i]};
		vector<vec3> color5 = {
			blue,blue,blue,blue
		};
		vector<vec2> square6 = {
			ptSE, ptNE, pt32B, pt32A};
		vector<vec3> color6 = {
			grey,grey,grey,grey
		};
		vector<vec2> square7 = {
			pt20A, ptSE, initialPoints[2 + i], pt32B};
		vector<vec3> color7 = {
			green,green,green,green
		};
		vector<vec2> square8 = {
			pt20B, ptSW, pt20A, ptSE};
		vector<vec3> color8 = {
			grey,grey,grey,grey
		};

		

		nextPoints.insert(nextPoints.end(), square1.begin(), square1.end());
		nextPoints.insert(nextPoints.end(), square2.begin(), square2.end());
		nextPoints.insert(nextPoints.end(), square3.begin(), square3.end());
		nextPoints.insert(nextPoints.end(), square4.begin(), square4.end());
		nextPoints.insert(nextPoints.end(), square5.begin(), square5.end());
		nextPoints.insert(nextPoints.end(), square6.begin(), square6.end());
		nextPoints.insert(nextPoints.end(), square7.begin(), square7.end());
		nextPoints.insert(nextPoints.end(), square8.begin(), square8.end());

		nextColors.insert(nextColors.end(), color1.begin(), color1.end());
		nextColors.insert(nextColors.end(), color2.begin(), color2.end());
		nextColors.insert(nextColors.end(), color3.begin(), color3.end());
		nextColors.insert(nextColors.end(), color4.begin(), color4.end());
		nextColors.insert(nextColors.end(), color5.begin(), color5.end());
		nextColors.insert(nextColors.end(), color6.begin(), color6.end());
		nextColors.insert(nextColors.end(), color7.begin(), color7.end());
		nextColors.insert(nextColors.end(), color8.begin(), color8.end());
	}
	pair<vector<vec2>, vector<vec3>> next = make_pair(nextPoints, nextColors);
	return (depth == 0) ? next : spongeRecurse(next, depth - 1);
}
void createSponge(int depth)
{
	const vec3 grey = vec3(1, 1, 1);
	pair<vector<vec2>, vector<vec3>> spongeData = {};
	if (depth > 0)
	{
		spongeData.first = {
			vec2(-1, -1),
			vec2(-1, 1),
			vec2(1, -1),
			vec2(1, 1)};
		spongeData.second = {
			grey, grey, grey, grey};
	}

	if (depth > 1) {
		pair<vector<vec2>, vector<vec3>> spongeDataPairs = spongeRecurse(spongeData, depth - 2);
		spongeData.first.insert(spongeData.first.end(), spongeDataPairs.first.begin(), spongeDataPairs.first.end());
		spongeData.second.insert(spongeData.second.end(), spongeDataPairs.second.begin(), spongeDataPairs.second.end());
	}

	if (!InitializeVAO(&sponge))
		cout << "Program failed to intialize geometry!" << endl;

	if (!LoadGeometry(&sponge, spongeData.first.data(), spongeData.second.data(), spongeData.second.size()))
		cout << "Failed to load geometry" << endl;
	sponge.chunkSize = 4;
	sponge.mode = GL_TRIANGLE_STRIP;
}

void createKoshSnowflake(int depth)
{
	auto pair = crystalize(2);
	auto pts = pair.first;
	auto col = pair.second;

	if (!InitializeVAO(&koshSnowflake))
		cout << "Program failed to intialize geometry!" << endl;

	if (!LoadGeometry(&koshSnowflake, pts.data(), col.data(), pts.size()))
		cout << "Failed to load geometry" << endl;
	koshSnowflake.chunkSize = NULL;
	koshSnowflake.mode = GL_LINE_STRIP;
}

pair<vector<vec2>, vector<vec3>> crystalize(int depth)
{
	//start w/ triangle
	if (depth == 0) {
		vector<vec2> triangle ={
			vec2(-1, -sqrt(3) / 2),
			vec2(1, -sqrt(3) / 2),
			vec2(0, sqrt(3) / 2)};
		vector<vec3> colors = {
			vec3(1.0f,1.0f,1.0f),
			vec3(1.0f,1.0f,1.0f),
			vec3(1.0f,1.0f,1.0f)
		};
		return make_pair(triangle, colors);
	} else {
		auto next = crystalize(depth - 1);
		vector<vec2> crystal = {
			vec2((-1 - cos(1)) / (cos(1) + 1), 0),
			vec2(-cos(1) / (cos(1) + 1), 0),
			vec2(0, sin(1) / (cos(1) + 1)),
			vec2(cos(1) / (cos(1) + 1), 0),
			vec2((1 + cos(1)) / (cos(1) + 1), 0)
		};

		vector<vec3> colors = { 
			vec3(1.0f, 1.0f, 1.0f),
			vec3(1.0f, 1.0f, 1.0f),
			vec3(1.0f, 1.0f, 1.0f),
			vec3(1.0f, 1.0f, 1.0f),
			vec3(1.0f, 1.0f, 1.0f), };

		//find rotation of any two points on a line
		//find absolute distance, and scale factor.
		auto rotation = makeRotationMatrix(pi<float>());
		for (size_t i = 0; i < crystal.size(); i++)
		{
			crystal[i] = crystal[i] * rotation;
		}
		return make_pair(crystal, colors);

		
	}
}

mat2x2 makeRotationMatrix(float theta) {
	return mat2x2(cos(theta), -sin(theta), sin(theta), cos(theta));
}
void createBarnsleyFern(int depth)
{
}

// ==========================================================================
// PROGRAM ENTRY POINT

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	int width = 512, height = 512;
	window = glfwCreateWindow(width, height, "CPSC 453 OpenGL Boilerplate", 0, 0);
	if (!window)
	{
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, KeyCallback);
	glfwMakeContextCurrent(window);

//Intialize GLAD
#ifndef LAB_LINUX
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}
#endif

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	// call function to load and compile shader programs
	MyShader shader;
	if (!InitializeShaders(&shader))
	{
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}

	// three vertex positions and assocated colours of a triangle
	currentShape = Squares;
	createSquares(2);
	createSpiral(2);
	createSierpinskiTriangle(2);
	createSponge(2);
	createKoshSnowflake(2);

	toRender = &square;

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
		// call function to draw our scene
		RenderScene(toRender, &shader);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// clean up allocated resources before exit
	DestroyGeometry(&square);
	DestroyShaders(&shader);
	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Goodbye!" << endl;
	return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		 << "with GLSL [ " << glslver << " ] "
		 << "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors()
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag)
		{
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM" << endl;
			break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE" << endl;
			break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION" << endl;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl;
			break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY" << endl;
			break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input)
	{
		copy(istreambuf_iterator<char>(input),
			 istreambuf_iterator<char>(),
			 back_inserter(source));
		input.close();
	}
	else
	{
		cout << "ERROR: Could not load shader source from file "
			 << filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl
			 << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)
		glAttachShader(programObject, vertexShader);
	if (fragmentShader)
		glAttachShader(programObject, fragmentShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}
