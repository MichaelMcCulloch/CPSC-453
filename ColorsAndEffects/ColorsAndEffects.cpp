#include "ColorsAndEffects.h"

double WIDTH = 800, HEIGHT = 600;
double xOld, yOld;
double renderX = 0, renderY = 0;
double rotation = 0;
double zoom = 0;

bool leftPressed = false;
bool rightPressed = false;

int colorVariant = -1, //-1 = off; 0 = per the document
	edgeVariant = -1,  //-1 = off; 0 = verticalsobol, etc
	blurVariant = -1;  //-1 = off; 0 = on
Geometry *currentGeo;
MyTexture *currentTex;
Geometry geo1, geo2, geo3, geo4, geo5;
MyTexture tex1, tex2, tex3, tex4, tex5;

void configureTexture(const char *fileName, Geometry *geo, MyTexture *tex)
{
	InitializeTexture(tex, fileName, GL_TEXTURE_RECTANGLE);
	float h = (float)tex->height, w = (float)tex->width, max = glm::max(h, w);
	//create vertices
	vector<float> vertices = {
		w / max, h / max, 0.0f,   //top right
		w / max, -h / max, 0.0f,  //bottom right
		-w / max, -h / max, 0.0f, //bottom left
		-w / max, h / max, 0.0f   //top left
	};
	//texture coordinates
	vector<float> texCoords = {
		w, h,		//top right
		w, 0.0f,	//bottom right
		0.0f, 0.0f, //bottom left
		0.0f, h,	//top left
	};

	InitializeVAO(geo);
	LoadGeometry(geo, vertices, texCoords);
}

int main()
{
	GLFWwindow *window = InitializeGLFW();
	GLuint program = InitializeShaders();
	if (window == NULL || program == NULL)
		return -1;

	configureTexture("./resource/image1-mandrill.png", &geo1, &tex1);
	configureTexture("./resource/image2-uclogo.png", &geo2, &tex2);
	configureTexture("./resource/image3-aerial.jpg", &geo3, &tex3);
	configureTexture("./resource/image4-thirsk.jpg", &geo4, &tex4);
	configureTexture("./resource/image5-pattern.png", &geo5, &tex5);

	currentGeo = &geo1;
	currentTex = &tex1;

	while (!glfwWindowShouldClose(window))
	{
		//clear to color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat3 identity = {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

		glm::mat3 gaussianBlurKernel = {{0.04f, 0.12f, 0.04f}, {0.12f, 0.36f, 0.12f}, {0.04f, 0.12f, 0.04f}};

		glm::mat3 edgeKernel[3];
		edgeKernel[0] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
		edgeKernel[1] = {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}};
		edgeKernel[2] = {{0.0f, -1.0f, 0.0f}, {-1.0f, 5.0f, -1.0f}, {0.0f, -1.0f, 0.0f}};

		glm::vec3 luminance[3];
		luminance[0] = {0.333, 0.333, 0.333};
		luminance[1] = {0.299, 0.587, 0.114};
		luminance[2] = {0.213, 0.715, 0.072};

		glm::mat3 kernelToUse;
		glm::vec3 colorToUse;

		int usingKernel = false;

		if (blurVariant >= 0)
		{
			kernelToUse = gaussianBlurKernel;
			usingKernel = 0;
		}
		else if (edgeVariant >= 0)
		{
			kernelToUse = edgeKernel[edgeVariant];
			usingKernel = 0;
		}
		else if (colorVariant >= 0)
		{
			colorToUse = luminance[colorVariant];
			usingKernel = 1;
		}
		else
		{
			kernelToUse = identity;
			colorToUse = {1, 1, 1};
			usingKernel = -1;
		}

		float square = (float)glm::min(WIDTH, HEIGHT);
		float maxDim = (float)glm::max(WIDTH, HEIGHT);
		float xOffset = (float)renderX / square * 2;
		float yOffset = (float)renderY / square * 2;
		glm::mat4 transform;
		float zoomFactor = glm::pow(2, zoom);
		transform = glm::scale(transform, glm::vec3(HEIGHT / maxDim, WIDTH / maxDim, 1.0));
		transform = glm::translate(transform, glm::vec3(xOffset, yOffset, 0.0f));
		transform = glm::rotate(transform, (float)(rotation / square * 2), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, glm::vec3(zoomFactor, zoomFactor, zoomFactor));

		int transformLocation = glGetUniformLocation(program, "transform");
		int kernelLocation = glGetUniformLocation(program, "kernel");		//no effect = identity
		int greyscaleLocation = glGetUniformLocation(program, "greyscale"); //
		int colorOrKernelLocation = glGetUniformLocation(program, "usingKernel");

		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
		glUniformMatrix3fv(kernelLocation, 1, GL_FALSE, glm::value_ptr(kernelToUse));
		glUniform3fv(greyscaleLocation, 1, glm::value_ptr(colorToUse));
		glUniform1i(colorOrKernelLocation, usingKernel);
		//we're going to use this texture as a 2D target
		glBindTexture(currentTex->target, currentTex->textureID);

		//on this vertex data
		glBindVertexArray(currentGeo->vertexArray);

		//draw it like 2 triangles
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetErrorCallback(ErrorCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	return window;
}

bool InitializeVAO(Geometry *geometry)
{
	const GLuint VERTEX_INDEX = 0,
				 TEXCOORD_INDEX = 1;

	//create the buffers

	glGenBuffers(1, &geometry->vertexBuffer);
	glGenBuffers(1, &geometry->textureBuffer);

	//create and bind VAO
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	//setup vertex attributes
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glVertexAttribPointer(VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(VERTEX_INDEX);

	//setup texture coordinate attributes
	glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
	glVertexAttribPointer(TEXCOORD_INDEX, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	glEnableVertexAttribArray(TEXCOORD_INDEX);

	//unbind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	if (CheckGLErrors())
	{
		cout << "AT InitializeVAO" << endl;
		return false;
	}
	else
	{
		return true;
	}
}

bool LoadGeometry(Geometry *geometry, vector<float> vertices, vector<float> texCoords)
{
	geometry->elementCount = vertices.size();

	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, geometry->textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), texCoords.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (CheckGLErrors())
	{
		cout << "AT loadGeometry" << endl;
		return true;
	}
	else
	{
		return false;
	}
}

void DestroyGeometry(Geometry *geometry)
{
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &geometry->vertexArray);
	glDeleteBuffers(1, &geometry->vertexBuffer);
	glDeleteBuffers(1, &geometry->textureBuffer);
}

bool InitializeTexture(MyTexture *texture, const char *filename, GLuint target)
{
	int numComponents;
	stbi_set_flip_vertically_on_load(true); //invert the texture coordinate frame vertically so it matches our draw frame;
	UCHAR *data = stbi_load(filename, &texture->width, &texture->height, &numComponents, 0);
	if (data != nullptr)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		texture->target = target;
		glGenTextures(1, &texture->textureID);
		glBindTexture(texture->target, texture->textureID);

		GLuint format = GL_RGB;
		switch (numComponents)
		{
		case 4:
			format = GL_RGBA;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 2:
			format = GL_RG;
			break;
		case 1:
			format = GL_RED;
			break;
		default:
			cout << "Invalid Texture Format" << endl;
			break;
		};
		glTexImage2D(texture->target, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data);
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(texture->target, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		stbi_image_free(data);

		if (CheckGLErrors())
		{
			cout << "AT InitializeTexture" << endl;
			return true;
		}
		else
		{
			return false;
		}
	}

	return true; //error
}

void DestroyTexture(MyTexture *texture)
{
	glBindTexture(texture->target, 0);
	glDeleteTextures(1, &texture->textureID);
}

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char *description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

void CursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
	if (leftPressed)
	{
		renderX += (xpos - xOld);
		renderY -= (ypos - yOld);
	}
	else if (rightPressed)
	{
		rotation -= (ypos - yOld);
	}
	xOld = xpos;
	yOld = ypos;
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	int diffX = 0, diffY = 0;
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		leftPressed = (action == GLFW_PRESS);
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		rightPressed = (action == GLFW_PRESS);
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	zoom = (float)(zoom + yoffset);
}

// handles keyboard input events
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{

		case GLFW_KEY_1:
			currentGeo = &geo1;
			currentTex = &tex1;
			break;
		case GLFW_KEY_2:
			currentGeo = &geo2;
			currentTex = &tex2;
			break;
		case GLFW_KEY_3:
			currentGeo = &geo3;
			currentTex = &tex3;
			break;
		case GLFW_KEY_4:
			currentGeo = &geo4;
			currentTex = &tex4;
			break;
		case GLFW_KEY_5:
			currentGeo = &geo5;
			currentTex = &tex5;
			break;
		case GLFW_KEY_Q:
			colorVariant = (colorVariant + 1) % 3;
			blurVariant = -1;
			edgeVariant = -1;
			break;
		case GLFW_KEY_W:
			edgeVariant = (edgeVariant + 1) % 3;
			blurVariant = -1;
			colorVariant = -1;
			break;
		case GLFW_KEY_E:
			blurVariant = (blurVariant + 1) % 1;
			edgeVariant = -1;
			colorVariant = -1;
			break;
		case GLFW_KEY_R:
			blurVariant = -1;
			edgeVariant = -1;
			colorVariant = -1;
		default:
			break;
		}
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// handles resizing the window
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	WIDTH = width;
	HEIGHT = height;
	glViewport(0, 0, WIDTH, HEIGHT); //square
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

//load compile and link shaders
GLuint InitializeShaders()
{
	string vertexShaderSource = LoadSource("./shaders/vertex.vert");
	string fragmentShaderSource = LoadSource("./shaders/fragment.frag");

	if (vertexShaderSource.empty() || fragmentShaderSource.empty())
		return NULL;

	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	GLuint program = LinkProgram(vertex, fragment);
	glUseProgram(program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return program;
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