#include "Shader.h"

string LoadSource(const string &filename) {
	string src;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(src));
		input.close();
	}
	else {
		cout << "Could not read source file \'" << filename << "\'" << endl;
	}
	return src;
}
GLuint CompileShader(GLenum shaderType, const string &src) {
	GLuint shaderObject = glCreateShader(shaderType);

	const GLchar *src_ptr = src.c_str();

	glShaderSource(shaderObject, 1, &src_ptr, 0);
	glCompileShader(shaderObject);

	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (GL_FALSE == status) {
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		string type;
		switch (shaderType) {
		case GL_VERTEX_SHADER: type = "VERTEX"; break;
		case GL_FRAGMENT_SHADER: type = "FRAGMENT"; break;
		case GL_TESS_CONTROL_SHADER: type = "TESS CONTROL"; break;
		case GL_TESS_EVALUATION_SHADER: type = "TESS EVAL"; break;
		}
		cout << "ERROR: Could not compile " << type << " shader" << endl << endl;
		cout << info << endl;
	}

	return shaderObject;
}
GLuint LinkProgram(GLuint vertShader, GLuint fragShader) {
	GLuint programObject = glCreateProgram();
	if (vertShader && fragShader) {
		glAttachShader(programObject, vertShader);
		glAttachShader(programObject, fragShader);
	}

	glLinkProgram(programObject);

	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}
GLuint LinkProgram(GLuint vertShader, GLuint fragShader, GLuint tescShader, GLuint teseShader) {
	GLuint programObject = glCreateProgram();
	if (vertShader && fragShader) {
		glAttachShader(programObject, vertShader);
		glAttachShader(programObject, fragShader);
	}
	if (tescShader && teseShader) {
		glAttachShader(programObject, tescShader);
		glAttachShader(programObject, teseShader);
	}
	glLinkProgram(programObject);

	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}
GLuint InitializeShaders() {
	string vertSRC = LoadSource("./shaders/orrery.vert");
	string fragSRC = LoadSource("./shaders/orrery.frag");
	string tescSRC = LoadSource("./shaders/orrery.tesc");
	string teseSRC = LoadSource("./shaders/orrery.tese");

	GLuint vertex, fragment, tessEval, tessCtrl, program;

	if (vertSRC.empty() || fragSRC.empty()) {
		return NULL;
	}
	else {
		vertex = CompileShader(GL_VERTEX_SHADER, vertSRC);
		fragment = CompileShader(GL_FRAGMENT_SHADER, fragSRC);
	}

	if (!tescSRC.empty() && !teseSRC.empty()) {
		tessEval = CompileShader(GL_TESS_CONTROL_SHADER, tescSRC);
		tessCtrl = CompileShader(GL_TESS_EVALUATION_SHADER, teseSRC);
		program = LinkProgram(vertex, fragment, tessCtrl, tessEval);
		glDeleteShader(tessEval);
		glDeleteShader(tessCtrl);
	}
	else {
		program = LinkProgram(vertex, fragment);
	}

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